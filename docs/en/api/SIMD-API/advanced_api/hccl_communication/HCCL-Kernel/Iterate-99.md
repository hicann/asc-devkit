# Iterate

<!-- md-trans-meta sourceCommit=dc7c96244be34fce76a6f9a5327431cf64d741de translatedAt=2026-09-10T02:23:53.600Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Under certain algorithms, a complete collective communication task can be subdivided into multiple steps, with point-to-point communication tasks completed for the data of each step. This is called fine-grained communication. Take the AlltoAllV communication task with the communication algorithm "AlltoAll=level0:fullmesh;level1:pairwise" and a communication step size of 1 as an example. Here, the parameter level0 configures the intra-Server communication algorithm, and the parameter level1 configures the inter-Server communication algorithm. fullmesh is the fullmesh communication algorithm, and pairwise is the pairwise communication algorithm. As shown in the following figure, this example illustrates all data to be sent in the AlltoAllV communication and the data received by each device after each communication step is completed.

**Figure 1**  Schematic diagram of AlltoAllV communication steps using the pairwise algorithm  
![](../../../../figures/schematic_diagram_of alltoallv_communication_steps_using_the_pairwise_algorithm.png "Schematic diagram of AlltoAllV communication steps using the pairwise algorithm")

In a communication-computation fusion operator, by calling this API together with the corresponding Prepare primitive, you can obtain the input or output of each step of the communication algorithm, enabling finer-grained pipelining of computation and communication and thereby achieving better performance gains.

## Prototype

```
template <bool sync = true>
__aicore__ inline int32_t Iterate(HcclHandle handleId, uint16_t *seqSlices, uint16_t seqSliceLen)
```

## Parameters

**Table 1** Template parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| sync | Input | bool type. Whether to wait for the current communication step to complete before performing subsequent computation or communication tasks. The value options are as follows:<br>true: Default value, indicating blocking and waiting for the current communication step to complete. When this parameter is set to true, you do not need to call the [Wait](Wait-98.md) API to wait for the communication task to complete.<br>false: Indicates not waiting for the current communication step to complete. |

**Table 2** API parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| handleId | Input | ID of the corresponding communication task. Only the return value of the Prepare primitive API can be used.<br>using HcclHandle = int8_t; |
| seqSlices | Output | Stack space allocated by the user, used to save the index subscripts of the input or output data blocks of the current communication step. In the computation-before-communication scenario, this parameter returns the input data block index required by the current communication step; in the communication-before-computation scenario, this parameter returns the output data block index of the current communication step. |
| seqSliceLen | Input | Length of the seqSlices array. Based on the communication step size and algorithm logic of the algorithm, the number of data block indexes to be saved for each communication step is used as the array length. |

## Return Value

-   When the communication task has not finished:
    -   In the computation-before-communication scenario, the return value is the number of input data blocks required by the current communication step, which is the same as the value of the **seqSliceLen** parameter.
    -   In the communication-before-computation scenario, the return value is the number of output data blocks produced by the current communication step, which is the same as the value of the **seqSliceLen** parameter.

-   After the communication task finishes, the return value is 0.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the API corresponding to the Prepare primitive.
-   The communication algorithm currently supported by this API is "AlltoAll=level0:fullmesh;level1:pairwise".

## Examples

