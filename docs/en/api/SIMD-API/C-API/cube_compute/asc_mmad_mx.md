# asc_mmad_mx

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:34:16.298Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
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

## Description

Performs a matrix multiply-add operation with scaling. The calculation formula is as follows:

$$
c_{matrix} = (a_{matrix} * b_{matrix}) + c_{matrix}
$$

## Prototype

- Regular computation

    ```cpp
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val) 
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    ```

## Parameters

| Parameter       | Input/Output | Description               |
| --------- | ----- | ---------------- |
| **c_matrix**      | Output    | 	Destination operand, the result matrix.            |
| **a_matrix**      | Input    |   Source operand, left matrix A.|
| **b_matrix**      | Input    | 	Source operand, right matrix B.      |
| **left_height**   | Input    | 	Height of the left matrix, with a value range of [0, 4095].      |
| **n_dim**         | Input    | 	Width of the left matrix and height of the right matrix, with a value range of [0, 4095].      |
| **right_width**   | Input    | 	Width of the right matrix, with a value range of [0, 4095].      |
| **unit_flag**     | Input    | 	**unit_flag** is a fine-grained parallelism feature of the **asc_mmad_mx** API. When this feature is enabled, the hardware moves out the computation result each time it finishes computing one fractal. This feature is not applicable to the scenario of L0C Buffer accumulation. The value description is as follows: <br>&bull; **0**: Reserved value. <br>&bull; **2**: Enables **unit_flag**. After the hardware finishes executing the instruction, the **unit_flag** feature is not disabled.<br>&bull; **3**: Enables **unit_flag**. After the hardware finishes executing the instruction, the **unit_flag** feature is disabled.<br> When enabling this feature, set **unit_flag** of the matrix computation to 3 for the last fractal and to 2 for the remaining fractals.     |
| **disable_gemv**  | Input    | Whether to disable GEMV mode. **false** enables GEMV mode, and **true** disables GEMV mode.<br>GEMV (General Matrix-Vector Multiplication) implements the product of a matrix and a vector. When **left_height** is 1, after GEMV is enabled, data is read from the L0A Buffer in ND format instead of being treated as ZZ format. |
| **c_matrix_source**       | Input    | 	Configures whether the initial value of the C matrix comes from the BiasTable (the hardware cache area that stores Bias). The value description is as follows:  <br>&bull; **true**: Comes from the BiasTable. <br>&bull; **false**: Comes from L0C. |
| **c_matrix_init_val**      | Input    | 	Configures whether the initial value of the C matrix is 0. The value description is as follows:  <br>&bull; **true**: The initial value of the C matrix is 0. <br>&bull; **false**: The initial value of the C matrix is configured through the **c_matrix_source** parameter.     |

## Return Value

None

## Pipeline Type

PIPE_M

## Constraints

- When any of **left_height**, **right_width**, or **n_dim** is **0**, this instruction is not executed.
- When GEMV mode is enabled, that is, **disable_gemv=false**, **left_height=1** must be satisfied.
- For operand address alignment constraints, see [General Address Alignment Constraints](../general_description_and_constraints.md#general-address-alignment-constraints).

## Example

```cpp
// total_length indicates the total length of data involved in the transfer.
constexpr uint64_t total_length = 128;
// The following three parameters correspond to the addresses of matrices c, a, and b, respectively.
__cc__ float c_matrix[total_length];
__ca__ fp8_e4m3fn_t a_matrix[total_length];
__cb__ fp8_e4m3fn_t b_matrix[total_length];

uint16_t left_height = 16;
uint16_t n_dim = 16;
uint16_t right_width =16;
uint8_t unit_flag = 0;
bool disable_gemv = false;
bool c_matrix_source = false;
bool c_matrix_init_val = true;
// Function call.
asc_mmad_mx_sync(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
```
