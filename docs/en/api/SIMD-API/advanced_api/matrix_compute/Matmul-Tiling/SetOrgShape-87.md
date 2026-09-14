# SetOrgShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:26:36.853Z pushedAt=2026-09-12T09:55:18.205Z -->

## Description

Sets the original complete shape **M**, **N**, **K**, or **Ka/Kb** for **Matmul** computation, with the unit being the number of elements.

## Prototype

```
int32_t SetOrgShape(int32_t orgMIn, int32_t orgNIn, int32_t orgKIn)
```

```
int32_t SetOrgShape(int32_t orgMIn, int32_t orgNIn, int32_t orgKaIn, int32_t orgKbIn)
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| orgMIn | Input | Sets the original complete shape M size, in elements. |
| orgNIn | Input | Sets the original complete shape N size, in elements. |
| orgKIn | Input | Sets the original complete shape K size, in elements. This can be set when the original complete shapes satisfy Ka = Kb. |
| orgKaIn | Input | Sets the original complete shape Ka size of matrix A, in elements. |
| orgKbIn | Input | Sets the original complete shape Kb size of matrix B, in elements. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

The parameters **orgKaIn** and **orgKbIn** may be unequal, meaning that the original matrix shapes Ka and Kb are unequal. They are not the K used in the actual Matmul computation; these parameters only assist in the offset calculation during data movement by the Matmul API.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.SetShape(1024, 1024, 1024);
tiling.SetOrgShape(1024, 1024, 1024);  // Set the original complete shape.
```

