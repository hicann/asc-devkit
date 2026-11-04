# DataSyncBarrier\(ISASI\)<a name="ZH-CN_TOPIC_0000001787873298"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:58:37.047Z pushedAt=2026-10-26T07:41:44.819Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

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
- Atlas 200I/500 A2 inference products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/DataSyncBarrier_ISASI_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

Blocks the execution of subsequent instructions until all previous memory access instructions (the memory locations to wait for can be controlled through parameters) have completed, which is used to resolve Scalar memory dependencies.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <MemDsbT arg0>
__aicore__ inline void DataSyncBarrier()
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| arg0 | Template parameter, indicating the memory location to wait for. The type is **MemDsbT**, and the value can be:<br>&bull; **ALL**: Waits for the access instructions of GM and UB (excluding other memory locations, such as L0C Buffer and L1 Buffer).<br>&bull; **DDR**: Waits for the GM access instruction.<br>&bull; **UB**: Waits for the UB access instruction.<br>&bull; **SEQ**: Waits for the SEQ access instruction. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

When the memory location to wait for is set to **ALL**, only the access instructions of GM and Unified Buffer (UB) are waited for, excluding other memory locations such as L0C Buffer and L1 Buffer.

## Example<a name="section837496171220"></a>

```cpp
    if (blockIdx == 0) {
        // Write the dependent data first, and then wait for the GM access to complete through DataSyncBarrier.
        AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm) + 1, DATA_VALUE);
        // DataSyncBarrier<DDR> blocks subsequent GM writes to ensure that the previous GM write is visible to other cores.
        AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();
        // Write the sync flag last. After block 1 reads this flag, it can safely read srcGm[1].
        AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(srcGm), SYNC_FLAG);
    }

    if (blockIdx == 1) {
        while (true) {
            __gm__ T *addr = const_cast<__gm__ T *>(srcGlobal.GetPhyAddr());
            // Poll element 0 of GM and wait for block 0 to write the sync flag.
            T flagValue = AscendC::ReadGmBypassDCache<T>(addr);
            if (flagValue == SYNC_FLAG) {
                // DataSyncBarrier ensures that the write to srcGm[1] before the sync flag has completed.
                T dataValue = AscendC::ReadGmBypassDCache<T>(addr + 1);
                AscendC::WriteGmBypassDCache<T>(reinterpret_cast<__gm__ T *>(dstGm), 2 * dataValue);
                return;
            }
        }
    }
```

For the complete sample, see [DataSyncBarrier API sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/data_sync_barrier).
