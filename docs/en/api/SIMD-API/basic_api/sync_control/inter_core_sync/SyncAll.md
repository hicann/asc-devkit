# SyncAll<a name="ZH-CN_TOPIC_0000001538296817"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T13:43:02.495Z -->

## Applicable Products<a name="section1550532418810"></a>

### Software Synchronization Prototype

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
- Atlas 200I/500 A2 inference product: Not supported
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
### Hard Synchronization (Not Supporting config-Specified Pipeline) Prototype

<!-- npu="950" id10 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
### Hard Synchronization (Supports config-Specified Pipeline) Prototype

<!-- npu="950" id19 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id19 -->
<!-- npu="A3" id20 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id20 -->
<!-- npu="910b" id21 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id21 -->
<!-- npu="310b" id22 -->
- Atlas 200I/500 A2 inference product: Not supported
<!-- end id22 -->
<!-- npu="310p" id23 -->
- Atlas inference products AI Core: Not supported
<!-- end id23 -->
<!-- npu="310p" id24 -->
- Atlas inference products Vector Core: Not supported
<!-- end id24 -->
<!-- npu="910" id25 -->
- Atlas training products: Not supported
<!-- end id25 -->
## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

**SyncAll** is an inter-core synchronization control API. Its functions are described below based on different prototypes:

