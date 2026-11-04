# ReadGmBypassDCache\(ISASI\)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:23:51.383Z pushedAt=2026-10-26T07:41:44.767Z nl=2 -->

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

Reads data from the GM address without going through the DCache.

## Function Prototype

```cpp
template <typename T>
__aicore__ inline T ReadGmBypassDCache(__gm__ T* addr)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| T | Data type of the operand. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| addr | Input | Source GM address. |

## Data Types

The supported data types are **int8_t**, **uint8_t**, **int16_t**, **uint16_t**, **int32_t**, **uint32_t**, **int64_t**, and **uint64_t**.

## Return Value

Data at the source GM address.

## Constraints

None

## Example

```cpp
if (blockIdx == 0) {
    // Write the dependent data first, and then wait for DDR access to complete through DataSyncBarrier.
    AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm) + 1, DATA_VALUE);
    // DataSyncBarrier<DDR> blocks subsequent GM writes to ensure that the previous GM write is visible to other cores.
    AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
    // Finally write the synchronization flag so that block 1 can safely read srcGm[1] after reading the flag.
    AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm), SYNC_FLAG);
}

if (blockIdx == 1) {
    while (true) {
        __gm__ T *addr = const_cast<__gm__ T *>(srcGlobal.GetPhyAddr());
        // Poll the 0th element of GM and wait for block 0 to write the synchronization flag.
        T flagValue = AscendC::ReadGmBypassDCache<T>(addr);
        if (flagValue == SYNC_FLAG) {
            // DataSyncBarrier ensures that the write to srcGm[1] before the synchronization flag is complete.
            T dataValue = AscendC::ReadGmBypassDCache<T>(addr + 1);
            AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(dstGm), 2 * dataValue);
            return;
        }
    }
}
```

For the complete sample, see [DataSyncBarrier sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier).
