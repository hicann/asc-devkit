# SetFixSplit

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-09-09T07:25:07.124Z pushedAt=2026-09-12T09:55:18.200Z -->

## Description

Sets fixed **baseM**, **baseN**, and **baseK** values, in units of elements.

## Prototype

```
int32_t SetFixSplit(int32_t baseMIn = -1, int32_t baseNIn = -1, int32_t baseKIn = -1)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| baseMIn | Input | Sets a fixed **baseM**. The default value is -1, which means no fixed **baseM** is set and the value is computed by the tiling function. |
| baseNIn | Input | Sets a fixed **baseN**. The default value is -1, which means no fixed **baseN** is set and the value is computed by the tiling function. |
| baseKIn | Input | Currently only -1 is supported; setting other values is not supported yet. |

## Return Value

-1 indicates that the setting fails; 0 indicates that the setting succeeds.

## Constraints

-   The storage space occupied by baseM\*baseN output elements must not exceed the L0C Buffer size, that is, **baseM** \* **baseN** \* sizeof\([C\_TYPE](../Matmul-Kernel/matmul_usage.md#table1188045714378)\) <= L0CSize.
-   **baseM** must be less than or equal to the value of **singleM** aligned upward to 16 elements (for example, ceil\(singleM/16\)\*16), and **baseN** must be less than or equal to the value of **singleN** aligned upward to C0\_size elements, where **singleM** is the length of the M axis within a single core and **singleN** is the length of the N axis within a single core. The half/bfloat16\_t data type has a C0\_size of 16, the float data type has a C0\_size of 8, the int8\_t data type has a C0\_size of 32, and the int4b\_t data type has a C0\_size of 64. For example, if **singleM** is 12, **baseM** must be less than or equal to 16, and **baseM** must also meet the requirements of [fractal alignment](../../../basic_api/cube_compute_ISASI/mmad_compute/Mmad.md#constraints). Therefore, **baseM** can only be 16. If **baseM** takes any other value greater than 16, obtaining the tiling will fail.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);

tiling.SetFixSplit(16, 16, -1);  // Set fixed baseM and baseN.
```
