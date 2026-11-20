# ReadGmByPassDCache\(ISASI\) (Deprecated)

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T06:06:15.283Z pushedAt=2026-11-18T01:25:06.976Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/scalar_compute/ReadGmByPassDCache_ISASI_res.md#id1 -->

## Description

**This API is deprecated. Use the [ReadGmBypassDCache\(ISASI\)](../scalar_compute/ReadGmBypassDCache_ISASI.md) API instead.**

The header file path is `"basic_api/kernel_operator_scalar_intf.h"`.

Reads data from a GM address without going through the DCache.

## Function Prototype

```cpp
template <typename T>
__aicore__ inline T ReadGmByPassDCache(__gm__ T* addr)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Data type of the operand. |

**Table 2** API parameter description

| Parameter | Input/Output | Meaning |
|--------|-----------|------|
| addr | Input | Source GM address. |

## Data Types

The supported data types are int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t.

## Return Value

Data at the source GM address.

## Constraints

None

## Example

```cpp
if (blockIdx == 0) {
    // Write the dependent data first, and then wait for the DDR access to complete through DataSyncBarrier.
    AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm) + 1, DATA_VALUE);
    // DataSyncBarrier<DDR> blocks subsequent GM writes to ensure that the previous GM write is visible to other cores.
    AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
    // Finally, write the synchronization flag. After block 1 reads this flag, it can safely read srcGm[1].
    AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm), SYNC_FLAG);
}

if (blockIdx == 1) {
    while (true) {
        __gm__ T *addr = const_cast<__gm__ T *>(srcGlobal.GetPhyAddr());
        // Poll element 0 of GM and wait for block 0 to write the synchronization flag.
        T flagValue = AscendC::ReadGmByPassDCache<T>(addr);
        if (flagValue == SYNC_FLAG) {
            // DataSyncBarrier ensures that the write to srcGm[1] before the synchronization flag is complete.
            T dataValue = AscendC::ReadGmByPassDCache<T>(addr + 1);
            AscendC::WriteGmByPassDCache<T>(reinterpret_cast<__gm__ T *>(dstGm), 2 * dataValue);
            return;
        }
    }
}
```

For the complete sample, see [DataSyncBarrier sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier).
