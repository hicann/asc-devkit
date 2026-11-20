# More Examples

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T11:01:51.368Z pushedAt=2026-11-18T08:54:29.140Z nl=2 -->

## Example 1<a name="section5279737173215"></a>

The following example shows how the math library kernel-side API is used together with the Tiling API GetXxxMaxMinTmpSize. The specific process is as follows:

On the host side, the Tiling API is called to obtain the required temporary space size, which is then written into the tiling data. On the kernel side, the tiling data is read to obtain the corresponding temporary space size, based on which the temporary space is allocated.

The host-side Tiling API uses [AscendC::TensorShape](../data_structures/TensorShape.md) to describe the tensor shape of the input data. A usage example is as follows:

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
    AscendC::TensorShape shape(srcDims);
    uint32_t minValue = 0;
    uint32_t maxValue = 0;
    AscendC::GetAsinMaxMinTmpSize(shape, typeSize, false, maxValue, minValue);

    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining UB space size.
    ascendcPlatform.GetCoreMemSize(
        platform_ascendc::CoreMemType::UB,
        tailSize); // In this example, the full UB space is used. In practice, tailSize needs to subtract the UB space already used by the user.
    auto tmpSize = tailSize >= maxValue ? maxValue : tailSize;

    AsinCustomTilingData tiling;
    tiling.set_srcSize(srcSize);
    tiling.set_tmpBufferSize(tmpSize);
    context->SetSimdNumBlocks(1);
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
class KernelAsin {
public:
    __aicore__ inline KernelAsin() {}
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t srcSize, uint32_t tmpBufferSize)
    {
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(srcGm), srcSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(dstGm), srcSize);

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
    if (TILING_KEY_IS(1)) {
        op.Process();
    }
}
```

## Example 2<a name="section577043422516"></a>

The following example shows how the math library kernel-side API works together with [PlatformAscendC::ReserveLocalMemory](../../../Utils-API/platform_info/PlatformAscendC/ReserveLocalMemory.md). The process is as follows:

On the host side, the ReserveLocalMemory API is called to reserve Unified Buffer (UB) memory space, and the GetCoreMemSize API is used to obtain the actually available UB memory size. Based on the actually available memory size, the maximum supported Shape (maximum data scale) is computed. This approach avoids calling the GetXXXTmpMaxMinSize API multiple times to obtain a suitable temporary space size.

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
    uint64_t tailSize = 0; // Remaining UB space size.
    ascendcPlatform.ReserveLocalMemory(platform_ascendc::ReservedSize::RESERVED_SIZE_8K);
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize);

    uint64_t calSize = tailSize / (3 * 4); // src + dst + tmp float type
    MathCustomTilingData tiling;
    tiling.set_calSize(calSize);
    context->SetSimdNumBlocks(1);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    context->SetTilingKey(1);
    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```

On the kernel side, the corresponding computation scale parameters are obtained by reading TilingData. Then, the kernel calls the math library API that does not require temporary space to perform various math operations:

```
#include "kernel_operator.h"

template <typename srcType>
class KernelMath {
public:
    __aicore__ inline KernelMath() {}
    __aicore__ inline void Init(GM_ADDR srcGm, GM_ADDR dstGm, uint32_t srcSize)
    {
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(srcGm), srcSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(dstGm), srcSize);

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
    if (TILING_KEY_IS(1)) {
        op.Process();
    }
}
```

## Example 3<a name="section488747123318"></a>

The following example shows how the math library kernel-side API and the Tiling API GetXxxTmpBufferFactorSize are used together. The specific process is as follows:

