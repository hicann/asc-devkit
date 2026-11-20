# SetMadType

<!-- md-trans-meta sourceCommit=5c07153f668df7dd8585d7a94d76b4114ca4b1ac translatedAt=2026-11-02T07:15:08.340Z pushedAt=2026-11-18T04:55:33.866Z nl=2 -->

## Description

Sets whether to enable HF32 mode. **This is not supported in the current version.**

<!-- npu="950" id1 -->
For Ascend 950PR&950DT products, this API can be used to set whether to enable HF32 mode (**not supported in the current version**) and whether to enable the MxMatmul scenario. In the MxMatmul scenario, you must call this API and configure it to enable the MxMatmul scenario to ensure correct computation and return of Tiling parameters in this scenario.
<!-- end id1 -->

## Prototype

```
int32_t SetMadType(MatrixMadType madType)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| madType | Input | Sets the Matmul mode. The type is **MatrixMadType**, defined as shown in the following code, where the parameters mean:<br><br>**MatrixMadType::NORMAL**: Normal mode, that is, neither HF32 mode nor the MxMatmul scenario.<br><br>**MatrixMadType::HF32**: Enables HF32 mode.<!-- npu="950" id2 --><br><br>**MatrixMadType::MXMODE**: Enables the MxMatmul scenario.<!-- end id2 --> |

```
enum class MatrixMadType : int32_t {
    NORMAL = 0,
    HF32 = 1,
    MXMODE = 2,
};
```

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

<!-- npu="950" id3 -->

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
tiling.SetTraverse(MatrixTraverse::FIRSTM); // Set the traversal mode.
tiling.SetMadType(MatrixMadType::MXMODE);   // Enable the MxMatmul scenario.
```
<!-- end id3 -->
