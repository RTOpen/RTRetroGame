/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
* 2012-11-27      prife        the first version
* 2013-03-03      aozima       add dfs_win32_stat st_mtime support.
* 2017-10-20      urey         support rt-thread 3.0
* 2021-06-30      tyx          rename winfs
*/

#include <rtthread.h>
#include <dfs_fs.h>
#include <dfs_file.h>
#include "sim.h"

#if defined(__MINGW32__) && defined(_NO_OLDNAMES)
#define O_RDONLY    _O_RDONLY
#define O_WRONLY    _O_WRONLY
#define O_RDWR      _O_RDWR
#define O_ACCMODE   _O_ACCMODE
#define O_APPEND    _O_APPEND
#define O_CREAT     _O_CREAT
#define O_TRUNC     _O_TRUNC
#define O_EXCL      _O_EXCL
#define O_TEXT      _O_TEXT
#define O_BINARY    _O_BINARY
#define O_TEMPORARY _O_TEMPORARY
#define O_NOINHERIT _O_NOINHERIT
#define O_SEQUENTIAL   _O_SEQUENTIAL
#define O_RANDOM    _O_RANDOM
#endif

/*
 * RT-Thread DFS Interface for win-directory as an disk device
 */
#define FILE_PATH_MAX           256  /* the longest file path */

enum winfs_type
{
    WINFS_TYPE_ROOT,
    WINFS_TYPE_DIRECTORY,
    WINFS_TYPE_REGULAR,
    WINFS_TYPE_UNKNOWN,
};

struct winfs_object
{
    enum winfs_type type;
    rt_size_t size;
    rt_list_t node;
    void (*destroy)(struct winfs_object* object);   /* free subobject */
    rt_size_t path_len;
    char path[FILE_PATH_MAX];
};

struct winfs_root
{
    struct winfs_object parent;
    rt_list_t list;
};

struct winfs_directory
{
    struct winfs_object parent;
    struct dfs_fd* dfs_object;
    struct __file_stat_t
    {
        sim_file_stat_t stat;
        char name[FILE_PATH_MAX];
    }*tab;
    long count;
    long pos;
};

struct winfs_regular
{
    struct winfs_object parent;
    sim_file_t file;
    struct dfs_fd* dfs_object;
};

struct winfs_errcode_map
{
    rt_uint16_t dfserr;
    rt_uint16_t win32err;
};

/* There are so many error codes in windows, you'd better google for details.
 * google  "System Error Codes (Windows)"
 * http://msdn.microsoft.com/ZH-CN/library/windows/desktop/ms681381(v=vs.85).aspx
 */
static const struct winfs_errcode_map errcode_map[] =
{
    {ENOENT, ERROR_FILE_NOT_FOUND },
    {ENOENT, ERROR_PATH_NOT_FOUND },
    {EEXIST, ERROR_FILE_EXISTS },
    {EEXIST, ERROR_ALREADY_EXISTS },
    {ENOTEMPTY, ERROR_DIR_NOT_EMPTY },
    {EBUSY, ERROR_PATH_BUSY },
    {EINVAL, ERROR_ACCESS_DENIED },
#if 0 /* TODO: MORE NEED BE ADDED */
    {DFS_STATUS_EISDIR, ERROR_FILE_EXISTS },
    {DFS_STATUS_ENOTDIR, ERROR_FILE_EXISTS },
    {DFS_STATUS_EBADF, ERROR_FILE_EXISTS },
    {DFS_STATUS_EBUSY, ERROR_FILE_EXISTS },
    {DFS_STATUS_ENOMEM, ERROR_FILE_EXISTS },
    {DFS_STATUS_ENOSPC, ERROR_FILE_EXISTS },
#endif
};

static rt_list_t fs_list = RT_LIST_OBJECT_INIT(fs_list);

static char* winpath_dirdup(char* des, const char* src)
{
    int i;
    char* pstr = des;

    if (*pstr != '\0')
    {
        pstr = pstr + strlen(pstr) - 1;
        if (*pstr != '/' && *pstr != '//')
        {
            *(++pstr) = '\\';
            *(++pstr) = '\0';
        }
        pstr = des;
    }
    if (*src == '/' || *src == '//')
    {
        src++;
    }
    strcat(pstr, src);
    for (i = 0; i < FILE_PATH_MAX; i++)
    {
        if (des[i] == '\0')
            break;
        if (des[i] == '/')
            des[i] = '\\';
    }
    RT_ASSERT(i < FILE_PATH_MAX);
    return des;
}

