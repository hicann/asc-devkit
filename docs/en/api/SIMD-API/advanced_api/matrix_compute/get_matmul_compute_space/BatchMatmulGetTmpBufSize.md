# BatchMatmulGetTmpBufSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T06:20:07.883Z pushedAt=2026-09-12T09:55:18.076Z -->

## Description

After BatchMatmul Tiling calls [GetTiling](../Matmul-Tiling/GetTiling.md) to obtain the tiling parameters, it obtains the size of the L1 Buffer, Unified Buffer, or L0C Buffer based on the tiling structure information.

## Prototype

```
int32_t BatchMatmulGetTmpBufSize(optiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Structure of BatchMatmul tiling, that is, the **TCubeTiling** structure obtained from the **BatchMatmulTiling** object. |
| bufSize | Output | Size of the L1 Buffer, Unified Buffer, or L0C Buffer obtained based on the **TCubeTiling** structure information.<br><br>The **SysTilingTempBufSize** structure is defined as shown in the following code. |

```
struct SysTilingTempBufSize {
    int32_t ubSize = 0; // Unified Buffer size.
    int32_t l1Size = 0; // L1 Buffer size.
    int32_t l0cSize = 0; // L0C Buffer size.
};
```

## Return Value

**-1** indicates a failure to obtain the buffer size; **0** indicates success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::BatchMatmulTiling tiling(ascendcPlatform);
optiling::TCubeTiling tilingData;
...  // Initialize tilingData. For details, see the usage instructions of the MatmulTiling class.
int ret = tiling.GetTiling(tilingData);    // Obtain the Tiling parameters.
SysTilingTempBufSize bufSize;
BatchMatmulGetTmpBufSize(tilingData, bufSize);
```

