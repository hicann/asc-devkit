# Iterate

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T05:34:51.008Z pushedAt=2026-11-18T04:56:55.640Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/HCCL_communication/HCCL_Kernel/Iterate_res.md#id1 -->

## Description

Under certain algorithms, a complete collective communication task can be subdivided into multiple steps, where a point-to-point communication task is completed for the data of each step. This is called fine-grained communication. Taking the AlltoAllV communication task with the communication algorithm "AlltoAll=level0:fullmesh;level1:pairwise" and a communication step size of 1 as an example, the parameter **level0** configures the intra-server communication algorithm (Ascend AI Server, a general term for servers composed of Ascend NPU devices, typically with 8 or 16 cards), and the parameter **level1** configures the inter-server communication algorithm. **fullmesh** is a fully connected communication algorithm, and **pairwise** is a pairwise communication algorithm. As shown in the following figure, this example illustrates all the data to be sent in the AlltoAllV communication and the data received by each card after each communication step is completed.

**Figure 1**  Schematic diagram of AlltoAllV communication steps using the pairwise algorithm  
![](../../../../figures/alltoallv_pairwise_steps.png "Schematic diagram of AlltoAllV communication steps using the pairwise algorithm")

In a communication-computation fusion operator, call this API together with the corresponding Prepare primitive to obtain the input or output of each communication step, so that computation and communication can be pipelined at a finer granularity and achieve better performance gains.

## Prototype

```
template <bool sync = true>
__aicore__ inline int32_t Iterate(HcclHandle handleId, uint16_t* seqSlices, uint16_t seqSliceLen)
```

## Parameters

**Table 1** Template parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sync | Input | bool type. Whether to wait for the current communication step to complete before performing subsequent computation or communication tasks. The values are as follows:<br>true: Default value, indicating blocking and waiting for the current communication step to complete. When this parameter is set to true, you do not need to call the [Wait](Wait.md) API to wait for the communication task to complete.<br>false: Indicates not waiting for the current communication step to complete. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| handleId | Input | Identifier ID of the corresponding communication task. Only the return value of the **Prepare** primitive API can be used.<br>using HcclHandle = int8_t; |
| seqSlices | Output | Stack space applied by the user, used to save the index subscripts of the input or output data blocks of the current communication step. In the compute-before-communication scenario, this parameter returns the input data block indexes required by the current communication step. In the communication-before-compute scenario, this parameter returns the output data block indexes of the current communication step. |
| seqSliceLen | Input | Length of the seqSlices array. Based on the communication step size and algorithm logic, the number of data block indexes to be saved in each communication step is used as the array length. |

## Return Value

-   When the communication task has not ended:
    -   In the compute-before-communication scenario, the return value is the number of input data blocks required by the current communication step, which is the same as the value of the seqSliceLen parameter.
    -   In the communication-before-compute scenario, the return value is the number of output data blocks produced by the current communication step, which is the same as the value of the seqSliceLen parameter.

-   After the communication task ends, the return value is 0.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   The input parameter **handleId** can only use the return value of the corresponding Prepare primitive API.
-   The communication algorithm currently supported by this API is "AlltoAll=level0:fullmesh;level1:pairwise".

## Examples

