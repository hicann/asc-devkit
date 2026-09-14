# asc_enable_fp8

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:28:30.191Z -->

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

Used to enable FP8 mode for Mmad computation. This API disables HiF8 mode (the opposite of [asc_enable_hif8](./asc_enable_hif8.md)). After this mode is enabled, FP8 data in the L0A Buffer/L0B Buffer is not converted to the hifloat8_t data type before participating in Mmad computation.

## Prototype

 ```cpp
__aicore__ inline void asc_enable_fp8()
```

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- This is the opposite of [asc_enable_hif8](./asc_enable_hif8.md), and the two do not take effect at the same time.
- It must be called before [asc_mmad](./asc_mmad.md) is executed.

## Example

```cpp
constexpr uint64_t total_length = 128;
__cc__ float c_matrix[total_length];
__ca__ fp8_e4m3fn_t a_matrix[total_length];
__cb__ fp8_e4m3fn_t b_matrix[total_length];

uint16_t left_height = 16;  // M
uint16_t n_dim = 16;        // K
uint16_t right_width = 16;  // N

// Call before asc_mmad to participate in matrix operations in FP8 format (non-HiF8 conversion path).
asc_enable_fp8();

uint8_t unit_flag = 0;
bool disable_gemv = false;
bool c_matrix_source = false;
bool c_matrix_init_val = true;
asc_mmad_sync(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
              unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
```
