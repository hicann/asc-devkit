# DataStoreBarrier<a name="ZH-CN_TOPIC_0000002517444769"></a>

<!-- md-trans-meta sourceCommit=13f1af5a09ac01160909f73c31bb7f8d23f4ec30 translatedAt=2026-11-17T03:49:47.376Z pushedAt=2026-11-17T09:52:41.779Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

<!-- @ref: asc-devkit/res/docs/zh/api/AI-CPU-API/DataStoreBarrier_res.md#id1 -->

## Description<a name="section259105813316"></a>

A data synchronization barrier instruction that blocks the current thread from executing until all previous write memory operations are completed and visible to other hardware units, after which subsequent instructions continue to execute. It is used to ensure data consistency between the AI CPU and multiple AI Core cores.

## Required Header Files<a name="section78885814919"></a>

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

-   This API can only be invoked through <<<...\>\>\> and is used in heterogeneous compilation scenarios.

## Example<a name="section82241477610"></a>

Call AscendC::DataStoreBarrier\(\) in the kernel function implementation of the AI CPU operator to ensure that the modifications to the Tiling data by the AI CPU operator are written to memory, so that the AI Core operator can correctly read the Tiling data:

```
namespace KernelInfo {
struct TilingInfo {
    uint64_t lock; // Lock for synchronization between the AI CPU and AI Core.
    int8_t type;
    int8_t mode;
    int8_t len;
};
struct KernelArgs {
    uint32_t *xDevice;
    uint32_t *yDevice;
    uint32_t *zDevice;
    TilingInfo *ti; // Parameter shared with the AI Core for synchronizing tiling selection.
};
}

template<typename T, int8_t mode, int8_t len>
__aicore__ void hello_world_impl(GM_ADDR m)
{
    if constexpr (std::is_same_v<T, float>) {
       AscendC::printf("Hello World: float mode %d len %d.\n", mode, len);
    } else if constexpr (std::is_same_v<T, int>) {
       AscendC::printf("Hello World: int mode %d len %d.\n", mode, len);
    }
}

// Main entry of the AI Core operator.
// tilingInfo: Parameter passed together with the AI CPU operator for data sharing.
template<typename T, int8_t mode, int8_t len>
__mix__(1,2) __global__ __aicore__ void hello_world(GM_ADDR m, GM_ADDR TilingPtr)
{
     __gm__ struct KernelInfo::TilingInfo *ti = (__gm__ struct KernelInfo::TilingInfo *)TilingPtr;
    AscendC::GlobalTensor<uint64_t> lock;
    lock.SetGlobalBuffer(reinterpret_cast<__gm__ uint64_t *>(&ti->lock));
    if ASCEND_IS_AIV {
        if (AscendC::GetBlockIdx() == 0) {
            while (*reinterpret_cast<volatile __gm__ uint64_t*>(lock.GetPhyAddr(0)) == 0) {   // In sink mode, the AI Core waits for the AI CPU to finish tiling computation.
                AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE,
                    AscendC::DcciDst::CACHELINE_OUT>(lock);    // Directly access Global Memory to obtain the latest data.
            }
        }
    }
    // The above shows one core waiting for the AI CPU to finish tiling computation. Inter-core synchronization is performed here.
    AscendC::SyncAll<false>();
    // Select different templates based on the tiling parameter value.
    if (ti->type ==0 && ti->mode == 1 && ti->len == 2) {
        hello_world_impl<float, 1, 2>(m);
    } else if (ti->type == 1 && ti->mode == 2 && ti->len == 4) {
        hello_world_impl<int, 2, 4>(m);
    }
    // After execution, leave one core to release the lock.
    if ASCEND_IS_AIV {
        if (AscendC::GetBlockIdx() == 0) {
            lock.SetValue(0, 0);  // Refresh the lock.
            AscendC::DataCacheCleanAndInvalid<uint64_t, AscendC::CacheLine::SINGLE_CACHE_LINE,
                AscendC::DcciDst::CACHELINE_OUT>(lock);    // Refresh the Dcache and synchronize data with the GM.
        }
    }
}

extern "C" __global__ __aicpu__ uint32_t MyAicpuKernel(void *arg)
{
    KernelInfo::KernelArgs* cfg = (KernelInfo::KernelArgs*)arg;
    AscendC::printf("MyAicpuKernel inited!\n");
    cfg->ti->lock = 1;
    cfg->ti->type = 1;
    cfg->ti->mode = 2;
    cfg->ti->len = 4;
    AscendC::DataStoreBarrier(); // Synchronize writes to tilingInfo.
    AscendC::printf("MyAicpuKernel inited type %d mode %d len %d end!\n", cfg->ti->type, cfg->ti->mode, cfg->ti->len);
    return 0;
}
```
