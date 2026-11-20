# SetFixSplit

<!-- md-trans-meta sourceCommit=ae444a8b0a91a144a05d68a7cd8d1d0e84475d04 translatedAt=2026-11-02T07:14:12.110Z pushedAt=2026-11-18T04:55:31.871Z nl=2 -->

## Description

Set fixed **baseM**, **baseN**, and **baseK**, in units of element count.

## Prototype

```
int32_t SetFixSplit(int32_t baseMIn = -1, int32_t baseNIn = -1, int32_t baseKIn = -1)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| baseMIn | Input | Set fixed **baseM**. The default value is -1, which means no fixed **baseM** is set and it is calculated by the tiling function. |
| baseNIn | Input | Set fixed **baseN**. The default value is -1, which means no fixed **baseN** is set and it is calculated by the tiling function. |
| baseKIn | Input | Currently only -1 is supported. Setting other values is not supported yet. |

## Return Value

-1 indicates that the setting failed; 0 indicates that the setting succeeded.

## Constraints

-   The storage space occupied by baseM\*baseN output elements must not exceed the L0C Buffer size, that is, baseM \* baseN \* sizeof\([C\_TYPE](../Matmul_Kernel/Matmul_usage.md#table1188045714378)\) <= L0CSize.
-   **baseM** must be less than or equal to the value of singleM rounded up to 16 elements (for example, ceil\(singleM/16\)\*16), and **baseN** must be less than or equal to the value of singleN rounded up to a multiple of C0\_size elements, where singleM is the M-axis length within a single core, singleN is the N-axis length within a single core, the half/bfloat16\_t data type has a C0\_size of 16, the float data type has a C0\_size of 8, the int8\_t data type has a C0\_size of 32, and the int4b\_t data type has a C0\_size of 64. For example, if singleM=12, then **baseM** must be less than or equal to 16. At the same time, **baseM** must meet the requirements of [fractal alignment](../../../basic_api/cube_compute_ISASI/mmad_compute/Mmad.md#constraints), so **baseM** can only be 16. If **baseM** takes any other value greater than 16, obtaining the tiling will fail.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.SetFixSplit(16, 16, -1); // Set fixed baseM and baseN.
```
