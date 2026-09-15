# ReadGmByPassDCache\(ISASI\)<a name="ZH-CN_TOPIC_0000002327311338"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-27T11:17:13.560Z -->

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
- Atlas 200I/500 A2 inference product: Not supported
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

Reads data from the GM address without going through the DCache.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline T ReadGmByPassDCache(__gm__ T* addr)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|--------|------|
| **T** | Data type of the operand. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
|--------|-----------|------|
| **addr** | Input | Source GM address. |

## Data Types

The supported data types are int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, and uint64_t.

## Return Value<a name="section640mcpsimp"></a>

Returns the data at the source GM address.

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
