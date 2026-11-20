# SetOrgShape

<!-- md-trans-meta sourceCommit=8d4511303e7599d86c4c8f565e5b0a328eb60b1e translatedAt=2026-11-02T07:15:47.690Z pushedAt=2026-11-18T04:55:37.836Z nl=2 -->

## Description

Sets the original complete shapes M, N, K, or Ka/Kb for Matmul computation, with each unit being the number of elements.

## Prototype

```
int32_t SetOrgShape(int32_t orgMIn, int32_t orgNIn, int32_t orgKIn)
```

```
int32_t SetOrgShape(int32_t orgMIn, int32_t orgNIn, int32_t orgKaIn, int32_t orgKbIn)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgMIn | Input | Sets the original complete shape M size, in elements. |
| orgNIn | Input | Sets the original complete shape N size, in elements. |
| orgKIn | Input | Sets the original complete shape K size, in elements. It can be set when the original complete shapes Ka and Kb are equal. |
| orgKaIn | Input | Sets the original complete shape Ka size of cube A, in elements. |
| orgKbIn | Input | Sets the original complete shape Kb size of cube B, in elements. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

The parameters **orgKaIn** and **orgKbIn** may be unequal, meaning that the original cube shapes Ka and Kb are unequal. This is not the K used in the actual Matmul computation; these parameters are only used to assist the offset calculation during Matmul API data movement.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024); // Set the original complete shape.
```
