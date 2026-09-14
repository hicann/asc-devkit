# More Examples

<!-- md-trans-meta sourceCommit=813f9eca2d93c325faf3d1705f71ca2f52aff5dd translatedAt=2026-09-10T11:23:37.220Z -->

## Example 1<a name="section5279737173215"></a>

The following example demonstrates how to use the math library kernel-side API together with the Tiling API GetXxxMaxMinTmpSize. The specific process is as follows:

On the Host side, the Tiling API is called to obtain the size of the required temporary space, which is then written into the tiling data. On the kernel side, the tiling data is read to obtain the corresponding temporary space size, based on which the temporary space is allocated.

Host-side Tiling API usage example:

```
#include <vector>

#include "register/op_def_registry.h"
#include "register/tilingdata_base.h"
#include "tiling/tiling_api.h"

namespace optiling {

BEGIN_TILING_DATA_DEF(AsinCustomTilingData)
  TILING_DATA_FIELD_DEF(uint32_t, srcSize);
  TILING_DATA_FIELD_DEF(uint32_t, tmpBufferSize);
END_TILING_DATA_DEF;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    // Input source shapes.
    auto shape_input = context->GetInputTensor(0)->GetOriginShape();
    std::vector<int64_t> srcDims = {shape_input.GetDim(0), shape_input.GetDim(1)};
    uint32_t srcSize = 1;
    for (auto dim : srcDims) {
        srcSize *= dim;
    }
    uint32_t typeSize = 2;
    ge::Shape shape(srcDims);
    uint32_t minValue = 0;
    uint32_t maxValue = 0;
    AscendC::GetAsinMaxMinTmpSize(shape, typeSize, false, maxValue, minValue);

    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining UB space size.
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize); // The full UB space is used in this example. In practice, tailSize needs to subtract the UB space already used by the user.
    auto tmpSize = tailSize >= maxValue ? maxValue : tailSize;

    AsinCustomTilingData tiling;
    tiling.set_srcSize(srcSize);
    tiling.set_tmpBufferSize(tmpSize);
    context->SetBlockDim(1);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    context->SetTilingKey(1);

    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

On the kernel side, the tiling data is read to obtain the corresponding temporary space size, based on which the temporary space is allocated:

```
#include "kernel_operator.h"

template <typename srcType>
class KernelAsin
{
public:
    __aicore__ inline KernelAsin() {}
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t srcSize, uint32_t tmpBufferSize)
    {
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType *>(srcGm), srcSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType *>(dstGm), srcSize);

        pipe.InitBuffer(inQueue, 1, srcSize * sizeof(srcType));
        pipe.InitBuffer(outQueue, 1, srcSize * sizeof(srcType));
        pipe.InitBuffer(tmpBuf, tmpBufferSize);
        bufferSize = srcSize;
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        AscendC::LocalTensor<srcType> srcLocal = inQueue.AllocTensor<srcType>();
        AscendC::DataCopy(srcLocal, srcGlobal, bufferSize);
        inQueue.EnQue(srcLocal);
    }
    __aicore__ inline void Compute()
    {
        AscendC::LocalTensor<srcType> dstLocal = outQueue.AllocTensor<srcType>();

        AscendC::LocalTensor<srcType> srcLocal = inQueue.DeQue<srcType>();
        AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpBuf.Get<uint8_t>();
        AscendC::Asin<srcType, false>(dstLocal, srcLocal, sharedTmpBuffer, bufferSize);

        outQueue.EnQue<srcType>(dstLocal);
        inQueue.FreeTensor(srcLocal);
    }
    __aicore__ inline void CopyOut()
    {
        AscendC::LocalTensor<srcType> dstLocal = outQueue.DeQue<srcType>();
        AscendC::DataCopy(dstGlobal, dstLocal, bufferSize);
        outQueue.FreeTensor(dstLocal);
    }

private:
    AscendC::GlobalTensor<srcType> srcGlobal;
    AscendC::GlobalTensor<srcType> dstGlobal;

    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueue;
    AscendC::TBuf<AscendC::TPosition::VECCALC> tmpBuf;
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueue;
    uint32_t bufferSize = 0;
};

