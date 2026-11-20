# SetFixpipeNz2dnFlag

<!-- md-trans-meta sourceCommit=d1c6d885aaa504b2fae9a963830336c6fe119808 translatedAt=2026-11-19T06:05:46.978Z pushedAt=2026-11-19T06:14:22.738Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixpipeNz2dnFlag_res.md#id1 -->

## Function Description

The header file path is `"basic_api/kernel_operator_fixpipe_intf.h"`.

When performing real-time format conversion (Nz format conversion to DN format) during DataCopy data movement ([L0C Buffer->UB](../cube_compute_store/DataCopy_L0CToUB.md)), call this API to set the related configuration for format conversion.

## Prototype

```cpp
__aicore__ inline void SetFixpipeNz2dnFlag(uint16_t dnNum, uint16_t srcNzMatrixStride, uint32_t dstDnMatrixStride, uint16_t srcNzC0Stride);
```

## Parameters

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dnNum | Input | Number of DN matrices to transfer, with a value range of dnNum∈[0, 65535]. Note: dnNum=0 means no transfer is performed, and this API is treated as a NOP (no operation). |
| srcNzMatrixStride | Input | Offset between different source Nz matrices (head to head), with a value range of srcNzMatrixStride∈[0, 65535], in C0_SIZE. When dnNum is set to 1, set srcNzMatrixStride to 0, which does not take effect. |
| dstDnMatrixStride | Input | Offset between the start addresses of adjacent destination ND matrices, with a value range of dstDnMatrixStride∈[0, 2^32 -1], in elements. When dnNum is set to 1, set dstDnMatrixStride to 0, which does not take effect. |
| srcNzC0Stride | Input | Address offset between adjacent rows in the Nz fractal of the source matrix (head to head), with a value range of srcNzC0Stride∈[0, 65535], in C0_SIZE. When NZ2DN is enabled, srcNzC0Stride cannot be 0. |

## Return Value

None

## Constraints

- Before calling the DataCopy API to transfer data from the L0C Buffer to UB and performing real-time Nz format conversion to DN format, SetFixpipeNz2dnFlag must be called first to set the format conversion configuration.

## Example

```cpp
uint16_t dnNum = 1;
uint16_t srcNzMatrixStride = 0;
uint32_t dstDnMatrixStride = 0;
uint16_t srcNzC0Stride = 1;
AscendC::DataCopyCO12DstParams params;
params.mSize = 16;
params.nSize = 32;
params.srcStride = 16;
params.dstStride = 16;
params.nz2dnEn = true;
AscendC::SetFixpipeNz2dnFlag(dnNum, srcNzMatrixStride, dstDnMatrixStride, srcNzC0Stride);
AscendC::DataCopy(dst, src, params);
```
