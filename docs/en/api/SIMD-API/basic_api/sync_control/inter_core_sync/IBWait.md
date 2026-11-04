# IBWait<a name="ZH-CN_TOPIC_0000001487056954"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:39:57.455Z pushedAt=2026-10-26T07:41:44.793Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/IBWait_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

When different cores operate on the same block of global memory and data dependencies such as read-after-write, write-after-read, and write-after-write may exist, call this function to insert synchronization statements to avoid data read/write errors that may occur due to these data dependencies.

Call **IBSet** to set the flag of a core. It is used in pairs with **IBWait** to represent the synchronization wait instruction between cores: the core being waited on calls IBSet to notify the waiting core by writing the value 1 to a certain address block in global memory; the waiting core calls **IBWait** to continuously read the value of the corresponding address block in global memory until the value becomes 1, thereby implementing inter-core synchronization.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <bool isAIVOnly = true>
__aicore__ inline void IBWait(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, int32_t blockIdx, int32_t eventID)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| isAIVOnly | Whether to use the **AIVOnly** mode, defaulting to **true**. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspace | Input | Public cache in external storage for storing the core state, of the **GlobalTensor** type. For the definition of the **GlobalTensor** data structure, see [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md). |
| ubWorkspace | Input | Public cache for storing the current core state.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), with supported TPosition values of **VECIN**/**VECCALC**/**VECOUT**. |
| blockIdx | Input | Index of the waiting core, with a value range of [0, number of cores - 1]. |
| eventID | Input | Controls the **set** and **wait** events of the current core. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The minimum space required for **gmWorkspace** is: number of cores * 32Bytes * eventID_max + blockIdx_max * 32Bytes + 32Bytes (eventID_max and blockIdx_max refer to the maximum values of eventID and blockIdx, respectively). Note that in **AIVOnly** mode, the number of cores = **GetBlockNum()**; in **MIX** mode, the number of cores = **GetBlockNum()** * 2.
- The minimum space required for **ubWorkspace** is: 32Bytes.
- When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified when the operator is invoked must not exceed the actual number of AI processor cores running the operator. Otherwise, abnormal synchronization is inserted during multi-round scheduling by the framework, causing the kernel function (**Kernel**) to hang.
- When **IBSet** and **IBWait** are used as a pair, all parameters except **ubWorkspace** must be identical. Otherwise, the program hangs at **IBWait**.
- In separate mode, when this API is used for synchronization between two cores, it takes effect only on **AIV** cores.
- The value cached in the **gmWorkspace** of the **IBSet** API must be initialized to 0.

## Example<a name="section177231425115410"></a>

For the complete sample, see [Example](IBSet.md#section177231425115410).
