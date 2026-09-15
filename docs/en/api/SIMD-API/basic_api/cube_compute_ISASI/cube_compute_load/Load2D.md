# LoadData (2D Cube Transfer)

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:27:56.236Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Supported
<!-- end id7 -->
## Description

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It transfers 2D-format data required for ordinary cube computation, in units of data fractals of 512 bytes each, and supports transfers over the following data paths:

- Supports GM->L0A Buffer, GM->L0B Buffer, L1 Buffer->L0A Buffer, and L1 Buffer->L0B Buffer.
<!-- npu="950" id10 -->
- Specifically for Ascend 950PR/Ascend 950DT: only L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer are supported.
<!-- end id10 -->

For different data types, the cube corresponding to each data fractal is as follows:

- For the b8 data type, each data fractal is a 16×32 cube in the L0A Buffer and a 32×16 cube in the L0B Buffer.
- For the b16 data type, each data fractal is a 16×16 cube.
- For the b32 data type, each data fractal is a 16×8 cube in the L0A Buffer and an 8×16 cube in the L0B Buffer.

For the implementation principle, see the pseudocode: [Load2D pseudocode](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load2d.py).

## Prototype

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)

template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

<!-- npu="950" id11 -->
For Ascend 950PR/Ascend 950DT:

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2DParams& loadDataParams)
```
<!-- end id11 -->

## Parameters

**Table 1** General parameter description

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| dst | Output | Destination operand, of type **LocalTensor**.<br>For fractal constraints, see [Cube Computation Input Transfer Constraints](matrix_computation_input_movement_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](matrix_computation_input_movement_constraint.md).<br>The data type is consistent with that of **src**.<br>The supported physical storage locations are **L0A Buffer** (**TPosition**: A2)/**L0B Buffer** (**TPosition**: B2).<br>The contiguous data arrangement order is determined by the **TPosition** of the destination operand: A2 corresponds to the ZZ/NZ format, with a fractal size of 16×(32 bytes/sizeof(T)); B2 corresponds to the ZN format, with a fractal size of (32 bytes/sizeof(T))×16. |
| src | Input | Source operand, of type **LocalTensor** or **GlobalTensor**.<br>For fractal constraints, see [Cube Computation Input Transfer Constraints](matrix_computation_input_movement_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](matrix_computation_input_movement_constraint.md).<br>The data type is consistent with that of **dst**.<br>When located in **L1 Buffer** (**TPosition**: A1/B1), there is no format requirement; in general, the NZ format is used, with a fractal size of 16×(32 bytes/sizeof(T)). |
| loadDataParams | Input | **LoadData** parameter structure, of type **LoadData2DParams**. For details, see [Table 2](#table_load2d_params). |

**Table 2** Parameter description of the LoadData2DParams structure<a id="table_load2d_params"></a>

| Parameter | Description |
| ---------- | ------ |
| startIndex | Fractal cube ID, indicating that the transfer start position is the Nth fractal in the source operand (0 indicates the first fractal cube in the source operand). Value range: **startIndex**∈[0, 65535]. Unit: 512 bytes. Default: 0.<br>For feature details, see [Setting the Transfer Start Position](#setting-the-transfer-start-position). |
| repeatTimes | Number of iterations. Each iteration can process 512 bytes of data. Value range: **repeatTimes**∈[0, 255].<br>**Note: repeatTimes=0 indicates that no transfer is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Interval between the start addresses of the previous and next fractals of the source operand across adjacent iterations, in units of 512 bytes. Value range: **srcStride**∈[0, 65535]. Default: 0.<br>For feature details, see [Non-contiguous Transfer In](#non-contiguous-transfer).<br>**Note: srcStride=0 indicates that the same fractal cube is fetched repeatedly across consecutive repetition cycles.** |
| sid | This parameter does not require user attention; set it to 0.<br>**Note: This is a reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no business processing is performed on it.** |
| dstGap | Interval between the end address of the previous fractal and the start address of the next fractal of the destination operand across adjacent iterations, in units of 512 bytes. Value range: **dstGap**∈[0, 65535]. Default: 0.<br>For feature details, see [Non-contiguous Transfer In](#non-contiguous-transfer).<br>**Note: dstGap=0 indicates that the start addresses of the destination operands of adjacent repeats are separated by one data fractal, that is, they are stored contiguously.** |
| ifTranspose | Whether to enable the transpose function to transpose each fractal cube. Default: **false**:<br>&nbsp;&nbsp;&bull; **true**: Enabled<br>&nbsp;&nbsp;&bull; **false**: Disabled<br>For feature details, see [Fractal Transpose](#fractal-transpose).<br>**Note: Transpose can be enabled only on the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths. When transpose is enabled, the source operand and destination operand support only the b16 data type.** |
| addrMode | Controls whether the fractal cube index ID of each iteration in the source operand increments or decrements in multi-iteration scenarios:<br>&nbsp;&nbsp;&bull; **0** (default): Increment. The next repeat index = startIndex + srcStride \* repeatTimes.<br>&nbsp;&nbsp;&bull; **1**: Decrement. The next repeat index = startIndex - srcStride \* repeatTimes.<br>For feature details, see [Controlling the Address Update Mode](#controlling-the-address-update-mode).<br>**Note: Currently, only the GM->L1 Buffer path supports this configuration. Keep the default value 0; this parameter does not affect performance.** |

## Data Types

<!-- npu="950" id12 -->
Ascend 950PR/Ascend 950DT supports the following data types: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float.
<!-- end id12 -->

<!-- npu="A3" id13 -->
Atlas A3 training products/Atlas A3 inference products support the following data types: int4b_t, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float. **Note: int4b_t supports only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.**
<!-- end id13 -->

<!-- npu="910b" id14 -->
Atlas A2 training products/Atlas A2 inference products support the following data types: int4b_t, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float. **Note: int4b_t supports only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.**
<!-- end id14 -->

<!-- npu="310b" id15 -->
Atlas 200I/500 A2 inference products support the following data types: uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, and float.
<!-- end id15 -->

<!-- npu="910" id16 -->
Atlas training products support the following data types: uint8_t, int8_t, uint16_t, int16_t, and half.
<!-- end id16 -->

<!-- npu="310p" id17 -->
Atlas inference products AI Core supports the following data types: int4b_t, uint8_t, int8_t, uint16_t, int16_t, and half. **Note: int4b_t supports only the L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer paths.**
<!-- end id17 -->

## Return Value

None

## Constraints

- When **repeatTimes** is 0, no transfer is performed, and the API is treated as a NOP (no operation).
- Transpose can be enabled only on the L1 Buffer -> L0A Buffer/L0B Buffer path. When transpose is enabled, both the source operand and the destination operand support only the b16 data type.
- When the destination address is in L0A Buffer/L0B Buffer, the address must be 512-byte aligned. When the source address or destination address is in L1 Buffer, the address must be 32-byte aligned. When the source address is in GM, the address must be 1-byte aligned.
- When the source address is in GM, the instruction occupies the PIPE_MTE2 pipeline during execution. When the source address is in L1 Buffer, the instruction occupies the PIPE_MTE1 pipeline during execution.
- When **srcStride** is 0, the same data fractal in the source operand is read between consecutive repeats.
<!-- npu="310p" id20 -->
- For the AI Core of Atlas inference products, when used with the **Mmad** API and the B cube data type is S4, if transpose is enabled through the **ifTranspose** parameter, only 64×64 fractals are supported.
<!-- end id20 -->
- The physical storage locations vary by model. Developers can refer to the following:
    - The supported physical storage locations are Global Memory (TPosition: GM)/L1 Buffer (TPosition: A1/B1).
    <!-- npu="950" id21 -->
    - Specifically for Ascend 950PR/Ascend 950DT, the supported physical storage location is L1 Buffer (TPosition: A1/B1).
    <!-- end id21 -->

<!-- npu="950" id22 -->
- For Ascend 950PR/Ascend 950DT in particular, setting the **dstGap** parameter is invalid.
<!-- end id22 -->

## Key Feature Details

### Non-contiguous Transfer

- Uses the **srcStride** and **dstGap** parameters to perform skip-read and skip-write, transferring four **float** data fractals from the L1 Buffer to the L0A Buffer.

    **startIndex** is 0: indicates that the transfer start position is the first fractal in the source operand (0 represents the first fractal cube in the source operand).

    **repeatTimes** is 4: indicates that each instruction transfers four data fractals.

    **srcStride** is 2: indicates that, on the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

    **dstGap** is 2: indicates that, on the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

    ![](../../../../figures/load2d_l12l0a_noncontinuous.png)

- Transfers six data fractals of the **float** data type from the L1 Buffer to the L0A Buffer without enabling transpose.

    The transfer instruction must be called three times in a loop. For each call, the source operand address must be offset by two data fractals, and the destination operand address must be offset by one data fractal.

    **repeatTimes** is 2: indicates that each instruction transfers two data fractals.

    **srcStride** is 1: On the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 1 data fractal, indicating that the data fractals are physically adjacent.

    **dstGap** is 2: On the destination operand, the tail address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    ![](../../../../figures/load2d_l12l0a_noncontinuous_float.png)

- Transfer 8 data fractals of the float data type from the L1 Buffer to the L0B Buffer without enabling transpose.

    The transfer instruction needs to be called repeatedly 2 times. For each call, the source operand address needs to be offset by 1 data fractal, and the destination operand address needs to be offset by 1 data fractal.

    **repeatTimes** is 4: Each instruction transfers 4 data fractals.

    **srcStride** is 2: On the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    **dstGap** is 1: On the destination operand, the tail address of the previous data fractal and the start address of the next data fractal are separated by 1 data fractal.

    ![](../../../../figures/load2d_l12l0b_noncontinuous_float.png)

### Setting the Transfer Start Position

Transfers four float data fractals from the L1 Buffer to the L0A Buffer, skipping the first fractal by setting **startIndex** to 1.

**startIndex** set to 1: Indicates that the transfer start position is the second fractal in the source operand.

**repeatTimes** set to 4: Indicates that each instruction transfers four data fractals.

**srcStride** set to 2: Indicates that, on the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

**dstGap** set to 2: Indicates that, on the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by two data fractals.

![](../../../../figures/load2d_l12l0a_set_start_pos.png)

### Fractal Transpose

- When the data type is half, set **ifTranspose** to 1 to enable transpose.

    **startIndex** is 0: The transfer start position is the first fractal in the source operand.

    **repeatTimes** is 2: Each instruction transfers 2 data fractals.

    **srcStride** is 2: In the source operand, the start address of the next data fractal is 2 data fractals away from the start address of the previous data fractal.

    **dstGap** is 2: In the destination operand, the start address of the next data fractal is 2 data fractals away from the end address of the previous data fractal.

    **ifTranspose**=1: Transpose is enabled.

    ![](../../../../figures/load2d_l12l0a_set_frac_trans_demo1.png)

- Transfer 6 data fractals of the half data type from the L1 Buffer to the L0A Buffer with transpose enabled.

    The transfer instruction needs to be called twice in a loop. For each call, the source operand address is offset by 1 data fractal, and the destination operand address is offset by 1 data fractal.

    **repeatTimes** is 3: Each instruction transfers 3 data fractals.

    **srcStride** is 2: On the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

    **dstGap** is 1: On the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by 1 data fractal.

    **ifTranspose** is 1: Transpose is enabled.

    ![](../../../../figures/load2d_l12l0a_set_frac_trans_demo2.png)

### Controlling the Address Update Mode

Transfers four float data fractals from GM to the L1 Buffer, and sets **addrMode** to 1 to change the update mode of the source operand address.

**addrMode** set to 1: The source operand address is decremented by **srcStride** from the previous address at each iteration.

**startIndex** set to 6: Indicates that the transfer start position is the 7th fractal in the source operand.

**repeatTimes** set to 4: Indicates that each instruction transfers 4 data fractals.

**srcStride** set to 2: Indicates that, on the source operand, the start address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

**dstGap** set to 2: Indicates that, on the destination operand, the end address of the previous data fractal and the start address of the next data fractal are separated by 2 data fractals.

![](../../../../figures/load2d_l12l0a_control_update_addr.png)

## Example

First, in the following example, when cube A is not transposed and the data type is half, the Load2D API can be called directly without enabling transpose-related parameters.

The following figure shows the data layout changes during the transfer process:

![](../../../../figures/load2d_l12l0a_nontrans.png)

The sample code snippet is as follows. Only part of the code in the sample is shown. For the complete sample, see [load_data_l12l0 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/load_data_l12l0).

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

Second, in the following example, when cube A is transposed and the data type is half, transpose-related parameters must be enabled when calling the Load2D API.

The following figure shows the data layout changes during the transfer process:

![](../../../../figures/load2d_l12l0a_trans.png)

The sample code snippet is as follows. Only part of the code in the sample is shown. For the complete sample, see [load_data_l12l0 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/load_data_l12l0).

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
