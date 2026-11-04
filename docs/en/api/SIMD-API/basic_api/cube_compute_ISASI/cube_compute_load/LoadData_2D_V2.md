# LoadData (2D Matrix Copy V2)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:52:54.161Z pushedAt=2026-10-26T07:41:44.088Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_V2_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It is responsible for copying 2D-format data required for ordinary matrix computation, performed in units of 512-byte data fractals, and supports the following data paths:

- L1 Buffer -> L0A Buffer, L1 Buffer -> L0B Buffer.

For different data types, the matrix corresponding to each data fractal is as follows:

- For the b4 data type, each data fractal is a 16×64 matrix in the L0A Buffer and a 64×16 matrix in the L0B Buffer.
- For the b8 data type, each data fractal is a 16×32 matrix in the L0A Buffer and a 32×16 matrix in the L0B Buffer.
- For the b16 data type, each data fractal is a 16×16 matrix.
- For the b32 data type, each data fractal is a 16×8 matrix in the L0A Buffer and an 8×16 matrix in the L0B Buffer.

## Function Prototype

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParamsV2& loadDataParams)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ------ |
| **T** | Data type of the source operand and destination operand. |

**Table 2** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| **dst** | Output | Destination operand, type is **LocalTensor**.<br>For fractal constraints, see [Matrix Compute Input Copy Constraints](cube_compute_input_move_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](cube_compute_input_move_constraint.md).<br>The data type must be consistent with that of **src**.<br>Supported physical storage locations are L0A Buffer (TPosition: A2)/L0B Buffer (TPosition: B2). |
| **src** | Input | Source operand, type is **LocalTensor**.<br>For fractal constraints, see [Matrix Compute Input Copy Constraints](cube_compute_input_move_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](cube_compute_input_move_constraint.md).<br>The data type must be consistent with that of **dst**.<br>Supported physical storage locations are L1 Buffer (TPosition: A1/B1).|
| **loadDataParams** | Input | LoadData parameter structure, type is **LoadData2DParamsV2**. For details, see [Table 3](#table_load2dv2_params). |

**Table 3** Parameters in the LoadData2DParamsV2 structure<a id="table_load2dv2_params"></a>

| Parameter | Description |
| ---------- | ------ |
| **mStartPosition** | Taking an M×K matrix as an example, the start position of the source matrix in the M axis direction, in units of 16 elements. |
| **kStartPosition** | Taking an M×K matrix as an example, the start position of the source matrix in the K axis direction, in units of 32 bytes. |
| **mStep** | Taking an M×K matrix as an example, the copy length of the source matrix in the M axis direction, in units of 16 elements. Value range: mStep∈[0, 255].<br>When the transpose function is enabled through the **ifTranspose** parameter, in addition to satisfying the value range, **mStep** must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, **mStep** must be a multiple of 4;<br>&nbsp;&nbsp;&bull;When the data type is b8, **mStep** must be a multiple of 2;<br>&nbsp;&nbsp;&bull;When the data type is b16, **mStep** must be a multiple of 1;<br>&nbsp;&nbsp;&bull;When the data type is b32, **mStep** has no additional constraint.<br>**Note: mStep=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).** |
| **kStep** | Taking an M×K matrix as an example, the copy length of the source matrix in the K axis direction, in units of 32 bytes. Value range: kStep∈[0, 255].<br>When the transpose function is enabled through the **ifTranspose** parameter, in addition to satisfying the value range, **kStep** must also satisfy the following additional constraints:<br>&nbsp;&nbsp;&bull;When the data type is b4, b8, or b16, **kStep** has no additional constraint;<br>&nbsp;&nbsp;&bull;When the data type is b32, **kStep** must be a multiple of 2.<br>**Note: kStep=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).** |
| **srcStride** | Taking an M×K matrix as an example, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction of the source matrix, in units of 512 bytes. |
| **dstStride** | Taking an M×K matrix as an example, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction of the destination matrix, in units of 512 bytes. |
| **ifTranspose** | Whether to enable the transpose function, which transposes each fractal matrix, defaults to **false**:<br>&nbsp;&nbsp;&bull; **true**: Enabled<br>&nbsp;&nbsp;&bull; **false**: Not enabled<br>Note: Only the L1 Buffer (TPosition: A1)->L0A Buffer (TPosition: A2) and L1 Buffer (TPosition: B1)->L0B Buffer (TPosition: B2) paths can enable transpose. When the transpose function is enabled, b4, b8, b16, and b32 data types are supported. |
| **sid** | Reserved parameter. Set it to 0.<br>Note: For compatibility with the API of earlier products, the Ascend 950PR&950DT products ignores this parameter. |

When transpose is not enabled, the diagram of the **LoadData2DParamsV2** structure is as follows, along with the parameter setting values and explanations:

