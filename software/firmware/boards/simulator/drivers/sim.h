/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-18     tyx          first implementation
 */

#ifndef __SIM_H__
#define __SIM_H__


#ifdef __cplusplus
extern "C" {
#endif


    // basic
    typedef char                        sim_char_t;
    typedef unsigned char               sim_uchar_t;
    typedef short                       sim_short_t;
    typedef unsigned short              sim_ushort_t;
    typedef int                         sim_int_t;
    typedef unsigned int                sim_uint_t;
    typedef long                        sim_long_t;
    typedef unsigned long               sim_ulong_t;
    typedef void* sim_pointer_t;
    typedef void const* sim_cpointer_t;

    typedef signed char                 sim_int8_t;
    typedef unsigned char               sim_uint8_t;
    typedef signed short                sim_int16_t;
    typedef unsigned short              sim_uint16_t;
    typedef signed int                  sim_int32_t;
    typedef unsigned int                sim_uint32_t;
    typedef signed long long            sim_int64_t;
    typedef unsigned long long          sim_uint64_t;

    typedef sim_int_t                   sim_bool_t;
    typedef sim_uint8_t                 sim_byte_t;
    typedef sim_pointer_t               sim_handle_t;
    typedef sim_ulong_t                 sim_size_t;
    typedef sim_ulong_t                 sim_flag_t;
    typedef sim_ulong_t                 sim_type_t;
    typedef sim_uint64_t                sim_time_t;
    typedef sim_long_t                  sim_err_t;
    typedef sim_long_t                  sim_off_t;

#define SIM_PATH_MAXN        (4096)
#define SIM_INTERRUPT_MAX    (256)

#define SIM_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define SIM_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#define SIM_WAITING_FOREVER              (-1)
#define SIM_WAITING_NO                   (0)

typedef struct __sim_object_t
    {
        sim_handle_t parent;
    }sim_object_t;

    // bool
#define SIM_TRUE                     ((sim_bool_t)1)
#define SIM_FALSE                    ((sim_bool_t)0)

// error
#define SIM_OK                       ((sim_err_t)0)
#define SIM_ERROR                    ((sim_err_t)1)
#define SIM_TIMEOUT                  ((sim_err_t)2)
#define SIM_BUSY                     ((sim_err_t)3)
#define SIM_INVAL                    ((sim_err_t)4)

//file def
#define SIM_FILE_MODE_IFMT   0xF000 // File type mask
#define SIM_FILE_MODE_IFDIR  0x4000 // Directory
#define SIM_FILE_MODE_IFCHR  0x2000 // Character special
#define SIM_FILE_MODE_IFIFO  0x1000 // Pipe
#define SIM_FILE_MODE_IFREG  0x8000 // Regular
#define SIM_FILE_MODE_IREAD  0x0100 // Read permission, owner
#define SIM_FILE_MODE_IWRITE 0x0080 // Write permission, owner
#define SIM_FILE_MODE_IEXEC  0x0040 // Execute/search permission, owner

#define SIM_FILE_SEEK_CUR    1
#define SIM_FILE_SEEK_END    2
#define SIM_FILE_SEEK_SET    0

#define SIM_FILE_O_RDONLY      0x0000  // open for reading only
#define SIM_FILE_O_WRONLY      0x0001  // open for writing only
#define SIM_FILE_O_RDWR        0x0002  // open for reading and writing
#define SIM_FILE_O_APPEND      0x0008  // writes done at eof
#define SIM_FILE_O_CREAT       0x0100  // create and open file
#define SIM_FILE_O_TRUNC       0x0200  // open and truncate
#define SIM_FILE_O_EXCL        0x0400  // open only if file doesn't already exist

typedef struct __sim_file_stat_t
{
    sim_flag_t mode;
    sim_uint64_t size;
    sim_time_t atime;
    sim_time_t mtime;
    sim_time_t ctime;
}sim_file_stat_t;

typedef sim_object_t sim_file_t;
//file api

sim_err_t sim_file_open(sim_file_t* file, sim_char_t const* path, sim_flag_t oflags);
void sim_file_close(sim_file_t* file);
sim_int64_t sim_file_read(sim_file_t* file, sim_pointer_t buff, sim_size_t size);
sim_int64_t sim_file_write(sim_file_t* file, sim_cpointer_t buff, sim_size_t size);
sim_int64_t sim_file_seek(sim_file_t* file, sim_off_t offset, sim_flag_t mode);
sim_err_t sim_file_flush(sim_file_t* file);
sim_err_t sim_file_rename(sim_char_t const* new_path, sim_char_t const* old_path);
sim_err_t sim_file_remove(sim_char_t const* path);
sim_err_t sim_file_stat(sim_char_t const* path, sim_file_stat_t* stat);

//directory def
typedef struct __sim_directory_space_t
{
    sim_uint64_t blocks;
    sim_uint64_t bfree;
    sim_uint64_t bsize;
}sim_dir_space_t;

typedef sim_bool_t(*sim_dir_walk_func_t)(sim_char_t const*, sim_file_stat_t*, sim_pointer_t);

//directory api
sim_err_t sim_directory_create(sim_char_t const* path, sim_bool_t recursion);
sim_err_t sim_directory_delete(sim_char_t const* path);
sim_err_t sim_directory_space(sim_char_t const* path, sim_dir_space_t* space);
sim_err_t sim_directory_getfiles(sim_char_t const* path, sim_dir_walk_func_t func, sim_pointer_t priv);
sim_size_t sim_directory_current(sim_char_t *path, sim_size_t size);



#ifdef __cplusplus
}
#endif

#endif