```
extern "C" __global__ __aicore__ void alltoallv_custom(GM_ADDR sendBuf, GM_ADDR recvBuf, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
    // Specify AIV core communication.
    if (AscendC::g_coreType != AIV) {
        return;
    }

    constexpr uint32_t RANK_NUM = 4U;
    constexpr uint32_t STEP_SIZE = 1U; // Fine-grained communication step size. It is usually set by using the SetStepSize API, and is simplified to 1 in the sample code.
    constexpr uint64_t sendCounts[RANK_NUM][RANK_NUM] = {
        {3, 3, 3, 3}, {2, 2, 3, 2},
        {1, 4, 4, 4}, {3, 3, 3, 3}
    };
    constexpr uint64_t sDisplacements[RANK_NUM][RANK_NUM] = {
        {0, 3, 6, 9}, {0, 2, 4, 7},
        {0, 1, 5, 9}, {0, 3, 6, 9}
    };
    constexpr uint64_t recvCounts[RANK_NUM][RANK_NUM] = {
        {3, 2, 1, 3}, {3, 2, 4, 3},
        {3, 3, 4, 3}, {3, 2, 4, 3}
    };
    constexpr uint64_t rDisplacements[RANK_NUM][RANK_NUM] = {
        {0, 3, 5, 6}, {0, 3, 5, 9},
        {0, 3, 6, 10}, {0, 3, 5, 9}
    };
    HcclDataType dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
    REGISTER_TILING_DEFAULT(AllToAllVCustomTilingData); // AllToAllVCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(AllToAllVCustomTilingData, tilingData, tilingGM);
    GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
    Hccl hccl;
    hccl.InitV2(contextGM, &tilingData);
    auto ret = hccl.SetCcTilingV2(offsetof(AllToAllVCustomTilingData, alltoallvCcTiling));
    if (ret != HCCL_SUCCESS) {
        return;
    }
    const uint32_t selfRankId = hccl.GetRankId();
    // When the communication task is "AlltoAll=level0:fullmesh;level1:pairwise":
    // 1. The number of data blocks generated in each communication step equals STEP_SIZE.
    // 2. The total number of communication steps is RANK_NUM/STEP_SIZE*repeat.
    uint16_t sliceInfo[STEP_SIZE];

    if (TILING_KEY_IS(1000UL)) {
        // In the communication-before-computation scenario of communication-computation fusion, each step performs communication first, and then uses the communication output as the computation input to execute computation.
        const auto handleId = hccl.AlltoAllV<true>(sendBuf, sendCounts[selfRankId], sDisplacements[selfRankId], dtype,
                                                   recvBuf, recvCounts[selfRankId], rDisplacements[selfRankId], dtype);
        // The template parameter sync = true indicates that this API blocks and waits for the result of each communication step, and fills the index of the output data block into sliceInfo.
        while (hccl.Iterate<true>(handleId, sliceInfo, sizeof(sliceInfo) / sizeof(sliceInfo[0]))) {
            // The subscript index of the output data block of each communication step is stored in sliceInfo, and the corresponding computation process can be inserted to implement fine-grained communication-computation fusion.
        }
        // Iterate already blocks and waits, so Wait is no longer needed.
        // hccl.Wait(handleId);
    } else if (TILING_KEY_IS(1001UL)) {
        // The computation-before-communication scenario in communication-computation fusion, where each step first performs computation, then uses the computation result as the communication input and submits the communication transaction.
        const uint8_t tileNum = 2U;
        const auto handleId = hccl.AlltoAllV<false>(sendBuf, sendCounts[selfRankId], sDisplacements[selfRankId], dtype,
                                                    recvBuf, recvCounts[selfRankId], rDisplacements[selfRankId], dtype,
                                                    tileNum);
        for (uint8_t i = 0; i < tileNum; ++i) {
            for (uint8_t j = 0; j < RANK_NUM; ++j) {
                // The template parameter sync = false indicates that this API does not block and wait, but only fills the input data block of the current communication step into sliceInfo.
                if (hccl.Iterate<false>(handleId, sliceInfo, sizeof(sliceInfo) / sizeof(sliceInfo[0])) <= 0) {
                    break;
                }
                // sliceInfo represents a relative address offset, which needs to be combined with sDisplacements to compute the GM address to ensure correct communication input.
                // After computation, inter-core synchronization is required, and then the Commit API notifies the server to perform collective communication.
                hccl.Commit(handleId);
            }
        }
        for (uint8_t i = 0; i < tileNum * RANK_NUM; ++i) {
            hccl.Wait(handleId);
        }
    }
    AscendC::SyncAll<true>();
    hccl.Finalize();
}
```
