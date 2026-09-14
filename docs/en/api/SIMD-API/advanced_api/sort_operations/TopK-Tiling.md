# TopK Tiling

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-08T17:24:18.860Z -->

## Description

Used to obtain the TopK Tiling parameters.

Ascend C provides the TopK Tiling API to help users obtain the Tiling parameters required for TopK kernel computation.

Obtaining the Tiling parameters mainly involves the following two steps:

1.  Obtain the minimum and maximum temporary space sizes required for TopK API computation. Note that this step is not mandatory; it only serves as a reference for reasonably allocating computation space.
2.  Obtain the tiling parameters required by the TopK kernel-side API.

    The TopK Tiling structure is defined as follows. Developers do not need to pay attention to the specific information of this tiling structure; they only need to pass it to the kernel side and then to the TopK high-level API for direct use.

    ```
    struct TopkTiling {
        int32_t tmpLocalSize = 0;
        int32_t allDataSize = 0;
        int32_t innerDataSize = 0;
        uint32_t sortRepeat = 0;
        int32_t mrgSortRepeat = 0;
        int32_t kAlignFourBytes = 0;
        int32_t kAlignTwoBytes = 0;
        int32_t maskOffset = 0;
        int32_t maskVreducev2FourBytes = 0;
        int32_t maskVreducev2TwoBytes = 0;
        int32_t mrgSortSrc1offset = 0;
        int32_t mrgSortSrc2offset = 0;
        int32_t mrgSortSrc3offset = 0;
        int32_t mrgSortTwoQueueSrc1Offset = 0;
        int32_t mrgFourQueueTailPara1 = 0;
        int32_t mrgFourQueueTailPara2 = 0;
        int32_t srcIndexOffset = 0;
        uint32_t copyUbToUbBlockCount = 0;
        int32_t topkMrgSrc1MaskSizeOffset = 0;
        int32_t topkNSmallSrcIndexOffset = 0;
        uint32_t vreduceValMask0 = 0;
        uint32_t vreduceValMask1 = 0;
        uint32_t vreduceIdxMask0 = 0;
        uint32_t vreduceIdxMask1 = 0;
        uint16_t vreducehalfValMask0 = 0;
        uint16_t vreducehalfValMask1 = 0;
        uint16_t vreducehalfValMask2 = 0;
        uint16_t vreducehalfValMask3 = 0;
        uint16_t vreducehalfValMask4 = 0;
        uint16_t vreducehalfValMask5 = 0;
        uint16_t vreducehalfValMask6 = 0;
        uint16_t vreducehalfValMask7 = 0;
    };
    ```

## Prototype

```
bool GetTopKMaxMinTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter, const bool isReuseSource, const bool isInitIndex, enum TopKMode mode, const bool isLargest, const uint32_t dataTypeSize, uint32_t& maxValue, uint32_t& minValue)
```

```
bool GetTopKMaxMinTmpSize(const int32_t inner, const int32_t outter, const int32_t k, const bool isReuseSource, const bool isInitIndex, enum TopKMode mode, const bool isLargest, ge::DataType dataType, const TopKConfig& config, uint32_t& maxValue, uint32_t& minValue)
```

```
bool TopKTilingFunc(const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex, enum TopKMode mode, const bool isLargest, optiling::TopkTiling& topKTiling)
```

```
bool TopKTilingFunc(const platform_ascendc::PlatformAscendC& ascendcPlatform, const int32_t inner, const int32_t outter, const int32_t k, const uint32_t dataTypeSize, const bool isInitIndex, enum TopKMode mode, const bool isLargest, AscendC::tiling::TopkTiling& topKTiling)
```

## Parameters

