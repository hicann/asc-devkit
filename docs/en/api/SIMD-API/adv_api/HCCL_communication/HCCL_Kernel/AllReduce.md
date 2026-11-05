# AllReduce

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:30:12.646Z pushedAt=2026-10-27T11:41:46.958Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
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
| commit | Input | bool type. The value can be: <br>**true**: When the Prepare API is called, Commit synchronously notifies the server that it can execute this communication task.<br>**false**: When the Prepare API is called, the server is not notified to execute this communication task. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address, to which the collective communication result is output. |
| count | Input | Number of data elements participating in the AllReduce operation. For example, if only one int32 data element participates, count=1. |
| dataType | Input | Data type of the AllReduce operation. Currently supported data types are float, half (float16), int8_t, int16_t, int32_t, and bfloat16_t, that is, the supported values are HCCL_DATA_TYPE_FP32, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_INT16, HCCL_DATA_TYPE_INT32, and HCCL_DATA_TYPE_BFP16. For details about the HcclDataType data type, see [Table 1](HCCL_usage.md#table116710585514). |
| op | Input | Reduce operation type. Currently supported operation types are sum, max, and min, that is, the supported values are HCCL_REDUCE_SUM, HCCL_REDUCE_MAX, and HCCL_REDUCE_MIN. For details about the HcclReduceOp data type, see [Table 2](HCCL_usage.md#table2469980529). |
| repeat | Input | Number of AllReduce communication tasks issued at a time. The value of repeat is ≥1, and the default value is 1. When repeat>1, the sendBuf and recvBuf addresses of each AllReduce task are automatically calculated by the server using the following formulas:<br><br>sendBuf[i] = sendBuf + count* sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + count* sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat>1 is set, it must be used together with the count parameter to plan the communication data addresses. |

**Figure 1**  AllReduce three-round split communication example  
![AllReduce three-round split communication example](../../../../figures/allreduce_3split_example.png)

## Return Value

Returns the handleId of the task. The handleId is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core for issuing the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core for issuing the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, based on the specified core type, the API issues the communication task on only one of the AIC core and the AIV core.
<!-- npu="910b" id7 -->
-   For Atlas A2 products, the total call count of all Prepare APIs within a communicator cannot exceed 63.
<!-- end id7 -->
<!-- npu="A3" id8 -->
-   For Atlas A3 products, the total call count of all Prepare APIs and the InterHcclGroupSync API within a communicator cannot exceed 63.
<!-- end id8 -->
<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the total call count of all Prepare APIs within a communicator cannot exceed 63.
-   For Ascend 950PR&950DT products, when the communication server is the CCU, a maximum of 8 cards in full connection is supported, only the FullMesh algorithm is supported by default, and the maximum data volume per communication cannot exceed 256 MB.
<!-- end id9 -->

## Examples

-   Non-multi-round splitting scenario

    As shown in the following figure, each of the four cards has count=300 float16 data elements. Each card obtains its own data from the xGM memory, performs reduce sum on the data of each card, and outputs the result to the yGM of each card.

    **Figure 2**  AllReduce communication across four cards in the non-multi-round splitting scenario

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
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllReduceCustomTilingData, mc2CcTiling));
            if (ret) {
                return;
            }
            HcclHandle handleId1 =
                hccl.AllReduce<true>(sendBuf, recvBuf, sendCount, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang at Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round splitting scenario

    When multi-round splitting is enabled, the communication in the non-multi-round splitting example above is processed equivalently. As shown in the following figure, the 300 float16 data elements on each card are split into two head blocks and one tail block. The data size of each head block, **tileLen**, is 128 float16 data elements, and the data size of the tail block, **tailLen**, is 44 float16 data elements. During the internal implementation of the operator, the split data must be processed in three rounds of AllReduce communication tasks, which yields the same communication result as the non-multi-round splitting described above.

    **Figure 3**  Data splitting diagram for each card  
    ![Data splitting diagram for each card](../../../../figures/per_rank_data_split.png)

    The specific implementation is as follows: in round 1, the 0-0\\1-0\\2-0\\3-0 data block on each rank is processed by AllReduce. In round 2, the 0-1\\1-1\\2-1\\3-1 data block on each rank is processed by AllReduce. In round 3, the 0-2\\1-2\\2-2\\3-2 data block on each rank is processed by AllReduce. The diagram and code example are as follows.

    **Figure 4**  AllReduce diagram for four cards  
    ![AllReduce diagram for four cards](../../../../figures/allreduce_4rank_diagram.png)

    ```
    extern "C" __global__ __aicore__ void all_reduce_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t tileNum = 2U;   // Number of head blocks.
        constexpr uint64_t tileLen = 128U; // Number of data elements in a head block.
        constexpr uint32_t tailNum = 1U;   // Number of tail blocks.
        constexpr uint64_t tailLen = 44U;  // Number of data elements in a tail block.
        auto sendBuf = xGM;                // xGM is the input GM address of AllReduce.
        auto recvBuf = yGM;                // yGM is the output GM address of AllReduce.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        REGISTER_TILING_DEFAULT(AllReduceCustomTilingData); // AllReduceCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllReduceCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.
        if (AscendC::g_coreType == AIV) {                 // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllReduceCustomTilingData, mc2CcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            // Process two head blocks.
            constexpr uint32_t tileRepeat = tileNum;
            // Except for the different sendBuf and recvBuf input parameters, the other parameters for processing the two head blocks are the same. Therefore, repeat=2 is used, and the sendBuf and recvBuf of the second head-block AllReduce task are updated internally by the API.
            HcclHandle handleId1 =
                hccl.AllReduce<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, tileRepeat);
            // Process one tail block.
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
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API in advance, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
