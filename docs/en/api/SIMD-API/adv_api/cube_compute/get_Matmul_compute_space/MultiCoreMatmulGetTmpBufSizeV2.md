# MultiCoreMatmulGetTmpBufSizeV2

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-02T06:04:22.101Z pushedAt=2026-11-18T04:52:08.165Z nl=2 -->

## Description

After the multi-core Matmul Tiling calls the [GetTiling](../Matmul_Tiling/GetTiling.md) API to obtain the Tiling parameters, it obtains the usage sizes of the L1 Buffer/Unified Buffer (UB)/L0C Buffer based on the Tiling structure information.

## Prototype

```
int32_t MultiCoreMatmulGetTmpBufSizeV2(AscendC::tiling::TCubeTiling& tiling, matmul_tiling::SysTilingTempBufSize& bufSize)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Structure of the Matmul multi-core Tiling, that is, the TCubeTiling structure obtained from the MultiCoreMatmulTiling object.<br><br>TCubeTiling is the Matmul TilingData defined on the kernel function (Kernel) side, and is used together with the [GetTiling](../Matmul_Tiling/GetTiling.md) API whose input parameter is the TCubeTiling structure with the AscendC::tiling namespace. |
| bufSize | Output | Usage sizes of the L1 Buffer/UB/L0C Buffer based on the TCubeTiling structure information. The SysTilingTempBufSize structure is defined as shown in the following code. |

```
struct SysTilingTempBufSize {
    int32_t ubSize = 0;  // UB size.
    int32_t l1Size = 0;  // L1 buffer size.
    int32_t l0cSize = 0; // L0C buffer size.
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
AscendC::tiling::TCubeTiling tilingData;
...                                         // Initialize tilingData. For details, see the MatmulTiling class usage description.
    int ret = tiling.GetTiling(tilingData); // Obtain the Tiling parameters.
SysTilingTempBufSize bufSize;
MultiCoreMatmulGetTmpBufSizeV2(tilingData, bufSize);
```
