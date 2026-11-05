# AlltoAll

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:34:46.999Z pushedAt=2026-10-27T11:41:59.579Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/AlltoAll_res.md#id1 -->

## Description

This is the task dispatch API of the collective communication **AlltoAll**, which returns the task identifier **handleId** to the user. The function of **AlltoAll** is as follows: each card sends data of the same amount to all cards within the communication domain, and receives data of the same amount from all cards. Combined with the parameters in the prototype, the API function is described as follows: card j receives the j-th data block from the **sendBuf** of card i, and stores this data at the i-th block position in the **recvBuf** of this card.

![](../../../../figures/AlltoAll.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAll(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t dataCount, HcclDataType dataType, uint64_t strideCount = 0, uint8_t repeat = 1)
```

## Parameters

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | **bool** type. The value is as follows:<br>**true**: When the Prepare API is called, Commit synchronously notifies the server that this communication task can be executed.<br>**false**: When the Prepare API is called, the server is not notified to execute this communication task. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address. The collective communication result is output to this buffer. |
| dataCount | Input | Amount of data sent and received by this card to and from each other card within the communication domain, in units of sizeof(dataType).<br><br>For example, if there are 4 cards in the communication domain and each card has 4 fp16 data elements in its sendBuf, then dataCount=1. |
| dataType | Input | Data type of the AlltoAll operation. Currently, all data types included in HcclDataType are supported. For details about HcclDataType, see [Table 1](HCCL_usage.md#table116710585514). |
| strideCount | Input | In the multi-round split scenario, the interval between data blocks that participate in communication within each card in a single AlltoAll task. The default value is 0, indicating that the data blocks are contiguous in memory.<br>When strideCount=0, the data blocks that participate in communication within each card are contiguous in memory. Card rank_j receives the j-th block of data from the sendBuf of card rank_i, with an offset data amount of j\*dataCount between data blocks, and stores this data at the i-th block position in its own recvBuf, with an offset data amount of i\*dataCount.<br>When strideCount>0, the offset data amount between the start addresses of adjacent data blocks that participate in communication within each card is strideCount. Card rank_j receives the j-th block of data from the sendBuf of card rank_i, with an offset data amount of j\*strideCount between data blocks, and stores this data at the i-th block position in its own recvBuf, with an offset data amount of i\*strideCount.<br><br>Note: The offset data amount mentioned above is the number of data elements, in units of sizeof(dataType). |
| repeat | Input | Number of AlltoAll communication tasks delivered at a time. The value of repeat is ≥1, and the default value is 1. When repeat>1, the sendBuf and recvBuf addresses of each round of AlltoAll tasks are updated by the server. The update formula for each round of task i is as follows:<br><br>sendBuf[i] = sendBuf + dataCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + dataCount * sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When repeat>1 is set, it must be used together with the strideCount parameter to plan the communication data addresses. |

## Return Value

Returns the handleId of the task. handleId is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core for delivering the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core for delivering the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, based on the specified core type, the API delivers the communication task only on one of the AIC core and the AIV core.
<!-- npu="950" id7 -->
-   For the Ascend 950PR&950DT products, when the communication server is the CCU, only the FullMesh algorithm is supported by default, and the maximum amount of communication data in a single operation cannot exceed 256M.
-   For the Ascend 950PR&950DT products, within a communication domain, the total call count of all Prepare APIs cannot exceed 63.
<!-- end id7 -->
<!-- npu="910b" id9 -->
-   For the Atlas A2 products, within a communication domain, the total call count of all Prepare APIs cannot exceed 63.
<!-- end id9 -->
<!-- npu="A3" id10 -->
-   For the Atlas A3 products, within a communication domain, the total call count of all Prepare APIs and InterHcclGroupSync APIs cannot exceed 63.
-   For the Atlas A3 products, within a communication domain, communication of up to 128 cards is supported.
<!-- end id10 -->

## Examples

-   Non-multi-round split scenario

    Four cards execute the AlltoAll communication task. In the non-multi-round split scenario, the data blocks and data volume on each card are identical, such as the A\\B\\C\\D data blocks on each card in the following figure, with the data volume being dataCount.

    **Figure 1**  AlltoAll communication across 4 cards in the non-multi-round split scenario  
    ![](../../../../figures/alltoall_4rank_no_multisplit.png "AlltoAll communication across 4 cards in the non-multi-round split scenario")

    ```
    extern "C" __global__ __aicore__ void alltoall_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint64_t dataCount = 128U;               // Data volume.
        auto sendBuf = xGM;                                // xGM is the input GM address of AlltoAll.
        auto recvBuf = yGM;                                // yGM is the output GM address of AlltoAll.
        REGISTER_TILING_DEFAULT(AllToAllCustomTilingData); // AllToAllCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllCustomTilingData, alltoallCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            HcclHandle handleId = hccl.AlltoAll<true>(sendBuf, recvBuf, dataCount, HcclDataType::HCCL_DATA_TYPE_FP16);
            hccl.Wait(handleId);
            AscendC::SyncAll<true>(); // Fully synchronize AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() in advance, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round split scenario

    Enable multi-round split to equivalently process the communication in the non-multi-round split example above. On the basis that the data on each card is evenly divided into 4 blocks (A\\B\\C\\D), each block is further split into several blocks. In this example, each block is further split into 3 blocks, as shown in the following figure. The 3 blocks obtained from the further split include 2 data blocks with a data size of tileLen and 1 tail block with a data size of tailLen. After the split, the AlltoAll communication task needs to be performed in 3 rounds, which is equivalent to the communication result of the non-multi-round split described above.

    **Figure 2**  AlltoAll communication among 4 cards in the 3-round split scenario  
    ![](../../../../figures/alltoall_4rank_3split.png "AlltoAll communication among 4 cards in the 3-round split scenario")

    The specific implementation is as follows. In round 1, the 0-0\\1-0\\2-0\\3-0 data blocks on each rank are processed by AlltoAll. On the same card, the interval between adjacent data blocks that participate in communication is the value of the strideCount parameter. In round 2, the 0-1\\1-1\\2-1\\3-1 data blocks on each rank are processed by AlltoAll. In round 3, the 0-2\\1-2\\2-2\\3-2 data blocks on each rank are processed by AlltoAll. The figure and code example of round 1 are as follows.

    **Figure 3**  Schematic diagram of the first round of AlltoAll among 4 cards  
    ![](../../../../figures/first_round_4rank_alltoall.png "Schematic diagram of the first round of AlltoAll among 4 cards")

    ```
    extern "C" __global__ __aicore__ void alltoall_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t tileNum = 2U;                   // Number of head blocks.
        constexpr uint64_t tileLen = 128U;                 // Number of data elements in each head block.
        constexpr uint32_t tailNum = 1U;                   // Number of tail blocks.
        constexpr uint64_t tailLen = 100U;                 // Number of data elements in each tail block.
        auto sendBuf = xGM;                                // xGM is the input GM address of AlltoAll.
        auto recvBuf = yGM;                                // yGM is the output GM address of AlltoAll.
        REGISTER_TILING_DEFAULT(AllToAllCustomTilingData); // AllToAllCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllCustomTilingData, alltoallCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            uint64_t strideCount = tileLen * tileNum + tailLen * tailNum;
            // Process 2 head blocks.
            HcclHandle handleId1 =
                hccl.AlltoAll<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tileNum);
            // Process 1 tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            HcclHandle handleId2 =
                hccl.AlltoAll<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, strideCount, tailNum);

            for (uint8_t i = 0; i < tileNum; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API in advance, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
