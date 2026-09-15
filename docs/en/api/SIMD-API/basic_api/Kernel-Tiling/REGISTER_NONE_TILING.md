# REGISTER\_NONE\_TILING<a name="ZH-CN_TOPIC_0000002473236240"></a>

<!-- md-trans-meta sourceCommit=9f6ce11d8b0e7d296d9c9a7af96f6025bf5b48d1 translatedAt=2026-08-27T18:15:24.616Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

When using a TilingData structure customized with standard C++ syntax on the Kernel side, if you are unsure which structures need to be registered, you can use this API to inform the framework side that unregistered standard C++ syntax should be used to define TilingData, and use it together with [GET\_TILING\_DATA\_WITH\_STRUCT](GET_TILING_DATA_WITH_STRUCT.md), [GET\_TILING\_DATA\_MEMBER](GET_TILING_DATA_MEMBER.md), and [GET\_TILING\_DATA\_PTR\_WITH\_STRUCT](GET_TILING_DATA_PTR_WITH_STRUCT.md) to obtain the corresponding TilingData.

## Prototype<a name="zh-cn_topic_0000001526206862_section1630753514297"></a>

```
REGISTER_NONE_TILING
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

None

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

-   Kernel direct invocation projects are not supported yet.
-   Using [GET_TILING_DATA](GET_TILING_DATA.md) requires a TilingData structure registered by default, but this API does not register a TilingData structure. Therefore, it cannot be used in combination with [5.11.1-GET_TILING_DATA](GET_TILING_DATA.md).
-   It cannot be mixed with [REGISTER_TILING_DEFAULT](REGISTER_TILING_DEFAULT.md) or [REGISTER_TILING_FOR_TILINGKEY](REGISTER_TILING_FOR_TILINGKEY.md). That is, scenarios where a TilingData structure is registered cannot be mixed with scenarios where it is not registered.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
# The Tiling template library provider cannot predict which TilingData structure the user instantiates.
template <class BrcDag>
struct BroadcastBaseTilingData {
    int32_t scheMode;
    int32_t shapeLen;
    int32_t ubSplitAxis;
    int32_t ubFormer;
    int32_t ubTail;
    int64_t ubOuter;
    int64_t blockFormer;
    int64_t blockTail;
    int64_t dimProductBeforeUbInner;
    int64_t elemNum;
    int64_t blockNum;
    int64_t outputDims[BROADCAST_MAX_DIMS_NUM];
    int64_t outputStrides[BROADCAST_MAX_DIMS_NUM];
    int64_t inputDims[BrcDag::InputSize][2]; // Whole block + tail block.
    int64_t inputBrcDims[BrcDag::CopyBrcSize][BROADCAST_MAX_DIMS_NUM];
    int64_t inputVecBrcDims[BrcDag::VecBrcSize][BROADCAST_MAX_DIMS_NUM];
    int64_t inputStrides[BrcDag::InputSize][BROADCAST_MAX_DIMS_NUM];
    int64_t inputBrcStrides[BrcDag::CopyBrcSize][BROADCAST_MAX_DIMS_NUM];
    int64_t inputVecBrcStrides[BrcDag::VecBrcSize];
    char scalarData[BROADCAST_MAX_SCALAR_BYTES];
};

template <uint64_t schMode, class BrcDag> class BroadcastSch {
public:
    __aicore__ inline explicit BroadcastSch(GM_ADDR& tmpTiling)
        : tiling(tmpTiling)
    {}
    template <class... Args>
    __aicore__ inline void Process(Args... args)
    {
        REGISTER_NONE_TILING; // Inform the framework side to use the unregistered TilingData structure.
        if constexpr (schMode == 1) {
            GET_TILING_DATA_WITH_STRUCT(BroadcastBaseTilingData<BrcDag>, tilingData, tiling);
            GET_TILING_DATA_MEMBER(BroadcastBaseTilingData<BrcDag>, blockNum, blockNumVar, tiling);
            TPipe pipe;
            BroadcastNddmaSch<BrcDag, false> sch(&tilingData); // Obtain the Schedule.
            sch.Init(&pipe, args...);
            sch.Process();
        }   else if constexpr (schMode == 202) {
            GET_TILING_DATA_PTR_WITH_STRUCT(BroadcastOneDimTilingDataAdvance, tilingDataPtr, tiling);
            BroadcastOneDimAdvanceSch<BrcDag> sch(tilingDataPtr); // Obtain the Schedule.
            sch.Init(args...);
            sch.Process();
        }
    }
public:
    GM_ADDR tiling;
};
```

```
#The user instantiates the template library by passing the schMode and OpDag template parameters.
using namespace AscendC;
template <uint64_t schMode>
__global__ __aicore__ void mul(GM_ADDR x1, GM_ADDR x2, GM_ADDR y, GM_ADDR workspace, GM_ADDR tiling)
{
    if constexpr (std::is_same<DTYPE_X1, int8_t>::value) {
        // int8
        using OpDag = MulDag::MulInt8Op::OpDag;
        BroadcastSch<schMode, OpDag> sch(tiling);
        sch.Process(x1, x2, y);
    } else if constexpr (std::is_same<DTYPE_X1, uint8_t>::value) {
        // uint8
        using OpDag = MulDag::MulUint8Op::OpDag;
        BroadcastSch<schMode, OpDag> sch(tiling);
        sch.Process(x1, x2, y);
    }
}
```