```
extern "C" __global__ __aicore__ void alltoallv_custom(
    GM_ADDR sendBuf, GM_ADDR recvBuf, GM_ADDR workspaceGM, GM_ADDR tilingGM)
{
    // Specify AIV core communication.
    if (AscendC::g_coreType != AIV) {
        return;
    }

    constexpr uint32_t RANK_NUM = 4U;
    constexpr uint32_t STEP_SIZE = 1U; // Fine-grained communication step size, usually set by the SetStepSize API. Simplified to 1 in this example.
    constexpr uint64_t sendCounts[RANK_NUM][RANK_NUM] = {{3, 3, 3, 3}, {2, 2, 3, 2}, {1, 4, 4, 4}, {3, 3, 3, 3}};
    constexpr uint64_t sDisplacements[RANK_NUM][RANK_NUM] = {{0, 3, 6, 9}, {0, 2, 4, 7}, {0, 1, 5, 9}, {0, 3, 6, 9}};
    constexpr uint64_t recvCounts[RANK_NUM][RANK_NUM] = {{3, 2, 1, 3}, {3, 2, 4, 3}, {3, 3, 4, 3}, {3, 2, 4, 3}};
    constexpr uint64_t rDisplacements[RANK_NUM][RANK_NUM] = {{0, 3, 5, 6}, {0, 3, 5, 9}, {0, 3, 6, 10}, {0, 3, 5, 9}};
    HcclDataType dtype = HcclDataType::HCCL_DATA_TYPE_FP16;
    REGISTER_TILING_DEFAULT(AllToAllVCustomTilingData); // AllToAllVCustomTilingData is the structure defined in the corresponding operator header file.
    GET_TILING_DATA_WITH_STRUCT(AllToAllVCustomTilingData, tilingData, tilingGM);
    GM_ADDR contextGM = AscendC::GetHcclContext<0>(); // In the AscendC custom operator kernel function (Kernel), obtain the HCCL context in this way.
    Hccl hccl;
    hccl.InitV2(contextGM, &tilingData);
    auto ret = hccl.SetCcTilingV2(offsetof(AllToAllVCustomTilingData, alltoallvCcTiling));
    if (ret != HCCL_SUCCESS) {
        return;
    }
    const uint32_t selfRankId = hccl.GetRankId();
    // When the communication task is "AlltoAll=level0:fullmesh;level1:pairwise".
    // 1. The number of data blocks generated in each communication step equals STEP_SIZE.
    // 2. The total number of communication steps is RANK_NUM/STEP_SIZE*repeat.
    uint16_t sliceInfo[STEP_SIZE];

    if (TILING_KEY_IS(1000UL)) {
        // The "communicate first, then compute" scenario in communication-computation fusion, where each step first communicates, then uses the communication output as the compute input and performs the computation.
        const auto handleId = hccl.AlltoAllV<true>(
            sendBuf, sendCounts[selfRankId], sDisplacements[selfRankId], dtype, recvBuf, recvCounts[selfRankId],
            rDisplacements[selfRankId], dtype);
        // The template parameter sync = true indicates that this API blocks and waits for the result of each communication step, and fills the index of the output data block into sliceInfo.
        while (hccl.Iterate<true>(handleId, sliceInfo, sizeof(sliceInfo) / sizeof(sliceInfo[0]))) {
            // The output data block index of each communication step is stored in sliceInfo. You can insert it into the corresponding computation flow to implement fine-grained communication-computation fusion.
        }
        // Iterate already blocks and waits, so Wait is no longer needed.
        // hccl.Wait(handleId);
    } else if (TILING_KEY_IS(1001UL)) {
        // In the "compute first, then communicate" scenario of communication-computation fusion, each step first performs computation, then uses the computation result as the communication input and submits the communication transaction.
        const uint8_t tileNum = 2U;
        const auto handleId = hccl.AlltoAllV<false>(
            sendBuf, sendCounts[selfRankId], sDisplacements[selfRankId], dtype, recvBuf, recvCounts[selfRankId],
            rDisplacements[selfRankId], dtype, tileNum);
        for (uint8_t i = 0; i < tileNum; ++i) {
            for (uint8_t j = 0; j < RANK_NUM; ++j) {
                // The template parameter sync = false indicates that this API does not block and wait, but only fills the input data block of the current communication step into sliceInfo.
                if (hccl.Iterate<false>(handleId, sliceInfo, sizeof(sliceInfo) / sizeof(sliceInfo[0])) <= 0) {
                    break;
                }
                // sliceInfo represents a relative address offset. It must be combined with sDisplacements to compute the GM address, ensuring that the communication input is correct.
                // After computation, inter-core synchronization is required, and then the Commit API is used to notify the server to perform collective communication.
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
