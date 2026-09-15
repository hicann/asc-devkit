# IBSet<a name="ZH-CN_TOPIC_0000001538096733"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T06:46:28.315Z -->

## Product Support<a name="section1550532418810"></a>

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
- Atlas training products: Supported
<!-- end id7 -->


## Function Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

When different cores operate on the same block of global memory and there may be data dependencies such as read-after-write, write-after-read, and write-after-write, call this function to insert synchronization statements to avoid potential data read/write errors caused by such data dependencies.

Call IBSet to set the flag of a certain core. It is used in pairs with IBWait to represent the synchronization wait instruction between cores: the waited core calls IBSet to notify the waiting core by writing the value 1 to a certain address in global memory; the waiting core calls IBWait to continuously read the value at the corresponding address block in global memory until it becomes 1, thereby achieving inter-core synchronization.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <bool isAIVOnly = true>
__aicore__ inline void IBSet(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, int32_t blockIdx, int32_t eventID)
```

## Parameter Description<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter Name | Description |
| --- | --- |
| isAIVOnly | Controls whether it is AIVOnly mode. The default value is true. |

**Table 2**  API parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| gmWorkspace | Input | Common cache for storing core states in external storage, of the GlobalTensor type. For the definition of the GlobalTensor data structure, see [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md). |
| ubWorkspace | Input | Common cache for storing the current core state.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| blockIdx | Input | Indicates the idx number of the waiting core. Value range: [0, number of cores - 1]. |
| eventID | Input | Used to control the set and wait events of the current core. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The minimum space required for gmWorkspace is: number of cores*32Bytes*eventID_max+blockIdx_max*32Bytes+32Bytes (eventID_max and blockIdx_max refer to the maximum values of eventID and blockIdx, respectively). Note that in AIVOnly mode, the number of cores = GetBlockNum(); in MIX mode, the number of cores = GetBlockNum()*2.
- The minimum space required for ubWorkspace is: 32Bytes.
- When using this API for multi-core control, the number of logical AI Core cores numBlocks specified during operator invocation must not be greater than the number of AI processor cores actually running the operator. Otherwise, the framework inserts abnormal synchronization during multi-round scheduling, causing the Kernel to "hang".
- When IBSet and IBWait are used as a pair, all parameters except ubWorkspace must be identical. Otherwise, the program hangs at IBWait.
- In separate mode, when using this API for synchronization between two cores, it takes effect only on AIV cores.
- The value cached in the gmWorkspace of the IBSet API must be initialized to 0.

## Example<a name="section177231425115410"></a>

This example uses 2 cores for data processing, with each core processing 256 half-type data elements. Core 0 performs the x+y operation and places the result in the first half of z. Core 1 places the computation result of core 0 into x, then adds it to y, and stores the result in the second half of z. Therefore, data synchronization is required between multiple cores.

```cpp
// sync_gm is the common cache for storing core states externally, of the GlobalTensor type; sync_buf is the common cache for storing the current core state, of the LocalTensor type.
int32_t blockIdx = AscendC::GetBlockIdx(); // Obtain the current core.
if (blockIdx == 1) { // Set IBWait on core 1 to block instruction execution until the operation on core 0 is complete.
    AscendC::IBWait(sync_gm, sync_buf, 0, 0);
}
...
if (blockIdx == 0) { // Set IBSet on core 0 so that the instructions on core 1 are executed only after the operation on core 0 is complete.
    AscendC::IBSet(sync_gm, sync_buf, 0, 0);
}
```

For the complete sample, see [IBSet and IBWait Inter-Core Synchronization Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/ib_set_wait).
