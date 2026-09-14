# MultiCoreMatmulGetTmpBufSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T06:21:22.550Z pushedAt=2026-09-12T09:55:18.080Z -->

## Description

After the multi-core Matmul Tiling calls the [GetTiling](../Matmul-Tiling/GetTiling.md) API to obtain the tiling parameters, **MultiCoreMatmulGetTmpBufSize** obtains the size of the L1 Buffer, Unified Buffer, L0C Buffer based on the tiling structure information.

## Prototype

```
int32_t MultiCoreMatmulGetTmpBufSize(optiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Structure of multi-core Matmul tiling, that is, the **TCubeTiling** structure obtained from the **MultiCoreMatmulTiling** instance. |
| bufSize | Output | Size of the L1 Buffer, Unified Buffer, or L0C Buffer obtained based on the **TCubeTiling** structure information. The **SysTilingTempBufSize** structure is defined as shown in the following code. |

```
struct SysTilingTempBufSize {
    int32_t ubSize = 0;  // Size of the Unified Buffer
    int32_t l1Size = 0;  // Size of the L1 Buffer
    int32_t l0cSize = 0; // Size of the L0C Buffer
};
```

## Return Value

**-1** indicates a failure to obtain the buffer size; **0** indicates success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
optiling::TCubeTiling tilingData;
...  // Initialize tilingData. For details, see the usage instructions of the Matmul tiling class.
int ret = tiling.GetTiling(tilingData);    // Obtain the tiling parameters.
SysTilingTempBufSize bufSize;
MultiCoreMatmulGetTmpBufSize(tilingData, bufSize);
```