extern "C" __global__ __aicore__ void asin_custom(GM_ADDR srcGm, GM_ADDR dstGm, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    KernelAsin<half> op;
    op.Init(srcGm, dstGm, tilingData.srcSize, tilingData.tmpBufferSize);
    if (TILING_KEY_IS(1))
    {
        op.Process();
    }
}
```

## Example 2<a name="section577043422516"></a>

The following example shows how to use the math library kernel-side API together with [PlatformAscendC::ReserveLocalMemory](../../../Utils-API/platform_info/PlatformAscendC/ReserveLocalMemory.md). The process is as follows:

On the Host side, call the **ReserveLocalMemory** API to reserve Unified Buffer memory space, and call the **GetCoreMemSize** API to obtain the actually available Unified Buffer memory size. Based on the actually available memory size, calculate the maximum **Shape** (maximum data scale) that can be supported. This approach avoids calling the **GetXXXTmpMaxMinSize** API multiple times to obtain an appropriate temporary space size.

Host-side Tiling API usage example:

```
#include <vector>

#include "register/op_def_registry.h"
#include "register/tilingdata_base.h"
#include "tiling/tiling_api.h"

namespace optiling {

BEGIN_TILING_DATA_DEF(MathCustomTilingData)
  TILING_DATA_FIELD_DEF(uint32_t, calSize);
END_TILING_DATA_DEF;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining Unified Buffer space size.
    ascendcPlatform.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_8K);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize);

    uint64_t calSize = tailSize / (3 * 4); // src + dst + tmp float type
    MathCustomTilingData tiling;
    tiling.set_calSize(calSize);
    context->SetBlockDim(1);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    context->SetTilingKey(1);
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

On the kernel side, read the **TilingData** to obtain the corresponding computation scale parameters. Then, the kernel calls the APIs provided by the math library that do not require temporary space to perform various mathematical operations:

```
#include "kernel_operator.h"

template <typename srcType>
class KernelMath
{
public:
    __aicore__ inline KernelMath() {}
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t srcSize)
    {
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType *>(srcGm), srcSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType *>(dstGm), srcSize);

        pipe.InitBuffer(inQueue, 1, srcSize * sizeof(srcType));
        pipe.InitBuffer(outQueue, 1, srcSize * sizeof(srcType));
        pipe.InitBuffer(tmpBuf, srcSize * sizeof(srcType));
        bufferSize = srcSize;
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        AscendC::LocalTensor<srcType> srcLocal = inQueue.AllocTensor<srcType>();
        AscendC::DataCopy(srcLocal, srcGlobal, bufferSize);
        inQueue.EnQue(srcLocal);
    }
    __aicore__ inline void Compute()
    {
        AscendC::LocalTensor<srcType> dstLocal = outQueue.AllocTensor<srcType>();

        AscendC::LocalTensor<srcType> srcLocal = inQueue.DeQue<srcType>();
        AscendC::LocalTensor<srcType> tmp = tmpBuf.Get<srcType>();
        AscendC::Asin<srcType, false>(tmp, srcLocal, bufferSize);
        AscendC::PipeBarrier<PIPE_V>();
        AscendC::Sin<srcType, false>(dstLocal, tmp, bufferSize);

        outQueue.EnQue<srcType>(dstLocal);
        inQueue.FreeTensor(srcLocal);
    }
    __aicore__ inline void CopyOut()
    {
        AscendC::LocalTensor<srcType> dstLocal = outQueue.DeQue<srcType>();
        AscendC::DataCopy(dstGlobal, dstLocal, bufferSize);
        outQueue.FreeTensor(dstLocal);
    }

private:
    AscendC::GlobalTensor<srcType> srcGlobal;
    AscendC::GlobalTensor<srcType> dstGlobal;

    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueue;
    AscendC::TBuf<AscendC::TPosition::VECCALC> tmpBuf;
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueue;
    uint32_t bufferSize = 0;
};

extern "C" __global__ __aicore__ void math_custom(GM_ADDR srcGm, GM_ADDR dstGm, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    KernelMath<float> op;
    op.Init(srcGm, dstGm, tilingData.calSize);
    if (TILING_KEY_IS(1))
    {
        op.Process();
    }
}
```

## Example 3<a name="section488747123318"></a>