static int win32_result_to_dfs(DWORD res)
{
    int i;

    for (i = 0; i < sizeof(errcode_map) / sizeof(errcode_map[0]); i++)
    {
        if (errcode_map[i].win32err == (res & 0xFFFF))
        {
            return -errcode_map[i].dfserr;
        }
    }
    /* unknown error */
    rt_kprintf("dfs win32 error not supported yet: %d\n", res);
    return -1;
}

static struct winfs_object* dfs_winfs_object_init(struct winfs_object* object,
    rt_size_t size,
    rt_list_t* list,
    enum winfs_type type,
    const char* path,
    void (*destroy)(struct winfs_object* object))
{
    if (object)
    {
        RT_ASSERT(list != RT_NULL);
        RT_ASSERT(path != RT_NULL);
        RT_ASSERT(size >= sizeof(struct winfs_object));
        memset(object, 0, size);
        /* init object */
        object->type = type;
        object->size = size;
        object->destroy = destroy;
        object->path_len = strlen(path);
        rt_list_init(&object->node);
        RT_ASSERT(object->path_len < sizeof(object->path));
        strncpy(object->path, path, object->path_len + 1);
        /* insert list */
        rt_list_insert_before(list, &object->node);
    }
    return object;
}

static void dfs_winfs_object_deinit(struct winfs_object* object)
{
    if (object != RT_NULL)
    {
        rt_list_remove(&object->node);
        /* free subobject */
        if (object->destroy)
            object->destroy(object);
    }
}

static void dfs_winfs_root_destroy(struct winfs_object* object)
{
    struct winfs_root* root = (struct winfs_root*)object;
    struct winfs_object* sub_object;

    RT_ASSERT(root != RT_NULL);
    /* free subobject */
    while (!rt_list_isempty(&root->list))
    {
        sub_object = rt_list_first_entry(&root->list, struct winfs_object, node);
        dfs_winfs_object_deinit(sub_object);
    }
    free(root);
}

static int dfs_winfs_mount(struct dfs_filesystem* fs,
    unsigned long rwflag,
    const void* data)
{
    struct winfs_root* root;
    sim_file_stat_t sst = { 0 };
    char abs_path[FILE_PATH_MAX];

    if (data == RT_NULL)
    {
        rt_kprintf("root path setting error\n");
        return -EINVAL;
    }
    sim_directory_current(abs_path, FILE_PATH_MAX);
    strcpy(abs_path, (const char*)data);
    // check dir
    if (sim_file_stat(abs_path, &sst) != RT_EOK)
    {
        // create dir
        if (sim_directory_create(abs_path, RT_TRUE) != RT_EOK)
            return -1;
    }
    else if (!(sst.mode & SIM_FILE_MODE_IFDIR))
    {
        rt_kprintf("The destination path is not a folder\n");
        return -2;
    }
    root = (struct winfs_root*)dfs_winfs_object_init(
        malloc(sizeof(struct winfs_root)),
        sizeof(struct winfs_root),
        &fs_list,
        WINFS_TYPE_ROOT,
        abs_path,
        dfs_winfs_root_destroy);
    RT_ASSERT(root->parent.path_len > 0);
    RT_ASSERT(root->parent.path_len < (FILE_PATH_MAX - 1));
    /* init node */
    rt_list_init(&root->list);
    /* mount succeed! */
    fs->data = root;
    return 0;
}

static int dfs_winfs_unmount(struct dfs_filesystem* fs)
{
    RT_ASSERT(fs != RT_NULL);
    RT_ASSERT(fs->data != RT_NULL);
    /* free fs root */
    dfs_winfs_object_deinit(fs->data);
    fs->data = RT_NULL;
    return 0;
}

static int dfs_winfs_mkfs(rt_device_t devid)
{
    return -ENOSYS;
}

static int dfs_winfs_statfs(struct dfs_filesystem* fs,
    struct statfs* buf)
{
    struct winfs_root* root;
    sim_dir_space_t space = { 0 };

    RT_ASSERT(buf != RT_NULL);
    RT_ASSERT(fs != RT_NULL);
    RT_ASSERT(fs->data != RT_NULL);

    root = fs->data;

    if (sim_directory_space(root->parent.path, &space) != RT_EOK)
        return win32_result_to_dfs(GetLastError());

    buf->f_blocks = (size_t)space.blocks;
    buf->f_bfree = (size_t)space.bfree;
    buf->f_bsize = (size_t)space.bsize;

    return 0;
}

