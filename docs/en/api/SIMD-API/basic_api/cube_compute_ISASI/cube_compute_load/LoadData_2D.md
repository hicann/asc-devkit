# LoadData (2D Matrix Move)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:53:50.735Z pushedAt=2026-10-26T07:41:44.090Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It moves the 2D-format data required for ordinary matrix computation, moving data in units of 512-byte data fractals. It supports data movement over the following data paths:

- Supports GM->L0A Buffer, GM->L0B Buffer, L1 Buffer->L0A Buffer, and L1 Buffer->L0B Buffer.
<!-- npu="950" id10 -->
- Specifically for the Ascend 950PR&950DT products: only L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer are supported.
<!-- end id10 -->

For different data types, the matrix corresponding to each data fractal is as follows:

- For the b8 data type, each data fractal is a 16×32 matrix in the L0A Buffer and a 32×16 matrix in the L0B Buffer.
- For the b16 data type, each data fractal is a 16×16 matrix.
- For the b32 data type, each data fractal is a 16×8 matrix in the L0A Buffer and an 8×16 matrix in the L0B Buffer.

For the implementation principle, see the pseudocode: [LoadData (2D matrix move) pseudocode](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load2d.py).

## Prototype

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)

template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id11 -->
For the Ascend 950PR&950DT products:

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```
<!-- end id11 -->

## Parameters

**Table 1** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| **dst** | Output | Destination operand, of type **LocalTensor**.<br>For fractal constraints, see [Matrix Compute Input Move Constraints](cube_compute_input_move_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](cube_compute_input_move_constraint.md).<br>The data type is consistent with that of **src**.<br>Supported physical storage locations are **L0A Buffer** (**TPosition**: **A2**)/**L0B Buffer** (**TPosition**: **B2**).<br>The contiguous data arrangement order is determined by the **TPosition** where the destination operand resides: **A2** corresponds to the **ZZ** format/**NZ** format, with a fractal size of 16×(32 bytes/sizeof(T)); **B2** corresponds to the **ZN** format, with a fractal size of (32 bytes/sizeof(T))×16. |
| **src** | Input | Source operand, of type **LocalTensor** or **GlobalTensor**.<br>For fractal constraints, see [Matrix Compute Input Move Constraints](cube_compute_input_move_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](cube_compute_input_move_constraint.md).<br>The data type is consistent with that of **dst**.<br>When located in **L1 Buffer** (**TPosition**: **A1**/**B1**), there is no format requirement; generally the **NZ** format is used, with a fractal size of 16×(32 bytes/sizeof(T)). |
| **loadDataParams** | Input | **LoadData** parameter structure, of type **LoadData2DParams**. For details, see [Table 2](#table_load2d_params). |

**Table 2** Parameter description of the **LoadData2DParams** structure<a id="table_load2d_params"></a>

| Parameter | Description |
| ---------- | ------ |
| **startIndex** | Fractal matrix ID, indicating which fractal in the source operand the move start position is (0 indicates the first fractal matrix in the source operand). Value range: **startIndex**∈[0, 65535]. Unit: 512 bytes. Defaults to 0.<br>For feature details, see [Setting the Move Start Position](#setting-the-move-start-position). |
| **repeatTimes** | Number of iterations, where each iteration can process 512 bytes of data. Value range: **repeatTimes**∈[0, 255].<br>**Note: repeatTimes=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| **srcStride** | Interval between the start addresses of the previous fractal and the next fractal of the source operand between adjacent iterations, in units of 512 bytes. Value range: **srcStride**∈[0, 65535]. Defaults to 0.<br>For feature details, see [Non-contiguous Move](#non-contiguous-move).<br>**Note: srcStride=0 means the same fractal matrix is fetched repeatedly between consecutive repeat execution cycles.** |
| **sid** | Users do not need to pay attention to this parameter; set it to 0.<br>**Note: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed.** |
| **dstGap** | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand between adjacent iterations, in units of 512 bytes. Value range: **dstGap**∈[0, 65535]. Defaults to 0.<br>For feature details, see [Non-contiguous Move](#non-contiguous-move).<br>**Note: dstGap=0 means the start addresses of the destination operands of adjacent repeats are separated by one data fractal, that is, stored contiguously.** |
| **ifTranspose** | Whether to enable transpose, which transposes each fractal matrix. Defaults to **false**:<br>&nbsp;&nbsp;&bull; **true**: Enabled<br>&nbsp;&nbsp;&bull; **false**: Disabled<br>For feature details, see [Fractal Transpose](#fractal-transpose).<br>**Note: Transpose can be enabled only on the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths. When transpose is enabled, the source operand and destination operand support only the b16 data type.** |
| **addrMode** | Controls whether the fractal matrix index ID of each iteration in the source operand increments or decrements in multi-iteration scenarios:<br>&nbsp;&nbsp;&bull; **0** (default): Increment, next repeat index = startIndex + srcStride \* repeatTimes;<br>&nbsp;&nbsp;&bull; **1**: Decrement, next repeat index = startIndex - srcStride \* repeatTimes.<br>For feature details, see [Control Address Update Mode](#control-address-update-mode).<br>**Note: Currently, only the GM->L1 Buffer path supports this configuration. Keep the default value 0; this parameter does not affect performance.** |

## Data Type

<!-- npu="950" id12 -->
Ascend 950PR&950DT products support the following data types: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float.
<!-- end id12 -->

<!-- npu="A3" id13 -->
Atlas A3 products support the following data types: int4b_t, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float. **Note: int4b_t supports only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.**
<!-- end id13 -->

<!-- npu="910b" id14 -->
Atlas A2 products support the following data types: int4b_t, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float. **Note: int4b_t supports only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.**
<!-- end id14 -->

<!-- npu="310b" id15 -->
Atlas 200I/500 A2 inference products support the following data types: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float.
<!-- end id15 -->

<!-- npu="910" id16 -->
Atlas training products support the following data types: uint8_t, int8_t, uint16_t, int16_t, and half.
<!-- end id16 -->

<!-- npu="310p" id17 -->
Atlas inference products AI Core support the following data types: int4b_t, uint8_t, int8_t, uint16_t, int16_t, and half. **Note: int4b_t supports only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.**
<!-- end id17 -->

## Return Value Description

None

## Constraints

- When **repeatTimes** is 0, no move is performed, and the API is treated as a NOP (no operation).
- Transpose can be enabled only on the L1 Buffer -> L0A Buffer/L0B Buffer path. When transpose is enabled, both the source operand and the destination operand support only the **b16** data type.
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned. When the source address or destination address is in the L1 Buffer, the address must be 32-byte aligned. When the source address is in GM, the address must be 1-byte aligned.
- When the source address is in GM, the instruction is executed on the **PIPE_MTE2** pipeline. When the source address is in the L1 Buffer, the instruction is executed on the **PIPE_MTE1** pipeline.
- When **srcStride** is 0, the same data fractal in the source operand is read across consecutive repeats.
<!-- npu="310p" id20 -->
- For the Atlas inference products AI Core, when used with the **Mmad** API and the B matrix data type is **S4**, if transpose is enabled through the **ifTranspose** parameter, only the 64×64 fractal is supported.
<!-- end id20 -->
- The physical storage locations vary by model. Developers can refer to the following:
    - The supported physical storage locations are Global Memory (TPosition: GM)/L1 Buffer (TPosition: A1/B1).
    <!-- npu="950" id21 -->
    - Specifically for the Ascend 950PR&950DT products, the supported physical storage location is the L1 Buffer (TPosition: A1/B1).
    <!-- end id21 -->

<!-- npu="950" id22 -->
- Especially for the Ascend 950PR&950DT products, this API is a compatibility implementation. Compared with the Atlas A3 products and the Atlas A2 products, because the fractal supported on the L0A Buffer has changed from Zz to Nz, the operator implementation code cannot be directly reused and must be re-adapted according to the fractal change. For the specific adaptation solution, see the matrix computation section in [Basic API Migration Guide](../../../../../guide/cross_gen_migration_guide/3510_arch_migration/2201_to_3510_guide/basic_api_migration.md#cube-computation).
<!-- end id22 -->

## Key Feature Description

### Non-contiguous Move

- Use the **srcStride** and **dstGap** parameters to perform skip-read and skip-write, moving four **float**-type fractals from the L1 Buffer to the L0A Buffer.

    **startIndex** is 0: indicates that the start position is the first fractal in the source operand (0 is the first fractal matrix in the source operand).

    **repeatTimes** is 4: indicates that each instruction moves four data fractals.

    **srcStride** is 2: indicates that on the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

    **dstGap** is 2: indicates that on the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

    ![](../../../../figures/load2d_l12l0a_noncontinuous.png)

- Move six **float**-type data fractals from the L1 Buffer to the L0A Buffer without enabling transpose.

    The move instruction must be called in a loop three times. For each call, the source operand address must be offset by two data fractals, and the destination operand address must be offset by one data fractal.

    **repeatTimes** is 2: indicates that each instruction moves two data fractals.

    **srcStride** is 1: On the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 1 data fractal, indicating that the data fractals are physically adjacent.

    **dstGap** is 2: On the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    ![](../../../../figures/load2d_l12l0a_noncontinuous_float.png)

- Move 8 data fractals of the **float** data type from **L1 Buffer** to **L0B Buffer** without enabling transpose.

    The move instruction must be called in a loop twice. For each call, the source operand address must be offset by 1 data fractal, and the destination operand address must be offset by 1 data fractal.

    **repeatTimes** is 4: Each instruction moves 4 data fractals.

    **srcStride** is 2: On the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    **dstGap** is 1: On the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by 1 data fractal.

    ![](../../../../figures/load2d_l12l0b_noncontinuous_float.png)

### Setting the Move Start Position

Move four **float** data fractals from **L1 Buffer** to **L0A Buffer**, and set **startIndex** to 1 to skip the first fractal.

**startIndex** is 1: The move start position is the second fractal in the source operand.

**repeatTimes** is 4: Each instruction moves four data fractals.

**srcStride** is 2: In the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

**dstGap** is 2: In the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

![](../../../../figures/load2d_l12l0a_set_start_pos.png)

### Fractal Transpose

- When the data type is half, set **ifTranspose** to 1 to enable transpose.

    **startIndex** is 0: Indicates that the move start position is the first fractal in the source operand.

    **repeatTimes** is 2: Indicates that each instruction moves 2 data fractals.

    **srcStride** is 2: Indicates that, on the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    **dstGap** is 2: Indicates that, on the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    **ifTranspose**=1: Indicates that transpose is enabled.

    ![](../../../../figures/load2d_l12l0a_set_frac_trans_demo1.png)

- Move 6 data fractals of the half data type from the L1 Buffer to the L0A Buffer, with transpose enabled.

    The move instruction needs to be called in a loop 2 times. For each call, the source operand address needs to be offset by 1 data fractal, and the destination operand address needs to be offset by 1 data fractal.

    **repeatTimes** is 3: Each instruction moves 3 data fractals.

    **srcStride** is 2: On the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    **dstGap** is 1: On the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by 1 data fractal.

    **ifTranspose**=1: Transpose is enabled.

    ![](../../../../figures/load2d_l12l0a_set_frac_trans_demo2.png)

### Control Address Update Mode

Move four **float** fractals from GM to L1 Buffer, and set **addrMode** to 1 to change the update mode of the source operand address.

**addrMode** is 1: The source operand address subtracts **srcStride** from the previous address in each iteration.

**startIndex** is 6: The move start position is the 7th fractal in the source operand.

**repeatTimes** is 4: Each instruction moves 4 data fractals.

**srcStride** is 2: On the source operand, the interval between the start address of the previous data fractal and the start address of the next data fractal is 2 data fractals.

**dstGap** is 2: On the destination operand, the interval between the end address of the previous data fractal and the start address of the next data fractal is 2 data fractals.

![](../../../../figures/load2d_l12l0a_control_update_addr.png)

## Example

First, in the following example, when matrix A is not transposed and the data type is half, you can directly call the **LoadData** (2D matrix move) API without enabling transpose-related parameters.

The following figure shows the data layout changes during the move process:

![](../../../../figures/load2d_l12l0a_nontrans.png)

The sample code snippet is as follows. It shows only part of the sample code. For the complete sample, see [load_data_l12l0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize;
uint32_t srcOffset = fractalSize;
// Nz -> Zz
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(m, fractalShape[0]);
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = false;
for (int i = 0; i < CeilDivision(m, fractalShape[0]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

Second, in the following example, when matrix A is transposed and the data type is half, you need to enable transpose-related parameters when calling the **LoadData** (2D matrix move) API.

The following figure shows the data layout changes during the move process:

![](../../../../figures/load2d_l12l0a_trans.png)

The sample code snippet is as follows. It shows only part of the sample code. For the complete sample, see [load_data_l12l0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
uint32_t dstOffset = CeilDivision(k, fractalShape[0]) * fractalSize;
uint32_t srcOffset = CeilDivision(k, fractalShape[0]) * fractalSize;
// Nz -> Zz
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0]);
// Source operand, inner axis, between adjacent iterations.
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.ifTranspose = true;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
    AscendC::LoadData(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```
