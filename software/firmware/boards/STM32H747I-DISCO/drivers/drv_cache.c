#include <rthw.h>
#include <board.h>

void cache_icache_enable(void)
{
    SCB_EnableICache();
}

void cache_icache_disable(void)
{
    SCB_DisableICache();
}

rt_base_t cache_icache_status(void)
{
    return 0;
}

void cache_icache_ops(int ops, void *addr, int size)
{
    UNUSED(addr);
    UNUSED(size);
    if (ops & RT_HW_CACHE_INVALIDATE)
    {
        SCB_InvalidateICache();
    }
}

void cache_dcache_enable(void)
{
    SCB_EnableDCache();
}

void cache_dcache_disable(void)
{
    SCB_DisableDCache();
}

rt_base_t cache_dcache_status(void)
{
    return 0;
}

void cache_dcache_ops(int ops, void *addr, int size)
{
    if (ops & (RT_HW_CACHE_FLUSH | RT_HW_CACHE_INVALIDATE))
    {
        SCB_CleanInvalidateDCache_by_Addr(addr, size);
    }
    else if (ops & RT_HW_CACHE_FLUSH)
    {
        SCB_CleanDCache_by_Addr(addr, size);
    }
    else if (ops & RT_HW_CACHE_INVALIDATE)
    {
        SCB_InvalidateDCache_by_Addr(addr, size);
    }
    else
    {
        RT_ASSERT(0);
    }
}

int cache_hw_init(void)
{
    cache_icache_enable();
    cache_dcache_enable();
    return 0;
}
INIT_BOARD_EXPORT(cache_hw_init);