static void dfs_winfs_dir_destroy(struct winfs_object* object)
{
    struct winfs_directory* dir = (struct winfs_directory*)object;

    RT_ASSERT(dir != RT_NULL);
    RT_ASSERT(dir->dfs_object != RT_NULL);

    if (dir->tab != RT_NULL)
    {
        free(dir->tab);
        dir->tab = RT_NULL;
    }
    dir->dfs_object->data = RT_NULL;
    free(dir);
}

static rt_bool_t sim_dir_walk_func(sim_char_t const* path, sim_file_stat_t* st, sim_pointer_t prv)
{
    struct winfs_directory* dir = (struct winfs_directory*)prv;

    dir->tab = realloc(dir->tab, sizeof(dir->tab[0]) * (dir->count + 1));
    dir->tab[dir->count].stat = *st;
    strncpy(dir->tab[dir->count].name, path, sizeof(dir->tab[dir->count].name));
    dir->count += 1;
    return RT_TRUE;
}

static int dfs_winfs_open_dir(struct dfs_fd* file)
{
    struct winfs_root* root;
    struct winfs_directory* dir;
    char file_path[FILE_PATH_MAX];

    RT_ASSERT(file != RT_NULL);
    RT_ASSERT(file->fs != RT_NULL);
    RT_ASSERT(file->fs->data != RT_NULL);
    RT_ASSERT(file->path != RT_NULL);

    root = file->fs->data;
    /* make full path */
    winpath_dirdup(strncpy(file_path, root->parent.path, root->parent.path_len + 1), file->path);
    /* create a dir*/
    if (file->flags & O_CREAT)
    {
        if (sim_directory_create(file_path, RT_FALSE) != RT_EOK)
        {
            return win32_result_to_dfs(GetLastError());
        }
    }
    /* _findfirst will get '.' */
    /* save this pointer,will used by  dfs_win32_getdents*/
    dir = (struct winfs_directory*)dfs_winfs_object_init(
        malloc(sizeof(struct winfs_directory)),
        sizeof(struct winfs_directory),
        &root->list,
        WINFS_TYPE_DIRECTORY,
        file_path,
        dfs_winfs_dir_destroy);
    dir->dfs_object = file;
    /* open dir */
    if (sim_directory_getfiles(file_path, sim_dir_walk_func, dir) != RT_EOK)
    {
        dfs_winfs_object_deinit(&dir->parent);
        return win32_result_to_dfs(GetLastError());
    }
    file->data = (void*)dir;
    return 0;
}

static void dfs_winfs_file_destroy(struct winfs_object* object)
{
    struct winfs_regular* regular = (struct winfs_regular*)object;

    RT_ASSERT(object != RT_NULL);
    RT_ASSERT(regular->dfs_object != RT_NULL);

    sim_file_close(&regular->file);
    regular->dfs_object->data = RT_NULL;
    free(regular);
}

static int dfs_winfs_open_file(struct dfs_fd* file)
{
    struct winfs_root* root;
    struct winfs_regular* regular;
    rt_uint32_t oflag;
    sim_flag_t mode;
    char file_path[FILE_PATH_MAX];

    RT_ASSERT(file != RT_NULL);
    RT_ASSERT(file->fs != RT_NULL);
    RT_ASSERT(file->fs->data != RT_NULL);
    RT_ASSERT(file->path != RT_NULL);

    root = file->fs->data;
    oflag = file->flags;

    mode = SIM_FILE_O_RDONLY;

    /* regular file operations */
    if (oflag & O_RDONLY) mode |= SIM_FILE_O_RDONLY;
    if (oflag & O_WRONLY) mode |= SIM_FILE_O_WRONLY;
    if (oflag & O_RDWR)   mode |= SIM_FILE_O_RDWR;
    /* Opens the file, if it is existing. If not, a new file is created. */
    if (oflag & O_CREAT) mode |= SIM_FILE_O_CREAT | SIM_FILE_O_APPEND;
    /* Creates a new file. If the file is existing, it is truncated and overwritten. */
    if (oflag & O_TRUNC) mode |= SIM_FILE_O_TRUNC;
    /* Creates a new file. The function fails if the file is already existing. */
    if (oflag & O_EXCL) mode |= SIM_FILE_O_EXCL;
    /* make full path */
    winpath_dirdup(strncpy(file_path, root->parent.path, sizeof(file_path)), file->path);
    regular = (struct winfs_regular*)dfs_winfs_object_init(
        malloc(sizeof(struct winfs_regular)),
        sizeof(struct winfs_regular),
        &root->list,
        WINFS_TYPE_REGULAR,
        file_path,
        dfs_winfs_file_destroy);
    regular->dfs_object = file;
    if (sim_file_open(&regular->file, file_path, mode) != RT_EOK) /* _S_IREAD | _S_IWRITE */
    {
        dfs_winfs_object_deinit(&regular->parent);
        return win32_result_to_dfs(GetLastError());
    }
    /* save this pointer, it will be used when calling read(), write(),
     * flush(), seek(), and will be free when calling close()*/
    file->pos = 0;
    file->size = (size_t)sim_file_seek(&regular->file, 0, SIM_FILE_SEEK_END);
    if (oflag & O_APPEND)
    {
        file->pos = file->size;
    }
    else
    {
        sim_file_seek(&regular->file, 0, SIM_FILE_SEEK_SET);
    }
    file->data = (void*)regular;
    return 0;
}