The following example demonstrates how to use the math library kernel-side API together with the Tiling API GetXxxTmpBufferFactorSize. The specific process is as follows:

On the host side, call the Tiling API to obtain maxLiveNodeCount and extraBuf, calculate the maximum number of elements that the operator can compute in a single pass, and write it into the tiling data. On the kernel side, read the tiling data to obtain this value, and allocate temporary space based on it.

Host-side Tiling API usage example:

```
#include <vector>
#include <cassert>
#include "register/op_def_registry.h"
#include "register/tilingdata_base.h"
#include "tiling/tiling_api.h"

namespace optiling {
BEGIN_TILING_DATA_DEF(AsinCustomTilingData)
TILING_DATA_FIELD_DEF(uint32_t, srcSize);
TILING_DATA_FIELD_DEF(uint32_t, tmpBufferSize);
END_TILING_DATA_DEF;

static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    // Input source shapes.
    auto shape_input = context->GetInputTensor(0)->GetOriginShape();
    std::vector<int64_t> srcDims = { shape_input.GetDim(0), shape_input.GetDim(1) };
    uint32_t srcSize = 1;
    uint32_t srcCurSize = 1;
    for (auto dim : srcDims) {
        srcSize *= dim;
    }
    uint32_t typeSize = 2;

    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining space size of the UB.
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize);

    uint32_t asinMaxLiveNodeCount = 0;
    uint32_t asinExtraBuf = 0;

    uint32_t acosMaxLiveNodeCount = 0;
    uint32_t acosExtraBuf = 0;

    AscendC::GetAsinTmpBufferFactorSize(typeSize, asinMaxLiveNodeCount, asinExtraBuf);
    AscendC::GetAcosTmpBufferFactorSize(typeSize, acosMaxLiveNodeCount, acosExtraBuf);
    // The size of tmp must have the space occupied by the input and output of the API called on the UB subtracted.
    // This example includes the input and output of the Asin API, as well as the input and output of the Acos API, where the output of the Asin API serves as the input of the Acos API, thus requiring a total of 3 src space sizes.
    auto tmpSize = tailSize - srcSize * typeSize * 3;
    assert(tmpSize >= asinExtraBuf);
    assert(tmpSize >= acosExtraBuf);
    // Calculate the maximum number of elements that the Asin operator can compute in a single pass.
    if (asinMaxLiveNodeCount != 0) {
        srcAsinCurSize = (tmpSize - asinExtraBuf) / asinMaxLiveNodeCount / typeSize;
    } else {
        srcAsinCurSize = srcSize;
    }
    // Calculate the maximum number of elements that the Acos operator can compute in a single pass.
    if (acosMaxLiveNodeCount != 0) {
        srcAcosCurSize = (tmpSize - acosExtraBuf) / acosMaxLiveNodeCount / typeSize;
    } else {
        srcAcosCurSize = srcSize;
    }
    srcCurSize = std::min(srcAsinCurSize, srcAcosCurSize);

    AsinCustomTilingData tiling;
    tiling.set_srcSize(srcSize);
    tiling.set_srcCurSize(srcCurSize);
    tiling.set_tmpBufferSize(tmpSize);
    context->SetBlockDim(1);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    context->SetTilingKey(1);

    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

Kernel-side example:

```
#include "kernel_operator.h"
template <typename srcType>
class KernelAsin {
public:
    __aicore__ inline KernelAsin(){}
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t srcSizeIn, uint32_t srcCurSizeIn, uint32_t tmpBufferSize)
    {
        srcSize = srcSizeIn;
        srcCurSize = srcCurSizeIn;
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType *>(srcGm), srcSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType *>(dstGm), srcSize);

        pipe.InitBuffer(inQueue, 1, srcSize * sizeof(srcType));
        pipe.InitBuffer(outQueue, 1, srcSize * sizeof(srcType));
        pipe.InitBuffer(tmpBuf1, 1, srcCurSize * sizeof(srcType));
        pipe.InitBuffer(tmpBuf, 1, tmpBufferSize);
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        Compute();
        CopyOut();
    }

