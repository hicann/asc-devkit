# SyncAll<a id="ZH-CN_TOPIC_0000001538296817"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T18:47:17.889Z pushedAt=2026-10-26T07:41:44.801Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

### Soft Synchronization Prototype

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll_res.md#id1 -->

### hard synchronization (specified pipeline not supported) Prototype

<!-- npu="950" id10 -->
- Ascend 950PR&950DT products: supported
<!-- end id10 -->
<!-- npu="A3" id11 -->
- Atlas A3 products: supported
<!-- end id11 -->
<!-- npu="910b" id12 -->
- Atlas A2 products: supported
<!-- end id12 -->
<!-- npu="310b" id13 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products AI Core: not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products Vector Core: not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll_res.md#id2 -->

### Hard Synchronization (Supports Config-Specified Pipeline) Prototype

<!-- npu="950" id19 -->
- Ascend 950PR&950DT products: supported
<!-- end id19 -->
<!-- npu="A3" id20 -->
- Atlas A3 products: not supported
<!-- end id20 -->
<!-- npu="910b" id21 -->
- Atlas A2 products: not supported
<!-- end id21 -->
<!-- npu="310b" id22 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id22 -->
<!-- npu="310p" id23 -->
- Atlas inference products AI Core: not supported
<!-- end id23 -->
<!-- npu="310p" id24 -->
- Atlas inference products Vector Core: not supported
<!-- end id24 -->
<!-- npu="910" id25 -->
- Atlas training products: not supported
<!-- end id25 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_sync/SyncAll_res.md#id3 -->

## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_block_sync_intf.h"`.

**SyncAll** is an inter-core synchronization control API. Its functions are described based on different function prototypes:

- Hardware synchronization API:
    Used for synchronization control among multiple cores, supporting the following synchronization scenarios:
    - Full-core synchronization of pure Vector operators. For details, see [mode 0 of CrossCoreSetFlag](key_features.md).
    - Full-core synchronization of fusion operators, which implements three inter-core synchronization capabilities at once (see [key feature description of CrossCoreSetFlag](key_features.md)): first, within a single **AI Core**, synchronize the **AIC** with all **AIVs** (the **AIC** waits for the **AIVs**); then, synchronize all **AICs** across multiple **AI Cores**; finally, within a single **AI Core**, synchronize all **AIVs** with the **AIC** (the **AIVs** wait for the **AIC**).

- Software synchronization API:
    Supports only multi-core synchronization of pure Vector operators (the number of cores participating in synchronization can be specified through an input parameter). Each **AIV** writes the signal value 1 to the corresponding location in global memory, and then each **AIV** continuously polls the signal values corresponding to all **AIVs** in global memory until the signal values corresponding to all **AIV** cores become 1.

In the pure Vector operator scenario, if all **AIV** cores participate in synchronization by default, the hardware synchronization API with better performance is recommended; if only some **AIV** cores need to participate in synchronization, use the software synchronization API and configure it through the input parameter **usedCores**.

## Prototype<a id="section620mcpsimp"></a>

- Software synchronization

    ```cpp
    template <bool isAIVOnly = true>
    __aicore__ inline void SyncAll(const GlobalTensor<int32_t>& gmWorkspace, const LocalTensor<int32_t>& ubWorkspace, const int32_t usedCores = 0)
    ```

- Hard synchronization
    - **config** does not support specifying a pipeline

        ```cpp
        template <bool isAIVOnly = true>
        __aicore__ inline void SyncAll()
        ```

    - **config** supports specifying a pipeline

        ```cpp
        template <bool isAIVOnly = true, const SyncAllConfig& config = DEFAULT_SYNC_ALL_CONFIG>
        __aicore__ inline void SyncAll()
        ```

