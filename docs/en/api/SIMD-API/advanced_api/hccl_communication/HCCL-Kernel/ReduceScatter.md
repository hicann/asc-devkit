# ReduceScatter

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:27:42.251Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- Atlas inference products AI Core: Not supported

- Atlas inference products Vector Core: Not supported

- Atlas training products: Not supported

## Description

This is the task dispatch API of the collective communication operator **ReduceScatter**, which returns the identifier **handleId** of the task to the user. **ReduceScatter** sums the inputs of all ranks (or performs another reduce operation), and then evenly distributes the result to the output buffer of each rank according to the rank number. Each process obtains 1/ranksize of the data from other processes to perform the reduce operation.

![](../../../../figures/allgather-57.png)

## Function Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle ReduceScatter(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t recvCount, HcclDataType dataType, HcclReduceOp op, uint64_t strideCount, uint8_t repeat = 1)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value can be:<br>true: When the Prepare API is called, Commit synchronously notifies the server that this communication task can be executed.<br>false: When the Prepare API is called, the server is not notified to execute this communication task. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address. The collective communication result is output to this buffer. |
| recvCount | Input | Number of data elements in recvBuf that participate in the ReduceScatter operation. The number of data elements in sendBuf equals recvCount * rank size. |
| dataType | Input | Data type of the ReduceScatter operation. Currently, float, half, int8_t, int16_t, int32_t, and bfloat16_t are supported, that is, the supported values are HCCL_DATA_TYPE_FP32, HCCL_DATA_TYPE_FP16, HCCL_DATA_TYPE_INT8, HCCL_DATA_TYPE_INT16, HCCL_DATA_TYPE_INT32, and HCCL_DATA_TYPE_BFP16. For details about the HcclDataType data type, see [Table 1](hccl_usage.md#table116710585514). |
| op | Input | Operation type of ReduceScatter. Currently, sum, max, and min are supported, that is, the supported values are HCCL_REDUCE_SUM, HCCL_REDUCE_MAX, and HCCL_REDUCE_MIN. For details about the HcclReduceOp data type, see [Table 2](hccl_usage.md#hcclreduceop). |
| strideCount | Input | When the data in sendBuf on one card is scattered to recvBuf on multiple cards, the strideCount parameter is used to indicate the offset of the start address between adjacent data blocks in sendBuf.<br>strideCount=0 indicates that when the current card sends data to other cards, adjacent data blocks remain contiguous in address. The current card sends data to card rank[i], and the offset of the current card's data block in sendBuf is i\*recvCount. In the non-multi-round slicing scenario, it is recommended that you set this parameter to 0.<br>strideCount>0 indicates that when the current card sends data to other cards, the offset of the start address between adjacent data blocks in sendBuf is strideCount data elements. The current card sends data to card rank[i], and the offset of the current card's data block in SendBuf is i\*strideCount.<br><br>Note: The offset above is the number of data elements, and the unit is sizeof(dataType). |
| repeat | Input | Number of ReduceScatter communication tasks dispatched at a time. The value of repeat must be greater than or equal to 1, and the default value is 1. When repeat>1, the sendBuf and recvBuf addresses of each ReduceScatter task are automatically calculated by the server using the following formulas:<br><br>sendBuf[i] = sendBuf + recvCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + recvCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat>1 is set, it must be used together with the strideCount parameter to plan the communication data addresses. |

**Figure 1**  ReduceScatter communication example  
![ReduceScatter communication example-58](../../../../figures/reducescatter_communication_example_58.png)

Taking the preceding figure as an example, assume a scenario with four cards, where each portion of data is tiled into three blocks (TileCnt is 3). The 0-0, 0-1, and 0-2 data on each card are ultimately reduced and scattered to the recvBuf of card rank0, and the remaining 1-y, 2-y, and 3-y blocks are similar, ultimately reduced and scattered to the recvBuf of cards rank1, rank2, and rank3, respectively. Therefore, the ReduceScatter API must be called three times for the data on one card to complete the communication of the three tiled blocks of each portion of data. For each portion of data, the recvCount parameter in this API is TileLen, and strideCount is TileLen\*TileCnt (that is, the number of data elements between data block 0-0 and 1-0). Because this example uses a memory-contiguous scenario, the ReduceScatter API can also be called only once with the repeat parameter set to 3.

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.

-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core that issues the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core that issues the communication task, this API can be called on both the AIC core and the AIV core at the same time. The API internally issues the communication task on only one of the AIC core or the AIV core based on the specified core type.

-   For Atlas A2 training series products/Atlas A2 inference series products, within a communication domain, the total number of calls to all Prepare APIs cannot exceed 63.

-   For Atlas A3 training series products/Atlas A3 inference series products, within a communication domain, the total number of calls to all Prepare APIs and the InterHcclGroupSync API cannot exceed 63.

-   For Ascend 950PR/Ascend 950DT, within a communication domain, the total number of calls to all Prepare APIs cannot exceed 63.

-   For Ascend 950PR/Ascend 950DT, when the communication server is the CCU, the maximum amount of communication data in a single operation cannot exceed 256 MB.

## Examples

-   Non-multi-round slicing scenario

    As shown in the following figure, each of the four cards has 300 \* 4=1200 float16 data elements. Each card obtains its own data from the xGM memory, performs reduce sum computation on the data of each card, and then scatters the result data. Finally, each card obtains 300 float16 data elements after reduce sum.

    **Figure 2**  ReduceScatter communication across four cards in the non-multi-round slicing scenario

    ![](../../../../figures/ReduceScatter_gai.png)

    ```
    extern "C" __global__ __aicore__ void reduce_scatter_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        auto sendBuf = xGM;  // xGM is the input GM address of ReduceScatter.
        auto recvBuf = yGM;  // yGM is the output GM address of ReduceScatter.
        uint64_t recvCount = 300;  // Number of communication result data elements on each card.
        uint64_t strideCount = 0;  // In the non-slicing scenario, strideCount can be set to 0.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
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
            HcclHandle handleId1 = hccl.ReduceScatter<true>(sendBuf, recvBuf, recvCount, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, strideCount);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() in advance, which would cause other cores to hang while waiting.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round slicing scenario

> When multi-round slicing is enabled, the communication in the preceding non-multi-round slicing example is processed equivalently. As shown in the following figure, each 300-float16 data portion on each card is sliced into two head blocks and one tail block. The data volume of each head block, tileLen, is 128 float16 data elements, and the data volume of the tail block, tailLen, is 44 float16 data elements. During the internal implementation of the operator, the sliced data must be processed in three rounds of ReduceScatter communication tasks, producing a communication result equivalent to that of the non-multi-round slicing described above.

> **Figure 3**  Data slicing diagram of each card  
> ![Data slicing diagram of each card-59](../../../../figures/data_slicing_diagram_of_each_card_59.png)

> The specific implementation is as follows: in the first round of communication, the 0-0\1-0\2-0\3-0 data blocks on each rank are processed by ReduceScatter. In the second round of communication, the 0-1\1-1\2-1\3-1 data blocks on each rank are processed by ReduceScatter. In the third round of communication, the 0-2\1-2\2-2\3-2 data blocks on each rank are processed by ReduceScatter. In the input data of each round of communication, the number of data elements between the start addresses of adjacent data blocks on each card is strideCount. Taking the first round of communication as an example, for the 0-0 data block and the 1-0 data block, or the 1-0 data block and the 2-0 data block on rank0, the data volume between the start addresses of two adjacent data blocks is strideCount = 2\*tileLen+1\*tailLen=300.

> **Figure 4**  Schematic diagram of the first round of ReduceScatter across four cards  
> ![Schematic diagram of the first round of ReduceScatter across four cards](../../../../figures/schematic_diagram_of_the_first_round_of_ReduceScatter_across_four_cards.png)

``` 
extern "C" __global__ __aicore__ void reduce_scatter_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
    constexpr uint32_t tileNum = 2U;   // Number of head blocks.
    constexpr uint64_t tileLen = 128U; // Number of data elements in a head block.
    constexpr uint32_t tailNum = 1U;   // Number of tail blocks.
    constexpr uint64_t tailLen = 44U;  // Number of data elements in the tail block.
    auto sendBuf = xGM;  // xGM is the input GM address of ReduceScatter.
    auto recvBuf = yGM;  // yGM is the output GM address of ReduceScatter.
    HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
    uint64_t strideCount = tileLen * tileNum + tailLen * tailNum;
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
        // Process two head blocks.
        constexpr uint32_t tileRepeat = tileNum;
        // Except for the different sendBuf and recvBuf input parameters, the remaining parameters for processing the two head blocks are the same. Therefore, repeat=2 is used, and the sendBuf and recvBuf of the second head block ReduceScatter task are updated internally by the API.
        HcclHandle handleId1 = hccl.ReduceScatter<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, strideCount, tileRepeat);
        // Process one tail block.
        constexpr uint32_t kSizeOfFloat16 = 2U;
        sendBuf += tileLen * tileNum * kSizeOfFloat16;
        recvBuf += tileLen * tileNum * kSizeOfFloat16;
        constexpr uint32_t tailRepeat = tailNum;
        HcclHandle handleId2 = hccl.ReduceScatter<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, strideCount, tailRepeat);

        for (uint8_t i=0; i<tileRepeat; i++) {
            hccl.Wait(handleId1);
        }
        hccl.Wait(handleId2);
        AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from running too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang while waiting.
        hccl.Finalize();
    }
}
```