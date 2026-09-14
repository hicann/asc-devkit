# AllReduce

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-10T02:11:56.234Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Task delivery API of the **AllReduce** collective communication operator, which returns the task identifier **handleId** to the user. The **AllReduce** function performs a reduce operation on the tensors with the same name across all nodes in the communicator, and then sends the result to the output buffer of all nodes.

![](../../../../figures/allreduce.png)

## Prototype

```
template <bool commit = false>
__aicore__ inline HcclHandle AllReduce(GM_ADDR sendBuf, GM_ADDR recvBuf, uint64_t count, HcclDataType dataType, HcclReduceOp op, uint8_t repeat = 1)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| commit | Input | bool type. The value is as follows:<br>**true**: When the **Prepare** API is called, Commit synchronously notifies the server that this communication task can be executed.<br>**false**: When the **Prepare** API is called, the server is not notified to execute this communication task. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| sendBuf | Input | Source data buffer address. |
| recvBuf | Output | Destination data buffer address. The collective communication result is output to this buffer. |
| count | Input | Number of data elements participating in the AllReduce operation. For example, if only one int32 data element participates, count=1. |
| dataType | Input | Data type of the AllReduce operation. Currently, float, half (that is, float16), int8_t, int16_t, int32_t, and bfloat16_t are supported, that is, the supported values are **HCCL_DATA_TYPE_FP32**, **HCCL_DATA_TYPE_FP16**, **HCCL_DATA_TYPE_INT8**, **HCCL_DATA_TYPE_INT16**, **HCCL_DATA_TYPE_INT32**, and **HCCL_DATA_TYPE_BFP16**. For details about the **HcclDataType** data type, see [Table 1](hccl_usage.md#table116710585514). |
| op | Input | Reduce operation type. Currently, sum, max, and min are supported, that is, the supported values are **HCCL_REDUCE_SUM**, **HCCL_REDUCE_MAX**, and **HCCL_REDUCE_MIN**. For details about the **HcclReduceOp** data type, see [Table 2](hccl_usage.md#hcclreduceop). |
| repeat | Input | Number of **AllReduce** communication tasks delivered at a time. The value of repeat must be greater than or equal to 1, and the default value is 1. When repeat > 1, the sendBuf and recvBuf addresses of each AllReduce task are automatically calculated by the server using the following formulas:<br><br>sendBuf[i] = sendBuf + count* sizeof(datatype) * i, i∈[0, repeat)<br><br>recvBuf[i] = recvBuf + count* sizeof(datatype) * i, i∈[0, repeat)<br><br>Note: When **repeat>1** is set, it must be used together with the count parameter to plan the communication data addresses. |

**Figure 1** AllReduce three-round slicing communication example  
![AllReduce three-round slicing communication example](../../../../figures/allreduce_three_round_slicing_communication_example.png)

## Return Value

Returns the identifier **handleId** of the task, where **handleId** is greater than or equal to 0. If the call fails, -1 is returned.

## Constraints

-   Before calling this API, ensure that [InitV2](InitV2.md) and [SetCcTilingV2](SetCcTilingV2.md) have been called.
-   If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object does not specify the core that issues the communication task, this API can be called only on either the AIC core or the AIV core. If the [config template parameter](hccl_template_parameter.md#hccl-template-params) of the HCCL object specifies the core that issues the communication task, this API can be called on both the AIC core and the AIV core at the same time. Internally, the API issues the communication task on only one of the AIC core and the AIV core based on the type of the specified core.
-   For Atlas A2 training products/Atlas A2 inference products, the total number of calls to all Prepare APIs within a communicator must not exceed 63.
-   For Atlas A3 training products/Atlas A3 inference products, the total number of calls to all Prepare APIs and the InterHcclGroupSync API within a communicator must not exceed 63.
-   For Ascend 950PR/Ascend 950DT, the total number of calls to all Prepare APIs within a communicator must not exceed 63.
-   For Ascend 950PR/Ascend 950DT, when the communication server is the CCU, the maximum amount of communication data in a single operation must not exceed 256M.

## Examples

-   Non-multi-round slicing scenario

    As shown in the following figure, each of the four cards has count=300 float16 data elements. Each card obtains its own data from the xGM memory, performs a reduce sum computation on the per-card data, and outputs the result to the yGM of each card.

    **Figure 2**  AllReduce communication across four cards in the non-multi-round slicing scenario

    ![](../../../../figures/250902140829537_gai.png)

    ```
    extern "C" __global__ __aicore__ void all_reduce_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        auto sendBuf = xGM;  // xGM is the input GM address for AllReduce.
        auto recvBuf = yGM;  // yGM is the output GM address for AllReduce.
        uint64_t sendCount = 300;  // Each card has 300 float16 data elements.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        REGISTER_TILING_DEFAULT(AllReduceCustomTilingData); //AllReduceCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllReduceCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.

        if (AscendC::g_coreType == AIV) {  // Specify communication on the AIV core.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllReduceCustomTilingData, mc2CcTiling));
            if (ret) {
                return;
            }
            HcclHandle handleId1 = hccl.AllReduce<true>(sendBuf, recvBuf, sendCount, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp);
            hccl.Wait(handleId1);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```

-   Multi-round slicing scenario

    With multi-round slicing enabled, the communication in the non-multi-round slicing example above is equivalently processed. As shown in the following figure, the 300 float16 data elements on each card are sliced into two head blocks and one tail block. The data volume of each head block, tileLen, is 128 float16 data elements, and the data volume of the tail block, tailLen, is 44 float16 data elements. During the internal implementation of the operator, the sliced data must be processed through three rounds of AllReduce communication tasks, which is equivalent to the communication result of the non-multi-round slicing example above.

    **Figure 3**  Per-card data slicing diagram  
    ![Per-card data slicing diagram](../../../../figures/per_card_data_slicing_diagram.png)

    The specific implementation is as follows: in the first round of communication, the 0-0\\1-0\\2-0\\3-0 data block on each rank is processed by AllReduce. In the second round of communication, the 0-1\\1-1\\2-1\\3-1 data block on each rank is processed by AllReduce. In the third round of communication, the 0-2\\1-2\\2-2\\3-2 data block on each rank is processed by AllReduce. The diagram and code example are as follows.

    **Figure 4**  4-card AllReduce schematic diagram  
    ![4-card AllReduce schematic diagram](../../../../figures/4_card_allreduce_schematic_diagram.png)

    ```
    extern "C" __global__ __aicore__ void all_reduce_custom(GM_ADDR xGM, GM_ADDR yGM, GM_ADDR workspaceGM, GM_ADDR tilingGM) {
        constexpr uint32_t tileNum = 2U;   // Number of head blocks.
        constexpr uint64_t tileLen = 128U; // Number of data elements in a head block.
        constexpr uint32_t tailNum = 1U;   // Number of tail blocks.
        constexpr uint64_t tailLen = 44U;  // Number of tail block data elements.
        auto sendBuf = xGM;  // xGM is the input GM address for AllReduce.
        auto recvBuf = yGM;  // yGM is the output GM address for AllReduce.
        HcclReduceOp reduceOp = HcclReduceOp::HCCL_REDUCE_SUM;
        REGISTER_TILING_DEFAULT(AllReduceCustomTilingData); //AllReduceCustomTilingData is the structure defined in the corresponding operator header file.
        GET_TILING_DATA_WITH_STRUCT(AllReduceCustomTilingData, tilingData, tilingGM);

        Hccl hccl;
        GM_ADDR contextGM = AscendC::GetHcclContext<0>();  // In the AscendC custom operator kernel, obtain the HCCL context in this way.
        if (AscendC::g_coreType == AIV) {  // Specify communication on the AIV core.
            hccl.InitV2(contextGM, &tilingData);
            auto ret = hccl.SetCcTilingV2(offsetof(AllReduceCustomTilingData, mc2CcTiling));
            if (ret != HCCL_SUCCESS) {
                return;
            }
            // Process the two head blocks.
            constexpr uint32_t tileRepeat = tileNum;
            // Except for the different sendBuf and recvBuf input parameters, the remaining parameters for processing the two head blocks are the same. Therefore, repeat=2 is used, and the sendBuf and recvBuf of the AllReduce task for the second head block are updated internally by the API.
            HcclHandle handleId1 = hccl.AllReduce<true>(sendBuf, recvBuf, tileLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, tileRepeat);
            // Process one tail block.
            constexpr uint32_t kSizeOfFloat16 = 2U;
            sendBuf += tileLen * tileNum * kSizeOfFloat16;
            recvBuf += tileLen * tileNum * kSizeOfFloat16;
            constexpr uint32_t tailRepeat = tailNum;
            HcclHandle handleId2 = hccl.AllReduce<true>(sendBuf, recvBuf, tailLen, HcclDataType::HCCL_DATA_TYPE_FP16, reduceOp, tailRepeat);

            for (uint8_t i=0; i<tileRepeat; i++) {
                hccl.Wait(handleId1);
            }
            hccl.Wait(handleId2);
            AscendC::SyncAll<true>();  // Synchronize all AIV cores to prevent core 0 from executing too fast and calling hccl.Finalize() prematurely, which would cause other cores to hang on Wait.
            hccl.Finalize();
        }
    }
    ```
