/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-08-18     tyx          first implementation
 */

#ifdef _CRT_NO_TIME_T
#undef _CRT_NO_TIME_T
#endif
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <io.h>
#include <sim.h>


#define WIN32_PIPE_SIZE     (16384)
#define WIN32_PIPE_WRITE    (1)
#define WIN32_PIPE_READ     (0)


#if defined(_M_AMD64) || defined(_M_IX86)
#pragma intrinsic(_mm_pause)
#define __cpu_pause()       do { _mm_pause(); } while (0)
#elif defined(_M_IA64)
#pragma intrinsic(__yield)
#define __cpu_pause()       do { __yield(); } while (0)
#else
#define __cpu_pause()       do { YieldProcessor(); } while (0)
#endif


sim_size_t sim_directory_current(sim_char_t *path, sim_size_t size)
{
    return GetCurrentDirectory(size, path);
}

static sim_err_t sim_make_directory(sim_char_t* path, sim_size_t len)
{
    int index = len - 1;

    if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
    {
        if (strcmp("/", path) == 0 || strcmp("\\", path) == 0)
            return -1;

        while (index >= 0)
        {
            if (path[index] == '/' || path[index] == '\\')
            {
                path[index] = '\0';
                break;
            }
            index--;
        }
        if (sim_make_directory(path, index) != SIM_OK)
            return -SIM_ERROR;

        path[index] = '/';
        if (CreateDirectory(path, NULL) == FALSE)
        {
            return -SIM_ERROR;
        }
        return SIM_OK;
    }
    return SIM_OK;
}

 /*
  * @param path             Directory path
  * @param recursion        Recursive creation
  *
  * @return                 Error code, SIM_OK indicates success
  */
sim_err_t sim_directory_create(sim_char_t const* path, sim_bool_t recursion)
{
    sim_size_t path_len;
    sim_err_t err;

    if (path == NULL)
    {
        return -SIM_INVAL;
    }
    path_len = strlen(path);
    if (path_len >= SIM_PATH_MAXN)
    {
        // path is too long
        return -SIM_INVAL;
    }
    if (recursion)
    {
        sim_char_t *tmp_path = (sim_char_t*)malloc(SIM_PATH_MAXN);
        if (tmp_path == NULL)
        {
            return -SIM_ERROR;
        }
        memcpy(tmp_path, path, path_len + 1);
        err = sim_make_directory(tmp_path, path_len);
        free(tmp_path);
        return err;
    }
    else
    {
        if (CreateDirectory(path, NULL) == FALSE)
        {
            return -SIM_ERROR;
        }
        else
        {
            return SIM_OK;
        }
    }
}

sim_err_t sim_directory_delete(sim_char_t const* path)
{
    if (path == NULL)
    {
        return -SIM_INVAL;
    }
    if (RemoveDirectory(path) == FALSE)
    {
        return -SIM_ERROR;
    }
    return SIM_OK;
}

sim_err_t sim_directory_space(sim_char_t const* path, sim_dir_space_t* space)
{
    DWORD TotalClusters, FreeClusters, SectPerClust, BytesPerSect;

    if (path == NULL || space == NULL)
    {
        return -SIM_INVAL;
    }

    if (GetDiskFreeSpace(TEXT(path), &SectPerClust, &BytesPerSect,
        &FreeClusters, &TotalClusters) == FALSE)
    {
        return -SIM_ERROR;
    }
    space->blocks = TotalClusters;
    space->bfree = FreeClusters;
    space->bsize = (DWORD64)SectPerClust * (DWORD64)BytesPerSect;
    return SIM_OK;
}

