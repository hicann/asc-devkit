# AllGather

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:13:43.473Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference product AI Core: Not supported
- Atlas inference product Vector Core: Not supported
- Atlas training product: Not supported

## Description

This is the task delivery API of the collective communication operator **AllGather**, which returns the identifier **handleId** of the task to you. The function of **AllGather** is to reorder the inputs of all nodes in the communication domain by rank id, concatenate them, and then send the result to the outputs of all nodes.

![](../../../../figures/allgather.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AllGather(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t sendCount, HcclDataType dataType, uint64_t strideCount, uint8_t repeat = 1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value is as follows:<br>**true**: When the **Prepare** API is called, Commit synchronously notifies the server that the communication task can be executed.<br>**false**: When the **Prepare** API is called, the server is not notified to execute the communication task. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address. The collective communication result is output to this buffer. |
| sendCount | Input | Number of data elements in **sendBuf** that participate in the **AllGather** operation. The number of data elements in **recvBuf** equals **sendCount** * rank size, that is, **sendCount** * number of cards. |
| dataType | Input | Data type of the **AllGather** operation. Currently, all data types contained in **HcclDataType** are supported. For details about **HcclDataType**, see [Table 1](hccl_usage.md#table116710585514). |
| strideCount | Input | strideCount=0: When data on multiple cards is concatenated into the **recvBuf** of one card, adjacent data blocks remain address-contiguous. The data block of card rank[i] is placed in **recvBuf** with an offset data amount of i\***sendCount**. In the non-multi-round slicing scenario, it is recommended that you set this parameter to 0.<br>**strideCount** > 0: When data on multiple cards is concatenated into the **recvBuf** of one card, the offset data amount of the start address of adjacent data blocks in **recvBuf** is **strideCount**. The data block of card rank[i] is placed in **recvBuf** with an offset data amount of i\***strideCount**.<br><br>Note: The offset data amount mentioned above is the number of data elements, in units of **sizeof(dataType)**. |
| repeat | Input | Number of **AllGather** communication tasks delivered at a time. The value of repeat is ≥ 1, and the default value is 1. When repeat > 1, the **sendBuf** and **recvBuf** addresses of each **AllGather** task are automatically calculated by the server using the following formulas:<br><br>sendBuf[i] = sendBuf + sendCount* sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + sendCount* sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat > 1 is set, it must be used together with the **strideCount** parameter to plan the communication data addresses. |

**Figure 1** AllGather communication example  
![AllGather communication example](../../../../figures/allgather_communication_example.png)

## Return Value

Returns the identifier **handleId** of the task. **handleId** is greater than or equal to 0. Returns -1 if the call fails.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core that dispatches the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core that dispatches the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API dispatches the communication task on only one of the AIC core and the AIV core based on the type of the specified core.
-   For Atlas A2 training products/Atlas A2 inference products, the total number of calls to all Prepare APIs within a communicator cannot exceed 63.
-   For Atlas A3 training products/Atlas A3 inference products, the total number of calls to all Prepare APIs and the InterHcclGroupSync API within a communicator cannot exceed 63.
-   For Ascend 950PR/Ascend 950DT, the total number of calls to all Prepare APIs within a communicator cannot exceed 63.
-   For Ascend 950PR/Ascend 950DT, when the communication server is the CCU, the maximum amount of communication data in a single operation cannot exceed 256M.

## Examples

-   Non-multi-round slicing scenario

    As shown in the following figure, each of the 4 cards has sendCount = 300 float16 data elements. Each card obtains its own data from the xGM memory, and after gathering the data on each card, the result is output to the yGM of each card.

    **Figure 2**  AllGather communication across 4 cards in the non-multi-round slicing scenario  
    ![AllGather communication across 4 cards in the non-multi-round slicing scenario](../../../../figures/allgather_communication_across_4_cards_in_non_multi_round_slicing_scenario.png)

    ```
    extern "C" __global__ __aicore__ void all_gather_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        auto sendBuf = xGM;  // xGM is the input GM address of AllGather.
        auto recvBuf = yGM;  // yGM is the output GM address of AllGather.
        uint64_t sendCount = 300;  // Each card has 300 float16 data elements.
        uint64_t strideCount = 0;  // In the non-slicing scenario, strideCount can be set to 0.
        REGISTER_TILING_DEFAULT(AllGatherCustomTilingData); //AllGatherCustomTilingData is the struct defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) {  // Specify communication between AIV cores
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllGatherCustomTilingData, allGatherCcTiling));
            if (ret != HCCL_SUCCESS) {
              return;
            }
            HcclHandle handleId1 = hccl.AllGather<true>(sendBuf, recvBuf, sendCount, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API prematurely, which would cause the other cores to hang in Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round splitting scenario

    Enable multi-round splitting to equivalently handle the communication in the non-multi-round splitting example above. As shown in the following figure, the 300 float16 data elements on each card are split into 2 head blocks and 1 tail block. The data amount of each head block (tileLen) is 128 float16 data elements, and the data amount of the tail block (tailLen) is 44 float16 data elements. During the implementation inside the operator, the split data needs to be processed through 3 rounds of AllGather communication tasks, which is equivalent to the communication result of the non-multi-round splitting example above.

    **Figure 3** Data splitting diagram of each card  
    ![Data splitting diagram of each card-56](../../../../figures/data_splitting_diagram_of_each_card_56.png)

    The specific implementation is as follows: in round 1, the 0-0\1-0\2-0\3-0 data blocks on each rank are processed by AllGather. In round 2, the 0-1\1-1\2-1\3-1 data blocks on each rank are processed by AllGather. In round 3, the 0-2\1-2\2-2\3-2 data blocks on each rank are processed by AllGather. In the result of each round of communication, the number of data elements between the start addresses of adjacent data blocks on each card is strideCount. Taking the round-1 communication result as an example, the data amount between the start addresses of the 0-0 and 1-0 data blocks on rank0 is strideCount = 2\*tileLen+1\*tailLen=300.

    **Figure 4** Round-1 AllGather diagram across 4 cards  
    ![Round-1 AllGather diagram across 4 cards](../../../../figures/round_1_allgather_diagram_across_4_cards.png)

    ```
    extern "C" __global__ __aicore__ void all_gather_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        constexpr uint32_t tileNum = 2U;   // Number of first blocks
        constexpr uint64_t tileLen = 128U; // Number of data elements in each first block
        constexpr uint32_t tailNum = 1U;   // Number of last blocks
        constexpr uint64_t tailLen = 44U;  // Number of data elements in the tail block.
        auto sendBuf = xGM;  // xGM is the input GM address of AllGather.
        auto recvBuf = yGM;  // yGM is the output GM address of AllGather.
        REGISTER_TILING_DEFAULT(AllGatherCustomTilingData); //AllGatherCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllGatherCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
        if (AscendC::g_coreType == AIV) {  // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllGatherCustomTilingData, allGatherCcTiling));
            if (ret != HCCL_SUCCESS) {
              return;
            }
            uint64_t strideCount = tileLen * tileNum + tailLen * tailNum;
            // Process two head blocks.
            constexpr uint32_t tileRepeat = tileNum;
            // Except for the different sendBuf and recvBuf input parameters, the remaining parameters for processing the two head blocks are the same. Therefore, use repeat=2, and the sendBuf and recvBuf of the second head block AllGather task will be updated internally by the API.
            HcclHandle handleId1 = hccl.AllGather<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tileRepeat);
            // Process one tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            constexpr uint32_t tailRepeat = tailNum;
            HcclHandle handleId2 = hccl.AllGather<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tailRepeat);

            for (uint8_t i=0; i<tileRepeat; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
