# GetDropOutMaxMinTmpSize

<!-- md-trans-meta sourceCommit=aa6d602ebc3dbc8f7b50f91dc72aceefdfa67b49 translatedAt=2026-11-10T06:29:58.769Z pushedAt=2026-11-18T11:07:26.744Z nl=2 -->

## Description

Used to obtain the DropOut Tiling parameters.

## Prototype

```
uint32_t GetDropOutMaxTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource)
```

```
uint32_t GetDropOutMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource)
```

```
void GetDropOutMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  Parameter List

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. The parameter type is [AscendC::TensorShape](../data_structures/TensorShape.md). |
| typeSize | Input | Size of the data type used for computation, where half=2 and float=4. |
| isReuseSource | Input | Reserved parameter, not enabled yet. Keep the default value false. |
| maxValue | Output | Outputs the tiling information (maximum temporary space size, in bytes) required by the DropOut API.<br> Note: maxValue is only a reference value and may be larger than the remaining UB space. In this case, you need to select an appropriate temporary space size based on the remaining UB space. |
| minValue | Output | Outputs the tiling information (minimum temporary space size, in bytes) required by the DropOut API. |

## Return Value

GetDropOutMaxTmpSize returns the maximum temporary space size required for the DropOut API to complete the computation.

GetDropOutMinTmpSize returns the minimum temporary space size required for the DropOut API to complete the computation.

GetDropOutMaxMinTmpSize has no return value.

## Constraints

None

## Examples

The following shows an example of calling the **GetDropOutMaxMinTmpSize** API on the host side to obtain the maximum and minimum temporary space sizes required for DropOut computation. Based on this range, you can select an appropriate space size and pass it to the kernel-side as a Tiling parameter. For the corresponding kernel-side example, see [Examples](DropOut.md#section642mcpsimp).

```
#include <vector>

#include "register/op_def_registry.h"
#include "register/tilingdata_base.h"
#include "tiling/tiling_api.h"

namespace optiling {

BEGIN_TILING_DATA_DEF(DropoutCustomTilingData)
    TILING_DATA_FIELD_DEF(uint32_t, firstAxis);
    TILING_DATA_FIELD_DEF(uint32_t, srcLastAxis);
    TILING_DATA_FIELD_DEF(uint32_t, maskLastAxis);
    TILING_DATA_FIELD_DEF(uint32_t, tmpBufferSize);
END_TILING_DATA_DEF;

static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    // Input source shapes.
    int64_t firstAxis = 16;
    int64_t srcLastAxis = 64;
    int64_t maskLastAxis = 64;

    std::vector<int64_t> srcDims = {firstAxis, srcLastAxis, maskLastAxis};

    uint32_t typeSize = 2;
    AscendC::TensorShape shape(srcDims);
    uint32_t minValue = 0;
    uint32_t maxValue = 0;
    AscendC::GetDropOutMaxMinTmpSize(shape, typeSize, false, maxValue, minValue);

    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining UB space size.
    // In this example, the full UB space is used. In practice, tailSize must subtract the UB space already used by the user.
    ascendcPlatform.GetCoreMemSize(
        platform_ascendc::CoreMemType::UB, tailSize); 
    auto tmpSize = tailSize >= maxValue ? maxValue : tailSize;

    DropoutCustomTilingData tiling;
    tiling.set_firstAxis(firstAxis);
    tiling.set_srcLastAxis(srcLastAxis);
    tiling.set_maskLastAxis(maskLastAxis);
    tiling.set_tmpBufferSize(tmpSize);
    context->SetSimdNumBlocks(1);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    context->SetTilingKey(1);

    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```
