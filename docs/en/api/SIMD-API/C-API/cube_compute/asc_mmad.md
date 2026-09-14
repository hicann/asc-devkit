# asc_mmad

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:33:56.345Z -->

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
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products AI Core: Not supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products Vector Core: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->

## Description

Performs a matrix multiply-add operation. The calculation formula is as follows:

$$
c_{matrix} = (a_{matrix} * b_{matrix}) + c_{matrix}
$$

## Prototype

- Regular computation
    ```cpp
    __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_s4(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix,  uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    ```

    <!-- npu="950" id4 -->

    ```cpp
    // The following prototype is supported only on Ascend 950PR/Ascend 950DT.
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad(__cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    ```
<!-- end id4 -->



- Synchronous computation
    ```cpp
    __aicore__ inline void asc_mmad_s4_sync(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_s4_sync(__cc__ int32_t* c_matrix, __ca__ int4b_t* a_matrix, __cb__ int4b_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix,  uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool k_direction_align, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t feat_offset, uint8_t smask_offset, uint8_t unit_flag, bool k_direction_align, bool is_weight_offset, bool c_matrix_source, bool c_matrix_init_val);
    ```

    <!-- npu="950" id5 -->

    ```cpp
    // The following prototype is supported only on Ascend 950PR/Ascend 950DT.
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ bfloat16_t* a_matrix, __cb__ bfloat16_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ half* a_matrix, __cb__ half* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ float* a_matrix, __cb__ float* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ int32_t* c_matrix, __ca__ int8_t* a_matrix, __cb__ int8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    __aicore__ inline void asc_mmad_sync(__cc__ float* c_matrix, __ca__ hifloat8_t* a_matrix, __cb__ hifloat8_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val);
    ```
<!-- end id5 -->



## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| c_matrix      | Output    | 	Destination operand, the result matrix.            |
| a_matrix      | Input    |   Source operand, left matrix A.|
| b_matrix      | Input    | 	Source operand, right matrix B.      |
| left_height   | Input    | 	Height of the left matrix, with a value range of [0, 4095].      |
| n_dim         | Input    | 	Width of the left matrix and height of the right matrix, with a value range of [0, 4095].      |
| right_width   | Input    | 	Width of the right matrix, with a value range of [0, 4095].      |
| feat_offset   | Input    |     Reserved parameter. |
| smask_offset  | Input    |     Offset bit of the weight matrix. |
| unit_flag     | Input    | 	**unit_flag** enables fine-grained parallelism between the **asc_mmad** API and Fixpipe instructions. When this feature is enabled, the hardware moves out the computation result after each fractal is computed. This feature is not applicable to L0C Buffer accumulation scenarios. The value description is as follows: <br>&bull; **0**: Disables **unit_flag**.<br>&bull; **1**: Reserved value.<br>&bull; **2**: Enables **unit_flag**. After the hardware finishes executing the instruction, the **unit_flag** feature is not disabled.<br>&bull; **3**: Enables **unit_flag**. After the hardware finishes executing the instruction, the **unit_flag** feature is disabled.<br> When this feature is enabled, set **unit_flag** to 3 for the last fractal of the matrix computation and to 2 for the remaining fractals.     |
| k_direction_align  | Input    | When the source operands and the destination operand are of the float type, the matrices in L0A and L0B are aligned to 48 in the right_width direction using ceil(right_width/16)*16. For right_width=44, all 12 fractals in L0A/L0B are read into the Cube, whereas for right_width=36, only 10 fractals in L0A/L0B are read into the Cube. |
| is_weight_offset  | Input    | Enables weight matrix offset.|
| c_matrix_source       | Input    | 	Configures whether the initial value of the C matrix comes from C2 (the hardware cache area that stores Bias). The value description is as follows:  <br>&bull; **true**: Comes from C2. <br>&bull; **false**: Comes from CO1 (L0C). |
| c_matrix_init_val      | Input    | 	Configures whether the initial value of the C matrix is 0. The value description is as follows:  <br>&bull; **true**: The initial value of the C matrix is 0. <br>&bull; **false**: The initial value of the C matrix is configured through the **c_matrix_source** parameter.     |
|<!-- npu="950" id6 --> disable_gemv | Input | Whether to disable GEMV mode. **false** enables GEMV mode, and **true** disables GEMV mode.<br>General Matrix-Vector Multiplication (GEMV) implements the product of a matrix and a vector. When left_height=1, after GEMV is enabled, data is read from the L0A Buffer in ND format instead of being treated as ZZ format.<!-- end id6 -->|

## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- When any of **left_height**, **right_width**, or **n_dim** is 0, this instruction is not executed.
- For operand address alignment constraints, see [General Address Alignment Constraints](../general_description_and_constraints.md#general-address-alignment-constraints).

## Example

```cpp
// total_length indicates the total length of data to be moved.
constexpr uint64_t total_length = 128;
// The following three parameters correspond to the addresses of matrices c, a, and b, respectively.
__cc__ int32_t c_matrix[total_length];
__ca__ int8_t a_matrix[total_length];
__cb__ int8_t b_matrix[total_length];
// The remaining input parameters are passed with their default values.
uint16_t left_height = 16;
uint16_t n_dim = 16;
uint16_t right_width =16;
uint8_t unit_flag = 0;
bool disable_gemv = false;
bool c_matrix_source = false;
bool c_matrix_init_val = true;
// Call the function.
asc_mmad_sync(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
```
