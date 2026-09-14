# HCCL Usage

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:21:14.115Z -->

Ascend C provides a set of high-level HCCL communication APIs, allowing operator kernel developers to flexibly manage the execution order of computation and communication tasks in compute-communication fusion operators on the AI Core side.

HCCL serves as the **collective communication task client**. It mainly provides collective communication primitive APIs (collectively referred to as Prepare APIs below), which are aligned with the [Communication Operator APIs](https://gitcode.com/cann/hccl/blob/9.1.0/docs/en/api_ref/comm_op_interface/README.md). Currently, it supports [AllReduce](AllReduce.md), [AllGather](AllGather.md), [ReduceScatter](ReduceScatter.md), and [AlltoAll](AlltoAll.md) APIs. All APIs in this chapter run on the AI Core and do not execute communication tasks. Instead, you call the Prepare APIs to send the communication task information of the corresponding type to the AI CPU or CCU server, and then call the [Commit](Commit.md) API at an appropriate time to notify the server on the AI CPU or CCU to execute the corresponding communication task. Note that currently only the CCU server is supported on Ascend 950PR/Ascend 950DT.

The so-called appropriate time depends on whether you orchestrate a communication-before-computation task or a computation-before-communication task. These two scenarios are briefly described as follows:

-   Communication-before-computation tasks: a typical example is the AllGather communication + Matmul computation task orchestration. In this scenario, after calling the AllGather API to issue the communication task, you can immediately call the Commit API, using the communication task identifier **handleId** returned by the AllGather API, to notify the server to execute the task corresponding to **handleId**. At the same time, you call the [Wait](Wait-98.md) blocking API to wait for the server to notify that the communication task corresponding to **handleId** has completed execution. After the communication task completes, the computation task is then executed.
-   Computation-before-communication tasks: a typical example is the Matmul computation + AllReduce communication task orchestration. In this scenario, you can first call the AllReduce API to notify the server to issue the communication task, and then call the Matmul computation API to perform computation. In this way, the assembly, task issuance, and execution of the AllReduce task can be hidden by the Matmul computation pipeline. After the computation task completes, you call the Commit API to notify the server to execute the AllReduce task, without calling the Wait API to wait for the communication task to complete.

When there are no subsequent communication tasks, call the [Finalize](Finalize.md) API to notify the server that there are no subsequent communication tasks. The server exits after execution completes, and the client detects and waits for the last communication task to complete. The mechanism by which the AI Core issues HCCL communication tasks, as described above, is shown in the following figure.

**Figure 1** AI Core issuing HCCL communication tasks  
![](../../../../figures/ai_core_issuing_hccl_communication_tasks.png "AI Core issuing HCCL communication tasks")

**Figure 2** Ascend 950PR/Ascend 950DT AI Core issuing HCCL communication tasks  
![](../../../../figures/ascend_950pr_ascend_950dt_ai_core_issuing_hccl_communication_tasks.png "Ascend 950PR/Ascend 950DT AI Core issuing HCCL communication tasks")

> [!CAUTION]Note
>For Atlas A3 training products/Atlas A3 inference products, in the scenario where the AI CPU serves as the server, the functionality of the HCCL communication APIs depends on enabling AI CPU user-mode task dispatch, which poses certain security risks. You must ensure the security and reliability of AI Core custom operators to prevent malicious attacks.

The specific steps for the AI Core to issue a communication task are as follows:

1.  Create an HCCL object and call the initialization API [InitV2](InitV2.md).

    ```
    // Call method that passes the initTiling address. This method is recommended.
    GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM); // AllGatherCustomTilingData is the structure defined in the header file of the corresponding operator.

    Hccl<HcclServerType::HCCL_SERVER_TYPE_AICPU> hccl; // Select the hardware type through the template parameter.
    GM_ADDR contextGM = GetHcclContext<0>();  // In the Ascend C custom operator kernel, obtain the HCCL context in this way.

    hccl.InitV2(contextGM, &tilingData);
    ```

    When calling the InitV2 API, you must use the development method of defining the TilingData structure with standard C++ syntax. In the sample code above, tilingGM is the GM address of the operator TilingData passed from the host side as a kernel function parameter. Obtain the TilingData through [GET\_TILING\_DATA\_WITH\_STRUCT](../../../basic_api/Kernel-Tiling/GET_TILING_DATA_WITH_STRUCT.md). When calling the [InitV2](InitV2.md) initialization API, you need to pass in the communication context information, which can be obtained through the framework-provided API [GetHcclContext](../HCCL-Context/GetHcclContext.md) for obtaining the communication context.

2.  Set the Tiling address of the corresponding communication algorithm.

    Set the Tiling address of the corresponding communication algorithm through the [SetCcTilingV2](SetCcTilingV2.md) API. After the Commit API is called, this address is sent to the server and parsed by the server. The [SetCcTilingV2](SetCcTilingV2.md) API must be used together with the [InitV2](InitV2.md) API. The following is an example.

    ```
    // Call method that passes the initTiling address.
    GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM);

    Hccl hccl;
    GM_ADDR contextGM = GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

    hccl.InitV2(contextGM, &tilingData);
    if (SetCcTilingV2(offsetof(AllGatherCustomTilingData, mc2CcTiling)) != HCCL_SUCCESS) {
      return;
    }
    ```

3.  You asynchronously submit the corresponding type of communication task through the corresponding Prepare API and obtain the identifier handleId of the task. After the server receives it, it starts expanding and submitting the communication task. The following is an example.

    ```
    auto handleId = hccl.ReduceScatter<false>(aGM, cGM, recvCount,
                                              AscendC::HCCL_DATA_TYPE_FP16,
                                              HCCL_REDUCE_SUM, strideCount, 1);
    // For the Prepare API, add exception value validation and PRINTF printing during debugging.
    // if (handleId == INVALID_HANDLE_ID) {
    // 	PRINTF("[ERROR] call ReduceScatter failed, handleId is -1.");
    //	return;
    // }
    ```

    The Prepare API in the example is [ReduceScatter](ReduceScatter.md). For other APIs, see the content in the subsequent sections. The parameter AscendC::HCCL\_DATA\_TYPE\_FP16 is the data type of the HCCL task, whose data structure is HcclDataType. For the corresponding parameter description, see [Table 1](#table116710585514). The parameter HCCL\_REDUCE\_SUM is a Reduce operation. For the Reduce operation types supported by the AllReduce and ReduceScatter reduction operations, see [Table 2](#hcclreduceop).

    **Table 1**  HcclDataType Parameter Description

    <a name="table116710585514"></a>
    | Data Type | Description |
    | --- | --- |
    | HcclDataType | Data type of the HCCL task. HcclDataType is an enumeration type, defined as shown in the following code.<br>    HCCL_DATA_TYPE_INT8: int8 <br>    HCCL_DATA_TYPE_INT16: int16 <br>    HCCL_DATA_TYPE_INT32: int32 <br>    HCCL_DATA_TYPE_FP16: half or float16 <br>    HCCL_DATA_TYPE_FP32: float <br>    HCCL_DATA_TYPE_INT64: int64 <br>    HCCL_DATA_TYPE_UINT64: uint64 <br>    HCCL_DATA_TYPE_UINT8: uint8 <br>    HCCL_DATA_TYPE_UINT16: uint16 <br>    HCCL_DATA_TYPE_UINT32: uint32 <br>    HCCL_DATA_TYPE_FP64: float64 <br>    HCCL_DATA_TYPE_BFP16: bfloat16 <br>    HCCL_DATA_TYPE_INT128: int128, reserved type, not supported yet <br>    HCCL_DATA_TYPE_HIF8: hif8 <br>    HCCL_DATA_TYPE_FP8E4M3: fp8e4m3 <br>    HCCL_DATA_TYPE_FP8E5M2: fp8e5m2 <br>    HCCL_DATA_TYPE_FP8E8M0: fp8e8m0 <br>    HCCL_DATA_TYPE_RESERVED: not supported for use  |

    ```
    enum HcclDataType {
        HCCL_DATA_TYPE_INT8 = 0,   /* int8 */
        HCCL_DATA_TYPE_INT16 = 1,  /* int16 */
        HCCL_DATA_TYPE_INT32 = 2,  /* int32 */
        HCCL_DATA_TYPE_FP16 = 3,   /* half or float16 */
        HCCL_DATA_TYPE_FP32 = 4,   /* float */
        HCCL_DATA_TYPE_INT64 = 5,  /* int64 */
        HCCL_DATA_TYPE_UINT64 = 6, /* uint64 */
        HCCL_DATA_TYPE_UINT8 = 7,  /* uint8 */
        HCCL_DATA_TYPE_UINT16 = 8, /* uint16 */
        HCCL_DATA_TYPE_UINT32 = 9, /* uint32 */
        HCCL_DATA_TYPE_FP64 = 10,  /* float64 */
        HCCL_DATA_TYPE_BFP16 = 11, /* bfloat16 */
        HCCL_DATA_TYPE_INT128 = 12, /* int128, reserved type, not supported yet */
        HCCL_DATA_TYPE_HIF8 = 14,  /* hif8 */
        HCCL_DATA_TYPE_FP8E4M3 = 15,  /* fp8e4m3 */
        HCCL_DATA_TYPE_FP8E5M2 = 16,  /* fp8e5m2 */
        HCCL_DATA_TYPE_FP8E8M0 = 17,  /* fp8e8m0 */
        HCCL_DATA_TYPE_RESERVED    /* reserved */
    };
    ```
    <a id="hcclreduceop"></a>
    **Table 2**  HcclReduceOp Parameter Description
    | Data Type | Description |
    | --- | --- |
    | HcclReduceOp | Reduce operation type. **HcclReduceOp** is an enumeration type, defined as shown in the following code.<br> HCCL_REDUCE_SUM: sum <br>    HCCL_REDUCE_PROD: prod <br>    HCCL_REDUCE_MAX: max <br>    HCCL_REDUCE_MIN: min <br>    HCCL_REDUCE_RESERVED: Not Supported Yet |

    ```
    enum HcclReduceOp {
        HCCL_REDUCE_SUM = 0,  /* sum */
        HCCL_REDUCE_PROD = 1, /* prod */
        HCCL_REDUCE_MAX = 2,  /* max */
        HCCL_REDUCE_MIN = 3,  /* min */
        HCCL_REDUCE_RESERVED  /* reserved */
    }
    ```

4.  You call the [Commit](Commit.md) API to notify the server to execute the Communication Task corresponding to the handleId.

    ```
    // Wait until the communication task is ready for execution, and call the Commit API to notify the server to execute it.
    hccl.Commit(handleId);
    ```

5.  You call the blocking [Wait](Wait-98.md) API to wait for the server to finish executing the corresponding Communication Task.

    ```
    auto ret = hccl.Wait(handleId);
    // For the Wait and Query APIs, add exception value validation and PRINTF output during debugging.
    // if (ret == HCCL_FAILED) {
    //	PRINTF("[ERROR] call Wait for handleId[%d] failed.", handleId);
    //	return;
    // }

    // Call the inter-core synchronization API to prevent some cores from exiting early and triggering Hccl destruction, which would affect the slower cores.
    // Based on the actual business scenario, you can call [SyncAll](../../../Basic API/Synchronization Control/Inter-Core Synchronization/SyncAll.md), [CrossCoreSetFlag(ISASI)](../../../Basic API/Synchronization Control/Inter-Core Synchronization/CrossCoreSetFlag_ISASI.md), or [CrossCoreWaitFlag(ISASI)](../../../Basic API/Synchronization Control/Inter-Core Synchronization/CrossCoreWaitFlag_ISASI.md) to ensure that all cores finish their tasks before exiting execution.
    ```

6.  You call the [Finalize](Finalize.md) API to notify the server that there are no more communication tasks, and the server exits after Execution Complete. The client detects and waits for the last Communication Task to complete execution.

    ```
    hccl.Finalize();
    ```

Note: If the template parameter of the HCCL object does not specify the core that issues the communication task, the Prepare API can run only on either AIC or AIV. Before calling the APIs in steps 2 to 5, you must specify that the API code runs on the AIC or AIV core, as shown in the following code.

```
// Use the built-in constant g_coreType to determine whether the core is an AIC core or an AIV core.
if (g_coreType == AIV) {
// if (g_coreType == AIC) {
        Call the HCCL API
}
```

Based on the preceding understanding of how a single communication task is issued, this section describes the flexible use of the **repeat** parameter in the **Prepare** API. One **Prepare** API invocation corresponds to one **handleId**. The **repeat** parameter in the **Prepare** API represents the number of communication tasks in this **Prepare** invocation, and this value must be consistent with the number of **Commit** API invocations and the number of **Wait** API invocations for this **handleId**. Take [Figure 3 ReduceScatter communication example](#fig19780192164210) as an example. Assume there are 4 cards in total. The source data on each card is first evenly divided into 4 parts by **rankId**, and each part is further split into 3 parts. The number of data elements in each final split part is **TileLen**. Each **ReduceScatter** communication transmits only one group of split data (for example, data 0-0, 1-0, 2-0, and 3-0 in the figure form one group of split data). Therefore, 3 **ReduceScatter** operations are required to complete the communication of all data.

In this scenario, you can invoke the **ReduceScatter** API 3 times with the **repeat** parameter set to 1 to issue 3 communication tasks, while updating the send and receive addresses of each **ReduceScatter** task. This yields the **handleId** of the 3 tasks, and each **handleId** task invokes the **Commit** and **Wait** APIs once. The corresponding code snippet is as follows.

```
extern "C" __global__ __aicore__ void reduce_scatter_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
    auto sendBuf = xGM;  // xGM is the input GM address of ReduceScatter.
    auto recvBuf = yGM;  // yGM is the output GM address of ReduceScatter.
    constexpr size_t rankSize = 4U; // 4 cards.
    constexpr size_t tileCnt = 3U;  // The data on each card is evenly divided into rankSize parts, and each part is further split into 3 parts.
    constexpr size_t tileLen = 100U;  // Number of data elements in each split part.
    uint64_t strideCount = tileLen*tileCnt;  // Offset between the start addresses of adjacent data blocks on sendBuf.

    REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);

    Hccl hccl;
    GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
    if (AscendC::g_coreType == AIV) {  // Specify AIV core communication.
        hccl.InitV2(contextGM, &tilingData);
        auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, reduceScatterCcTiling));
        if (ret != HCCL_SUCCESS) {
          return;
        }
	// Three handleIds are generated in the for loop, and each handleId invokes Commit and Wait once with repeat=1.
        for (int i = 0; i < tileCnt; ++i) {
	    auto handleId = hccl.ReduceScatter(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP32, HcclReduceOp::HCCL_REDUCE_SUM, strideCount, 1); //See the ReduceScatter API description for details.
	    hccl.Commit(handleId);
	    auto ret = hccl.Wait(handleId);
	    // Execute other computation logic ....

	    // Update the send and receive addresses of ReduceScatter.
	    sendBuf += tileLen * sizeof(float);
	    recvBuf += tileLen * sizeof(float);
	}
        AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from running too fast and invoking hccl.Finalize() in advance, which would cause other cores to hang in Wait.
        hccl.Finalize();
    }
}
```

Since the source addresses SendBuf of the three data blocks on each card are contiguous, and the destination address recvBuf on each card used to store the three communication result data blocks is also contiguous, the preceding code can be optimized by setting the repeat parameter in the ReduceScatter API to 3. This invokes the ReduceScatter API once to achieve the effect of issuing three communication tasks. In this case, there is only one handleId task, but Commit and Wait must be invoked three times. The corresponding code snippet is as follows.

```
extern "C" __global__ __aicore__ void reduce_scatter_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
    auto sendBuf = xGM;  // xGM is the input GM address of ReduceScatter.
    auto recvBuf = yGM;  // yGM is the output GM address of ReduceScatter.
    constexpr size_t rankSize = 4U; // Four cards.
    constexpr size_t tileCnt = 3U;  // The data on each card is evenly divided into rankSize parts, and each part is further divided into 3 segments.
    constexpr size_t tileLen = 100U;  // Number of data elements in each segment after division.
    uint64_t strideCount = tileLen*tileCnt;  // Offset of the start address between adjacent data blocks on sendBuf.

    REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); //ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);

    Hccl hccl;
    GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
    if (AscendC::g_coreType == AIV) {  // Specify communication on the AIV core.
        hccl.InitV2(contextGM, &tilingData);
        auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, reduceScatterCcTiling));
        if (ret != HCCL_SUCCESS) {
          return;
        }

	auto handleId = hccl.ReduceScatter(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP32, HcclReduceOp::HCCL_REDUCE_SUM, strideCount, tileCnt); //For details about the parameters, see the ReduceScatter API description.
	for (int i = 0; i < tileCnt; ++i) {
	    hccl.Commit(handleId);
	    auto ret = hccl.Wait(handleId);
	    // Execute other computation logic ....
	}
        AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from running too fast and calling hccl.Finalize() in advance, which would cause other cores to hang in Wait.
        hccl.Finalize();
    }
}
```

**Figure 3**  ReduceScatter communication example<a name="fig19780192164210"></a>  
![](../../../../figures/reducescatter_communication_example.png "ReduceScatter Communication Example")

**Table 3**  MC2\_BUFFER\_LOCATION Parameter Description

| Data Type | Description |
| --- | --- |
| MC2_BUFFER_LOCATION | Reserved parameter. Storage location of the buffer for intermediate results of computation and communication. You can set this field on the Tiling side. |

Note: When debugging an operator that uses HCCL high-level APIs, add the compilation option -DASCENDC\_DEBUG to the operator compilation project to enable interception of abnormal scenarios. For details, see and use the [assert API](../../../basic_api/debug_interface/exception_detection/assert.md).
