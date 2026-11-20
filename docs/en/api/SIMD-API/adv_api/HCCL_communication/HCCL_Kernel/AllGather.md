# AllGather

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:15:43.628Z pushedAt=2026-11-18T04:56:10.104Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/AllGather_res.md#id1 -->

## Description

**AllGather** is a task dispatch API of the collective communication operator that returns the task identifier **handleId** to the user. The function of **AllGather** is to reorder the inputs of all nodes within the communicator by rank id, concatenate them, and then send the result to the outputs of all nodes.

![](../../../../figures/allgather.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AllGather(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat = 1)
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
| sendCount | Input | Number of data elements in **sendBuf** that participate in the AllGather operation. The number of data elements in **recvBuf** equals sendCount * rank size, that is, sendCount * the number of cards. |
| dataType | Input | Data type of the AllGather operation. Currently, all data types included in **HcclDataType** are supported. For details about **HcclDataType**, see [Table 1](HCCL_usage.md#table116710585514). |
| strideCount | Input | strideCount=0: When data from multiple cards is concatenated into the **recvBuf** of one card, adjacent data blocks remain address-contiguous. The data block of card rank[i] is placed in **recvBuf** with a data offset of i\*sendCount. In non-multi-round split scenarios, it is recommended that users set this parameter to 0. <br>strideCount>0: When data from multiple cards is concatenated into the **recvBuf** of one card, the offset between the start address of adjacent data blocks in **recvBuf** is strideCount. The data block of card rank[i] is placed in **recvBuf** with a data offset of i\*strideCount. <br><br>Note: The data offset mentioned above is the number of data elements, with the unit being sizeof(dataType). |
| repeat | Input | Number of AllGather communication tasks delivered at one time. repeat must be ≥ 1, and the default value is 1. When repeat>1, the **sendBuf** and **recvBuf** addresses of each AllGather task are automatically calculated by the server using the following formulas: <br><br>sendBuf[i] = sendBuf + sendCount* sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + sendCount* sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat>1 is set, this parameter must be used together with the **strideCount** parameter to plan the communication data addresses. |

**Figure 1** AllGather communication example  
![AllGather communication example](../../../../figures/allgather_example.png)

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
-   For Ascend 950PR&950DT products, when the communication server is the CCU, only the FullMesh algorithm is supported by default, and the maximum communication data volume per operation cannot exceed 256M.
<!-- end id9 -->

## Examples

-   Non-multi-round split scenario

    As shown in the following figure, each of the four cards has sendCount=300 float16 data elements. Each card obtains its own data from the xGM memory. After gather processes the data of each card, the result is output to the yGM of each card.

    **Figure 2**  AllGather communication across four cards in the non-multi-round split scenario  
    ![AllGather communication across four cards in the non-multi-round split scenario](../../../../figures/allgather_4rank_no_multisplit.png)

    ```
    extern "C" __global__ __aicore__ void all_gather_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        auto sendBuf = xGM;                                 // xGM is the input GM address of AllGather.
        auto recvBuf = yGM;                                 // yGM is the output GM address of AllGather.
        uint64_t sendCount = 300;                           // Each card has 300 float16 data elements.
        uint64_t strideCount = 0;                           // In non-split scenarios, strideCount can be set to 0.
        REGISTER_TILING_DEFAULT(AllGatherCustomTilingData); // AllGatherCustomTilingData is the struct defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllGatherCustomTilingData, allGatherCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            HcclHandle handleId1 =
                hccl.AllGather<true>(sendBuf, recvBuf, sendCount, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round split scenario

    With multi-round split enabled, the communication in the non-multi-round split example above is processed equivalently. As shown in the following figure, the 300 float16 data elements on each card are split into 2 head blocks and 1 tail block. The data size of each head block, **tileLen**, is 128 float16 data elements, and the data size of the tail block, **tailLen**, is 44 float16 data elements. During the internal implementation of the operator, the split data needs to be processed in 3 rounds of AllGather communication tasks, producing a result equivalent to that of the non-multi-round split example above.

    **Figure 3**  Data split diagram of each card  
    ![Data split diagram of each card-56](../../../../figures/per_rank_data_split_56.png)

    The specific implementation is as follows: in round 1, the 0-0\\1-0\\2-0\\3-0 data blocks on each rank are processed by AllGather. In round 2, the 0-1\\1-1\\2-1\\3-1 data blocks on each rank are processed by AllGather. In round 3, the 0-2\\1-2\\2-2\\3-2 data blocks on each rank are processed by AllGather. In the result of each round of communication, the number of data elements between the start addresses of adjacent data blocks on each card is **strideCount**. Taking the result of round 1 as an example, the number of data elements between the start addresses of the 0-0 data block and the 1-0 data block on rank0 is **strideCount** = 2\*tileLen+1\*tailLen=300.

    **Figure 4**  Diagram of round 1 4-card AllGather  
    ![Diagram of round 1 4-card AllGather](../../../../figures/first_round_4rank_allgather.png)

    ```
    extern "C" __global__ __aicore__ void all_gather_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t tileNum = 2U;                    // Number of head blocks.
        constexpr uint64_t tileLen = 128U;                  // Number of data elements in a head block.
        constexpr uint32_t tailNum = 1U;                    // Number of tail blocks.
        constexpr uint64_t tailLen = 44U;                   // Number of data elements in a tail block.
        auto sendBuf = xGM;                                 // xGM is the input GM address of AllGather.
        auto recvBuf = yGM;                                 // yGM is the output GM address of AllGather.
        REGISTER_TILING_DEFAULT(AllGatherCustomTilingData); // AllGatherCustomTilingData is the struct defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel function, obtain the HCCL context in this way.
        if (AscendC::g_coreType == AIV) {                 // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllGatherCustomTilingData, allGatherCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            uint64_t strideCount = tileLen * tileNum + tailLen * tailNum;
            // Process the two head blocks.
            constexpr uint32_t tileRepeat = tileNum;
            // Except for the sendBuf and recvBuf input parameters, the remaining parameters for processing the two head blocks are the same. Therefore, repeat=2 is used, and the sendBuf and recvBuf of the AllGather task for the second head-block are updated internally by the API.
            HcclHandle handleId1 =
                hccl.AllGather<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tileRepeat);
            // Process one tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            constexpr uint32_t tailRepeat = tailNum;
            HcclHandle handleId2 =
                hccl.AllGather<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tailRepeat);

            for (uint8_t i = 0; i < tileRepeat; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() early, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