**Table 1**  GetTopKMaxMinTmpSize API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes the hardware platform information. For the PlatformAscendC definition, see [Constructors and Destructors](../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| inner | Input | Indicates the inner axis length of the TopK API input srcLocal. The value of this parameter is an integer multiple of 32. |
| outter | Input | Indicates the outer axis length of the TopK API input srcLocal. |
| k | Input | Retrieves the top k largest or smallest values and their corresponding indices. |
| isReuseSource | Input | Whether the intermediate variable can reuse the input memory. Keep it consistent with the isReuseSrc of the kernel-side API. |
| isInitIndex | Input | Whether to pass in the indices corresponding to the input data. Keep it consistent with the kernel-side API. |
| mode | Input | Selects the TopKMode::TOPK_NORMAL mode or the TopKMode::TOPK_NSMALL mode. Keep it consistent with the kernel-side API. |
| isLargest | Input | Indicates descending order/ascending order. true indicates descending order, and false indicates ascending order. Keep it consistent with the kernel-side API. |
| dataType | Input | Indicates the data type of the data to be sorted. The value of this parameter must be consistent with the data type of the kernel API parameter srcLocal. |
| config | Input | Related configuration for TopK computation. The TopKConfig type is defined as shown in the following code, including algorithm selection, retrieving the maximum or minimum values, and whether to sort the results. The configuration of this parameter must be consistent with the configuration of the TopK kernel API template parameters.<br>algo: The sorting algorithm selected. The default is the MERGE_SORT algorithm. Currently, only the RADIX_SELECT algorithm is supported, and users need to explicitly specify algo as TopKAlgo::RADIX_SELECT.<br>order: Indicates retrieval of the top k maximum values or the top k minimum values. The values are as follows: UNSET: Default value, implemented according to the configuration of the function parameter isLargest. When isLargest is true, the top k maximum values and their corresponding indices are retrieved; when isLargest is false, the top k minimum values and their corresponding indices are retrieved. LARGEST: Indicates retrieval of the top k maximum values and their corresponding indices. When the value is LARGEST, the configuration of the function parameter isLargest does not take effect. SMALLEST: Indicates retrieval of the top k minimum values and their corresponding indices. When the value is SMALLEST, the configuration of the function parameter isLargest does not take effect.<br>sorted: Indicates whether to sort the output results. When the value is true, the output results are sorted; when the value is false, the output results are not sorted. |
| dataTypeSize | Input | The size of the srcLocal data type involved in the computation, for example, half=2, float=4. |
| maxValue | Output | The maximum temporary space size required for the TopK API to complete the computation internally, in bytes.<br> Note: maxValue is only a reference value and may be larger than the remaining space of the Unified Buffer. In this scenario, developers need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | The minimum temporary space size required for the TopK API to complete the computation internally, in bytes. |

```
struct TopKConfig {
    TopKAlgo algo = TopKAlgo::MERGE_SORT;
    TopKOrder order = TopKOrder::UNSET;
    bool sorted = true;
};
enum class TopKAlgo {
    RADIX_SELECT,
    MERGE_SORT
};
enum class TopKOrder {
    UNSET,
    LARGEST,
    SMALLEST
};
```

**Table 2**  TopKTilingFunc API parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **ascendcPlatform** | Input | Passes in the hardware platform information. For the definition of **PlatformAscendC**, see [Constructors and Destructors](../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| **inner** | Input | Indicates the inner-axis length of the **srcLocal** input of the TopK API. The value of this parameter is a multiple of 32. |
| **outter** | Input | Indicates the outer-axis length of the **srcLocal** input of the TopK API. |
| **k** | Input | Retrieves the top k largest or smallest values and their corresponding indexes. |
| **dataTypeSize** | Input | Size of the **srcLocal** data type involved in the computation, for example, half = 2 and float = 4. |
| **isInitIndex** | Input | Whether to pass in the indexes corresponding to the input data, consistent with the kernel-side API. |
| **mode** | Input | Selects **TopKMode::TOPK_NORMAL** or **TopKMode::TOPK_NSMALL** mode, consistent with the kernel-side API. |
| **isLargest** | Input | Indicates descending/ascending order. **true** indicates descending order, and **false** indicates ascending order. Consistent with the kernel-side API. |
| **topKTiling** | Output | Outputs the tiling information required by the TopK API. |

## Return Value

**GetTopKMaxMinTmpSize** returns **true**/**false**. **true** indicates that the maximum and minimum temporary space sizes required for the internal computation of the TopK API are successfully obtained; **false** indicates retrieval failure.

**TopKTilingFunc** returns **true**/**false**. **true** indicates that all tiling parameter values of TopK are successfully obtained; **false** indicates retrieval failure.

## Constraints

None

## Examples

The following example describes the process of obtaining tiling parameters on the host side when using the TopK high-level API, and how these parameters are used on the kernel side.

1. Add the TopK Tiling structure parameters to the TilingData structure as a field of the TilingData structure.

    ```
    namespace optiling {
    BEGIN_TILING_DATA_DEF(TilingData)
        TILING_DATA_FIELD_DEF(uint32_t, totalLength);
        TILING_DATA_FIELD_DEF(uint32_t, tilenum);
        // Add other tiling fields.
        ...
        TILING_DATA_FIELD_DEF(int32_t, k);
        TILING_DATA_FIELD_DEF(bool, islargest);
        TILING_DATA_FIELD_DEF(bool, isinitindex);
        TILING_DATA_FIELD_DEF(bool, ishasfinish);
        TILING_DATA_FIELD_DEF(uint32_t, tmpsize);
        TILING_DATA_FIELD_DEF(int32_t, outter);
        TILING_DATA_FIELD_DEF(int32_t, inner);
        TILING_DATA_FIELD_DEF(int32_t, n);
        TILING_DATA_FIELD_DEF(int32_t, order);
        TILING_DATA_FIELD_DEF(int32_t, sorted);
        TILING_DATA_FIELD_DEF_STRUCT(TopkTiling, topkTilingData);
    END_TILING_DATA_DEF;
    REGISTER_TILING_DATA_CLASS(TopkCustom, TilingData)
    }
    ```

2. In the Tiling implementation function, first call the **GetTopKMaxMinTmpSize** API to obtain the maximum/minimum temporary space sizes required for the TopK API to complete the computation, and set an appropriate space size based on this range and the actual memory usage. Then, obtain the tiling parameters required by the TopK kernel-side API based on the input shape and other information. MERGE\_SORT For the algorithm, see the following example.

    ```
    namespace optiling {
    const uint32_t NUM_BLOCKS = 8;
    const uint32_t TILE_NUM = 8;
    const int32_t OUTTER = 2;
    const int32_t INNER = 32;
    const int32_t N = 32;
    const int32_t K = 8;
    const bool IS_LARGEST = true;
    const bool IS_INITINDEX = true;
    const bool IS_REUSESOURCE = false;
    static ge::graphStatus TilingFunc(gert::TilingContext* context)
    {
        TilingData tiling;
        uint32_t totalLength = context->GetInputTensor(0)->GetShapeSize();
        context->SetBlockDim(NUM_BLOCKS);
        tiling.set_totalLength(totalLength);
        tiling.set_tileNum(TILE_NUM);
        tiling.set_k(K);
        tiling.set_outter(OUTTER);
        tiling.set_inner(INNER);
        tiling.set_n(N);
        tiling.set_islargest(IS_LARGEST);
        tiling.set_isinitindex(IS_INITINDEX);
        // Set other Tiling parameters.
        ...
        // This sample is for illustration only. The minimum value obtained by GetTopKMaxMinTmpSize is passed in to ensure correctness. Developers can pass in an appropriate space size as needed.
        uint32_t maxsize = 0;
        uint32_t minsize = 0;
        uint32_t dtypesize = 4;  // float type
        auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
        AscendC::TopKTilingFunc(ascendcPlatform, tiling.inner, tiling.outter, tiling.k, dtypesize, tiling.isinitindex, AscendC::TopKMode::TOPK_NSMALL, tiling.islargest, tiling.topkTilingData);
        AscendC::GetTopKMaxMinTmpSize(ascendcPlatform, tiling.inner, tiling.outter, IS_REUSESOURCE, tiling.isinitindex, AscendC::TopKMode::TOPK_NSMALL, tiling.islargest, dtypesize, maxsize, minsize);
        tiling.set_tmpsize(minsize);
         ... // Other logic.
        tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
        context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
        size_t *currentWorkspace = context->GetWorkspaceSizes(1);
        currentWorkspace[0] = 0;
        return ge::GRAPH_SUCCESS;
    }
    } // namespace optiling
    ```

    RADIX\_SELECT Refer to the following example for the algorithm.

    ```
    namespace optiling
    {
        static ge::graphStatus TilingFunc(gert::TilingContext *context)
        {
            std::map<ge::DataType, uint32_t> dtypeSizes = {
                {ge::DataType::DT_UINT32, 4},
                {ge::DataType::DT_INT32, 4}
            };
            RadixtopkCustomTilingData tiling;
            const gert::RuntimeAttrs *attrs = context->GetAttrs();
            const uint32_t is_init_index = *(attrs->GetAttrPointer<uint32_t>(0));
            const uint32_t is_reuse_src = *(attrs->GetAttrPointer<uint32_t>(1));
            const uint32_t order = *(attrs->GetAttrPointer<uint32_t>(2));
            const uint32_t is_largest = *(attrs->GetAttrPointer<uint32_t>(3));
            const uint32_t outter = *(attrs->GetAttrPointer<uint32_t>(4));
            const uint32_t inner = *(attrs->GetAttrPointer<uint32_t>(5));
            const uint32_t n = *(attrs->GetAttrPointer<uint32_t>(6));
            const uint32_t k = *(attrs->GetAttrPointer<uint32_t>(7));
            const uint32_t k_pad = *(attrs->GetAttrPointer<uint32_t>(8));
            const uint32_t sorted = *(attrs->GetAttrPointer<uint32_t>(9));
            const uint32_t top_mode = *(attrs->GetAttrPointer<uint32_t>(10));

            auto xDType = context->GetInputTensor(0)->GetDataType();
            uint32_t typeSize = dtypeSizes.at(xDType);
            AscendC::TopKConfig config;
            config.algo = AscendC::TopKAlgo::RADIX_SELECT;
            if (order == 1) {
                config.order = AscendC::TopKOrder::LARGEST;
            } else if (order == 2) {
                config.order = AscendC::TopKOrder::SMALLEST;
            } else {
                config.order = AscendC::TopKOrder::UNSET;
            }
            if (sorted == 0) {
                config.sorted = false;
            } else {
                config.sorted = true;
            }
            uint32_t maxValue = 0;
            uint32_t minValue = 0;

            if (top_mode == 0) {
                AscendC::GetTopKMaxMinTmpSize(inner, outter, k, is_reuse_src, is_init_index, AscendC::TopKMode::TOPK_NORMAL, is_largest, xDType, config, maxValue, minValue);
                context->SetTilingKey(0);
            } else {
                AscendC::GetTopKMaxMinTmpSize(inner, outter, k, is_reuse_src, is_init_index, AscendC::TopKMode::TOPK_NSMALL, is_largest, xDType, config, maxValue, minValue);
                context->SetTilingKey(1);
            }
            context->SetBlockDim(1);
            tiling.set_is_init_index(is_init_index);
            tiling.set_is_reuse_src(is_reuse_src);
            tiling.set_order(order);
            tiling.set_is_largest(is_largest);
            tiling.set_outter(outter);
            tiling.set_inner(inner);
            tiling.set_n(n);
            tiling.set_k(k);
            tiling.set_k_pad(k_pad);
            tiling.set_sorted(sorted);
            tiling.set_top_mode(top_mode);
            tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
            context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
            size_t *currentWorkspace = context->GetWorkspaceSizes(1);
            currentWorkspace[0] = 0;
            return ge::GRAPH_SUCCESS;
        }
    } // namespace optiling
    ```

3.  On the corresponding kernel side, call GET\_TILING\_DATA in the kernel function to obtain TilingData, and then pass the TopK Tiling information in TilingData to the TopK API for computation. For the complete kernel-side example, see [Examples](TopK.md#examples).

    ```
    extern "C" __global__ __aicore__ void topk_custom(GM_ADDR srcVal, GM_ADDR srcIdx, GM_ADDR finishLocal, GM_ADDR dstVal, GM_ADDR dstIdx, GM_ADDR tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        KernelTopK<float, true, true, false, false, AscendC::TopKMode::TOPK_NSMALL> op;
        op.Init(srcVal, srcIdx, finishLocal, dstVal, dstIdx, tilingData.k, tilingData.islargest, tilingData.tmpsize, tilingData.outter, tilingData.inner, tilingData.n,tilingData.topkTilingData);
        op.Process();
    }
    ```
