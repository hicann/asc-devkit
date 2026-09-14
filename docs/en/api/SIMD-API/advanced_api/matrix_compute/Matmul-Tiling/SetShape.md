# SetShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:28:09.684Z pushedAt=2026-09-12T09:55:18.211Z -->

## Description

Sets the shape m, n, and k of the Matmul computation. The shape can be the original complete matrix or its partial matrix, in unit of element. The matrix multiplication of this shape can be completed by a single core or multiple cores.

When using this API, there are two parameter passing methods:

-   Pass the shape m, n, and k of the Matmul computation. When the [GetTiling](GetTiling.md) API is called, the Tiling parameters are computed and returned based on m, n, and k.
-   Pass -1 at any one or more of the m, n, and k positions. When the [GetTiling](GetTiling.md) API is called, the original shape M, N, K, or Ka/Kb set in the [SetOrgShape](SetOrgShape-87.md) API is used at that position, and then the optimal Tiling parameters are computed internally by the API. As shown in the following figure, the last column in the K direction of the original A matrix contains dirty data that does not participate in the computation. Set the original shape containing this column in the [SetOrgShape](SetOrgShape-87.md) API, set the K direction and size of the Matmul computation in this API, and set the m and n parameters to -1 to indicate that the Tiling is computed based on the original shapes M and N.

**Figure 1**  Scenario of passing -1 as a parameter  
![](../../../../figures/pass_-1_for_parameter.png)

## Function Prototype

```
int32_t SetShape(int32_t m, int32_t n, int32_t k)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| m | Input | Sets the M direction and size of the Matmul computation, unit: element. |
| n | Input | Sets the N direction and size of the Matmul computation, unit: element. |
| k | Input | Sets the K direction and size of the Matmul computation, unit: element. |

## Return Value

-1 indicates a setting failure; 0 indicates a setting success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024);  // Set the shape of the Matmul computation.
tiling.SetOrgShape(1024, 1024, 1024);
```
