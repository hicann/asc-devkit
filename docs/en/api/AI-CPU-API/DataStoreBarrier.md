# DataStoreBarrier<a name="ZH-CN_TOPIC_0000002517444769"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-08-28T06:19:01.551Z pushedAt=2026-08-28T06:39:20.245Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported


## Description<a name="section259105813316"></a>

A data synchronization barrier instruction that blocks the current thread from executing until all previous write memory operations are completed and visible to other hardware units, after which subsequent instructions are executed. It is used to ensure data consistency across the multi-core architecture of the AI CPU and AI Core.

## Header Files to Include<a name="section78885814919"></a>

```
#include "aicpu_api.h"
```

## Prototype<a name="section2067518173415"></a>

```
DataStoreBarrier(void)
```

## Parameters<a name="section158061867342"></a>

None

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

-   This API can only be invoked through <<<...>>> and is used in heterogeneous compilation scenarios.

## Example<a name="section82241477610"></a>

In the Kernel-side implementation of the AI CPU operator, call `AscendC::DataStoreBarrier()` to ensure that the modifications to the Tiling data made by the AI CPU operator are written to memory, so that the AI Core operator can correctly read the Tiling data:

```
struct TilingInfo {
    uint64_t lock; // Lock for synchronization between AI CPU and AI Core.
    int8_t type;
    int8_t mode;
    int8_t len;
};
struct KernelArgs {
    uint32_t *xDevice;
    uint32_t *yDevice;
    uint32_t *zDevice;
    TilingInfo *ti; // Parameters shared with AI Core, used to synchronize tiling selection.
};

template<typename T, int8_t mode, int8_t len>
__aicore__ void hello_world_impl(GM_ADDR m)
{
    if constexpr (std::is_same_v<T, float>) {
       AscendC::printf("Hello World: float mode %u len %u.\n", mode, len);
    } else if constexpr (std::is_same_v<T, int>) {
       AscendC::printf("Hello World: int mode %u len %u.\n", mode, len);
    }
}

// Main entry of the AI Core operator.
// tilingInfo: Parameters passed together with the AI CPU operator, used for data sharing.
template<typename T, int8_t mode, int8_t len>
__mix__(1,2) __global__ __aicore__ void hello_world(GM_ADDR m, GM_ADDR TilingPtr)
{
     __gm__ struct KernelInfo::TilingInfo *ti = (__gm__ struct KernelInfo::TilingInfo *)TilingPtr;
    AscendC::GlobalTensor<uint64_t> lock;
    lock.SetGlobalBuffer(reinterpret_cast<__gm__ uint64_t *>(&ti->lock));
    if ASCEND_IS_AIV {
        if (AscendC::GetBlockIdx() == 0) {
            while (*reinterpret_cast<volatile __gm__ uint64_t*>(lock.GetPhyAddr(0)) == 0) {   // In sinking mode, AI Core waits for AICPU tiling computation to complete.
                AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE,
                    AscendC::DcciDst::CACHELINE_OUT>(lock);    //Directly access Global Memory to obtain the latest data.
            }
        }
    }
    // The above is one core waiting for AI CPU tiling computation to complete; inter-core synchronization is performed here.
    AscendC::SyncAll<false>();
    // Select different templates based on the tiling parameter values.
    if (ti->type ==0 && ti->mode == 1 && ti->len == 2) {
        hello_world_impl<float, 1, 2>(m);
    } else if (ti->type == 1 && ti->mode == 2 && ti->len == 4) {
        hello_world_impl<int, 2, 4>(m);
    }
    // Leave one core to release the lock after execution.
    if ASCEND_IS_AIV {
        if (AscendC::GetBlockIdx() == 0) {
            lock.SetValue(0, 0);  // Refresh the lock.
            AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE,
                AscendC::DcciDst::CACHELINE_OUT>(lock);    //Refresh the Dcache to synchronize data with the GM.
        }
    }
}

extern "C" __global__ __aicpu__ uint32_t MyAicpuKernel(void *arg)
{
    KernelArgs* cfg = (KernelArgs*)arg;
    AscendC::printf("MyAicpuKernel inited!\n");
    cfg->ti->lock = 1;
    cfg->ti->type = 1;
    cfg->ti->mode = 2;
    cfg->ti->len = 4;
    AscendC::DataStoreBarrier(); // Synchronize the write to tilingInfo.
    AscendC::printf("MyAicpuKernel inited type %u mode %u len %u end!\n", cfg->ti->type, cfg->ti->mode, cfg->ti->len);
    return 0;
}
```

