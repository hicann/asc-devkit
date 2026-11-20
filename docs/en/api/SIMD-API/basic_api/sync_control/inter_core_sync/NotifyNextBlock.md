# NotifyNextBlock<a name="ZH-CN_TOPIC_0000001723989788"></a>

<!-- md-trans-meta sourceCommit=3bfc0e6742a9966e438f7e009014de3dc9906d53 translatedAt=2026-11-05T05:10:08.486Z pushedAt=2026-11-18T01:25:31.148Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/NotifyNextBlock_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_determine_compute_sync_intf.h"`.

Notifies the subsequent AIV core that the current core has completed the current round of operations. The API writes the completion status of the current core to global memory (`gmWorkspace`), allowing the AIV core corresponding to the next `blockIdx` to end its wait and continue execution. When the current core is the last core, the API resets the synchronization status so that all cores can proceed to the next round of sequential execution.

This API must be used together with [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md) and [WaitPreBlock](WaitPreBlock.md): call InitDetermineComputeWorkspace at the beginning of the kernel to initialize the synchronization status, call WaitPreBlock before the operations that require sequential execution, and call this API after the operations complete to notify the subsequent core. The combination of the three APIs ensures that multiple AIV cores execute strictly in ascending order of `blockIdx`, which is suitable for scenarios that require [deterministic computation](../../atomic_operations/key_features.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void NotifyNextBlock(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  API parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| gmWorkspace | Input | Global memory shared by multiple AIVs, used to store the synchronization status for sequential inter-core execution. It resides in Global Memory and is of type [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md). This API notifies the subsequent AIV core to continue execution by writing to this memory. |
| ubWorkspace | Input | In-core buffer of the current AIV, used to generate and temporarily store in the Unified Buffer the inter-core synchronization status to be written to `gmWorkspace`. It is of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- To implement deterministic computation, ensure that each core calls the **NotifyNextBlock**/**WaitPreBlock** APIs the same number of times. If **NotifyNextBlock** is called without a matching **WaitPreBlock**, nondeterministic computation may occur and the output result may not meet expectations. If **WaitPreBlock** is called without a matching **NotifyNextBlock**, the program may hang.
- Before using the API, ensure that the [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md) API has been called to initialize the shared memory (`gmWorkspace`).
- When [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md), [WaitPreBlock](WaitPreBlock.md), and [NotifyNextBlock](NotifyNextBlock.md) are used together, the `gmWorkspace` of the three APIs must point to the same Global Memory, and the `ubWorkspace` must point to the same Unified Buffer on the current AIV. Typically, the same set of variables should be passed to the three APIs. Otherwise, WaitPreBlock may fail to read from `gmWorkspace` the inter-core synchronization status initialized by InitDetermineComputeWorkspace or written by NotifyNextBlock, and NotifyNextBlock may fail to read from `ubWorkspace` the synchronization round status initialized by InitDetermineComputeWorkspace or updated by the previous NotifyNextBlock, resulting in incorrect execution order or program hang.

- The space allocated for `gmWorkspace` and `ubWorkspace` must satisfy the following formula:

$$
\begin{aligned}
\text{gmWorkspace space size} &\geq \mathrm{GetBlockNum}() \times 32\text{ bytes} \\
\text{ubWorkspace space size} &\geq (\mathrm{GetBlockNum}() + 1) \times 32\text{ bytes}
\end{aligned}
$$

- In [separate mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), WaitPreBlock and NotifyNextBlock take effect only on AIV cores. When either API is called on an AIC core, the API returns immediately without performing waiting, notification, or inter-core synchronization.
- Only vector computation instructions can be inserted between WaitPreBlock and NotifyNextBlock. Matrix computation instructions do not take effect.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the actual number of AI processor cores running the operator. Otherwise, abnormal synchronization is inserted during multi-round scheduling by the framework, causing the kernel function (**Kernel**) to hang.

## Example<a name="section177231425115410"></a>

For the complete sample, see [InitDetermineComputeWorkspace Examples](InitDetermineComputeWorkspace.md#example).
