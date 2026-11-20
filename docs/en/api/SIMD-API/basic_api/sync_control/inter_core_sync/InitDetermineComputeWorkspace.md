# InitDetermineComputeWorkspace<a name="ZH-CN_TOPIC_0000001723825540"></a>

<!-- md-trans-meta sourceCommit=3bfc0e6742a9966e438f7e009014de3dc9906d53 translatedAt=2026-11-05T05:04:35.960Z pushedAt=2026-11-18T01:25:31.143Z nl=2 -->

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

InitDetermineComputeWorkspace is an initialization configuration API for deterministic computation based on inter-core sequential execution. It initializes the value of the shared memory (`gmWorkspace`). Only after the initialization is complete can [WaitPreBlock](WaitPreBlock.md) and [NotifyNextBlock](NotifyNextBlock.md) be called. The three APIs above jointly implement deterministic computation based on inter-core sequential execution. For the specific meaning of deterministic computation, see [Deterministic Computation](../../atomic_operations/key_features.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void InitDetermineComputeWorkspace(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

| Parameter | Input/Output | Meaning |
| :--- | :--- | :--- |
| gmWorkspace | Input | Global memory shared by multiple AI Vector cores, used to store the synchronization state of inter-core sequential execution. It resides in Global Memory and is of type [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md). This API initializes the synchronization state in it to 0. |
| ubWorkspace | Input | On-core cache of the current AI Vector core, used to temporarily store the inter-core synchronization state to be written to `gmWorkspace` in the Unified Buffer and to save the synchronization round state used by subsequent APIs. It is of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- You do not need to initialize `gmWorkspace` and `ubWorkspace` in advance. This API initializes the inter-core synchronization state in `gmWorkspace` to 0 and the synchronization round state in `ubWorkspace` to 1.
- When [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md), [WaitPreBlock](WaitPreBlock.md), and [NotifyNextBlock](NotifyNextBlock.md) are used together, the `gmWorkspace` of the three APIs must point to the same Global Memory, and the `ubWorkspace` must point to the same Unified Buffer on the current AI Vector core. Typically, the same set of variables should be passed to the three APIs. Otherwise, WaitPreBlock may fail to read from `gmWorkspace` the inter-core synchronization state initialized by InitDetermineComputeWorkspace or written by NotifyNextBlock, and NotifyNextBlock may fail to read from `ubWorkspace` the synchronization round state initialized by InitDetermineComputeWorkspace or updated by the previous NotifyNextBlock, resulting in incorrect execution order or program hang.
- The space allocated for `gmWorkspace` and `ubWorkspace` must satisfy the following formula:

$$
\begin{aligned}
\text{gmWorkspace space size} &\geq \mathrm{GetBlockNum}() \times 32\text{ bytes} \\
\text{ubWorkspace space size} &\geq (\mathrm{GetBlockNum}() + 1) \times 32\text{ bytes}
\end{aligned}
$$

- In [separate mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), this API takes effect only on AI Vector cores. When called on an AI Cube core, the API returns directly without initializing `gmWorkspace` and `ubWorkspace`.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the actual number of AI processor cores running the operator. Otherwise, abnormal synchronization is inserted during multi-round scheduling by the framework, causing the kernel function (**Kernel**) to hang.

## Example<a name="section177231425115410"></a>

```cpp
    // Initialize the synchronization state of the shared memory (gmWorkspace). This must be called first at the beginning of the kernel function (Kernel).
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

For the complete sample, see [sequential\_block\_sync example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/sequential_block_sync).
