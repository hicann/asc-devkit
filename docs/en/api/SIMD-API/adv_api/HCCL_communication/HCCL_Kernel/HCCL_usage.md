# HCCL Usage

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:33:10.978Z pushedAt=2026-11-18T04:56:46.824Z nl=2 -->

Ascend C provides a set of high-level HCCL communication APIs, allowing operator kernel function (**Kernel**) developers to flexibly manage the execution order of computation and communication tasks in compute-communication fusion operators on the AI Core side.

HCCL serves as the **collective communication task client**, primarily providing collective communication primitive APIs (collectively referred to as Prepare APIs below), aligned with the [collective communication C++ API](https://gitcode.com/cann/hccl/blob/master/docs/en/api_ref/comm_op_interface/README.md). It currently supports the [AllReduce](AllReduce.md), [AllGather](AllGather.md), [ReduceScatter](ReduceScatter.md), [AlltoAll](AlltoAll.md), [AlltoAllV](AlltoAllV.md), [AlltoAllvWrite](AlltoAllvWrite.md), and [BatchWrite](BatchWrite.md) APIs. All APIs in this chapter run on the AI Core and do not execute communication tasks. Instead, you invoke the Prepare API to send the communication task information of the corresponding type to the AI CPU or CCU server, and at an appropriate time invoke the [Commit](Commit.md) API to notify the server on the AI CPU or CCU to execute the corresponding communication task.<!-- npu="950" id1 -->Note that currently only the CCU server is supported on the Ascend 950PR&950DT products.<!-- end id1 -->

The so-called appropriate time depends on whether the user orchestrates a communication-before-computation task or a computation-before-communication task. These two scenarios are briefly described as follows:

-   Communication-before-computation task: A typical example is the AllGather communication + Matmul computation task orchestration. In this scenario, after the user invokes the AllGather API to deliver the communication task, the user can immediately invoke the Commit API via the communication task identifier **handleId** returned by the AllGather API to notify the server to execute the task corresponding to **handleId**. Meanwhile, the user invokes the [Wait](Wait.md) blocking API to wait for the server to notify that the communication task corresponding to **handleId** has completed execution. After the communication task completes, the computation task is then executed.
-   Computation-before-communication task: A typical example is the Matmul computation + AllReduce communication task orchestration. In this scenario, the user can first invoke the **AllReduce** API to notify the server to deliver the communication task first, and then invoke the **Matmul** computation API to perform computation. In this way, the assembly, task delivery, and execution of the AllReduce task can be hidden by the Matmul computation pipeline. After the computation task completes, the **Commit** API is invoked to notify the server to execute the AllReduce task, without invoking the **Wait** API to wait for the communication task to complete execution.

When there are no subsequent communication tasks, invoke the [Finalize](Finalize.md) API to notify the server that there are no further communication tasks. Exit after execution completes. The client detects and waits for the last communication task to complete. The mechanism by which the AI Core delivers HCCL communication tasks, as described above, is shown in the following figure.

**Figure 1** AI Core HCCL communication task delivery mechanism  
![](../../../../figures/ai_core_hccl_dispatch.png "AI Core HCCL communication task delivery mechanism")

<!-- npu="950" id3 -->
**Figure 2** Ascend 950PR&950DT product AI Core HCCL communication task delivery mechanism  
![](../../../../figures/ascend950pr_950dt_ai_core_hccl_dispatch.png "Ascend 950PR/Ascend 950DT AI Core HCCL communication task delivery mechanism")
<!-- end id3 -->

<!-- npu="A3" id2 -->
> [!CAUTION]Note
>For Atlas A3 products, in the scenario where the AI CPU serves as the server, the functionality of the HCCL communication APIs depends on opening the AI CPU user-mode task delivery and scheduling, which poses certain security risks. Users must ensure the security and reliability of their custom AI Core operators to prevent malicious attacks.
<!-- end id2 -->

The specific steps to implement delivering a communication task on the AI Core are as follows:

1.  Create an HCCL object and invoke the initialization API [InitV2](InitV2.md).

    ```
    // Pass the initTiling address. This method is recommended.
    GET_TILING_DATA_WITH_STRUCT(
        AllGatherCustomTilingData, tilingData, tilingGM); // AllGatherCustomTilingData is the structure defined in the corresponding operator header file.

    Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU> hccl; // Select the hardware type via the template parameter.
    GM_ADDR contextGM = GetHcclContext<0>(); // In an Ascend C custom operator kernel, obtain the HCCL context in this way.

    hccl.InitV2(contextGM, &tilingData);
    ```

    When invoking the InitV2 API, you must use the development method of defining the TilingData structure with standard C++ syntax. In the example code above, tilingGM is the GM address of the operator TilingData passed in from the host side as the kernel function (Kernel) input, and the TilingData is obtained via [GET\_TILING\_DATA\_WITH\_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md). When invoking the [InitV2](InitV2.md) initialization API, you need to pass in the communication context information, which can be obtained via the API [GetHcclContext](../HCCL-Context/GetHcclContext.md) provided by the framework for obtaining the communication context.

2.  Set the Tiling address of the corresponding communication algorithm.

    Set the Tiling address of the corresponding communication algorithm via the [SetCcTilingV2](SetCcTilingV2.md) API. After the Commit API is invoked, the address is sent to the server and parsed by the server. The [SetCcTilingV2](SetCcTilingV2.md) API must be used together with the [InitV2](InitV2.md) API. The following is an example.

    ```
    // Pass the initTiling address.
    GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM);

    Hccl hccl;
    GM_ADDR contextGM = GetHcclContext<0>(); // In an Ascend C custom operator kernel, obtain the HCCL context in this way.

    hccl.InitV2(contextGM, &tilingData);
    if (SetCcTilingV2(offsetof(AllGatherCustomTilingData, mc2CcTiling)) != HCCL_SUCCESS) {
        return;
    }
    ```

3.  The user asynchronously delivers the corresponding type of communication task via the corresponding Prepare API and obtains the task identifier handleId. After receiving it, the server starts expanding and delivering the communication task. The example is as follows.

    ```
    auto handleId =
        hccl.ReduceScatter<false>(aGM, cGM, recvCount, AscendC::HCCL_DATA_TYPE_FP16, HCCL_REDUCE_SUM, strideCount, 1);
    // For the Prepare API, add exception value validation and PRINTF output during debugging.
    // if (handleId == INVALID_HANDLE_ID) {
    // 	PRINTF("[ERROR] call ReduceScatter failed, handleId is -1.");
    //	return;
    // }
    ```

    The Prepare API in the example is [ReduceScatter](ReduceScatter.md). For other APIs, see the content in the subsequent sections. The parameter AscendC::HCCL\_DATA\_TYPE\_FP16 is the data type of the HCCL task, whose data structure is HcclDataType. For the corresponding parameter description, see [Table 1](#table116710585514). The parameter HCCL\_REDUCE\_SUM is a Reduce operation. For the Reduce operation types supported by the AllReduce and ReduceScatter reduction operations, see [Table 2](#table2469980529).

    **Table 1**  HcclDataType Parameter Description

    <a name="table116710585514"></a>

    | Data Type | Description |
    | --- | --- |
    | HcclDataType | Data type of the HCCL task. HcclDataType is an enumeration type, defined as shown in the following code.<br>    HCCL_DATA_TYPE_INT8: int8 <br>    HCCL_DATA_TYPE_INT16: int16 <br>    HCCL_DATA_TYPE_INT32: int32 <br>    HCCL_DATA_TYPE_FP16: half or float16 <br>    HCCL_DATA_TYPE_FP32: float <br>    HCCL_DATA_TYPE_INT64: int64 <br>    HCCL_DATA_TYPE_UINT64: uint64 <br>    HCCL_DATA_TYPE_UINT8: uint8 <br>    HCCL_DATA_TYPE_UINT16: uint16 <br>    HCCL_DATA_TYPE_UINT32: uint32 <br>    HCCL_DATA_TYPE_FP64: float64 <br>    HCCL_DATA_TYPE_BFP16: bfloat16 <br>    HCCL_DATA_TYPE_INT128: int128, reserved type, not supported <br>    HCCL_DATA_TYPE_HIF8: hif8 <br>    HCCL_DATA_TYPE_FP8E4M3: fp8e4m3 <br>    HCCL_DATA_TYPE_FP8E5M2: fp8e5m2 <br>    HCCL_DATA_TYPE_FP8E8M0: fp8e8m0 <br>    HCCL_DATA_TYPE_RESERVED: not supported for use  |

    ```
    enum HcclDataType {
        HCCL_DATA_TYPE_INT8 = 0,     /* int8 */
        HCCL_DATA_TYPE_INT16 = 1,    /* int16 */
        HCCL_DATA_TYPE_INT32 = 2,    /* int32 */
        HCCL_DATA_TYPE_FP16 = 3,     /* half or float16 */
        HCCL_DATA_TYPE_FP32 = 4,     /* float */
        HCCL_DATA_TYPE_INT64 = 5,    /* int64 */
        HCCL_DATA_TYPE_UINT64 = 6,   /* uint64 */
        HCCL_DATA_TYPE_UINT8 = 7,    /* uint8 */
        HCCL_DATA_TYPE_UINT16 = 8,   /* uint16 */
        HCCL_DATA_TYPE_UINT32 = 9,   /* uint32 */
        HCCL_DATA_TYPE_FP64 = 10,    /* float64 */
        HCCL_DATA_TYPE_BFP16 = 11,   /* bfloat16 */
        HCCL_DATA_TYPE_INT128 = 12,  /* Reserved int128 type, not supported yet. */
        HCCL_DATA_TYPE_HIF8 = 14,    /* hif8 */
        HCCL_DATA_TYPE_FP8E4M3 = 15, /* fp8e4m3 */
        HCCL_DATA_TYPE_FP8E5M2 = 16, /* fp8e5m2 */
        HCCL_DATA_TYPE_FP8E8M0 = 17, /* fp8e8m0 */
        HCCL_DATA_TYPE_RESERVED      /* reserved */
    };
    ```

    **Table 2**  HcclReduceOp parameters

    <a name="table2469980529"></a>

    | Data Type | Description |
    | --- | --- |
    | HcclReduceOp | Reduce operation type. **HcclReduceOp** is an enumeration type, defined as shown in the following code.<br> **HCCL_REDUCE_SUM**: sum <br>    **HCCL_REDUCE_PROD**: prod <br>    **HCCL_REDUCE_MAX**: max <br>    **HCCL_REDUCE_MIN**: min <br>    **HCCL_REDUCE_RESERVED**: Not supported yet |

    ```
    enum HcclReduceOp {
        HCCL_REDUCE_SUM = 0,  /* sum */
        HCCL_REDUCE_PROD = 1, /* prod */
        HCCL_REDUCE_MAX = 2,  /* max */
        HCCL_REDUCE_MIN = 3,  /* min */
        HCCL_REDUCE_RESERVED  /* reserved */
    }
    ```

4.  The user invokes the [Commit](Commit.md) API to notify the server to execute the communication task corresponding to **handleId**.

    ```
    // Wait until the communication task is ready for execution, and invoke the Commit API to notify the server side to execute.
    hccl.Commit(handleId);
    ```

5.  The user invokes the blocking [Wait](Wait.md) API to wait for the server to finish executing the corresponding communication task.

    ```
    auto ret = hccl.Wait(handleId);
    // For the Wait and Query APIs, add exception value validation and PRINTF printing during debugging.
    // if (ret == HCCL_FAILED) {
    //	PRINTF("[ERROR] call Wait for handleId[%d] failed.", handleId);
    //	return;
    // }

    // Invoke the inter-core synchronization API to prevent some cores from exiting early after faster execution, which would trigger Hccl destruction and affect slower cores.
    // Based on the actual business scenario, you can choose to invoke the SyncAll, CrossCoreSetFlag, or CrossCoreWaitFlag API to ensure that all cores complete their tasks before exiting.
    ```

6.  The user invokes the [Finalize](Finalize.md) API to notify the server that there are no more communication tasks, and exits after execution completion. The client detects and waits for the last communication task to complete.

    ```
    hccl.Finalize();
    ```

Note: If the template parameter of the HCCL object does not specify the core that delivers the communication task, the Prepare API can run only on either AIC or AIV. Before invoking the APIs in steps 2 to 5, you must specify that the API code runs on the AIC or AIV core, as shown in the following code.

```
// Determine whether the core is an AIC or AIV core via the built-in constant g_coreType.
if (g_coreType == AIV) {
// if (g_coreType == AIC) {
        Invoke the HCCL API.
}
```

Based on the above understanding of delivering a single communication task, this section describes the flexible use of the repeat parameter in the Prepare API. One Prepare API invocation corresponds to one handleId. The repeat parameter in the Prepare API represents the number of communication tasks in this Prepare invocation, and this value must be consistent with the number of Commit API invocations and Wait API invocations for this handleId. Take [Figure 3 ReduceScatter communication example](#fig19780192164210) as an example. Assume there are 4 cards in total. On each card, the source data is first evenly divided into 4 parts by rankId, and each part is further split into 3 parts. The number of data elements in each split part is TileLen. Each ReduceScatter communication only communicates one group of split data (for example, data 0-0, 1-0, 2-0, and 3-0 in the figure form one group of split data). Therefore, 3 ReduceScatter operations are required to complete the communication of all data.

In this scenario, you can invoke the ReduceScatter API 3 times with the repeat parameter set to 1 to deliver 3 communication tasks, and update the send and receive addresses of each ReduceScatter task to obtain the handleId of the 3 tasks. Each handleId task invokes the Commit and Wait APIs once. The corresponding code snippet is as follows.

```
extern "C" __global__ __aicore__ void reduce_scatter_custom(
    GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    auto sendBuf = xGM;                       // xGM is the input GM address of ReduceScatter.
    auto recvBuf = yGM;                       // yGM is the output GM address of ReduceScatter.
    constexpr size_t rankSize = 4U;           // 4 cards.
    constexpr size_t tileCnt = 3U;            // The data on each card is evenly divided into rankSize parts, and each part is further divided into 3 parts.
    constexpr size_t tileLen = 100U;          // Number of data elements in each divided part.
    uint64_t strideCount = tileLen * tileCnt; // Offset of the start address between adjacent data blocks on sendBuf.

    REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);

    Hccl hccl;
    GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.
    if (AscendC::g_coreType == AIV) {                 // Specify AIV core communication.
        hccl.InitV2(contextGM, &tilingData);
        auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, reduceScatterCcTiling));
        if (ret != HCCL_SUCCESS) {
            return;
        }
        // Three handleIds are generated in the for loop, and each handleId invokes the Commit and Wait APIs only once with repeat=1.
        for (int i = 0; i < tileCnt; ++i) {
            auto handleId = hccl.ReduceScatter(
                sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP32, HcclReduceOp::HCCL_REDUCE_SUM,
                strideCount, 1); // For details about the parameters, see the ReduceScatter API description.
            hccl.Commit(handleId);
            auto ret = hccl.Wait(handleId);
            // Execute other computation logic ....

            // Update the send and receive addresses of ReduceScatter.
            sendBuf += tileLen * sizeof(float);
            recvBuf += tileLen * sizeof(float);
        }
        AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from running too fast and calling hccl.Finalize() early, which would cause other cores to hang on Wait.
        hccl.Finalize();
    }
}
```

Since the source address SendBuf of the three data copies on each card is contiguous, and the destination address recvBuf on each card, which stores the three communication result data copies, is also contiguous, the preceding code can be optimized by setting the repeat parameter in the ReduceScatter API to 3, thereby invoking the ReduceScatter API once to deliver three communication tasks. In this case, there is only one handleId task, but the Commit and Wait APIs need to be invoked three times. The corresponding code snippet is as follows.

```
extern "C" __global__ __aicore__ void reduce_scatter_custom(
    GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    auto sendBuf = xGM;                       // xGM is the input GM address of ReduceScatter.
    auto recvBuf = yGM;                       // yGM is the output GM address of ReduceScatter.
    constexpr size_t rankSize = 4U;           // 4 cards.
    constexpr size_t tileCnt = 3U;            // The data on the card is evenly divided into rankSize parts, and each part is further divided into 3 parts.
    constexpr size_t tileLen = 100U;          // Number of data elements in each divided part.
    uint64_t strideCount = tileLen * tileCnt; // Offset of the start address between adjacent data blocks on sendBuf.

    REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);

    Hccl hccl;
    GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.
    if (AscendC::g_coreType == AIV) {                 // Specify communication on the AIV core.
        hccl.InitV2(contextGM, &tilingData);
        auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, reduceScatterCcTiling));
        if (ret != HCCL_SUCCESS) {
            return;
        }

        auto handleId = hccl.ReduceScatter(
            sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP32, HcclReduceOp::HCCL_REDUCE_SUM, strideCount,
            tileCnt); // For specific parameters, see the ReduceScatter API description.
        for (int i = 0; i < tileCnt; ++i) {
            hccl.Commit(handleId);
            auto ret = hccl.Wait(handleId);
            // Execute other computation logic ....
        }
        AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and invoking the hccl.Finalize() API early, which would cause other cores to hang in Wait.
        hccl.Finalize();
    }
}
```

**Figure 3**  ReduceScatter communication example<a name="fig19780192164210"></a>  
![](../../../../figures/reducescatter_example.png "ReduceScatter communication example")

**Table 3**  MC2\_BUFFER\_LOCATION parameters

| Data Type | Description |
| --- | --- |
| MC2_BUFFER_LOCATION | Reserved parameter. The storage location of the intermediate results of computation and communication. Users can set this field on the Tiling side. |

Tip: When debugging an operator that uses HCCL high-level APIs, add the compilation option -DASCENDC\_DEBUG to the operator compilation project to enable interception of abnormal scenarios. For details, see and use the [assert API](../../../../Utils-API/tuning_interface/assert.md).