sim_err_t sim_directory_getfiles(sim_char_t const* path, sim_dir_walk_func_t func, sim_pointer_t priv)
{
    WIN32_FIND_DATAA finddata = { 0 };
    sim_file_stat_t stat;
    HANDLE directory;
    sim_char_t* fullpath;
    size_t slen;

    if (path == NULL || func == NULL)
    {
        return -SIM_INVAL;
    }
    slen = strlen(path);
    if (slen == 0)
    {
        return -SIM_INVAL;
    }
    fullpath = malloc(SIM_PATH_MAXN);
    if (fullpath == NULL)
    {
        return -SIM_ERROR;
    }
    snprintf(fullpath, SIM_PATH_MAXN - 1, "%s%s*.*", path, path[slen - 1] == '\\' ? "" : "\\");
    // open directory
    directory = FindFirstFile(fullpath, &finddata);
    if (directory == INVALID_HANDLE_VALUE)
    {
        free(fullpath);
        return -SIM_ERROR;
    }
    // read directory
    do
    {
        if (strcmp(finddata.cFileName, ".") && strcmp(finddata.cFileName, ".."))
        {
            snprintf(fullpath, SIM_PATH_MAXN - 1, "%s\\%s", path, finddata.cFileName);
            fullpath[SIM_PATH_MAXN - 1] = '\0';
            // get file stat
            memset(&stat, 0, sizeof(stat));
            if (sim_file_stat(fullpath, &stat) == SIM_OK)
            {
                //call walk func
                if (func(finddata.cFileName, &stat, priv) == SIM_FALSE)
                {
                    break;
                }
            }
        }
        memset(&finddata, 0, sizeof(finddata));
    } while (FindNextFile(directory, &finddata));
    // end
    FindClose(directory);
    free(fullpath);
    return SIM_OK;
}

 /*
  * @param file             File object to be initialized, input parameter is not empty
  * @param path             File absolute path
  * @param oflags           Open flag
  * 
  * @return                 Error code, SIM_OK indicates success
  */
sim_err_t sim_file_open(sim_file_t* file, sim_char_t const *path, sim_flag_t oflags)
{
    DWORD access = GENERIC_READ;
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD attr = FILE_ATTRIBUTE_NORMAL;
    DWORD cflag = 0;
    HANDLE handle;

    // check parameter
    if (file == NULL || path == NULL)
    {
        return -SIM_INVAL;
    }
    // init access
    access = GENERIC_READ;
    if ((oflags & SIM_FILE_O_WRONLY) == SIM_FILE_O_WRONLY)
    {
        access = GENERIC_WRITE;
    }
    if ((oflags & SIM_FILE_O_RDWR) == SIM_FILE_O_RDWR)
    {
        access = GENERIC_WRITE | GENERIC_READ;
    }
    // init flag
    if (oflags & SIM_FILE_O_CREAT)
    {
        if (oflags & SIM_FILE_O_TRUNC)
        {
            // always create a new empty file
            cflag |= CREATE_ALWAYS;
        }
        else if (oflags & SIM_FILE_O_APPEND)
        {
            // create or open and append file
            cflag |= OPEN_ALWAYS;
        }
        else
        {
            // create a new file only if file not exists
            cflag |= CREATE_NEW;
        }
    }
    else if (oflags & SIM_FILE_O_TRUNC)
    {
        // open and truncate an existing file
        cflag |= TRUNCATE_EXISTING;
    }
    if (cflag == 0)
    {
        cflag = OPEN_EXISTING;
    }
    // open file
    handle = CreateFile(path, access, share, NULL, cflag, attr, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        file->parent = INVALID_HANDLE_VALUE;
        return -SIM_ERROR;
    }
    file->parent = (sim_handle_t)handle;
    return SIM_OK;
}

/*
 * @param file             File object, input parameter is not empty
 */
void sim_file_close(sim_file_t* file)
{
    HANDLE handle;

    if (file && file->parent != (sim_handle_t)INVALID_HANDLE_VALUE)
    {
        handle = (HANDLE)file->parent;
        file->parent = (sim_handle_t)INVALID_HANDLE_VALUE;
        if (CloseHandle(handle) == FALSE)
        {
            assert(0);
        }
    }
}