static int dfs_winfs_open(struct dfs_fd* file)
{
    if (file->flags & O_DIRECTORY)   /* operations about dir */
    {
        return dfs_winfs_open_dir(file);
    }
    else
    {
        return dfs_winfs_open_file(file);
    }
}

static int dfs_winfs_close(struct dfs_fd* file)
{
    RT_ASSERT(file != RT_NULL);

    dfs_winfs_object_deinit(file->data);
    return 0;
}

static int dfs_winfs_ioctl(struct dfs_fd* file, int cmd, void* args)
{
    return -ENOSYS;
}

static int dfs_winfs_read(struct dfs_fd* file, void* buf, size_t len)
{
    struct winfs_regular* regular;
    sim_int64_t char_read;
    sim_file_stat_t stat = { 0 };

    RT_ASSERT(file != RT_NULL);
    RT_ASSERT(file->data != RT_NULL);

    regular = file->data;
    /* read */
    char_read = sim_file_read(&regular->file, (sim_pointer_t)buf, (sim_size_t)len);
    if (char_read < 0)
        return win32_result_to_dfs(GetLastError());
    /* update size */
    if (sim_file_stat(regular->parent.path, &stat) != RT_EOK)
        return win32_result_to_dfs(GetLastError());
    file->size = stat.size;
    /* update position */
    file->pos = (off_t)sim_file_seek(&regular->file, 0, SIM_FILE_SEEK_CUR);
    return (int)char_read;
}

static int dfs_winfs_write(struct dfs_fd* file, const void* buf, size_t len)
{
    struct winfs_regular* regular;
    sim_int64_t char_write;
    sim_file_stat_t stat = { 0 };

    RT_ASSERT(file != RT_NULL);
    RT_ASSERT(file->data != RT_NULL);

    regular = file->data;
    /* write */
    char_write = sim_file_write(&regular->file, (sim_pointer_t)buf, (sim_size_t)len);
    if (char_write < 0)
        return win32_result_to_dfs(GetLastError());
    /* update size */
    if (sim_file_stat(regular->parent.path, &stat) != RT_EOK)
        return win32_result_to_dfs(GetLastError());
    file->size = stat.size;
    /* update position */
    file->pos = (off_t)sim_file_seek(&regular->file, 0, SIM_FILE_SEEK_CUR);
    return (int)char_write;
}

static int dfs_winfs_flush(struct dfs_fd* file)
{
    struct winfs_regular* regular;

    RT_ASSERT(file != RT_NULL);
    RT_ASSERT(file->data != RT_NULL);

    regular = file->data;
    sim_file_flush(&regular->file);

    return 0;
}

static int dfs_winfs_seek(struct dfs_fd* file,
    rt_off_t offset)
{
    sim_int64_t res;

    /* set offset as current offset */
    if (file->type == FT_REGULAR)
    {
        struct winfs_regular* regular;

        RT_ASSERT(file != RT_NULL);
        RT_ASSERT(file->data != RT_NULL);

        regular = file->data;
        /* seek */
        res = sim_file_seek(&regular->file, offset, SIM_FILE_SEEK_SET);
        if (res >= 0)
            return (int)res;
        else
            return win32_result_to_dfs(GetLastError());
    }
    else if (file->type == FT_DIRECTORY)
    {
        struct winfs_directory* dir;

        RT_ASSERT(file != RT_NULL);
        RT_ASSERT(file->data != RT_NULL);

        dir = file->data;
        /* seek */
        if (offset >= dir->count)
        {
            dir->pos = dir->count - 1;
        }
        else
        {
            dir->pos = offset;
        }
        return dir->pos;
    }
    else
    {
        RT_ASSERT(0);
    }
    return -1;
}

