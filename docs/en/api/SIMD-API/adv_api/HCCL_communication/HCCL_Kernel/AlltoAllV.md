# AlltoAllV

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:36:40.885Z pushedAt=2026-10-27T11:42:10.268Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/AlltoAllV_res.md#id1 -->

## Description

This is the task dispatch API of the collective communication **AlltoAllV**, which returns the task identifier **handleId** to the user. **AlltoAll** is a subset of **AlltoAllV**. **AlltoAll** requires all cards to send and receive the same data size, while **AlltoAllV** does not require the same data size, making it more flexible to use.

The function of **AlltoAllV** is as follows: cards within the communicator send and receive data to and from each other, and customize the data size that each card sends to and receives from other cards, as well as the offsets of the sent and received data in memory. Based on the parameters in the prototype, the API function is described as follows: card i sends the j-th data block in the **sendBuf** memory to card j. The offset of this data block in **sendBuf** is sdispls\[j\], and the data size is sendCounts\[j\]. Card j stores this data in its own **recvBuf** at the offset rdispls\[i\], and the received data size is recvCounts\[i\]. Here, sendCounts\[j\] and recvCounts\[i\] must be equal. Note: The offsets and data sizes here are all in terms of the number of data elements, with the unit sizeof\(sendType\).

![](../../../../figures/AlltoAllV.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAllV(GM_ADDR sendBuf, void* sendCounts, void* sdispls, HcclDataType sendType, GM_ADDR recvBuf, void* recvCounts, void* rdispls, HcclDataType recvType, uint8_t repeat = 1)
```

## Parameters

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value is as follows:<br>**true**: When the Prepare API is called, Commit synchronously notifies the server that this communication task can be executed.<br>**false**: When the Prepare API is called, the server is not notified to execute this communication task. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| sendCounts | Input | Data size sent by this card to every other card within the communicator. **sendCounts[i]** indicates the data size sent by this card to the i-th card **rank_i**, in units of sizeof(sendType).<br><br>**sendCounts** is an array of the uint64_t type, and the array length must be the total number of ranks within the communicator.<br><br>For example, if the data type of the data in **sendBuf** is fp16, **sendCounts[0]**=1 and **sendCounts[1]**=2 indicate that this rank sends one fp16 data element to rank0 and two fp16 data elements to rank1. |
| sdispls | Input | Offset of the data sent by this card to other cards in **sendBuf**. **sdispls[i]**=n indicates that the offset data size of the data block sent by this card to **rank_i** in **sendBuf** is n.<br><br>**sdispls** is an array of the uint64_t type, and the array length must be the total number of ranks within the communicator. |
| sendType | Input | Data type of the data in **sendBuf**. Currently, all data types included in **HcclDataType** are supported. For details about **HcclDataType**, see [Table 1](HCCL_usage.md#table116710585514). |
| recvBuf | Output | Destination data buffer address. The collective communication result is output to this buffer. |
| recvCounts | Input | Data size received by this card from other cards. **recvCounts[i]** indicates the number of data elements received by this rank from **rank_i**, in units of sizeof(recvType).<br><br>**recvCounts** is an array of the uint64_t type, and the array length must be the total number of ranks within the communicator.<br><br>For example, if the data type of the data in **recvBuf** is fp16, **recvCounts[0]**=1 and **recvCounts[1]**=2 indicate that this rank receives one fp16 data element from rank0 and two fp16 data elements from rank1. |
| rdispls | Input | Offset at which the data received by this card is stored in **recvBuf**. **rdispls[i]**=n indicates that the offset data size of the data block received by this card from **rank_i** in **recvBuf** is n.<br><br>**rdispls** is an array of the uint64_t type, and the array length must be the total number of ranks within the communicator. |
| recvType | Input | Data type of the data in **recvBuf**. Currently, all data types included in **HcclDataType** are supported. For details about **HcclDataType**, see [Table 1](HCCL_usage.md#table116710585514).<br><br>Note: **recvType** and **sendType** must be the same. |
| repeat | Input | Number of AlltoAllV communication tasks delivered at a time. The value of **repeat** is greater than or equal to 1, and the default value is 1. When **repeat**>1, the sendBuf, sendCounts, recvBuf, and recvCounts parameters of each AlltoAllV task remain unchanged, while **sdispls** and **rdispls** are updated by the server. The update formula for each round of task i is as follows:<br><br>sdispls[i] = sdispls[i] + sendCounts[i], i∈[0, sdispls.size())<br><br>rdispls[i] = rdispls[i] + recvCounts[i], i∈[0, rdispls.size())<br><br>Note: When **repeat**>1 is set, plan the communication memory according to this calculation formula. |

## Return Value

Returns the handleId that identifies this task. **handleId** is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object does not specify the core for delivering the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](HCCL_template_params.md#hccl-template-params) of the HCCL object specifies the core for delivering the communication task, this API can be called on both the AIC core and the AIV core simultaneously. Internally, the API delivers the communication task on only one of the AIC core and the AIV core based on the specified core type.
-   Within a communicator, the total number of calls to all Prepare APIs and the InterHcclGroupSync API cannot exceed 63.
-   The data size sendCounts\[j\] sent by each card to card rank\_j must be equal to the data size recvCounts\[i\] received by rank\_j from the corresponding card rank\_i.
<!-- npu="A3" id7 -->
-   For Atlas A3 products, a communicator supports communication among a maximum of 128 cards.
<!-- end id7 -->
<!-- npu="950" id8 -->
-   For Ascend 950PR&950DT products, within a communicator, the total number of calls to all Prepare APIs cannot exceed 63.
-   For the Ascend 950PR&950DT products, when the communication server is the CCU, only the FullMesh algorithm is supported by default, and the maximum data size of a single communication cannot exceed 256M.
<!-- end id8 -->

## Examples

-   Use the AlltoAllV API to equivalently implement AlltoAll communication among four cards.

    Four cards call the AlltoAllV API. In a non-split scenario, the data blocks and data sizes on each card are the same, as shown by the A\\B\\C\\D data blocks on each card in the following figure, where the data size is dataCount.

    **Figure 1**  AlltoAllV diagram for four cards in a non-split scenario  
    ![](../../../../figures/alltoallv_4rank_no_split.png "AlltoAllV diagram for four cards in a non-split scenario")

    ```
    extern "C" __global__ __aicore__ void alltoallv_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t rankNum = 4U;
        constexpr uint32_t dataCount = 10U; // Assume that the number of A\B\C\D data blocks in the figure is 10.
        uint64_t sendCounts[rankNum] = {0};
        uint64_t sDisplacements[rankNum] = {0};
        uint64_t recvCounts[rankNum] = {0};
        uint64_t rDisplacements[rankNum] = {0};
        for (uint32_t i = 0U; i < rankNum; ++i) {
            sendCounts[i] = dataCount;
            sDisplacements[i] = i * dataCount;
            recvCounts[i] = dataCount;
            rDisplacements[i] = i * dataCount;
        }
        auto sendBuf = xGM; // xGM is the input GM address of AlltoAllV.
        auto recvBuf = yGM; // yGM is the output GM address of AlltoAllV.
        auto dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
        REGISTER_TILING_DEFAULT(AllToAllVCustomTilingData); // AllToAllVCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllVCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllVCustomTilingData, alltoallvCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            auto handleId1 = hccl.AlltoAllV<true>(
                sendBuf, sendCounts, sDisplacements, dtype, recvBuf, recvCounts, rDisplacements, dtype);

            hccl.Wait(handleId1);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```

-   Use the AlltoAllV API to send and receive data of different sizes across 4 cards.

    As shown in the following figure, the number in each cell under each rank indicates the number of data elements sent or received. Take rank1 as an example: rank1 sends 2, 2, 3, and 2 data elements to rank0, rank1, rank2, and rank3, respectively, and receives 3, 2, 4, and 3 data elements from rank0, rank1, rank2, and rank3, respectively. The corresponding code example is as follows.

    **Figure 2**  Uneven send/receive across 4 cards in the non-split scenario  
    ![](../../../../figures/4rank_unequal_no_split.png "Uneven send/receive across 4 cards in the non-split scenario")

    ```
    extern "C" __global__ __aicore__ void alltoallv_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM)
    {
        constexpr uint32_t rankNum = 4U;
        uint64_t sendCounts[rankNum] = {0};
        uint64_t sDisplacements[rankNum] = {0};
        uint64_t recvCounts[rankNum] = {0};
        uint64_t rDisplacements[rankNum] = {0};
        auto sendBuf = xGM; // xGM is the input GM address of AlltoAllV.
        auto recvBuf = yGM; // yGM is the output GM address of AlltoAllV.
        auto dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
        REGISTER_TILING_DEFAULT(AllToAllVCustomTilingData); // AllToAllVCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllVCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In an AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) { // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllVCustomTilingData, alltoallvCcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            if (hccl.GetRankId() == 0) {
                sendCounts[0] = 3;
                sendCounts[1] = 3;
                sendCounts[2] = 3;
                sendCounts[3] = 3;
                sDisplacements[1] = 3;
                sDisplacements[2] = 6;
                sDisplacements[3] = 9;
                recvCounts[0] = 3;
                recvCounts[1] = 2;
                recvCounts[2] = 1;
                recvCounts[3] = 3;
                rDisplacements[1] = 3;
                rDisplacements[2] = 5;
                rDisplacements[3] = 6;
            } else if (hccl.GetRankId() == 1) {
                sendCounts[0] = 2;
                sendCounts[1] = 2;
                sendCounts[2] = 3;
                sendCounts[3] = 2;
                sDisplacements[1] = 2;
                sDisplacements[2] = 4;
                sDisplacements[3] = 7;
                recvCounts[0] = 3;
                recvCounts[1] = 2;
                recvCounts[2] = 4;
                recvCounts[3] = 3;
                rDisplacements[1] = 3;
                rDisplacements[2] = 5;
                rDisplacements[3] = 9;
            } else if (hccl.GetRankId() == 2) {
                sendCounts[0] = 1;
                sendCounts[1] = 4;
                sendCounts[2] = 4;
                sendCounts[3] = 4;
                sDisplacements[1] = 1;
                sDisplacements[2] = 5;
                sDisplacements[3] = 9;
                recvCounts[0] = 3;
                recvCounts[1] = 3;
                recvCounts[2] = 4;
                recvCounts[3] = 3;
                rDisplacements[1] = 3;
                rDisplacements[2] = 6;
                rDisplacements[3] = 10;
            } else if (hccl.GetRankId() == 3) {
                sendCounts[0] = 3;
                sendCounts[1] = 3;
                sendCounts[2] = 3;
                sendCounts[3] = 3;
                sDisplacements[1] = 3;
                sDisplacements[2] = 6;
                sDisplacements[3] = 9;
                recvCounts[0] = 3;
                recvCounts[1] = 2;
                recvCounts[2] = 4;
                recvCounts[3] = 3;
                rDisplacements[1] = 3;
                rDisplacements[2] = 5;
                rDisplacements[3] = 9;
            }
            auto handleId = hccl.AlltoAllV<true>(
                sendBuf, sendCounts, sDisplacements, dtype, recvBuf, recvCounts, rDisplacements, dtype);

            hccl.Wait(handleId);
            AscendC::SyncAll<true>(); // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
