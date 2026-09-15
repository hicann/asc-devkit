# WriteGmByPassDCache\(ISASI\)<a name="ZH-CN_TOPIC_0000002327151478"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T11:17:54.727Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id8 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id8 -->
<!-- npu="A3" id9 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id9 -->
<!-- npu="910b" id10 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310b" id11 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products AI Core: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products Vector Core: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Writes data to a GM address without going through the DCache. Use cases:

- When data written by multiple cores falls within the same Cache Line, reads and writes through the DCache are performed at a 64B granularity, which may cause random data overwriting across cores (see [Example 3 of DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md#example3_multi_core)). Using this API to write to GM directly at the operand size without going through the DCache avoids this issue.
- Writing to GM through the DCache may cause data inconsistency across cores (for details, see [Cache Write Policy and Cache Coherency Issues](../cache_control/system_cache_overview.md#zh-cn_topic_0000002583420201_section053731716357)). Using this API to write data directly to GM without going through the DCache avoids this issue.

Because **WriteGmByPassDCache** writes data to GM without going through the DCache, developers need to consider the following scenarios:
- When the address **addr** in GM is already cached in the DCache and its corresponding Cache Line is marked as "dirty" (indicating that the data has been modified but not yet written back to GM), developers should call [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) before **WriteGmByPassDCache** to immediately write the Cache Line corresponding to **addr** back to GM. Otherwise, the data written to **addr** by **WriteGmByPassDCache** may later be overwritten by the dirty data written back from the DCache.
- The hardware cannot guarantee the execution order of **DataCacheCleanAndInvalid** and **WriteGmByPassDCache** when they write data to GM. Therefore, developers should call [DataSyncBarrier](../sync_control/intra_core_sync/DataSyncBarrier_ISASI.md) before **WriteGmByPassDCache** to synchronize the two APIs. Otherwise, the data written to **addr** by **WriteGmByPassDCache** may still be overwritten by the dirty data written back from the DCache.

Based on the description above, to simplify programming (so that developers do not need to care whether **addr** is cached in the DCache or marked as "dirty"), it is recommended that developers use the following code snippet when calling **WriteGmByPassDCache**:

```cpp
AscendC::GlobalTensor<T> global; // global is the GlobalTensor corresponding to addr.
AscendC::DataCacheCleanAndInvalid<T, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
// Ensure that the dirty data in the DCache has been written back to the GM before the WriteGmByPassDCache API writes value to addr.
AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
AscendC::WriteGmByPassDCache<T>(addr, value);
```

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void WriteGmByPassDCache(__gm__ T* addr, T value)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Data type of the operand. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| addr | Input | Target GM address. |
| value | Input | Target data to be written. |

## Data Types

The supported data types are int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section6191129670"></a>

```cpp
if (blockIdx == 0) {
    // Write the dependent data first, and then wait for the DDR access to complete through DataSyncBarrier.
    AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm) + 1, DATA_VALUE);
    // DataSyncBarrier<DDR> blocks subsequent GM writes to ensure that the previous GM write is visible to other cores.
    AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
    // Write the synchronization flag last. After block 1 reads this flag, it can safely read srcGm[1].
    AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm), SYNC_FLAG);
}

if (blockIdx == 1) {
    while (true) {
        __gm__ T *addr = const_cast<__gm__ T *>(srcGlobal.GetPhyAddr());
        // Poll element 0 of GM and wait for block 0 to write the synchronization flag.
        T flagValue = AscendC::ReadGmByPassDCache<T>(addr);
        if (flagValue == SYNC_FLAG) {
            // DataSyncBarrier ensures that the write to srcGm[1] before the synchronization flag has completed.
            T dataValue = AscendC::ReadGmByPassDCache<T>(addr + 1);
            AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(dstGm), 2 * dataValue);
            return;
        }
    }
}
```

For the complete sample, see [DataSyncBarrier sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier).
