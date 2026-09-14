# MatmulGetTmpBufSize

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T06:20:25.001Z pushedAt=2026-09-12T09:55:18.079Z -->

## Description

This API is used to obtain the size of the L1 Buffer, Unified Buffer, or L0C Buffer based on the tiling structure information after the tiling parameters are obtained by calling [GetTiling](../Matmul-Tiling/GetTiling.md).

## Prototype

```
int32_t MatmulGetTmpBufSize(optiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Structure of single-core Matmul tiling, that is, the **TCubeTiling** structure obtained from the **MatmulTiling** object. |
| bufSize | Output | Size of the L1 Buffer, Unified Buffer, or L0C Buffer in tiling.<br><br>The **SysTilingTempBufSize** structure is defined as shown in the following code. |

```
struct SysTilingTempBufSize {
    int32_t ubSize = 0; // Size of the Unified Buffer
    int32_t l1Size = 0; // Size of the L1 Buffer
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
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
optiling::TCubeTiling tilingData;
...  // Initialize tilingData. For details, see the usage instructions of the Matmul tiling class.
int ret = tiling.GetTiling(tilingData);    // Obtain the tiling parameters.
SysTilingTempBufSize bufSize;
MatmulGetTmpBufSize(tilingData, bufSize);
```

