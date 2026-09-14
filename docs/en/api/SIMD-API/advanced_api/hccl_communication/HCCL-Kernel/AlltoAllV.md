# AlltoAllV

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:18:26.763Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

This is the task dispatch API for the **AlltoAllV** collective communication, which returns the task identifier **handleId** to you. **AlltoAll** is a subset of **AlltoAllV**. **AlltoAll** requires that all cards send and receive the same amount of data, whereas **AlltoAllV** does not require equal data amounts, making it more flexible to use.

The function of **AlltoAllV** is as follows: cards in the communicator send and receive data to and from each other, with the amount of data sent by each card to other cards and received from other cards, as well as the offsets of the sent and received data in memory, being customizable. Based on the parameters in the prototype, the API function is described as follows: card i sends the j-th data block in the **sendBuf** memory to card j. The offset of this data block in **sendBuf** is sdispls\[j\], and the data amount is sendCounts\[j\]. Card j stores this data in its own **recvBuf** at the offset rdispls\[i\], with a receive data amount of recvCounts\[i\]. Here, sendCounts\[j\] and recvCounts\[i\] must be equal. Note: The offset and data amount here are both in terms of the number of data elements, with the unit being sizeof\(sendType\).

![](../../../../figures/allreduce-61.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AlltoAllV(GM_ADDR sendBuf, void *sendCounts, void *sdispls, HcclDataType sendType, GM_ADDR recvBuf, void *recvCounts, void *rdispls, HcclDataType recvType, uint8_t repeat = 1)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | Data type: bool. The values are as follows:<br>true: When calling the Prepare API, Commit synchronously notifies the server to execute this communication task.<br>false: When calling the Prepare API, the server is not notified to execute this communication task. |

**Table 2**  Interface parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| sendCounts | Input | Amount of data this card sends to every other card in the communication domain. sendCounts[i] indicates the amount of data this card sends to card i (rank_i), in units of sizeof(sendType).<br><br>sendCounts is a uint64_t array whose length must be the total number of ranks in the communication domain.<br><br>Example: If the data type in sendBuf is fp16 and sendCounts[0]=1, sendCounts[1]=2, it means this rank sends one fp16 value to rank0 and two fp16 values to rank1. |
| sdispls | Input | Offset of the data that this card sends to other cards in sendBuf. sdispls[i]=n indicates that the data block sent by this card to rank_i has an offset of n in sendBuf.<br><br>sdispls is a uint64_t array whose length must be the total number of ranks in the communication domain. |
| sendType | Input | Data type of the data in sendBuf. All data types supported by HcclDataType are supported. For details about HcclDataType, see [Table 1](../HCCL-Kernel/hccl_usage.md#table116710585514). |
| recvBuf | Output | Destination data buffer address. The result of the collective communication is output to this buffer. |
| recvCounts | Input | Amount of data this card receives from other cards. recvCounts[i] indicates the number of data elements this rank receives from rank_i, in units of sizeof(recvType).<br><br>recvCounts is a uint64_t array whose length must be the total number of ranks in the communication domain.<br><br>Example: If the data type in recvBuf is fp16 and recvCounts[0]=1, recvCounts[1]=2, it means this rank receives one fp16 value from rank0 and two fp16 values from rank1. |
| rdispls | Input | Offset at which the data received by this card is stored in recvBuf. rdispls[i]=n indicates that the data block received by this card from rank_i has an offset of n in recvBuf.<br><br>rdispls is a uint64_t array whose length must be the total number of ranks in the communication domain. |
| recvType | Input | Data type of the data in recvBuf. All data types supported by HcclDataType are supported. For details about HcclDataType, see [Table 1](../HCCL-Kernel/hccl_usage.md#table116710585514).<br><br>Note: recvType and sendType must be the same. |
| repeat | Input | Number of AlltoAllV communication tasks delivered at a time. The value of repeat must be ≥1, and the default value is 1. When repeat>1, the sendBuf\sendCounts\recvBuf\recvCounts parameters of each AlltoAllV task remain unchanged, while sdispls and rdispls are updated by the server. The update formula for each round of task i is as follows:<br><br>sdispls[i] = sdispls[i] + sendCounts[i], i∈[0, sdispls.size())<br><br>rdispls[i] = rdispls[i] + recvCounts[i], i∈[0, rdispls.size())<br><br>Note: When repeat>1 is set, the communication memory must be planned according to this formula. |

## Return Value

Returns the task identifier **handleId**, which is greater than or equal to 0. On failure, returns -1.

## Constraints

-   Before calling this API, ensure that the [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) APIs have been called.
-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core for dispatching the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core for dispatching the communication task, this API can be called on both the AIC core and the AIV core simultaneously; internally, the API dispatches the communication task on only one of the AIC core and the AIV core based on the specified core type.
-   Within a communicator, the total number of calls to all Prepare APIs and InterHcclGroupSync APIs cannot exceed 63.
-   The amount of data sendCounts\[j\] that each card sends to card rank\_j must be equal to the amount of data recvCounts\[i\] that rank\_j receives from the corresponding card rank\_i.
-   For Atlas A3 training products/Atlas A3 inference products, a communicator supports communication among a maximum of 128 cards.
-   For Ascend 950PR/Ascend 950DT, within a communicator, the total number of calls to all Prepare APIs cannot exceed 63.
-   For Ascend 950PR/Ascend 950DT, when the communication server is the CCU, the maximum amount of data communicated in a single operation cannot exceed 256M.

## Examples

-   Use the **AlltoAllV** API to equivalently implement **AlltoAll** communication across 4 cards.

    Four cards call the **AlltoAllV** API. In the non-split scenario, the data blocks and data amounts on each card are identical, as shown in the following figure, where the data amount of each data block on each card is **dataCount**.

    **Figure 1**  Illustration of AlltoAllV across 4 cards in the non-split scenario  
    ![](../../../../figures/illustration_of_alltoallv_across_4_cards_in_the_non_split_scenario.png "Illustration of AlltoAllV across 4 cards in the non-split scenario")

    ```
    extern "C" __global__ __aicore__ void alltoallv_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        constexpr uint32_t rankNum = 4U;
        constexpr uint32_t dataCount = 10U;   // Assume that the number of data blocks in the figure is 10.f data blocks is 10 for each.
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
        auto sendBuf = xGM;  // xGM is the input GM address of AlltoAllV.
        auto recvBuf = yGM;  // yGM is the output GM address of AlltoAllV.
        auto dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
        REGISTER_TILING_DEFAULT(AllToAllVCustomTilingData); //AllToAllVCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllVCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) {  // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllVCustomTilingData, alltoallvCcTiling));
            if (ret != HCCL_SUCCESS) {
              return;
            }
            auto handleId1 = hccl.AlltoAllV<true>(sendBuf, sendCounts, sDisplacements, dtype,
                                                   recvBuf, recvCounts, rDisplacements, dtype);

            hccl.Wait(handleId1);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```

-   Use the AlltoAllV API to send and receive data of different sizes among four cards.

    As shown in the following figure, the number in each cell under each rank indicates the number of data elements sent or received. Take rank1 as an example: rank1 sends 2, 2, 3, and 2 data elements to rank0, rank1, rank2, and rank3, respectively, and receives 3, 2, 4, and 3 data elements from rank0, rank1, rank2, and rank3, respectively. The corresponding code example is as follows.

    **Figure 2**  Uneven send/receive among four cards in the non-split scenario  
    ![](../../../../figures/uneven_send_receive_among_four_cards_in_non_split_scenario.png "Uneven send/receive among four cards in the non-split scenario")

    ```
    extern "C" __global__ __aicore__ void alltoallv_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        constexpr uint32_t rankNum = 4U;
        uint64_t sendCounts[rankNum] = {0};
        uint64_t sDisplacements[rankNum] = {0};
        uint64_t recvCounts[rankNum] = {0};
        uint64_t rDisplacements[rankNum] = {0};
        auto sendBuf = xGM;  // xGM is the input GM address of AlltoAllV.
        auto recvBuf = yGM;  // yGM is the output GM address of AlltoAllV.
        auto dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
        REGISTER_TILING_DEFAULT(AllToAllVCustomTilingData); //AllToAllVCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllToAllVCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) {  // Specify AIV core communication.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllToAllVCustomTilingData, alltoallvCcTiling));
            if (ret != HCCL_SUCCESS) {
              return;
            }
            if(hccl.GetRankId() == 0) {
                sendCounts[0] = 3; sendCounts[1] = 3; sendCounts[2] = 3; sendCounts[3] = 3;
                sDisplacements[1] = 3; sDisplacements[2] = 6; sDisplacements[3] = 9;
                recvCounts[0] = 3; recvCounts[1] = 2; recvCounts[2] = 1; recvCounts[3] = 3;
                rDisplacements[1] = 3; rDisplacements[2] = 5; rDisplacements[3] = 6;
            } else if(hccl.GetRankId() == 1) {
                sendCounts[0] = 2; sendCounts[1] = 2; sendCounts[2] = 3; sendCounts[3] = 2;
                sDisplacements[1] = 2; sDisplacements[2] = 4; sDisplacements[3] = 7;
                recvCounts[0] = 3; recvCounts[1] = 2; recvCounts[2] = 4; recvCounts[3] = 3;
                rDisplacements[1] = 3; rDisplacements[2] = 5; rDisplacements[3] = 9;
            } else if(hccl.GetRankId() == 2) {
                sendCounts[0] = 1; sendCounts[1] = 4; sendCounts[2] = 4; sendCounts[3] = 4;
                sDisplacements[1] = 1; sDisplacements[2] = 5; sDisplacements[3] = 9;
                recvCounts[0] = 3; recvCounts[1] = 3; recvCounts[2] = 4; recvCounts[3] = 3;
                rDisplacements[1] = 3; rDisplacements[2] = 6; rDisplacements[3] = 10;
            } else if(hccl.GetRankId() == 3) {
                sendCounts[0] = 3; sendCounts[1] = 3; sendCounts[2] = 3; sendCounts[3] = 3;
                sDisplacements[1] = 3; sDisplacements[2] = 6; sDisplacements[3] = 9;
                recvCounts[0] = 3; recvCounts[1] = 2; recvCounts[2] = 4; recvCounts[3] = 3;
                rDisplacements[1] = 3; rDisplacements[2] = 5; rDisplacements[3] = 9;
            }
            auto handleId = hccl.AlltoAllV<true>(sendBuf, sendCounts, sDisplacements, dtype,
                                                   recvBuf, recvCounts, rDisplacements, dtype);

            hccl.Wait(handleId);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling the hccl.Finalize() API prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