On the host side, the Tiling API is called to obtain maxLiveNodeCount and extraBuf, and the maximum number of elements that can be computed in a single pass is computed and written into the tiling data. On the kernel side, the tiling data is read to obtain this value, based on which temporary space is allocated.

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

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    // Input source shapes.
    auto shape_input = context->GetInputTensor(0)->GetOriginShape();
    std::vector<int64_t> srcDims = {shape_input.GetDim(0), shape_input.GetDim(1)};
    uint32_t srcSize = 1;
    uint32_t srcCurSize = 1;
    for (auto dim : srcDims) {
        srcSize *= dim;
    }
    uint32_t typeSize = 2;

    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining UB space size.
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize);

    uint32_t asinMaxLiveNodeCount = 0;
    uint32_t asinExtraBuf = 0;

    uint32_t acosMaxLiveNodeCount = 0;
    uint32_t acosExtraBuf = 0;

    AscendC::GetAsinTmpBufferFactorSize(typeSize, asinMaxLiveNodeCount, asinExtraBuf);
    AscendC::GetAcosTmpBufferFactorSize(typeSize, acosMaxLiveNodeCount, acosExtraBuf);
    // The size of tmp needs to subtract the space occupied by the input and output of the API called on the UB.
    // This example includes the input and output of the Asin API and the input and output of the Acos API. The output of the Asin API serves as the input of the Acos API, so a total of three src space sizes are required.
    auto tmpSize = tailSize - srcSize * typeSize * 3;
    assert(tmpSize >= asinExtraBuf);
    assert(tmpSize >= acosExtraBuf);
    // Calculate the maximum number of elements computed in a single call of the Asin operator.
    if (asinMaxLiveNodeCount != 0) {
        srcAsinCurSize = (tmpSize - asinExtraBuf) / asinMaxLiveNodeCount / typeSize;
    } else {
        srcAsinCurSize = srcSize;
    }
    // Calculate the maximum number of elements computed in a single call of the Acos operator.
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
    context->SetSimdNumBlocks(1);
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
    __aicore__ inline KernelAsin() {}
    __aicore__ inline void Init(
        GM_ADDR srcGm, GM_ADDR dstGm, uint32_t srcSizeIn, uint32_t srcCurSizeIn, uint32_t tmpBufferSize)
    {
        srcSize = srcSizeIn;
        srcCurSize = srcCurSizeIn;
        srcGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(srcGm), srcSize);
        dstGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ srcType*>(dstGm), srcSize);

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

extern "C" __global__ __aicore__ void kernel_asin_operator(
    GM_ADDR srcGm, GM_ADDR dstGm, GM_ADDR workspace, GM_ADDR tiling)
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

The following uses the key call code of the [Exp](Exp_interface/Exp_interface.md) API as an example, supplemented with the data printing results before and after the call, to demonstrate the use of the template parameter isReuseSource and its related effects.

The template parameter isReuseSource is of the bool type. If isReuseSource is false, the API does not reuse the memory space of the source operand during internal computation. If isReuseSource is true, the API reuses the memory space of the source operand during internal computation to store some intermediate results, thereby saving memory space. Note that after the API finishes execution, the memory space of the source operand no longer holds the original value.

-   isReuseSource = false

    ```
    // Values of dstLocal and srcLocal before calling Exp.
    // dstLocal: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    // srcLocal: [-7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5]

    AscendC::Exp<float, 15, false>(dstLocal, srcLocal, 16);

    // Values of dstLocal and srcLocal after calling Exp.
    // dstLocal: [0.000553084, 0.00150344, 0.00408677, 0.011109, 0.0301974, 0.082085, 0.22313,
    // 0.606531, 1.64872, 4.48169, 12.1825, 33.1155, 90.0171, 244.692, 665.142, 1808.04] srcLocal: [-7.5, -6.5, -5.5, -4.5,
    // -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5]
    ```

-   isReuseSource = true

    ```
    // Values of dstLocal and srcLocal before calling Exp.
    // dstLocal: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    // srcLocal: [-7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5]

    AscendC::Exp<float, 15, true>(dstLocal, srcLocal, 16);

    // Values of dstLocal and srcLocal after calling Exp.
    // dstLocal: [0.000553084, 0.00150344, 0.00408677, 0.011109, 0.0301974, 0.082085, 0.22313,
    // 0.606531, 1.64872, 4.48169, 12.1825, 33.1155, 90.0171, 244.692, 665.142, 1808.04] srcLocal: [0.5, 0.5, 0.5, 0.5, 0.5,
    // 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]
    ```
