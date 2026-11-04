# WriteGmBypassDCache\(ISASI\)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:25:37.849Z pushedAt=2026-10-26T07:41:44.771Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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

## Description

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Writes data to a GM address without going through the DCache.
<!-- npu="A3,910b" id8 -->
> [!CAUTION]Note
> For [NPU architecture 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), whether the API can successfully write **value** to GM also depends on the target address **addr**. For details, see [Constraints](#constraints).
<!-- end id8 -->

Use cases:

- When data written by multiple cores falls within the same Cache Line, reads and writes through the DCache are performed at a 64B granularity, which may cause random data overwriting across cores (see [DataCacheCleanAndInvalid Example 3](../cache_control/DataCacheCleanAndInvalid.md#example3_multi_core)). Using this API to write to GM directly by operand size without going through the DCache avoids this problem.

    ![Schematic diagram of the multi-core random data overwriting problem](../../../figures/write_gm_bypass_dcache.png)

- Writing to GM through the DCache may cause data inconsistency across cores (for details, see [Cache Write Policy and Cache Coherency Issues](../cache_control/system_cache_overview.md#zh-cn_topic_0000002583420201_section053731716357)). Using this API to write data directly to GM without going through the DCache avoids this problem.

Because the **WriteGmBypassDCache** API writes data to GM without going through the DCache, developers need to consider the following scenarios:
- When the address **addr** on GM is already cached in the DCache and its corresponding Cache Line is marked as "dirty" (indicating that the data has been modified but not yet written back to GM), developers should call [DataCacheCleanAndInvalid](../cache_control/DataCacheCleanAndInvalid.md) before the **WriteGmBypassDCache** API to immediately write the Cache Line corresponding to **addr** back to GM. Otherwise, the data written to **addr** by the **WriteGmBypassDCache** API may later be overwritten by the dirty data written back from the DCache.
- The hardware cannot guarantee the execution order of the **DataCacheCleanAndInvalid** API and the **WriteGmBypassDCache** API when writing data to GM. Therefore, developers should call the [DataSyncBarrier](../sync_control/intra_core_sync/DataSyncBarrier_ISASI.md) API before the **WriteGmBypassDCache** API to synchronize these two APIs. Otherwise, the data written to addr by the **WriteGmBypassDCache** API may still be overwritten by the dirty data written back from the DCache.

Based on the description above, to simplify programming (so that developers do not need to care whether addr is cached in the DCache or marked as "dirty"), it is recommended that developers use the following code snippet when using the **WriteGmBypassDCache** API:

```cpp
AscendC::GlobalTensor<T> global; // global is the GlobalTensor corresponding to addr.
AscendC::DataCacheCleanAndInvalid<T, AscendC::CacheLine::SINGLE_CACHE_LINE, AscendC::DcciDst::CACHELINE_OUT>(global);
// Ensure that the dirty data in the DCache has been written back to GM before the WriteGmBypassDCache API writes value to addr.
AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
AscendC::WriteGmBypassDCache<T>(addr, value);
```

## Prototype

```cpp
template <typename T>
__aicore__ inline void WriteGmBypassDCache(__gm__ T* addr, T value)
```

## Parameters

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

## Return Value

None

## Constraints

<!-- npu="A3,910b" id9 -->
For [NPU architecture 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), whether the API performs the write depends on the offset of the target address **addr** within the current 128-byte aligned interval. Let $\mathrm{offset} = addr \bmod 128$. When $0 \leq \mathrm{offset} < 32$, the write takes effect; when $32 \leq \mathrm{offset} < 128$, the write is not performed and the data at the target address remains unchanged. That is, the API performs the write only when the target address is located in the first 32 bytes of each 128-byte aligned interval. When the AscendC debug macro [-DASCENDC_DEBUG](../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches) is enabled, an assert check takes effect, and if the check fails, an assert log is printed to the screen.

Let the data at the target address before and after the API call be $\mathrm{GM}_{\mathrm{before}}(addr)$ and $\mathrm{GM}_{\mathrm{after}}(addr)$, respectively. Then:

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
    // Write the dependent data first, and then wait for the DDR access to complete through DataSyncBarrier.
    AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm) + 1, DATA_VALUE);
    // DataSyncBarrier<DDR> blocks subsequent GM writes to ensure that the previous GM write is visible to other cores.
    AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
    // Write the synchronization flag last. After block 1 reads this flag, it can safely read srcGm[1].
    AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm), SYNC_FLAG);
}

if (blockIdx == 1) {
    while (true) {
        __gm__ T *addr = const_cast<__gm__ T *>(srcGlobal.GetPhyAddr());
        // Poll element 0 of GM and wait for block 0 to write the synchronization flag.
        T flagValue = AscendC::ReadGmBypassDCache<T>(addr);
        if (flagValue == SYNC_FLAG) {
            // DataSyncBarrier ensures that the write to srcGm[1] before the synchronization flag has completed.
            T dataValue = AscendC::ReadGmBypassDCache<T>(addr + 1);
            AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(dstGm), 2 * dataValue);
            return;
        }
    }
}
```

For the complete sample, see [DataSyncBarrier sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier).