/*
 * @param file             File object, input parameter is not empty
 * @param path             File absolute path
 * @param oflags           Open flag
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_int64_t sim_file_read(sim_file_t* file, sim_pointer_t buff, sim_size_t size)
{
    DWORD real_size = 0;

    // check parameter
    if (file == NULL || file->parent == (sim_handle_t)INVALID_HANDLE_VALUE)
    {
        return -SIM_INVAL;
    }
    if (size > 0 && buff == NULL)
    {
        return -SIM_INVAL;
    }
    if (size == 0)
    {
        return 0;
    }
    if (ReadFile((HANDLE)file->parent, buff, (DWORD)size, &real_size, NULL) == FALSE)
    {
        return -SIM_ERROR;
    }
    return real_size;
}

/*
 * @param file             File object, input parameter is not empty
 * @param path             File absolute path
 * @param oflags           Open flag
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_int64_t sim_file_write(sim_file_t* file, sim_cpointer_t buff, sim_size_t size)
{
    DWORD real_size = 0;

    // check parameter
    if (file == NULL || file->parent == (sim_handle_t)INVALID_HANDLE_VALUE)
    {
        return -SIM_INVAL;
    }
    if (size > 0 && buff == NULL)
    {
        return -SIM_INVAL;
    }
    if (size == 0)
    {
        return 0;
    }
    if (WriteFile((HANDLE)file->parent, buff, (DWORD)size, &real_size, NULL) == FALSE)
    {
        return -SIM_ERROR;
    }
    return real_size;
}

/*
 * @param file             File object, input parameter is not empty
 * @param offset           Offset address
 * @param mode             Offset mode
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_int64_t sim_file_seek(sim_file_t* file, sim_off_t offset, sim_flag_t mode)
{
    LARGE_INTEGER des_pos, new_pos;
    DWORD MoveMethod = FILE_BEGIN;

    if (file == NULL || file->parent == (sim_handle_t)INVALID_HANDLE_VALUE)
    {
        return -SIM_INVAL;
    }
    memset(&des_pos, 0, sizeof(des_pos));
    memset(&new_pos, 0, sizeof(new_pos));
    des_pos.QuadPart = (LONGLONG)offset;
    if (mode == SIM_FILE_SEEK_CUR)
    {
        MoveMethod = FILE_CURRENT;
    }
    else if (mode == SIM_FILE_SEEK_END)
    {
        MoveMethod = FILE_END;
    }
    else if (mode == SIM_FILE_SEEK_SET)
    {
        MoveMethod = FILE_BEGIN;
    }
    else
    {
        assert(0);
    }
    if (SetFilePointerEx((HANDLE)file->parent, des_pos, &new_pos, MoveMethod) == FALSE)
    {
        return -SIM_ERROR;
    }
    return new_pos.QuadPart;
}

/*
 * @param file             File object, input parameter is not empty
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_err_t sim_file_flush(sim_file_t* file)
{
    if (file == NULL || file->parent == (sim_handle_t)INVALID_HANDLE_VALUE)
    {
        return -SIM_INVAL;
    }
    if (FlushFileBuffers((HANDLE)file->parent) == FALSE)
    {
        return -SIM_ERROR;
    }
    return SIM_OK;
}

/*
 * @param new_path         Target absolute paths
 * @param new_path         Current absolute path
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_err_t sim_file_rename(sim_char_t const* new_path, sim_char_t const* old_path)
{
    if (new_path == NULL || old_path == NULL)
    {
        return -SIM_INVAL;
    }
    if (MoveFileEx(old_path, new_path, MOVEFILE_REPLACE_EXISTING) == FALSE)
    {
        return -SIM_ERROR;
    }
    return SIM_OK;
}

/*
 * @param path             File absolute path
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_err_t sim_file_remove(sim_char_t const* path)
{
    DWORD attrs;

    if (path == NULL)
    {
        return -SIM_INVAL;
    }
    attrs = GetFileAttributes(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
    {
        return -SIM_ERROR;
    }
    if (attrs & FILE_ATTRIBUTE_READONLY)
    {
        if (SetFileAttributes(path, attrs & ~FILE_ATTRIBUTE_READONLY) == FALSE)
        {
            return -SIM_ERROR;
        }
    }
    if (attrs & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (RemoveDirectory(path) == FALSE)
            return -SIM_ERROR;
    }
    else if (attrs != 0xFFFFFFFF)
    {
        if (DeleteFile(path) == FALSE)
            return -SIM_ERROR;
    }
    else
    {
        return -SIM_ERROR;
    }
    return SIM_OK;
}

/*
 * @param path             File absolute path
 *
 * @return                 Error code, SIM_OK indicates success
 */
