# InitDetermineComputeWorkspace<a name="ZH-CN_TOPIC_0000001723825540"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:41:00.130Z pushedAt=2026-10-26T07:41:44.794Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/InitDetermineComputeWorkspace_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_determine_compute_sync_intf.h"`.

**InitDetermineComputeWorkspace** is an initialization configuration API for deterministic computation based on inter-core sequential execution. It initializes the value of the GM shared memory. Only after the initialization is complete can [WaitPreBlock](WaitPreBlock.md) and [NotifyNextBlock](NotifyNextBlock.md) be called. The three APIs above jointly implement deterministic computation based on inter-core sequential execution. For the specific meaning of deterministic computation, see [Deterministic Computation](../../atomic_operations/key_features.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void InitDetermineComputeWorkspace(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| **gmWorkspace** | Input | Temporary space that initializes the shared memory for inter-core synchronization, of the **GlobalTensor** type. |
| **ubWorkspace** | Input | Temporary space used to operate on **gmWorkspace**, of the **LocalTensor** type. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The space requested for **gmWorkspace** must be at least **GetBlockNum()**\*32Bytes, and the space requested for **ubWorkspace** must be at least **GetBlockNum()**\*32+32Bytes.
- Unlike IBSet/SyncAll, the value of the **gmWorkspace** buffer passed to this API does not need to be initialized to 0.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the number of AI processor cores that actually run the operator. Otherwise, the framework inserts abnormal synchronization during multi-round scheduling, causing the kernel function (**Kernel**) to hang.

## Example<a name="section177231425115410"></a>

```cpp
    // Initialize the synchronization state of the GM shared memory. This must be called first at the beginning of the kernel function.
    AscendC::InitDetermineComputeWorkspace(gmWorkspace, ubWorkspace);

    for(int64_t i = 0; i < tileNum; i++) {
        AscendC::LocalTensor<T> srcLocal = que.AllocTensor<T>();
        AscendC::DataCopy(srcLocal, srcGlobal[i * tileCount], tileCount);

        // Wait for the preceding block (blockIdx-1) to complete its operation.
        AscendC::WaitPreBlock(gmWorkspace, ubWorkspace);
        
        // Enable atomic accumulation.
        AscendC::SetAtomicAdd<T>();
        AscendC::DataCopy(dstGlobal[i * tileCount], srcLocal, tileCount);
        AscendC::DisableDmaAtomic();
        
        // Notify the subsequent block (blockIdx+1) that the current block has completed.
        AscendC::NotifyNextBlock(gmWorkspace, ubWorkspace);
        que.FreeTensor(srcLocal);
    }
```

For the complete sample, see [sequential\_block\_sync example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/sequential_block_sync).
