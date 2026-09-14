# AlltoAll

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:14:39.922Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

This is the task submission API for the AlltoAll collective communication operation. It returns the task identifier **handleId** to the user. AlltoAll enables each card to send data of the same size to all cards within the communicator and receive data of the same size from all cards. Combined with the parameters in the prototype, the API function is described as follows: the j-th card receives the j-th data block from the **sendBuf** of the i-th card and stores this data at the i-th block position in the **recvBuf** of the local card.

![](../../../../figures/allreduce-60.png)

## Function Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAll(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount = 0, uint8_t repeat = 1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The parameter values are as follows:<br>true: When the **Prepare** API is called, **Commit** synchronously notifies the server that the communication task can be executed.<br>false: When the **Prepare** API is called, the server is not notified to execute the communication task. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address, to which the collective communication result is output. |
| dataCount | Input | Amount of data sent and received by the local card to and from every other card within the communicator, in units of sizeof(dataType).<br><br>For example, if there are four cards in the communicator and each card has four fp16 data elements in its sendBuf, then dataCount = 1. |
| dataType | Input | Data type of the AlltoAll operation. All data types included in HcclDataType are currently supported. For details about HcclDataType, see [Table 1](hccl_usage.md#table116710585514). |
| strideCount | Input | In the multi-round partitioning scenario, the interval between data blocks participating in communication within each card in one AlltoAll task. The default value is 0, indicating that the data blocks are contiguous in memory.<br>When strideCount = 0, the data blocks participating in communication within each card are contiguous in memory. Card rank_j receives the j-th block of data from the sendBuf of card rank_i, with an offset data amount of j\*dataCount between data blocks, and stores the data at the i-th block position in the recvBuf of the local card, with an offset data amount of i\*dataCount.<br>When strideCount > 0, the offset data amount between the start addresses of adjacent data blocks participating in communication within each card is strideCount. Card rank_j receives the j-th block of data from the sendBuf of card rank_i, with an offset data amount of j\*strideCount between data blocks, and stores the data at the i-th block position in the recvBuf of the local card, with an offset data amount of i\*strideCount.<br><br>Note: The offset data amount mentioned above is the number of data elements, in units of sizeof(dataType). |
| repeat | Input | Number of AlltoAll communication tasks issued at a time. repeat takes a value greater than or equal to 1, with a default value of 1. When repeat > 1, the sendBuf and recvBuf addresses of each round of AlltoAll tasks are updated by the server. The update formula for each round of task i is as follows:<br><br>sendBuf[i] = sendBuf + dataCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + dataCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat is set to a value greater than 1, it must be used together with the strideCount parameter to plan the communication data addresses. |

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. On failure, returns -1.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core for delivering the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core for delivering the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API delivers the communication task only on either the AIC core or the AIV core based on the specified core type.
-   For Ascend 950PR/Ascend 950DT, when the communication server is the CCU, the maximum communication data volume in a single operation cannot exceed 256M.
-   For Ascend 950PR/Ascend 950DT, the total number of calls to all **Prepare** APIs within a communicator cannot exceed 63.
-   For Atlas A2 training products/Atlas A2 inference products, the total number of calls to all **Prepare** APIs within a communicator cannot exceed 63.
-   For Atlas A3 training products/Atlas A3 inference products, the total number of calls to all **Prepare** APIs and the InterHcclGroupSync API within a communicator cannot exceed 63.
-   For Atlas A3 training products/Atlas A3 inference products, a maximum of 128 ranks are supported for communication within a communicator.

## Examples

-   Non-multi-round partitioning scenario

    Four cards execute the AlltoAll communication task. In the non-multi-round partitioning scenario, the data blocks and data volume on each card are identical. As shown in the following figure, the A\\B\\C\\D data block on each card has a data volume of **dataCount**.

    **Figure 1**  AlltoAll communication across four cards in the non-multi-round partitioning scenario  
    ![](../../../../figures/alltoall_communication_across_four_cards_in_the_non_multi_round_partitioning_scenario.png "AlltoAll communication across four cards in the non-multi-round partitioning scenario")

    ```
    extern "C" __global__ __aicore__ void alltoall_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        constexpr uint64_t dataCount = 128U; // Data volume.
        auto sendBuf = xGM;  // xGM is the input GM address of AlltoAll.
        auto recvBuf = yGM;  // yGM is the output GM address of AlltoAll.
        REGISTER_TILING_DEFAULT(AllToAllCustomTilingData); //AllToAllCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) {  // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllCustomTilingData, alltoallCcTiling));
    	if (ret != HCCL_SUCCESS) {
    	    return;
    	}
    	HcclHandle handleId = hccl.AlltoAll<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16);
    	hccl.Wait(handleId);
    	AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
    	hccl.Finalize();
        }
    }
    ```

-   Multi-round partitioning scenario

    Enable multi-round partitioning to equivalently process the communication in the non-multi-round partitioning example above. On the basis that the data on each card is divided into 4 blocks (A\\B\\C\\D), each block is further divided into several blocks. In this example, each block is further divided into 3 blocks, as shown in the following figure. The 3 blocks obtained from the further division include 2 data blocks with a data volume of tileLen and 1 tail block with a data volume of tailLen. After division, the AlltoAll communication task needs to be performed in 3 rounds, which is equivalent to the communication result of the non-multi-round partitioning described above.

    **Figure 2**  AlltoAll communication across 4 cards in the 3-round partitioning scenario  
    ![](../../../../figures/alltoall_communication_across_4_cards_in_the_3_round_partitioning_scenario.png "AlltoAll communication across 4 cards in the 3-round partitioning scenario")

    The specific implementation is as follows: in round 1, the 0-0\\1-0\\2-0\\3-0 data blocks on each rank are processed by AlltoAll; on the same card, the interval between adjacent data blocks participating in the communication is the value of the strideCount parameter. In round 2, the 0-1\\1-1\\2-1\\3-1 data blocks on each rank are processed by AlltoAll. In round 3, the 0-2\\1-2\\2-2\\3-2 data blocks on each rank are processed by AlltoAll. The diagram and code example of round 1 communication are as follows.

    **Figure 3**  Schematic diagram of the first round of AlltoAll across 4 cards  
    ![](../../../../figures/schematic_diagram_of_the_first_round_of_alltoall_across_4_cards.png "Schematic diagram of the first round of AlltoAll across 4 cards")

    ```
    extern "C" __global__ __aicore__ void alltoall_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        constexpr uint32_t tileNum = 2U;   // Number of first blocks.
        constexpr uint64_t tileLen = 128U; // Number of data elements in the first block.
        constexpr uint32_t tailNum = 1U;   // Number of tail blocks.
        constexpr uint64_t tailLen = 100U; // Number of data elements in the tail block.
        auto sendBuf = xGM;  // xGM is the input GM address of AlltoAll.
        auto recvBuf = yGM;  // yGM is the output GM address of AlltoAll.
        REGISTER_TILING_DEFAULT(AllToAllCustomTilingData); //AllToAllCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) {  // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllCustomTilingData, alltoallCcTiling));
            if (ret != HCCL_SUCCESS) {
              return;
            }
            uint64_t strideCount = tileLen * tileNum + tailLen * tailNum;
            // Process two first blocks.
            HcclHandle handleId1 = hccl.AlltoAll<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tileNum);
            // Process one tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            HcclHandle handleId2 = hccl.AlltoAll<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tailNum);

            for (uint8_t i=0; i<tileNum; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() in advance, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
