# InitDetermineComputeWorkspace<a name="ZH-CN_TOPIC_0000001723825540"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-08-27T13:37:49.509Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_determine_compute_sync_intf.h"`.

**InitDetermineComputeWorkspace** is an initialization configuration API for deterministic computation based on sequential execution between cores. It initializes the value of the GM shared memory. Only after the initialization is complete can [WaitPreBlock](WaitPreBlock.md) and [NotifyNextBlock](NotifyNextBlock.md) be called. The three APIs above jointly implement deterministic computation based on sequential execution between cores. For details about deterministic computation, see [Deterministic Computation](../../atomic_operations/key_feature_description.md/).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void InitDetermineComputeWorkspace(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| gmWorkspace | Input | Temporary space that initializes the shared memory for inter-core synchronization, of the **GlobalTensor** type. |
| ubWorkspace | Input | Temporary space used to operate on **gmWorkspace**, of the **LocalTensor** type. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The minimum space to be allocated for **gmWorkspace** is **GetBlockNum()** * 32 bytes, and the minimum space to be allocated for **ubWorkspace** is **GetBlockNum()** * 32 + 32 bytes.
- Unlike **IBSet**/**SyncAll**, the value of the **gmWorkspace** buffer passed to this API does not need to be initialized to 0.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the actual number of AI processor cores running the operator. Otherwise, the framework inserts abnormal synchronization during multi-round scheduling, causing the **Kernel** to hang.

## Example<a name="section177231425115410"></a>

```cpp
    // Initialize the synchronization state of the GM shared memory. This must be called first at the beginning of the kernel function.
    AscendC::InitDetermineComputeWorkspace(gmWorkspace, ubWorkspace);

    for(int64_t i = 0; i < tileNum; i++) {
        AscendC::LocalTensor<T> srcLocal = que.AllocTensor<T>();
        AscendC::DataCopy(srcLocal, srcGlobal[i * tileCount], tileCount);

        // Wait for the preceding core (blockIdx-1) to complete its operation.
        AscendC::WaitPreBlock(gmWorkspace, ubWorkspace);
        
        // Enable atomic accumulation.
        AscendC::SetAtomicAdd<T>();
        AscendC::DataCopy(dstGlobal[i * tileCount], srcLocal, tileCount);
        AscendC::DisableDmaAtomic();
        
        // Notify the subsequent core (blockIdx+1) that the current core has completed.
        AscendC::NotifyNextBlock(gmWorkspace, ubWorkspace);
        que.FreeTensor(srcLocal);
    }
```

For the complete sample, see [sequential\_block\_sync example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/sequential_block_sync).
