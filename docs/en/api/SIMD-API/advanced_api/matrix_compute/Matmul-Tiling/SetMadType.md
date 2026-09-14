# SetMadType

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:25:27.146Z pushedAt=2026-09-12T09:55:18.202Z -->

## Description

Sets whether to enable the HF32 mode. **This is not supported in the current version.**

For Ascend 950PR/Ascend 950DT, this API can be used to set whether to enable the HF32 mode (**not supported in the current version**) and whether to enable the MxMatmul scenario. In the MxMatmul scenario, you must call this API and configure it to enable the MxMatmul scenario to ensure correct computation and return of Tiling parameters in this scenario.

## Prototype

```
int32_t SetMadType(MatrixMadType madType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| madType | Input | Sets the Matmul mode. The type is **MatrixMadType**, defined as shown in the following code, where the parameters have the following meanings:<br><br>**MatrixMadType::NORMAL**: Normal mode, that is, neither HF32 mode nor MxMatmul scenario.<br><br>**MatrixMadType::HF32**: Enables HF32 mode.<br><br>**MatrixMadType::MXMODE**: Enables the MxMatmul scenario. |

```
enum class MatrixMadType : int32_t {
NORMAL = 0,
HF32 = 1,
MXMODE = 2,
};
```

## Return Value

**-1** indicates a setting failure; **0** indicates a successful setting.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetTraverse(MatrixTraverse::FIRSTM);  // Set the traversal mode.
tiling.SetMadType(MatrixMadType::MXMODE);  // Enable the MxMatmul scenario.
```