- **mStartPosition**=2, indicating that the start position of the source matrix in the M axis direction is the 32nd element (2×16 elements).
- **kStartPosition**=2, indicating that the start position of the source matrix in the K axis direction is the 32nd element (2×32 bytes).
- **mStep**=2, indicates that the copy length of the source matrix in the M axis direction is 32 elements (2×16 elements).
- **kStep**=3, indicates that the copy length of the source matrix in the K axis direction is 48 elements (3×32 bytes).
- **srcStride**=5, indicates that the interval between the start address of the previous fractal and the start address of the next fractal in the K direction of the source matrix is 1280 elements (5×512 bytes).
- **dstStride**=3, indicates that the interval between the start address of the previous fractal and the start address of the next fractal in the K direction of the destination matrix is 768 elements (3×512 bytes).
- **ifTranspose**=false, indicates that transpose is not enabled.
- **sid**=0, a reserved parameter. Set it to 0.

**Figure 1** Example of the **LoadData2DParamsV2** structure parameters (using the half data type without transpose as an example)

![](../../../../figures/loaddata2dparamsv2_half_no_transpose.png)

## Data Type

Supported data types: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.

## Return Value

None

## Constraints

- When **mStep** and **kStep** are 0, it indicates not executing the copy, and this API will be considered as a NOP (no operation).
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned; when the source address or destination address is in the L1 Buffer, the address must be 32-byte aligned.
- For matrix transpose operations, different data types require different **mStep** and **kStep** constraints: for the b4 data type, **mStep** must be a multiple of 4; for the b8 data type, **mStep** must be a multiple of 2; for the b16 data type, **mStep** must be a multiple of 1; for the b32 data type, **kStep** must be a multiple of 2.

## Key Feature Description

### Data Load in Non-transpose Scenarios and Load Start Position Calculation

Use **mStep** and **kStep** to copy data blocks in multiple directions. Take the NZ fractal of [M, K] as an example:

- **mStartPosition** is 2, indicating that the copy start position in the M direction is 16 × 2 = 32 elements away from the start position of the source operand along the M axis direction.
- **kStartPosition** is 2, indicating that the copy start position in the K direction is 32B × 2 = 64B away from the start position of the source operand along the K axis direction.
- **srcStride** is 5, indicating that in the source operand, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction is 5.
- **dstStride** is 3, indicating that in the destination operand, the interval between the start address of the previous fractal and the start address of the next fractal in the K direction is 3.

The start address calculation formula is:

$$
startAddr = srcAddr + (kStartPosition \times \lvert srcStride \rvert + mStartPosition) \times 512B
$$

Therefore, the final copy start position is the thirteenth fractal matrix (the first fractal matrix starts from the start address of the source operand).

**Figure 2** Copy start position diagram

![](../../../../figures/load2dv2_start_addr_demo.png "Copy start position diagram")

### Transpose Scenario Copy-In

For matrix transpose operations on **b4**/**b8**/**b16**/**b32**, the following constraints on **mStep** and **kStep** must be satisfied:

- For the **b4** data type, **mStep** must be a multiple of 4.
- For the **b8** data type, **mStep** must be a multiple of 2.
- For the **b16** data type, **mStep** must be a multiple of 1.
- For the **b32** data type, **kStep** must be a multiple of 2.

The transpose diagrams for each data type are as follows:

**Figure 3** Transpose diagram for the b4 data type

![](../../../../figures/load2dv2_l12l0_trans_b4.png "Transpose diagram for the b4 data type")

**Figure 4** Transpose diagram for the b8 data type

![](../../../../figures/load2dv2_l12l0_trans_b8.png "b8 data type transpose diagram")

**Figure 5** b16 data type transpose diagram

![](../../../../figures/load2dv2_l12l0_trans_b16.png "b16 data type transpose diagram")

**Figure 6** b32 data type transpose diagram

![](../../../../figures/load2dv2_l12l0_trans_b32.png "b32 data type transpose diagram")

## Example

The sample code snippet is as follows, showing only part of the code in the sample. For the complete sample, see [LoadData (2D Matrix Copy V2) sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dv2_l12l0).

```cpp
// LoadData: Nz -> Nz
uint32_t m = 40;
uint32_t k = 70;
uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t fractalSize = fractalShape[0] * fractalShape[1];
uint16_t mStep = (m + fractalShape[0] - 1) / fractalShape[0];

AscendC::LoadData2DParamsV2 loadDataParams;
loadDataParams.mStep = mStep;
loadDataParams.kStep = (k + fractalShape[1] - 1) / fractalShape[1];
loadDataParams.srcStride = mStep;
loadDataParams.dstStride = mStep;
loadDataParams.ifTranspose = false;
AscendC::LoadData(a2Local, a1Local, loadDataParams);
```