private:
    __aicore__ inline void CopyIn()
    {
        AscendC::LocalTensor<srcType> srcLocal = inQueue.AllocTensor<srcType>();
        AscendC::DataCopy(srcLocal, srcGlobal, srcSize);
        inQueue.EnQue(srcLocal);
    }
    __aicore__ inline void Compute()
    {
        AscendC::LocalTensor<srcType> dstLocal = outQueue.AllocTensor<srcType>();
        AscendC::LocalTensor<srcType> srcLocal = inQueue.DeQue<srcType>();
        AscendC::LocalTensor<uint8_t> sharedTmpBuffer = tmpBuf.Get<uint8_t>();
        AscendC::LocalTensor<srcType> tmpresBuffer = tmpBuf1.Get<srcType>();

        for (int32_t offset = 0; offset < srcSize; offset += srcCurSize) {
            AscendC::Asin<srcType, false>(tmpresBuffer, srcLocal[offset], sharedTmpBuffer, srcCurSize);
            AscendC::PipeBarrier<PIPE_V>();
            AscendC::Acos<srcType, false>(dstLocal[offset], tmpresBuffer, sharedTmpBuffer, srcCurSize);
            AscendC::PipeBarrier<PIPE_V>();
        }
        outQueue.EnQue<srcType>(dstLocal);
        inQueue.FreeTensor(srcLocal);
    }
    __aicore__ inline void CopyOut()
    {
        AscendC::LocalTensor<srcType> dstLocal = outQueue.DeQue<srcType>();
        AscendC::DataCopy(dstGlobal, dstLocal, srcSize);
        outQueue.FreeTensor(dstLocal);
    }

private:
    AscendC::GlobalTensor<srcType> srcGlobal;
    AscendC::GlobalTensor<srcType> dstGlobal;

    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueue;
    AscendC::TBuf<AscendC::TPosition::VECCALC, 1> tmpBuf;
    AscendC::TBuf<AscendC::TPosition::VECCALC, 1> tmpBuf1;
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueue;
    uint32_t srcSize, srcCurSize;
};

extern "C" __global__ __aicore__ void kernel_asin_operator(GM_ADDR srcGm, GM_ADDR dstGm, GM_ADDR workspace, GM_ADDR tiling)
{
    GET_TILING_DATA(tilingData, tiling);
    KernelAsin<half> op;
    op.Init(srcGm, dstGm, tilingData.srcSize, tilingData.srcCurSize, tilingData.tmpBufferSize);
    if (TILING_KEY_IS(1)) {
        op.Process();
    }
}
```

## Example 4<a name="section639165323915"></a>

The following uses the key calling code of the [Exp](Exp_api/exp_api.md) API as an example, supplemented by the data printing results before and after the call, to demonstrate the usage of the template parameter **isReuseSource** and its related impact.

The template parameter **isReuseSource** is of the bool type. If **isReuseSource** is **false**, the source operand memory is not reused during the internal computation of the API. If **isReuseSource** is **true**, the source operand memory is reused during the internal computation to store some intermediate results, thereby saving memory. Note that after the API execution is complete, the source operand memory no longer contains the original values.

-   isReuseSource = false

    ```
    // Values of dstLocal and srcLocal before calling Exp.
    // dstLocal: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    // srcLocal: [-7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5]

    AscendC::Exp<float, 15, false>(dstLocal, srcLocal, 16);

    // Values of dstLocal and srcLocal after calling Exp.
    // dstLocal: [0.000553084, 0.00150344, 0.00408677, 0.011109, 0.0301974, 0.082085, 0.22313, 0.606531, 1.64872, 4.48169, 12.1825, 33.1155, 90.0171, 244.692, 665.142, 1808.04]
    // srcLocal: [-7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5]
    ```

-   isReuseSource = true

    ```
    // Values of dstLocal and srcLocal before calling Exp.
    // dstLocal: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    // srcLocal: [-7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5]

    AscendC::Exp<float, 15, true>(dstLocal, srcLocal, 16);

    // Values of dstLocal and srcLocal after calling Exp.
    // dstLocal: [0.000553084, 0.00150344, 0.00408677, 0.011109, 0.0301974, 0.082085, 0.22313, 0.606531, 1.64872, 4.48169, 12.1825, 33.1155, 90.0171, 244.692, 665.142, 1808.04]
    // srcLocal: [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
    ```