## Parameters<a id="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| isAIVOnly | Controls whether **SyncAll** applies to a pure Vector operator or a Mix operator (including Cube and Vector computation). Optional values:<br>&bull; **true** (default): Full-core synchronization of a pure Vector operator, which performs full-core synchronization of only the Vector cores.<br>&bull; **false**: Full-core synchronization of a Mix operator (including Cube and Vector computation), which first completes full-core synchronization of the Vector cores and the Cube cores separately, and then performs synchronization between them (the software synchronization API does not support this feature). |
| config | **This configuration takes effect only when isAIVOnly=True.**<br>struct SyncAllConfig {&#x000A;    pipe_t triggerPipe;&#x000A;    pipe_t waitPipe;&#x000A;};&#x000A;&#x000A;// Use all pipelines for triggering and waiting by default.<br>constexpr SyncAllConfig DEFAULT_SYNC_ALL_CONFIG = {PIPE_ALL, PIPE_ALL};<br>Controls the behavior of the **SyncAll** function. When performing pipeline synchronization among multiple AI Cores, specifies which pipelines (pipe) are used for triggering and waiting.<br>&bull; **triggerPipe**: Specifies which pipeline is used to "send the trigger signal". Only MTE2, MTE3, and PIPE_ALL are supported.<br>&bull; **waitPipe**: Specifies which pipeline is used to "receive the wait signal". Only MTE2, MTE3, and PIPE_ALL are supported.<br>// Multiple AIVs perform computation.&#x000A;<br>Compute();&#x000A;<br>// Multiple AIVs copy the computation results to GM.&#x000A;<br>CopyToGM();&#x000A;<br>// Wait until all data is copied to GM, then perform accumulation.&#x000A;<br>constexpr AscendC::SyncAllConfig CustomConfig = {PIPE_MTE3, PIPE_ALL};&#x000A;<br>AscendC::SyncAll&lt;true, CustomConfig&gt;();&#x000A;<br>for (int i = 0; i &lt; blockNum; i++) {&#x000A;<br>    if (i != blockIdx) {&#x000A;<br>        CopyFromGm();&#x000A;<br>        Accumulate();&#x000A;<br>    }&#x000A;<br>}&#x000A;<br>// Write back the accumulated data.&#x000A;<br>CopyToGm(); |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gmWorkspace | Input | **gmWorkspace** is a user-defined global space used as a shared cache for all cores to store the status flag of each core. Its type is **GlobalTensor**, and the supported data type is int32_t. For the definition of the **GlobalTensor** data structure, see [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor.md).<br>For the required space size and usage notes, see [Constraints](#section633mcpsimp).<br>The hard synchronization API does not support this parameter. |
| ubWorkspace | Input | **ubWorkspace** is a user-defined local space used independently by each core to mark the status of the current core.<br>Its type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), the supported TPosition values are VECIN, VECCALC, and VECOUT, and the supported data type is int32_t.<br>For the required space size, see [Constraints](#section633mcpsimp).<br>The hard synchronization API does not support this parameter. |
| usedCores | Input | Specifies the number of cores among which synchronization is performed. The passed value cannot exceed the logical numBlocks specified during operator invocation. This is a default parameter; if it is not passed, full-core software synchronization is performed.<br>Supported only in the software synchronization API; the hard synchronization API does not support this parameter. |

## Return Value<a id="section91032023123812"></a>

None

## Constraints<a id="section633mcpsimp"></a>

The internal implementations of the **SyncAll** hardware synchronization and software synchronization APIs differ, and their constraints also differ.

- Software synchronization API constraints:
    - The space size requested for the **gmWorkspace** cache must be greater than or equal to **GetBlockNum()** * 32 bytes, and the cache value must be initialized to 0. Currently, there are two common initialization methods:
        - Perform initialization on the host side to ensure that the **gmWorkspace** cache has been initialized to 0 before it is passed to this API.
        - Initialize the **gmWorkspace** cache during kernel-side initialization. Note that the entire **gmWorkspace** cache space must be initialized on each core.
    - The space size requested for **ubWorkspace** must be greater than or equal to **GetBlockNum()** * 32 bytes.
    - In pure **Vector** operator scenarios, if all AIV cores participate in synchronization by default, the hardware synchronization API with better performance is recommended. If only some AIV cores need to participate in synchronization, use the software synchronization API and configure it through the **usedCores** input parameter.
    - The value passed to **usedCores** must not exceed the number of AIVs in the logical AI Core specified during operator invocation: **GetBlockNum()** * **GetTaskRatio()**. If this parameter is not passed, full-core software synchronization is used.

- Hardware synchronization API constraints:
    - When using the hard synchronization API, use an appropriate **Kernel** modifier based on the scenario:
        - For pure Vector operators, use \_\_mix\_\_\(0, 1\) to modify the **Kernel** instead of \_\_vector\_\_. For details, see the Kernel type configuration description in [CrossCoreSetFlag Constraints](CrossCoreSetFlag_ISASI.md#constraints).
        - For a Mix operator (including Cube and Vector computation), use \_\_mix\_\_\(1, 1\) or \_\_mix\_\_\(1, 2\) to modify the **Kernel**.
    - For a pure Vector operator, the template parameter **isAIVOnly** must be set to **true**; otherwise, the program will hang.
    - For a Mix operator (including Cube and Vector computation), setting the template parameter **isAIVOnly** to **true** causes the API to actually complete only Vector full-core synchronization, which is inconsistent with the expected synchronization behavior.
    - When using this API, enable batchmode mode so that the operator exclusively occupies all required core resources; otherwise, a deadlock may occur if the following conditions are met:
        - Multi-stream concurrent scenario (≥ 2 execution streams).
        - ≥ 2 operators executing concurrently.
        - The total number of cores of all concurrent operators exceeds the number of physical cores.
        - ≥ 2 concurrent operators use the inter-core synchronization function.

        Specifically, in a multi-stream scenario, although the inter-core synchronization operator of a stream is assigned to n physical cores, only n-m cores may be scheduled and executed first, while the remaining m cores have not yet started because they are preempted by the inter-core synchronization operators of other streams. The n-m cores that start first wait for the remaining m cores to complete when they reach the inter-core synchronization point, while the remaining m cores cannot be released because they are occupied by the inter-core synchronization operators of other streams, resulting in a deadlock.
        In the kernel function (Kernel) launch scenario, the batchmode mode is set through the [\_\_schedmode\_\_\(mode\)](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) qualifier. In the engineering operator development scenario, the batchmode mode is set through the **SetScheduleMode** API of **TilingContext**. For details, see [*Basic Data Structures and APIs*](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900beta2/API/basicdataapi/atlasopapi_07_00001.html).
    - The internal implementation of the SyncAll hardware synchronization API uses [CrossCoreSetFlag](CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](CrossCoreWaitFlag_ISASI.md) for inter-core synchronization control. Therefore, when developers use CrossCoreSetFlag (or CrossCoreWaitFlag) together with the SyncAll hardware synchronization API, they must avoid flagId conflicts. For a complete description of the flagId usage of each API, see [Inter-core Synchronization flagId Usage](key_features.md#inter_core_sync_flagid_usage).

- Common constraints of the hardware synchronization API and the software synchronization API: When this API is used for multi-core control, the number of logical AI Core cores **numBlocks** specified during operator invocation must not be greater than the number of AI processor cores that actually run the operator. Otherwise, the framework inserts abnormal synchronization during multi-round scheduling, causing the kernel function (Kernel) to "hang".

## Example<a id="section642mcpsimp"></a>

This example uses eight cores to process data, with each core processing 32 float-type data elements. Each core multiplies its data by 2 and then adds the result to the data that has been similarly multiplied by 2 on other cores. The intermediate results are saved to **workGm**, so data synchronization is required among multiple cores. In this example, software synchronization is used, and the values in **syncGm** passed in by the entry function have all been initialized to 0 on the host side. If the following example is changed to use hard synchronization, **syncGm** does not need to be passed in, and **workQueue** is not required.

```cpp
// syncGlobal is a user-defined global space used as a shared cache for all cores, of the type GlobalTensor. workLocal is a user-defined local space used independently by each core, of the type LocalTensor.
int srcDataSize = 256; // Number of elements involved in the computation.
int32_t blockNum = AscendC::GetBlockNum(); // Obtain the total number of cores.
int32_t blockIdx = AscendC::GetBlockIdx(); // Obtain the ID of the current working core.
uint32_t perBlockSize = srcDataSize / blockNum; // Each core processes an equal number of elements.
// Save the data computed by the current working core to the external workspace first. workGlobal is a GlobalTensor, and dstLocal is a LocalTensor.
AscendC::DataCopy(workGlobal[blockIdx * perBlockSize], dstLocal, perBlockSize);
// Wait for all cores to finish computation.
AscendC::SyncAll(syncGlobal, workLocal);
```

For the complete example, see [SyncAll example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/sync_all).
