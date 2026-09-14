# GetTiling

<!-- md-trans-meta sourceCommit=e59e1725727133ce76073dba21b022a1b0c9fd56 translatedAt=2026-09-05T14:13:13.729Z -->

## Description

Obtains the Tiling parameters.

## Prototype

```
int64_t GetTiling(optiling::TConv3DApiTiling& tiling)
```

```
int64_t GetTiling(AscendC::tiling::TConv3DApiTiling& tiling)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Output | Tiling structure of Conv3D, used to store the final Tiling result. For details about the **TConv3DApiTiling** structure, see [TConv3DApiTiling Structure](tconv3dapitiling_structure.md). |

## Return Value

If the return value is not -1, the Tiling calculation succeeds and you can use the values in this Tiling structure. If the return value is -1, the Tiling calculation fails and the Tiling result cannot be used.

## Constraints

Before calling **GetTiling**, you must call **SetOrgInputShape**, **SetOrgWeightShape**, **SetSingleWeightShape**, and **SetSingleOutputShape**.

## Examples

```
// Instantiate the Conv3d API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform );
conv3dApiTiling.SetOrgInputShape(orgCi, orgDi, orgHi, orgWi);
conv3dApiTiling.SetOrgWeightShape(cout, kd, kh, kw);
conv3dApiTiling.SetSingleWeightShape(singleCi, singleKd, singleKh, singleKw);
conv3dApiTiling.SetSingleOutputShape(singleCo, singleDo, singleM);
...
conv3dApiTiling.GetTiling(tilingData.conv3ApiTilingData);
```