sim_err_t sim_file_stat(sim_char_t const* path, sim_file_stat_t* stat)
{
    WIN32_FILE_ATTRIBUTE_DATA attribute = { 0 };
    LARGE_INTEGER time_tmp;

    if (path == NULL || stat == NULL)
    {
        return -SIM_INVAL;
    }
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &attribute) == FALSE)
    {
        return -SIM_ERROR;
    }
    //file type
    if (attribute.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        stat->mode = SIM_FILE_MODE_IFDIR;
    }
    else if (attribute.dwFileAttributes != 0xFFFFFFFF)
    {
        stat->mode = SIM_FILE_MODE_IFREG;
    }
    //file size
    stat->size = ((sim_uint64_t)attribute.nFileSizeHigh << 32) | (sim_uint64_t)attribute.nFileSizeLow;
    //last access time
    time_tmp.LowPart = attribute.ftLastAccessTime.dwLowDateTime;
    time_tmp.HighPart = attribute.ftLastAccessTime.dwHighDateTime;
    /* removes the diff between 1970 and 1601. */
    time_tmp.QuadPart -= 11644473600000 * 10000;
    /* converts back from 100-nanoseconds to seconds. */
    time_tmp.QuadPart /= 10UL * 1000 * 1000;
    stat->atime = time_tmp.QuadPart;
    //last modify time
    time_tmp.LowPart = attribute.ftLastWriteTime.dwLowDateTime;
    time_tmp.HighPart = attribute.ftLastWriteTime.dwHighDateTime;
    /* removes the diff between 1970 and 1601. */
    time_tmp.QuadPart -= 11644473600000 * 10000;
    /* converts back from 100-nanoseconds to seconds. */
    time_tmp.QuadPart /= 10UL * 1000 * 1000;
    stat->mtime = time_tmp.QuadPart;
    //last create time
    time_tmp.LowPart = attribute.ftCreationTime.dwLowDateTime;
    time_tmp.HighPart = attribute.ftCreationTime.dwHighDateTime;
    /* removes the diff between 1970 and 1601. */
    time_tmp.QuadPart -= 11644473600000 * 10000;
    /* converts back from 100-nanoseconds to seconds. */
    time_tmp.QuadPart /= 10UL * 1000 * 1000;
    stat->ctime = time_tmp.QuadPart;
    return SIM_OK;
}


/*
 * @param us            Thread hang time in microseconds, non negative input parameter 0
 */
void sim_usleep(sim_time_t us)
{
    HANDLE timer;
    LARGE_INTEGER due;

    timer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (timer == NULL)
    {
        return;
    }
    due.QuadPart = -((long long)us * 1000 / 100);
    if (SetWaitableTimer(timer, &due, FALSE, NULL, NULL, 0) == TRUE)
    {
        if (WaitForSingleObject(timer, INFINITE) != WAIT_OBJECT_0)
        {
            assert(0);
        }
    }
    CloseHandle(timer);
}

/*
 * @param ms            The thread is suspended for milliseconds, and the input parameter is non negative 0
 */
void sim_msleep(sim_time_t ms)
{
    Sleep((DWORD)ms);
}

/*
 * @param s            Thread hang seconds, non negative input parameter 0
 */
void sim_sleep(sim_time_t s)
{
    sim_msleep(s * 1000);
}

/*
 * @return             Returns the millisecond time the program ran
 */
sim_time_t sim_mtime(void)
{
    return (sim_time_t)GetTickCount();
}

/*
 * @return             Returns the microsecond time the program was running
 */
sim_time_t sim_utime(void)
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;

    memset(&frequency, 0, sizeof(frequency));
    memset(&counter, 0, sizeof(counter));
    if (QueryPerformanceFrequency(&frequency) == FALSE)
    {
        return 0;
    }
    if (frequency.QuadPart == 0)
    {
        return 0;
    }
    if (QueryPerformanceCounter(&counter) == FALSE)
    {
        return 0;
    }
    if (counter.QuadPart == 0)
    {
        return 0;
    }
    return (sim_time_t)((counter.QuadPart * 1000000) / frequency.QuadPart);
}



/*
 * @param fd               fd[0] read pipi
 * @param fd               Read buffer
 * @param size             Buffer max size
 * 
 * @return                 Read length
 */
int sim_pipe_read(int fd, void *buf, int size)
{
    return _read(fd, buf, size);
}

/*
 * @param fd               fd[1] write pipi
 * @param fd               Write buffer
 * @param size             Buffer max size
 * 
 * @return                 Write length
 */
int sim_pipe_write(int fd, const void *buf, int size)
{
    return _write(fd, buf, size);
}


