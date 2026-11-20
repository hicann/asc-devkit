# SetShape

<!-- md-trans-meta sourceCommit=c9ba5e84f8f5d1c96c38636648c995a8c2814fbc translatedAt=2026-11-02T07:17:55.052Z pushedAt=2026-11-18T04:55:43.945Z nl=2 -->

## Description

Sets the shape **m**, **n**, and **k** for Matmul computation. The shape can be the original complete cube or a local cube of it, in elements. The cube multiplication of this shape can be completed by a single core or multiple cores.

When using this API, there are two ways to pass parameters:

-   Pass the shape **m**, **n**, and **k** for Matmul computation. When [GetTiling](GetTiling.md) is called, the Tiling parameters are calculated and returned based on **m**, **n**, and **k**.
-   Pass -1 at any one or more of the **m**, **n**, and **k** parameter positions. When [GetTiling](GetTiling.md) is called, the original shape **M**, **N**, and **K** or **Ka**/**Kb** set in [SetOrgShape](SetOrgShape.md) is used at that position, and the optimal Tiling parameters are then calculated internally by the API. As shown in the following figure, the last column in the K direction of the original cube A contains dirty data that does not participate in the computation. The original shape including this column is set in [SetOrgShape](SetOrgShape.md), the size in the K direction for Matmul computation is set in this API, and the parameters **m** and **n** are set to -1 to indicate that Tiling is calculated based on the original shape **M** and **N**.

**Figure 1**  Scenario of passing -1 as a parameter  
![](../../../../figures/param_minus1_scenario.png "Scenario of passing -1 as a parameter")

## Prototype

```
int32_t SetShape(int32_t m, int32_t n, int32_t k)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| m | Input | Sets the size in the M direction for Matmul computation, in elements. |
| n | Input | Sets the size in the N direction for Matmul computation, in elements. |
| k | Input | Sets the size in the K direction for Matmul computation, in elements. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024); // Set the shape of the Matmul computation.
tiling.SetOrgShape(1024, 1024, 1024);
```
