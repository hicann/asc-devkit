# NotifyNextBlock<a name="ZH-CN_TOPIC_0000001723989788"></a>

<!-- md-trans-meta sourceCommit=87fcc89e932521312a6dc152579c1037d8e11853 translatedAt=2026-08-27T13:40:34.928Z -->

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

**WaitPreBlock** and **NotifyNextBlock** are inter-core synchronization control APIs. **NotifyNextBlock** writes to global memory to notify other cores that the current core has completed execution and that they can proceed; **WaitPreBlock** reads global memory to determine whether the current core can proceed.

**WaitPreBlock** and **NotifyNextBlock** must be used together with [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md#). The combination of these three APIs ensures that multiple AIV cores execute strictly in ascending order of `blockIdx`, which is suitable for scenarios requiring deterministic computation. For the specific meaning of deterministic computation, see [Deterministic Computation](../../atomic_operations/key_feature_description.md/).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void NotifyNextBlock(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| gmWorkspace | Input | Temporary workspace. Writing to **gmWorkspace** notifies other cores that the current core has finished execution, so that other cores can continue. The type is **GlobalTensor**. |
| ubWorkspace | Input | Temporary workspace used to operate on **gmWorkspace**. The type is **LocalTensor**. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- To achieve deterministic computation, ensure that each core calls the **NotifyNextBlock**/**WaitPreBlock** API the same number of times. If **NotifyNextBlock** is called without a matching **WaitPreBlock**, it leads to non-deterministic computation and the output results may not meet expectations. If **WaitPreBlock** is called without a matching **NotifyNextBlock**, the program hangs.
- Before using the API, ensure that the [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md) API has been called to initialize the shared memory.
- The minimum space required for **gmWorkspace** is GetBlockNum()*32Bytes, and the minimum space required for **ubWorkspace** is GetBlockNum()*32+32Bytes.
- When this API is used for multi-core synchronization, it takes effect only on AIV cores. Only vector computation-related instructions are supported between **WaitPreBlock** and **NotifyNextBlock**; cube computation-related instructions do not take effect.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the actual number of AI processor cores running the operator. Otherwise, the framework inserts abnormal synchronization during multi-round scheduling, causing the Kernel to hang.

## Example<a name="section177231425115410"></a>

For the complete sample, see [Example](InitDetermineComputeWorkspace.md#section177231425115410).
