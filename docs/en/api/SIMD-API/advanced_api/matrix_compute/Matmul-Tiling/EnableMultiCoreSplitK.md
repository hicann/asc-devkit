# EnableMultiCoreSplitK

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:12:02.465Z pushedAt=2026-09-12T09:55:18.163Z -->

## Description

In a multi-core scenario, this API enables splitting the K axis. If this API is not called, the K axis is not split by default. Call this API before calling **GetTiling**.

## Prototype

```
void EnableMultiCoreSplitK(bool flag)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **flag** | Input | Whether to split the K axis.<br>true: Split the K axis.<br>false: Do not split the K axis. |

## Return Value

None

## Constraints

-   When this API is used in an operator, the C matrix result can only be output to Global Memory.
-   When this API is used in an operator, you must clear Global Memory before the result of the C matrix tile is first written to Global Memory in the kernel-side code, and then enable AtomicAdd accumulation when obtaining the result of the C matrix tile. If Global Memory is not cleared in advance, precision issues may occur due to accumulation of the original invalid data in Global Memory.
-   When this API is used in an operator, Bias is not supported in matrix multiplication.

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);

tiling.EnableMultiCoreSplitK(true);  // Enable split K axis.
```
