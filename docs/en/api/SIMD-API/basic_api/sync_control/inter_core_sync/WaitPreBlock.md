# WaitPreBlock<a name="ZH-CN_TOPIC_0000001771590261"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:26:28.784Z -->

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

**WaitPreBlock** and **NotifyNextBlock** are inter-core synchronization control APIs. **NotifyNextBlock** writes to global memory to notify other cores that the current core has finished execution and that they can proceed; **WaitPreBlock** reads global memory to determine whether the current core can continue execution.

**WaitPreBlock** and **NotifyNextBlock** must be used together with the [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md#) API. The combination of these three APIs ensures that multiple AIV cores execute strictly in ascending order of **blockIdx**, which is suitable for scenarios requiring deterministic computation. For the specific meaning of deterministic computation, see [Atomic Operations - Deterministic Computation](../../atomic_operations/key_feature_description.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void WaitPreBlock(GlobalTensor<int32_t>& gmWorkspace, LocalTensor<int32_t>& ubWorkspace)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspace | Input | Temporary workspace. Determines whether the current core can continue execution by reading **gmWorkspace**. The type is **GlobalTensor**. |
| ubWorkspace | Input | Temporary workspace used to operate on **gmWorkspace**. The type is **LocalTensor**. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- To achieve deterministic computation, ensure that each core calls the **NotifyNextBlock**/**WaitPreBlock** APIs the same number of times. If **NotifyNextBlock** is called without a matching **WaitPreBlock**, it leads to non-deterministic computation and the output results may not meet expectations; if **WaitPreBlock** is called without a matching **NotifyNextBlock**, it causes the program to hang.
- Before using the API, ensure that the [InitDetermineComputeWorkspace](InitDetermineComputeWorkspace.md) API has been called to initialize the shared memory.
- The minimum space required for **gmWorkspace** is: GetBlockNum()*32Bytes, and the minimum space required for **ubWorkspace** is: GetBlockNum()*32+32Bytes.
- When using this API for multi-core synchronization, it takes effect only on AIV cores. Between **WaitPreBlock** and **NotifyNextBlock**, only vector computation-related instructions are supported, and it does not take effect on cube computation-related instructions.
- When using this API for multi-core control, the number of logical AI Core cores **numBlocks** specified when the operator is invoked must not be greater than the actual number of AI processor cores running the operator. Otherwise, the framework inserts abnormal synchronization during multi-round scheduling, causing the Kernel to "hang".

## Example<a name="section177231425115410"></a>

For the complete sample, see [Example](InitDetermineComputeWorkspace.md#section177231425115410).