- Hardware synchronization API:
    Used for synchronization control among multiple cores, supporting the following synchronization scenarios:
    - Full-core synchronization of pure Vector operators. For details, see [Mode 0 of CrossCoreSetFlag](key_feature_description.md#ZH-CN_TOPIC_0000002586300741).
    - Full-core synchronization of fusion operators, which implements three inter-core synchronization capabilities at once (see [Key Feature Description of CrossCoreSetFlag](key_feature_description.md#ZH-CN_TOPIC_0000002586300741)): first, within a single AI Core, synchronize the AIC with all AIVs (the AIC waits for the AIVs); then, across multiple AI Cores, synchronize all AICs; finally, within a single AI Core, synchronize all AIVs with the AIC (the AIVs wait for the AIC).

- Software synchronization API:
    Supports only multi-core synchronization of pure Vector operators (the number of cores participating in synchronization can be specified through an input parameter). Each AIV writes the signal value 1 to the corresponding location in global memory, and then each AIV continuously polls the signal values of all AIVs in global memory until the signal values corresponding to all AIV cores become 1.

In the pure Vector operator scenario, if all AIV cores participate in synchronization by default, the hardware synchronization API with better performance is recommended; if only some AIV cores need to participate in synchronization, use the software synchronization API and configure it through the input parameter **usedCores**.

## Prototype<a name="section620mcpsimp"></a>

- Software synchronization

    ```cpp
    template <bool isAIVOnly = true>
    __aicore__ inline void SyncAll(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, const int32_t usedCores = 0)
    ```

- Hard synchronization
    - Specifying the pipeline via **config** is not supported.

        ```cpp
        template <bool isAIVOnly = true>
        __aicore__ inline void SyncAll()
        ```

    - Specifying the pipeline via **config** is supported.

        ```cpp
        template <bool isAIVOnly = true, const SyncAllConfig& config = DEFAULT_SYNC_ALL_CONFIG>
        __aicore__ inline void SyncAll()
        ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| isAIVOnly | Controls whether **SyncAll** applies to a pure Vector operator or a Mix operator (containing both Cube and Vector computation). Optional values:<br>&bull; **true** (default): Full-core synchronization for a pure Vector operator, performing full-core synchronization only on Vector cores.<br>&bull; **false**: Full-core synchronization for a Mix operator (containing both Cube and Vector computation). It first completes full-core synchronization of Vector cores and Cube cores separately, and then performs synchronization between the two (the software synchronization API does not support this feature). |
| config | **This configuration takes effect only when isAIVOnly is set to True.**<br>struct SyncAllConfig {&#x000A;    pipe_t triggerPipe;&#x000A;    pipe_t waitPipe;&#x000A;};&#x000A;&#x000A;// Use all pipelines for triggering and waiting by default.<br>constexpr SyncAllConfig DEFAULT_SYNC_ALL_CONFIG = {PIPE_ALL, PIPE_ALL};<br>Controls the behavior of the **SyncAll** function. When performing pipeline synchronization among multiple AI Cores, specifies which pipelines are used for triggering and waiting.<br>&bull; **triggerPipe**: Specifies which pipeline is used to "send the trigger signal". Only **MTE2**, **MTE3**, and **PIPE_ALL** are supported.<br>&bull; **waitPipe**: Specifies which pipeline is used to "receive the wait signal". Only **MTE2**, **MTE3**, and **PIPE_ALL** are supported.<br>// Multiple AIVs perform computation.&#x000A;<br>Compute();&#x000A;<br>// Multiple AIVs copy the computation results to GM.&#x000A;<br>CopyToGM();&#x000A;<br>// After all data is copied to GM, perform accumulation.&#x000A;<br>constexpr AscendC::SyncAllConfig CustomConfig = {PIPE_MTE3, PIPE_ALL};&#x000A;<br>AscendC::SyncAll&lt;true, CustomConfig&gt;();&#x000A;<br>for (int i = 0; i &lt; blockNum; i++) {&#x000A;<br>    if (i != blockIdx) {&#x000A;<br>        CopyFromGm();&#x000A;<br>        Accumulate();&#x000A;<br>    }&#x000A;<br>}&#x000A;<br>// Write back the accumulated data.&#x000A;<br>CopyToGm();<br>|

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspace | Input | **gmWorkspace** is a user-defined global space used as a shared cache for all cores to store the status flag of each core. Its type is **GlobalTensor**, and the supported data type is **int32_t**. For the definition of the **GlobalTensor** data structure, see [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md).<br>For the required space size and usage notes, see [Constraints](#section633mcpsimp).<br>The hard synchronization API does not support this parameter. |
| ubWorkspace | Input | **ubWorkspace** is a user-defined local space used independently by each core to mark the status of the current core.<br>Its type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). The supported **TPosition** values are **VECIN**, **VECCALC**, and **VECOUT**, and the supported data type is **int32_t**.<br>For the required space size, see [Constraints](#section633mcpsimp).<br>The hard synchronization API does not support this parameter. |
| usedCores | Input | Specifies the number of cores among which synchronization is performed. The passed value cannot exceed the logical **numBlocks** specified during operator invocation. This is a default parameter; if it is not passed, full-core software synchronization is performed.<br>It is supported only in the software synchronization API, and the hard synchronization API does not support this parameter. |

## Return Value<a name="section91032023123812"></a>

None

## Constraints<a name="section633mcpsimp"></a>

The internal implementations of the **SyncAll** hardware synchronization and software synchronization APIs differ, and their constraints also differ.

- Constraints on the software synchronization API:
    - The space requested for the **gmWorkspace** cache must be greater than or equal to **GetBlockNum()**\*32 bytes, and the cache value must be initialized to 0. Currently, there are two common initialization methods:
        - Perform initialization on the host side to ensure that the **gmWorkspace** cache has been initialized to 0 before it is passed to this API.
        - Initialize the **gmWorkspace** cache during kernel-side initialization. Note that each core must initialize the entire **gmWorkspace** cache space.
    - The space requested for **ubWorkspace** must be greater than or equal to **GetBlockNum()**\*32 bytes.
    - In a pure Vector operator scenario, if all AIV cores participate in synchronization by default, the hardware synchronization API with better performance is recommended. If only some AIV cores need to participate in synchronization, use the software synchronization API and configure it through the **usedCores** input parameter.
    - The value passed to **usedCores** must not exceed the number of AIVs in the logical AI Core specified during operator invocation: **GetBlockNum()**\***GetTaskRatio()**. If this parameter is not passed, full-core software synchronization is performed.

- Constraints on the hardware synchronization API:
    - When using the hard synchronization API, use an appropriate kernel function modifier based on the scenario:
        - For a pure Vector operator, use `__mix__(0, 1)` to modify the kernel function instead of `__vector__`. For details, see the Kernel type configuration description in [CrossCoreSet Constraints](CrossCoreSetFlag_ISASI.md#constraints).
        - For a Mix operator (involving both Cube and Vector computation), use `__mix__(1, 1)` or `__mix__(1, 2)` to modify the kernel function.
    - For a pure Vector operator, the template parameter `isAIVOnly` must be set to `true`; otherwise, the program may hang.
    - For a Mix operator (involving both Cube and Vector computation), setting the template parameter `isAIVOnly` to `true` causes the API to actually complete only Vector full-core synchronization, which is inconsistent with the expected synchronization behavior.
    - When using this API, enable the `batchmode` mode so that the operator exclusively occupies all required core resources; otherwise, a deadlock may occur when the following conditions are met:
        - Multi-stream concurrent scenario (with at least two execution streams).
        - At least two operators executing concurrently.
        - The total number of cores of all concurrent operators exceeds the number of physical cores.
        - At least two concurrent operators use the inter-core synchronization feature.

        Specifically, in a multi-stream scenario, although the inter-core synchronization operator of a stream is assigned to n physical cores, only n-m cores may be scheduled and executed first, while the remaining m cores have not yet started because they are preempted by inter-core synchronization operators of other streams. The n-m cores that start first wait for the remaining m cores to complete when they reach the inter-core synchronization point, while the remaining m cores cannot be released because they are occupied by inter-core synchronization operators of other streams, resulting in a deadlock.
        In the Kernel direct invocation scenario, the batchmode mode is set through the [\_\_schedmode\_\_\(mode\)](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMD-BuiltIn_keyword.md) qualifier. In the engineering operator development scenario, the batchmode mode is set through the **SetScheduleMode** API of **TilingContext**. For details, see *Basic Data Structures and APIs*.
    - The internal implementation of the SyncAll hardware synchronization API uses [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) for inter-core synchronization control. Therefore, developers are not advised to use **CrossCoreSetFlag** and the SyncAll hardware synchronization API at the same time, as this may cause flagID conflicts. The flagId range occupied by the SyncAll hardware synchronization API is [11-14].

- Common constraints of the hardware synchronization API and the software synchronization API: When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not exceed the number of AI processor cores that actually run the operator. Otherwise, abnormal synchronization is inserted when the framework performs multi-round scheduling, causing the Kernel to "hang".

## Example<a name="section642mcpsimp"></a>

This example uses eight cores for data processing. Each core processes 32 float-type data elements, multiplies them by 2, and then adds the result to the data multiplied by 2 on other cores. The intermediate result is saved to **workGm**, so data synchronization is required among multiple cores. In this example, software synchronization is used, and the values in **syncGm** passed in by the entry function have all been initialized to 0 on the host side. If the following example is changed to use hard synchronization, **syncGm** does not need to be passed in, and **workQueue** is not required.

```cpp
// syncGlobal is a user-defined global space used as a shared cache for all cores, of the GlobalTensor type. workLocal is a user-defined local space used independently by each core, of the LocalTensor type.
int srcDataSize = 256; // Number of elements involved in the computation.
int32_t blockNum = AscendC::GetBlockNum(); // Obtain the total number of cores.
int32_t blockIdx = AscendC::GetBlockIdx(); // Obtain the ID of the currently working core.
uint32_t perBlockSize = srcDataSize / blockNum; // Each core processes an equal number of elements.
// The data computed by the currently working core is first saved to the external workspace. workGlobal is of the GlobalTensor type, and dstLocal is of the LocalTensor type.
AscendC::DataCopy(workGlobal[blockIdx * perBlockSize], dstLocal, perBlockSize);
// Wait for all cores to complete the computation.
AscendC::SyncAll(syncGlobal, workLocal);
```

For the complete example, see [SyncAll example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/05_sync_control/sync_all).
