# MultiCoreMatmulGetTmpBufSize

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T06:03:59.631Z pushedAt=2026-11-18T04:52:06.558Z nl=2 -->

## Description

After the multi-core Matmul Tiling calls the [GetTiling](../Matmul_Tiling/GetTiling.md) API to obtain the Tiling parameters, it obtains the usage sizes of the L1 Buffer/Unified Buffer (UB)/L0C Buffer based on the Tiling structure information.

## Prototype

```
int32_t MultiCoreMatmulGetTmpBufSize(optiling::TCubeTiling& tiling, matmul_tiling::SysTilingTempBufSize& bufSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Matmul multi-core Tiling structure, that is, the TCubeTiling structure obtained from the MultiCoreMatmulTiling object. |
| bufSize | Output | Usage sizes of the L1 Buffer/UB/L0C Buffer based on the TCubeTiling structure information. The SysTilingTempBufSize structure is defined as shown in the following code. |

```
struct SysTilingTempBufSize {
    int32_t ubSize = 0;  // UB size.
    int32_t l1Size = 0;  // L1 Buffer size.
    int32_t l0cSize = 0; // L0C Buffer size.
};
```

## Return Value

-1 indicates failure; 0 indicates success.

## Constraints

None

## Examples

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
optiling::TCubeTiling tilingData;
...                                         // Initialize tilingData. For details, see the MatmulTiling class usage description.
    int ret = tiling.GetTiling(tilingData); // Obtain the Tiling parameters.
SysTilingTempBufSize bufSize;
MultiCoreMatmulGetTmpBufSize(tilingData, bufSize);
```
