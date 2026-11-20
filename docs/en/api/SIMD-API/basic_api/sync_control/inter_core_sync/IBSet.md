# IBSet<a name="ZH-CN_TOPIC_0000001538096733"></a>

<!-- md-trans-meta sourceCommit=3bfc0e6742a9966e438f7e009014de3dc9906d53 translatedAt=2026-11-05T05:01:18.804Z pushedAt=2026-11-18T01:25:31.137Z nl=2 -->

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
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/IBSet_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

Sets an inter-core synchronization flag. When different cores operate on the same global memory and have data dependencies such as read-after-write, write-after-read, or write-after-write, the waited core (producer) calls this API after completing the relevant operations to set the synchronization flag to 1, notifying the waiting core (consumer) that it can continue execution. This prevents data read/write errors caused by the uncertain execution order between cores.

This API must be used in pairs with [IBWait](IBWait.md). The waiting core (consumer) calls IBWait before accessing the dependent data. When IBSet and IBWait are used for the same synchronization event, the related parameter configurations must be consistent.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <bool isAIVOnly = true>
__aicore__ inline void IBSet(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, int32_t blockIdx, int32_t eventID)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| isAIVOnly | Controls how the storage location of the synchronization flag in `gmWorkspace` is calculated. When set to true (default), the storage location of the synchronization flag is calculated in AIVOnly mode. When set to false, it is calculated in MIX mode. In MIX mode, it must be explicitly set to false. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspace | Input | Global memory shared by multiple AIVs, used to store the inter-core synchronization flag. It is located in Global Memory and is of type [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md). Before the first inter-core synchronization, the inter-core synchronization flag in it must be initialized to 0. |
| ubWorkspace | Input | On-core buffer of the current AIV, used to move the inter-core synchronization flag from `gmWorkspace` to the Unified Buffer for reading and modification, and then write the modified synchronization flag back to `gmWorkspace`.<br>It is of type [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported [TPosition](../../aux_data_structures/TPosition.md) values are VECIN/VECCALC/VECOUT. |
| blockIdx | Input | Logical AIV index corresponding to the synchronization flag, that is, the logical AIV index of the waited core (producer, the core that calls IBSet). The value range is determined by the number of logical AI Cores `numBlocks` and the AIV ratio set when the operator is called. For example, when the kernel function is modified by `__mix__(1, 2)` and `numBlocks` is 4, the value range of `blockIdx` is `[0, 7]`. |
| eventID | Input | Logical number of the synchronization event. Together with `blockIdx`, it determines the synchronization flag in `gmWorkspace` and is used to distinguish different software synchronization events between the same pair of producer and consumer.<br>This parameter has the same name as the input parameter of [SetFlag/WaitFlag](../intra_core_sync/SetFlag_WaitFlag_ISASI.md), but the meanings are different: the `eventID` of this API is used to locate the software synchronization flag, while the `eventID` of SetFlag/WaitFlag is used for the intra-core pipeline synchronization event in hardware instructions.<br>Therefore, the `eventID` of this API does not apply to the value range limit of 0 to 7 in SetFlag/WaitFlag. The value is a non-negative integer, and it is recommended to number it consecutively starting from 0. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- Before calling IBSet and IBWait for inter-core synchronization for the first time, you must initialize the inter-core synchronization flag in the shared `gmWorkspace` to 0:
    - If the initial value of the synchronization flag is 1, IBWait may mistakenly determine that the producer has completed the operation and end the wait early, resulting in an incorrect synchronization result.
    - If the initial value is another unexpected value, IBSet and IBWait may keep waiting, causing the program to hang.

- The space requested for `gmWorkspace` must satisfy the following formula:

    $$
    \text{gmWorkspace space size} \geq (\mathrm{eventStride} \times \mathrm{eventID\_max} + \mathrm{blockIdx\_max} + 1) \times 32\text{ bytes}
    $$

    - `eventID_max` and `blockIdx_max` represent the maximum possible values of `eventID` and `blockIdx`, respectively, across all IBSet and IBWait calls that share the same `gmWorkspace`. `blockIdx_max` is not necessarily equal to the upper bound of the valid range of `blockIdx`. For example, when a kernel function is modified by `__mix__(1, 2)` and `numBlocks` is 4, the valid range of `blockIdx` is `[0, 7]`; if all calls sharing the same `gmWorkspace` use only `blockIdx = 0` and `blockIdx = 1`, then `blockIdx_max = 1`.
    - The value of `eventStride` depends on the template parameter isAIVOnly:
        - When `isAIVOnly` is true, `eventStride = GetBlockNum()`;
        - When `isAIVOnly` is false, `eventStride = GetBlockNum() × 2`, regardless of the AIC:AIV ratio.

    The following uses the multi-event synchronization scenario in AIVOnly mode as an example to illustrate how the preceding parameter values and the `gmWorkspace` space size are calculated.

    In AIVOnly mode, `GetBlockNum() = 4`. Core 0, as the producer, notifies two different synchronization events through `IBSet(sync_gm, sync_buf, 0, 0)` and `IBSet(sync_gm, sync_buf, 0, 1)`, respectively. Core 2, as the producer, notifies another synchronization event through `IBSet(sync_gm, sync_buf, 2, 0)`. The corresponding IBWait must pass the same `blockIdx` and `eventID`. For example, `IBWait(sync_gm, sync_buf, 0, 1)` can only wait for the synchronization flag set by `IBSet(sync_gm, sync_buf, 0, 1)`; if `blockIdx = 1` or `eventID = 0` is passed by mistake, another synchronization flag is accessed, which may cause a hang. In this scenario, `blockIdx_max = 2`, `eventID_max = 1`, and `eventStride = 4`, so `gmWorkspace` requires at least `(4 × 1 + 2 + 1) × 32 = 224` bytes.
- The minimum space required for **ubWorkspace** is: 32Bytes.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the actual number of AI processor cores running the operator. Otherwise, abnormal synchronization is inserted during multi-round scheduling by the framework, causing the kernel function (**Kernel**) to hang.
- When **IBSet** and **IBWait** are used as a pair, all parameters except **ubWorkspace** must be identical. Otherwise, the program hangs at **IBWait**.
- In [separate mode](../../../../../guide/programming_guide/advanced_programming/hardware_implementation/basic_architecture.md), this API takes effect only on AIV cores. When called on an AIC core, the API returns directly without setting the synchronization flag or performing inter-core synchronization.

## Example<a name="section177231425115410"></a>

This example uses two cores for data processing, with each core processing 256 half data elements. Core 0 performs the x+y operation and places the result in the first half of z. Core 1 places the result computed by core 0 into x, then adds it to y and stores the result in the second half of z. Therefore, data synchronization is required between the multiple cores.

```cpp
// sync_gm is the public cache for storing the core state in external storage, of the GlobalTensor type; sync_buf is the public cache for storing the current core state, of the LocalTensor type.
int32_t blockIdx = AscendC::GetBlockIdx(); // Obtain the current core.
if (blockIdx == 1) { // Set IBWait on core 1 to block instruction execution until the operation on core 0 completes.
    AscendC::IBWait(sync_gm, sync_buf, 0, 0);
}
...
if (blockIdx == 0) { // Set IBSet on core 0 so that core 1's instructions execute only after core 0's operations complete.
    AscendC::IBSet(sync_gm, sync_buf, 0, 0);
}
```

For the complete sample, see [IBSet and IBWait inter-core synchronization sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/ib_set_wait).
