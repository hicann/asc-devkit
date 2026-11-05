# ReduceScatter

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:56:25.463Z pushedAt=2026-10-27T11:46:18.028Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/ReduceScatter_res.md#id1 -->

## Description

**ReduceScatter** is the task dispatch API of the collective communication operator, which returns the task identifier **handleId** to the user. The function of **ReduceScatter** is to add the inputs of all ranks (or perform another reduction operation), and then evenly distribute the result to the output buffer of each rank according to the rank number. Each process obtains 1/ranksize of the data from other processes to perform the reduction operation.

![](../../../../figures/ReduceScatter.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle ReduceScatter(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op, uint64_t strideCount, uint8_t repeat = 1)
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
| recvCount | Input | Number of data elements in **recvBuf** that participate in the ReduceScatter operation. The number of data elements in **sendBuf** equals recvCount * rank size. |
| dataType | Input | Data type of the ReduceScatter operation. Currently supports float, half, int8_t, int16_t, int32_t, and bfloat16_t data types, that is, the supported values are HCCL_DATA_TYPE_FP32, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_INT16, HCCL_DATA_TYPE_INT32, and HCCL_DATA_TYPE_BFP16. For details about the HcclDataType data type, see [Table 1](HCCL_usage.md#table116710585514). |
| op | Input | Operation type of ReduceScatter. Currently supports sum, max, and min operation types, that is, the supported values are HCCL_REDUCE_SUM, HCCL_REDUCE_MAX, and HCCL_REDUCE_MIN. For details about the HcclReduceOp data type, see [Table 2](HCCL_usage.md#table2469980529). |
| strideCount | Input | When the data in **sendBuf** on one card is scattered to **recvBuf** on multiple cards, the **strideCount** parameter is used to indicate the offset of the start address between adjacent data blocks in **sendBuf**.<br>strideCount=0 indicates that when the current card sends data to other cards, adjacent data blocks remain address-contiguous. The current card sends data to card rank[i], and the offset of the current card's data block in **sendBuf** is i\*recvCount. In the non-multi-round split scenario, it is recommended that users set this parameter to 0.<br>strideCount>0 indicates that when the current card sends data to other cards, the offset of the start address between adjacent data blocks in **sendBuf** is strideCount data elements. The current card sends data to card rank[i], and the offset of the current card's data block in **SendBuf** is i\*strideCount.<br><br>Note: The offset mentioned above is the number of data elements, and the unit is sizeof(dataType). |
| repeat | Input | Number of ReduceScatter communication tasks delivered at a time. The value of repeat is ≥1, and the default value is 1. When repeat>1, the **sendBuf** and **recvBuf** addresses of each ReduceScatter task are automatically calculated by the server using the following formulas:<br><br>sendBuf[i] = sendBuf + recvCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + recvCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat>1 is set, it must be used together with the **strideCount** parameter to plan the communication data addresses. |

**Figure 1** ReduceScatter communication example  
![ReduceScatter communication example-58](../../../../figures/reducescatter_example_58.png)

Using the figure above as an example, assume a scenario with 4 cards, where each portion of data is split into 3 blocks (TileCnt is 3). The 0-0, 0-1, and 0-2 data on each card are finally reduced and scattered to the **recvBuf** of card rank0. The remaining blocks 1-y, 2-y, and 3-y are similar, and are finally reduced and scattered to the **recvBuf** of cards rank1, rank2, and rank3, respectively. Therefore, the ReduceScatter API must be called 3 times for the data on one card to complete the communication of the 3 split blocks of each portion of data. For each portion of data, the **recvCount** parameter in this API is TileLen, and **strideCount** is TileLen\*TileCnt (that is, the number of data elements between data blocks 0-0 and 1-0). Since this example is a memory-contiguous scenario, the ReduceScatter API can also be called only once with the **repeat** parameter set to 3.

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. Returns -1 on failure.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core for delivering the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core for delivering the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API delivers the communication task only on either the AIC core or the AIV core based on the type of the specified core.
<!-- npu="910b" id7 -->
-   For Atlas A2 products, within a communication domain, the total call count of all Prepare APIs must not exceed 63.
<!-- end id7 -->
<!-- npu="A3" id8 -->
-   For Atlas A3 products, within a communication domain, the total call count of all Prepare APIs and InterHcclGroupSync APIs must not exceed 63.
<!-- end id8 -->
<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, within a communication domain, the total call count of all Prepare APIs must not exceed 63.
-   For Ascend 950PR&950DT products, when the communication server is CCU, a maximum of 8 cards in full connection is supported, only the FullMesh algorithm is supported by default, and the maximum communication data volume in a single operation must not exceed 256 MB.
<!-- end id9 -->

## Examples

-   Non-multi-round split scenario

    As shown in the following figure, each of the 4 cards has 300 \* 4 = 1200 float16 data elements. Each card obtains its own data from the xGM memory, performs reduce sum on the data of all cards to obtain the result data, and then performs scatter processing on it. Finally, each card obtains 300 float16 data elements after reduce sum.

    **Figure 2** ReduceScatter communication across 4 cards in the non-multi-round split scenario

    ![](../../../../figures/ReduceScatter_gai.png)

    ```
    extern "C" __global__ __aicore__ void reduce_scatter_custom(
        GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        auto sendBuf = xGM;       // xGM is the input GM address of ReduceScatter.
        auto recvBuf = yGM;       // yGM is the output GM address of ReduceScatter.
        uint64_t recvCount = 300; // Number of communication result data elements per card.
        uint64_t strideCount = 0; // In the non-split scenario, strideCount can be set to 0.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.
        if (AscendC::g_coreType == AIV) {                 // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, reduceScatterCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            HcclHandle handleId1 = hccl.ReduceScatter<true>(
                sendBuf, recvBuf, recvCount, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, strideCount);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API in advance, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round split scenario

    Enabling multi-round split equivalently processes the communication in the non-multi-round split example above. As shown in the following figure, the 300 float16 data elements on each card are split into 2 head blocks and 1 tail block. The data volume of each head block, tileLen, is 128 float16 data elements, and the data volume of the tail block, tailLen, is 44 float16 data elements. During internal operator implementation, the split data must be processed in 3 rounds of ReduceScatter communication tasks, which yields the same communication result as the non-multi-round switch above.

    **Figure 3**  Data split diagram of each card  
    ![Data split diagram of each card-59](../../../../figures/per_rank_data_split_59.png)

    The specific implementation is as follows: in round 1 communication, the 0-0\\1-0\\2-0\\3-0 data blocks on each rank undergo ReduceScatter processing. In round 2 communication, the 0-1\\1-1\\2-1\\3-1 data blocks on each rank undergo ReduceScatter processing. In round 3 communication, the 0-2\\1-2\\2-2\\3-2 data blocks on each rank undergo ReduceScatter processing. In the input data of each round of communication, the number of data elements between the start addresses of adjacent data blocks on each card is strideCount. Taking the round 1 communication result as an example, for the 0-0 data block and 1-0 data block of rank0, or the 1-0 data block and 2-0 data block, the data volume between the start addresses of two adjacent data blocks is strideCount = 2\*tileLen+1\*tailLen=300.

    **Figure 4**  First-round 4-card ReduceScatter diagram  
    ![First-round 4-card ReduceScatter diagram](../../../../figures/first_round_4rank_reducescatter.png)

    ```
    extern "C" __global__ __aicore__ void reduce_scatter_custom(
        GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t tileNum = 2U;   // Number of head blocks.
        constexpr uint64_t tileLen = 128U; // Number of data elements per head block.
        constexpr uint32_t tailNum = 1U;   // Number of tail blocks.
        constexpr uint64_t tailLen = 44U;  // Number of data elements in the tail block.
        auto sendBuf = xGM;                // xGM is the input GM address of ReduceScatter.
        auto recvBuf = yGM;                // yGM is the output GM address of ReduceScatter.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        uint64_t strideCount = tileLen * tileNum + tailLen * tailNum;
        REGISTER_TILING_DEFAULT(ReduceScatterCustomTilingData); // ReduceScatterCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(ReduceScatterCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.
        if (AscendC::g_coreType == AIV) {                 // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(ReduceScatterCustomTilingData, reduceScatterCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            // Process two head blocks.
            constexpr uint32_t tileRepeat = tileNum;
            // Except for the different sendBuf and recvBuf input parameters, the remaining parameters for processing the two head blocks are the same. Therefore, use repeat=2, and the sendBuf and recvBuf of the second head-block ReduceScatter task will be updated internally by the API.
            HcclHandle handleId1 = hccl.ReduceScatter<true>(
                sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, strideCount, tileRepeat);
            // Process one tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            constexpr uint32_t tailRepeat = tailNum;
            HcclHandle handleId2 = hccl.ReduceScatter<true>(
                sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, strideCount, tailRepeat);

            for (uint8_t i = 0; i < tileRepeat; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API in advance, causing other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
