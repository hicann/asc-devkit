# MatmulGetTmpBufSizeV2

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T06:22:46.212Z pushedAt=2026-09-12T09:55:18.086Z -->

## Description

After the single-core Matmul Tiling calls the [GetTiling](../Matmul-Tiling/GetTiling.md) API to obtain the tiling parameters, it obtains the size of the L1 Buffer, Unified Buffer, L0C Buffer based on the tiling structure information.

## Prototype

```
int32_t MatmulGetTmpBufSizeV2(AscendC::tiling::TCubeTiling &tiling, matmul_tiling::SysTilingTempBufSize &bufSize)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| tiling | Input | Structure of single-core Matmul tiling, that is, the **TCubeTiling** structure obtained from the **MatmulTiling** instance.<br><br>**TCubeTiling** is the Matmul tiling data defined on the kernel, and is used together with the [GetTiling](../Matmul-Tiling/GetTiling.md) API whose input parameter is the **TCubeTiling** structure with the **AscendC::tiling** namespace. |
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

## Example

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
AscendC::tiling::TCubeTiling tilingData;
...  // Initialize tilingData. For details, see the usage description of the Matmul tiling class.
int ret = tiling.GetTiling(tilingData);    // Obtain the tiling parameters.
SysTilingTempBufSize bufSize;
MatmulGetTmpBufSizeV2(tilingData, bufSize);
```

