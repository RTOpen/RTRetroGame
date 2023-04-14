/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-04-03     Urey         the first version
 * 2022-06-01     Meco Man     improve the init process
 */
#include <rtthread.h>
#include <rtdevice.h>

#define DBG_TAG     "FileSystem"
#define DBG_LVL     DBG_INFO
#include <rtdbg.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>

char ROOT_PATH[512] = "..\\..\\rootfs";

static int mnt_init(void)
{
#ifdef RT_USING_DFS_WINSHAREDIR

    if (dfs_mount(RT_NULL, "/", "semihost", 0, ROOT_PATH) == 0)
    {
        LOG_I("[wshare] File System on root ('wshare') initialized!");
    }
    else
    {
        LOG_E("[wshare] File System on root ('wshare') initialization failed!");
    }
#endif
    return 0;
}
INIT_ENV_EXPORT(mnt_init);
#endif /* RT_USING_DFS */
