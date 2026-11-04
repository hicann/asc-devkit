# WriteGmByPassDCache\(ISASI\) (Deprecated)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:21:53.917Z pushedAt=2026-10-26T07:41:44.403Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/WriteGmByPassDCache_ISASI_res.md#id1 -->

## Description

**This API is deprecated. Use [WriteGmBypassDCache\(ISASI\)](../scalar_compute/WriteGmBypassDCache_ISASI.md) instead.**

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Writes data to a GM address without going through the DCache.
<!-- npu="A3,910b" id8 -->
> [!CAUTION]Note
> For [NPU architecture 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), whether the API can successfully write **value** to GM also depends on the target address **addr**. For details, see [Constraints](#constraints).
<!-- end id8 -->

Usage scenarios:

- When data written by multiple cores falls within the same Cache Line, reads and writes through the DCache are performed at a 64B granularity, which may cause random data overwriting across cores (see [DataCacheCleanAndInvalid Example 3](../cache_control/DataCacheCleanAndInvalid.md#example3_multi_core)). Using this API to write to GM directly at the operand size without going through the DCache avoids this issue.
- Writing to GM through the DCache may cause data inconsistency across cores (for details, see [Cache Write Policy and Cache Coherency Issues](../cache_control/system_cache_overview.md#zh-cn_topic_0000002583420201_section053731716357)). Using this API to write data directly to GM without going through the DCache avoids this issue.

Because the WriteGmByPassDCache API writes data to GM without going through the DCache, developers need to consider the following scenarios:
- When the address **addr** in GM is already cached in the DCache and its corresponding Cache Line is marked as "dirty" (indicating that the data has been modified but not yet written back to GM), developers should call [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) before the WriteGmByPassDCache API to immediately write the Cache Line corresponding to **addr** back to GM. Otherwise, the data written to **addr** by the WriteGmByPassDCache API may later be overwritten by the dirty data written back from the DCache.
- The hardware cannot guarantee the execution order of the **DataCacheCleanAndInvalid** API and the **WriteGmByPassDCache** API when they write data to GM. Therefore, developers should call the [DataSyncBarrier](../sync_control/intra_core_sync/DataSyncBarrier_ISASI.md) API before the **WriteGmByPassDCache** API to synchronize the two APIs. Otherwise, the data written to addr by the **WriteGmByPassDCache** API may still be overwritten by the dirty data written back from DCache.

Based on the description above, to simplify programming (so that developers do not need to care whether addr is cached in DCache or marked as "dirty"), it is recommended that developers use the following code snippet when using the **WriteGmByPassDCache** API:

```cpp
AscendC::GlobalTensor<T> global; // global is the GlobalTensor corresponding to addr.
AscendC::DataCacheCleanAndInvalid<T, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
// Ensure that the dirty data in DCache has been written back to GM before the WriteGmByPassDCache API writes value to addr.
AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
AscendC::WriteGmByPassDCache<T>(addr, value);
```

## Prototype

```cpp
template <typename T>
__aicore__ inline void WriteGmByPassDCache(__gm__ T* addr, T value)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
|--------|------|
| T | Data type of the operand. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| addr | Input | Target GM address. |
| value | Input | Data to be written. |

## Data Types

The supported data types are int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t.

## Return Value

None

## Constraints

<!-- npu="A3,910b" id9 -->
For [NPU architecture 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), whether the API performs the write depends on the offset of the target address **addr** within the current 128-byte aligned interval. Let $\mathrm{offset} = addr \bmod 128$, when $0 \leq \mathrm{offset} < 32$, the write takes effect; when $32 \leq \mathrm{offset} < 128$, the write is not performed, and the data at the target address remains unchanged. That is, the API performs the write only when the target address is located in the first 32 bytes of each 128-byte aligned interval.

Let the target address data before and after the API call be $\mathrm{GM}_{\mathrm{before}}(addr)$ and $\mathrm{GM}_{\mathrm{after}}(addr)$, respectively. Then:

  $$
  \mathrm{GM}_{\mathrm{after}}(addr) =
  \begin{cases}
  value, & 0 \leq \mathrm{offset} < 32 \\
  \mathrm{GM}_{\mathrm{before}}(addr), & 32 \leq \mathrm{offset} < 128
  \end{cases}
  $$
<!-- end id9 -->

## Example

```cpp
if (blockIdx == 0) {
    // Write the dependent data first, and then wait for DDR access to complete through DataSyncBarrier.
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

For the complete sample, see [DataSyncBarrier sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier).
