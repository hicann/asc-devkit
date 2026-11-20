# AllReduce

<!-- md-trans-meta sourceCommit=ff08ce34f762ef327d03181f997e9759eb37718b translatedAt=2026-11-02T05:16:07.876Z pushedAt=2026-11-18T04:56:13.493Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/AllReduce_res.md#id1 -->

## Description

The task dispatch API of the collective communication operator **AllReduce** returns the task identifier **handleId** to the user. The **AllReduce** function performs a reduce operation on the tensors with the same name across all nodes in the communicator, and then sends the result to the output buffer of all nodes.

![](../../../../figures/allreduce.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AllReduce(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint8_t repeat = 1)
```

## Parameters

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The parameter values are as follows:<br>**true**: When the **Prepare** API is called, Commit synchronously notifies the server that it can execute the communication task.<br>**false**: When the **Prepare** API is called, the server is not notified to execute the communication task. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address. The collective communication result is output to this buffer. |
| count | Input | Number of data elements participating in the AllReduce operation. For example, if only one int32 data element participates, count=1. |
| dataType | Input | Data type of the AllReduce operation. Currently supported data types are float, half (float16), int8_t, int16_t, int32_t, and bfloat16_t, that is, the supported values are HCCL_DATA_TYPE_FP32, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_INT16, HCCL_DATA_TYPE_INT32, and HCCL_DATA_TYPE_BFP16. For details about the HcclDataType data type, see [Table 1](HCCL_usage.md#table116710585514). |
| op | Input | Reduce operation type. Currently supports sum, max, min, and prod operation types, that is, HCCL_REDUCE_SUM, HCCL_REDUCE_MAX, HCCL_REDUCE_MIN, and HCCL_REDUCE_PROD. The prod operation does not support int16_t and bfloat16_t data types. For details about the HcclReduceOp data type, see [Table 2](HCCL_usage.md#table2469980529). |
| repeat | Input | Number of AllReduce communication tasks issued at a time. The value of repeat is ≥1, and the default value is 1. When repeat>1, the sendBuf and recvBuf addresses of each AllReduce task are automatically calculated by the server using the following formulas:<br><br>sendBuf[i] = sendBuf + count* sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + count* sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat>1 is set, it must be used together with the count parameter to plan the communication data addresses. |

**Figure 1**  AllReduce three-round split communication example  
![AllReduce three-round split communication example](../../../../figures/allreduce_3split_example.png)

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. Returns -1 on failure.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core for delivering the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core for delivering the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API delivers the communication task only on either the AIC core or the AIV core based on the type of the specified core.
<!-- npu="910b" id7 -->
-   For Atlas A2 products, within a communicator, the total call count of all Prepare APIs must not exceed 63.
<!-- end id7 -->
<!-- npu="A3" id8 -->
-   For Atlas A3 products, within a communicator, the total call count of all Prepare APIs and InterHcclGroupSync APIs must not exceed 63.
<!-- end id8 -->
<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, within a communicator, the total call count of all Prepare APIs must not exceed 63.
-   For Ascend 950PR&950DT products, when the communication server is CCU, a maximum of 8 cards in full connection are supported, only the FullMesh algorithm is supported by default, and the maximum communication data volume in a single operation must not exceed 256 MB.
<!-- end id9 -->

## Examples

-   Non-multi-round split scenario

    As shown in the following figure, each of the four cards has count=300 float16 data elements. Each card obtains its own data from the xGM memory, performs reduce sum on the data from all cards, and outputs the result to the yGM of each card.

    **Figure 2**  AllReduce communication across four cards in the non-multi-round split scenario

    ![](../../../../figures/250902140829537_gai.png)

    ```
    extern "C" __global__ __aicore__ void all_reduce_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        auto sendBuf = xGM;       // xGM is the input GM address of AllReduce.
        auto recvBuf = yGM;       // yGM is the output GM address of AllReduce.
        uint64_t sendCount = 300; // Each card has 300 float16 data elements.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        REGISTER_TILING_DEFAULT(AllReduceCustomTilingData); // AllReduceCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllReduceCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllReduceCustomTilingData, mc2CcTiling));
            if (ret) {
                return;
            }
            HcclHandle handleId1 =
                hccl.AllReduce<true>(sendBuf, recvBuf, sendCount, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from running too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang in Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round split scenario

    When multi-round split is enabled, the communication in the non-multi-round split example above is processed equivalently. As shown in the following figure, the 300 float16 data elements on each card are split into two head blocks and one tail block. The data size of each head block, **tileLen**, is 128 float16 data elements, and the data size of the tail block, **tailLen**, is 44 float16 data elements. During the internal implementation of the operator, the split data must be processed in three rounds of AllReduce communication tasks, which yields the same communication result as the non-multi-round split described above.

    **Figure 3**  Data splitting diagram for each card  
    ![Data splitting diagram for each card](../../../../figures/per_rank_data_split.png)

    The specific implementation is as follows: in round 1, the 0-0\\1-0\\2-0\\3-0 data block on each rank is processed by AllReduce. In round 2, the 0-1\\1-1\\2-1\\3-1 data block on each rank is processed by AllReduce. In round 3, the 0-2\\1-2\\2-2\\3-2 data block on each rank is processed by AllReduce. The diagram and code example are as follows.

    **Figure 4**  AllReduce diagram for four cards  
    ![AllReduce diagram for four cards](../../../../figures/allreduce_4rank_diagram.png)

    ```
    extern "C" __global__ __aicore__ void all_reduce_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t tileNum = 2U;   // Number of head blocks.
        constexpr uint64_t tileLen = 128U; // Number of data elements in the head block.
        constexpr uint32_t tailNum = 1U;   // Number of tail blocks.
        constexpr uint64_t tailLen = 44U;  // Number of data elements in the tail block.
        auto sendBuf = xGM;                // xGM is the input GM address of AllReduce.
        auto recvBuf = yGM;                // yGM is the output GM address of AllReduce.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        REGISTER_TILING_DEFAULT(AllReduceCustomTilingData); // AllReduceCustomTilingData is the struct defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllReduceCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // Obtain the HCCL context in this way in the AscendC custom operator kernel function (Kernel).
        if (AscendC::g_coreType == AIV) {                 // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllReduceCustomTilingData, mc2CcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            // Process the two head blocks.
            constexpr uint32_t tileRepeat = tileNum;
            // Except for the sendBuf and recvBuf input parameters, the remaining parameters for processing the two head blocks are the same. Therefore, repeat=2 is used, and the sendBuf and recvBuf of the second first-block AllReduce task are updated internally by the API.
            HcclHandle handleId1 =
                hccl.AllReduce<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, tileRepeat);
            // Process the one tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            constexpr uint32_t tailRepeat = tailNum;
            HcclHandle handleId2 =
                hccl.AllReduce<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, tailRepeat);

            for (uint8_t i = 0; i < tileRepeat; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>(); // Synchronize all AI Vector cores to prevent core 0 from running too fast and calling hccl.Finalize() in advance, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
