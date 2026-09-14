# GetDropOutMaxMinTmpSize

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-08T17:41:35.344Z -->

## Description

Obtains the DropOut Tiling parameters.

## Prototype

```
uint32_t GetDropOutMaxTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource)
```

```
uint32_t GetDropOutMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource)
```

```
void GetDropOutMaxMinTmpSize(const ge::Shape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue)
```

## Parameters

**Table 1**  Parameter list

| Parameter | Input/Output | Description |
| --- | --- | --- |
| srcShape | Input | Input shape information. |
| typeSize | Input | Size of the data type used for computation, where half = 2 and float = 4. |
| isReuseSource | Input | Reserved parameter, not enabled yet. Keep the default value **false**. |
| maxValue | Output | Outputs the Tiling information (maximum temporary space size) required by the DropOut API.<br> Note: **maxValue** is only a reference value and may be larger than the remaining space of the Unified Buffer. In this case, developers need to select an appropriate temporary space size based on the remaining space of the Unified Buffer. |
| minValue | Output | Outputs the Tiling information (minimum temporary space size) required by the DropOut API. |

## Return Value

GetDropOutMaxTmpSize returns the maximum temporary space size required for the DropOut API to complete the computation.

GetDropOutMinTmpSize returns the minimum temporary space size required for the DropOut API to complete the computation.

GetDropOutMaxMinTmpSize has no return value.

## Constraints

None

## Examples

The following shows an example of calling the **GetDropOutMaxMinTmpSize** API on the host side to obtain the maximum and minimum temporary space sizes required for the DropOut computation. Based on this range, developers select an appropriate space size as the Tiling parameter and pass it to the kernel side. For the corresponding kernel-side example, see [Examples](DropOut.md#examples).

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
    ge::Shape shape(srcDims);
    uint32_t minValue = 0;
    uint32_t maxValue = 0;
    AscendC::GetDropOutMaxMinTmpSize(shape, typeSize, false, maxValue, minValue);

    auto platformInfo = context->GetPlatformInfo();
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
    uint64_t tailSize = 0; // Remaining space size of the UB.
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize); // Use the full UB space in this example. In practice, subtract the UB space already used by the user from tailSize.
    auto tmpSize = tailSize >= maxValue ? maxValue : tailSize;

    DropoutCustomTilingData tiling;
    tiling.set_firstAxis(firstAxis);
    tiling.set_srcLastAxis(srcLastAxis);
    tiling.set_maskLastAxis(maskLastAxis);
    tiling.set_tmpBufferSize(tmpSize);
    context->SetBlockDim(1);
    tiling.SaveToBuffer(context->GetRawTilingData()->GetData(), context->GetRawTilingData()->GetCapacity());
    context->GetRawTilingData()->SetDataSize(tiling.GetDataSize());
    context->SetTilingKey(1);

    return ge::GRAPH_SUCCESS;
}
} // namespace optiling
```
