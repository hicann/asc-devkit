# LoadData (GMToL1-2D Matrix Transfer V2)<a id="ZH-CN_TOPIC_0000002594327301"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:03:43.541Z pushedAt=2026-10-26T07:41:44.099Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2_res.md#id1 -->

## Description<a id="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It loads the 2D-format data required for ordinary matrix computation, transferring data from Global Memory to the L1 Buffer (TPosition is A1/B1) in units of 512-byte data fractals.

## Function Prototype<a id="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand and destination operand. |

**Table 2** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, whose type is **LocalTensor**. The supported physical storage location is L1 Buffer (**TPosition** is A1/B1).<br>There is no format requirement for the data fractal, and the NZ format is used in general. In the NZ format, the corresponding fractal size is 16 \* (32 bytes / sizeof(T)). |
| src | Input | Source operand, whose type is **GlobalTensor**. The data type must be consistent with **dst**. |
| loadDataParams | Input | LoadData parameter structure, whose type is **LoadData2DParamsV2**. For details, see [Table 3](#table49630346128). |

**Table 3** Parameter description of the LoadData2DParamsV2 structure<a id="table49630346128"></a>

| Parameter | Description |
| ---------- | ---------- |
| mStartPosition | Taking the M*K matrix as an example, the start position of the source matrix in the M axis direction, in units of 16 elements. |
| kStartPosition | Taking the M*K matrix as an example, the start position of the source matrix in the K axis direction, in units of 32 bytes. |
| mStep | Taking the M*K matrix as an example, the load length of the source matrix in the M axis direction, in units of 16 elements. Value range: mStep∈[0, 255].<br>**Note: mStep=0 means no load is performed, and this API is treated as a NOP (no operation).** |
| kStep | Taking the M*K matrix as an example, the load length of the source matrix in the K axis direction, in units of 32 bytes. Value range: kStep∈[0, 255].<br>**Note: kStep=0 means no load is performed, and this API is treated as a NOP (no operation).** |
| srcStride | Taking the M*K matrix as an example, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction of the source matrix, in units of 512 bytes. |
| dstStride | Taking the M*K matrix as an example, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction of the destination matrix, in units of 512 bytes. |
| ifTranspose | Whether to enable the transpose function, which transposes each fractal matrix, defaulting to **false**.<br>&nbsp;&nbsp;&bull; **true**: Enable.<br>&nbsp;&nbsp;&bull; **false**: Disable.<br>Note: In this path scenario (GM->L1 Buffer), transpose is not supported, and this parameter is meaningless. Keep the default value. |
| sid | Reserved parameter. Set it to 0. |

When transpose is not enabled, the schematic diagram of the LoadData2DParamsV2 structure is as follows. The parameter setting values and explanations are as follows:

- mStartPosition = 2, indicating that the start position of the source matrix in the M axis direction is the 32nd element (2*16 elements).
- kStartPosition = 2, indicating that the start position of the source matrix in the K axis direction is the 32nd element (2*32 bytes).
- **mStep** = 2, indicating that the load length in the M-axis direction of the source matrix is 32 elements (2*16 elements).
- **kStep** = 3, indicating that the source matrix K axis direction movement length is 48 elements (3*32 bytes).
- **srcStride** = 5, indicating that the address interval between the previous fractal start address and the next fractal start address in the K direction of the source matrix is 1280 elements (5*512 bytes).
- **dstStride** = 3, indicating that the address interval between the previous fractal start address and the next fractal start address in the K direction of the destination matrix is 768 elements (3*512 bytes).
- **ifTranspose** = **false**, indicating that transpose is not enabled.
- **sid** = 0, a reserved parameter. Set it to 0.

**Figure 1** Example of the LoadData2DParamsV2 structure parameters (with half data type and transpose disabled as an example)<a id="fig13901164574218"></a>

![](../../../../figures/loaddata2dparamsv2_half_no_transpose.png)

## Data Type<a id="section4219135304818"></a>

Supported data types: uint8_t, int8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, uint32_t, int32_t, float.

## Return Value

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- Transpose is not supported in this path scenario.

## Example<a id="section6461234123118"></a>

Using the scenario shown in [Figure 1](#fig13901164574218) as an example, the source matrix is half data in NZ layout on GM. Starting from the second fractal on the M axis and the second 32B block on the K axis, load two M-direction fractals and three K-direction 32B blocks into the L1 Buffer.

```cpp
constexpr uint32_t fractalElemCount = 256;
constexpr uint32_t srcElemCount = 25 * fractalElemCount;
constexpr uint32_t dstElemCount = 9 * fractalElemCount;

// Source operand: half data stored in NZ fractal layout on GM. One 512B fractal contains 256 half elements.
// The GM shape is 80 * 80, with five small fractals in each of the M and K directions, so reserve 5 * 5 = 25 512B fractals.
AscendC::GlobalTensor<half> srcGm;
srcGm.SetGlobalBuffer((__gm__ half *)src, srcElemCount);

// Destination operand: L1 Buffer. The destination matrix is 3 * 3 small fractals, so reserve 9 512B fractals.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, 0, dstElemCount);

AscendC::LoadData2DParamsV2 loadDataParams;
// The M-axis start position of the source matrix is the 32nd element, that is, 2 * 16 elements.
loadDataParams.mStartPosition = 2;
// The K-axis start position of the source matrix is the 32nd half, that is, 2 * 32B.
loadDataParams.kStartPosition = 2;
// Load 32 elements in the M direction, that is, 2 * 16 elements.
loadDataParams.mStep = 2;
// Load 48 half elements in the K direction, that is, 3 * 32B.
loadDataParams.kStep = 3;
// The GM source matrix has 5 small fractals in the M direction.
loadDataParams.srcStride = 5;
// The destination A1 matrix has 3 small fractals in the M direction.
loadDataParams.dstStride = 3;
// Transpose is not enabled.
loadDataParams.ifTranspose = false;
// Reserved parameter, fixed to 0.
loadDataParams.sid = 0;

AscendC::LoadData(dstLocal, srcGm, loadDataParams);
```