/* return the size of struct dirent*/
static int dfs_winfs_getdents(struct dfs_fd* file, struct dirent* dirp, rt_uint32_t count)
{
    struct winfs_directory* dir;

    /* make integer count */
    if (count / sizeof(struct dirent) != 1)
        return -EINVAL;

    RT_ASSERT(file != RT_NULL);
    RT_ASSERT(file->data != RT_NULL);

    dir = file->data;

    if (dir->pos >= dir->count) //no more entries in this directory
        return 0;
    if (dir->tab[dir->pos].stat.mode & SIM_FILE_MODE_IFDIR)
        dirp->d_type = DT_DIR;
    else if (dir->tab[dir->pos].stat.mode & SIM_FILE_MODE_IFREG)
        dirp->d_type = DT_REG;
    else
        dirp->d_type = DT_UNKNOWN;

    dirp->d_namlen = (rt_uint8_t)strlen(dir->tab[dir->pos].name);
    strncpy(dirp->d_name, dir->tab[dir->pos].name, DFS_PATH_MAX);
    dirp->d_reclen = (rt_uint16_t)sizeof(struct dirent);

    dir->pos += 1;

    return sizeof(struct dirent);
}

static int dfs_winfs_unlink(struct dfs_filesystem* fs, const char* path)
{
    struct winfs_root* root;
    char full_path[FILE_PATH_MAX];

    RT_ASSERT(path != RT_NULL);
    RT_ASSERT(fs != RT_NULL);
    RT_ASSERT(fs->data != RT_NULL);

    root = fs->data;
    /* get full path */
    winpath_dirdup(strncpy(full_path, root->parent.path, sizeof(full_path)), path);

    if (sim_file_remove(full_path) != RT_EOK)
        return win32_result_to_dfs(GetLastError());

    return 0;
}

static int dfs_winfs_rename(struct dfs_filesystem* fs, const char* oldpath, const char* newpath)
{
    struct winfs_root* root;
    char op[FILE_PATH_MAX], np[FILE_PATH_MAX];

    RT_ASSERT(oldpath != RT_NULL);
    RT_ASSERT(newpath != RT_NULL);
    RT_ASSERT(fs != RT_NULL);
    RT_ASSERT(fs->data != RT_NULL);

    root = fs->data;

    winpath_dirdup(strncpy(op, root->parent.path, FILE_PATH_MAX), oldpath);
    winpath_dirdup(strncpy(np, root->parent.path, FILE_PATH_MAX), newpath);

    /* If the function fails, the return value is zero. */
    if (sim_file_rename(np, op) != RT_EOK)
        return win32_result_to_dfs(GetLastError());
    else
        return 0;
}

static int dfs_winfs_stat(struct dfs_filesystem* fs, const char* path, struct stat* st)
{
    struct winfs_root* root;
    char full_path[FILE_PATH_MAX];
    sim_file_stat_t sst = { 0 };

    RT_ASSERT(path != RT_NULL);
    RT_ASSERT(st != RT_NULL);
    RT_ASSERT(fs != RT_NULL);
    RT_ASSERT(fs->data != RT_NULL);

    root = fs->data;
    winpath_dirdup(strncpy(full_path, root->parent.path, FILE_PATH_MAX), path);

    if (sim_file_stat(full_path, &sst) != RT_EOK)
        return win32_result_to_dfs(GetLastError());

    /* make stat */
    st->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
        S_IWUSR | S_IWGRP | S_IWOTH;
    /* convert file info to dfs stat structure */
    if (sst.mode & SIM_FILE_MODE_IFDIR)
    {
        st->st_mode &= ~S_IFREG;
        st->st_mode |= S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH;
    }
    st->st_dev = 0;
    st->st_size = (uint32_t)sst.size;
    st->st_mtime = (time_t)sst.mtime;
    st->st_ctime = (time_t)sst.ctime;
    st->st_atime = (time_t)sst.atime;

    return 0;
}

static const struct dfs_file_ops dfs_winfile_ops =
{
    dfs_winfs_open,
    dfs_winfs_close,
    dfs_winfs_ioctl,
    dfs_winfs_read,
    dfs_winfs_write,
    dfs_winfs_flush,
    dfs_winfs_seek,
    dfs_winfs_getdents,
};

static const struct dfs_filesystem_ops dfs_windir_ops =
{
    "semihost", /* file system type: semihost */
    DFS_FS_FLAG_DEFAULT,
    &dfs_winfile_ops,
    dfs_winfs_mount,
    dfs_winfs_unmount,
    dfs_winfs_mkfs,
    dfs_winfs_statfs,
    dfs_winfs_unlink,
    dfs_winfs_stat,
    dfs_winfs_rename,
};

int dfs_winfs_init(void)
{
    /* register uffs file system */
    dfs_register(&dfs_windir_ops);
    return 0;
}
INIT_COMPONENT_EXPORT(dfs_winfs_init);
