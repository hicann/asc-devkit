# Summary of Boundary Values for Matrix Compute Move-Out APIs

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-29T10:37:27.860Z pushedAt=2026-10-29T10:51:49.313Z nl=2 -->

This section summarizes the output results of the L0C Buffer->GM/L0C Buffer->L1 Buffer/L0C Buffer->UB move APIs when boundary values such as inf/-inf/nan exist in the L0C Buffer. The APIs involved are as follows:

**Table 1** Description of matrix compute move-out APIs

| Data Movement | Basic API | C API |
|---|---|---|
| L0C Buffer->GM | [Fixpipe (L0C to GM)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) | [asc_copy_l0c2gm](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md) |
| L0C Buffer->L1 Buffer | [Fixpipe (L0C to L1)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToL1.md) | [asc_copy_l0c2l1](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md) |
| L0C Buffer->UB | [Fixpipe (L0C to UB)](../SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md) | [asc_copy_l0c2ub](../SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md) |

The following content applies to the following models:

<!-- npu="950" id1 -->
- Ascend 950PR & 950DT products
<!-- end id1 -->

## Overview

The three move instructions move the matrix computation results in the L0C Buffer to GM, L1 Buffer, and UB respectively. During the move process, real-time quantization is supported to convert the float or int32_t data in the L0C Buffer to the target output data type. The inf/nan handling behavior of real-time quantization is completely consistent across the three move instructions. The saturation mode is controlled by setting the CTRL[48] bit through the basic API [SetCtrlSpr](../SIMD-API/basic_api/special_register_access/SetCtrlSpr_ISASI.md) or the C API [asc_set_ctrl](../SIMD-API/c_api/spr/asc_set_ctrl.md):

- **Saturation mode (CTRL[48]=1'b0)**: The inf output is saturated to ±MAX, and the nan output is saturated to 0.
- **Non-saturation mode (CTRL[48]=1'b1)**: inf/nan are output as-is.

> [!NOTE] Note
>
>- CTRL[48] supports precision conversion only for the following data types: half, bfloat16_t, fp8_e4m3fn_t, and hifloat8_t. For the float and int32_t data types, CTRL[48] does not take effect, and the results are the same in saturation mode and non-saturation mode. For float, inf/nan are output as-is; int32_t does not contain inf/nan.
>
>- Integer types (int8_t, uint8_t, int4b_t) support saturation only, and are always processed in saturation mode regardless of the CTRL[48] setting.
>
>- For the fp8_e4m3fn_t type, the CTRL[50] bit further controls the nan output in saturation mode: when CTRL[50]=1'b0 (default), nan is converted to 0; when CTRL[50]=1'b1, nan remains unchanged.

### Data Movement Path Differences

The three move instructions behave identically in real-time quantization inf/nan handling; the only difference lies in the verification path:

- **L0C Buffer->GM**: Moves L0C Buffer data directly to GM without an intermediate path. In non-saturation mode, inf/nan are fully transferred to the GM output with no verification path restrictions.
- **L0C Buffer->L1 Buffer**: Moves L0C Buffer data to L1 Buffer, then moves the data from L1 Buffer to UB via a move instruction, and finally moves it from UB to GM. Because L1 Buffer cannot be moved directly to GM, an intermediate path of L1->UB->GM is used here. This path involves no secondary matrix computation.
- **L0C Buffer->UB**: Moves L0C Buffer data to UB, then moves it from UB to GM. The AIC handles the move from L0C Buffer to UB, and the AIV handles the move from UB to GM, coordinated through cross-core synchronization. This path involves no secondary matrix computation, and in non-saturation mode, inf/nan are fully transferred to the GM output.

## Moving float Data from L0C Buffer to Each Output Type

The float data stored in L0C Buffer comes from the result of matrix multiplication. When the input matrix A contains inf/-inf/nan, the corresponding rows in L0C Buffer contain inf/-inf/nan.

**Table 2**  Description of the computation results for each output type when inf/-inf/nan exist in float data in L0C Buffer

| Output Data Type | Real-time Quantization Mode | Element Value in L0C Buffer | Output Element Value in Saturation Mode | Output Element Value in Non-saturation Mode |
| --- | --- | --- |------------------|-------------|
| half | QF322F16_PRE / VQF322F16_PRE | +inf | 65504.0 (MAX) | +inf |
| half | QF322F16_PRE / VQF322F16_PRE | -inf | -65504.0 (MIN) | -inf |
| half | QF322F16_PRE / VQF322F16_PRE | nan | 0.0 | nan |
| half | F322F16 (Cast) | +inf | 65504.0 (MAX) | +inf |
| half | F322F16 (Cast) | -inf | -65504.0 (MIN) | -inf |
| half | F322F16 (Cast) | nan | 0.0 | nan |
| bfloat16_t | QF322BF16_PRE / VQF322BF16_PRE | +inf | 0x7F7F (MAX) | 0x7F80 (+inf) |
| bfloat16_t | QF322BF16_PRE / VQF322BF16_PRE | -inf | 0xFF7F (MIN) | 0xFF80 (-inf) |
| bfloat16_t | QF322BF16_PRE / VQF322BF16_PRE | nan | 0x0000 (0) | 0x7FFF (nan) |
| bfloat16_t | F322BF16 (Cast) | +inf | 0x7F7F (MAX) | 0x7F80 (+inf) |
| bfloat16_t | F322BF16 (Cast) | -inf | 0xFF7F (MIN) | 0xFF80 (-inf) |
| bfloat16_t | F322BF16 (Cast) | nan | 0x0000 (0) | 0x7FFF (nan) |
| float | QF322F32_PRE / VQF322F32_PRE | +inf | +inf | +inf |
| float | QF322F32_PRE / VQF322F32_PRE | -inf | -inf | -inf |
| float | QF322F32_PRE / VQF322F32_PRE | nan | nan | nan |
| fp8_e4m3fn_t | QF322FP8_PRE / VQF322FP8_PRE | +inf | 0x7E (448.0, MAX) | 0x7F (nan) |
| fp8_e4m3fn_t | QF322FP8_PRE / VQF322FP8_PRE | -inf | 0xFE (-448.0, MIN) | 0x7F (nan) |
| fp8_e4m3fn_t | QF322FP8_PRE / VQF322FP8_PRE | nan | 0x00 (0) | 0x7F (nan) |
| hifloat8_t | QF322HIF8_PRE / VQF322HIF8_PRE | +inf | 111 (HiF8+inf) | 111 (HiF8+inf) |
| hifloat8_t | QF322HIF8_PRE / VQF322HIF8_PRE | -inf | 239 (HiF8-inf) | 239 (HiF8-inf) |
| hifloat8_t | QF322HIF8_PRE / VQF322HIF8_PRE | nan | 0 | 128 (HiF8NAN) |
| hifloat8_t | QF322HIF8_PRE_HYBRID / VQF322HIF8_PRE_HYBRID | +inf | 111 (HiF8+inf) | 111 (HiF8+inf) |
| hifloat8_t | QF322HIF8_PRE_HYBRID / VQF322HIF8_PRE_HYBRID | -inf | 239 (HiF8-inf) | 239 (HiF8-inf) |
| hifloat8_t | QF322HIF8_PRE_HYBRID / VQF322HIF8_PRE_HYBRID | nan | 0 | 128 (HiF8NAN) |
| int8_t | QF322B8_PRE / VQF322B8_PRE | +inf | 127 (MAX) | 127 (MAX) |
| int8_t | QF322B8_PRE / VQF322B8_PRE | -inf | -128 (MIN) | -128 (MIN) |
| int8_t | QF322B8_PRE / VQF322B8_PRE | nan | 0 | 0 |
| uint8_t | QF322B8_PRE / VQF322B8_PRE | +inf | 255 (MAX) | 255 (MAX) |
| uint8_t | QF322B8_PRE / VQF322B8_PRE | -inf | 0 (MIN) | 0 (MIN) |
| uint8_t | QF322B8_PRE / VQF322B8_PRE | nan | 0 | 0 |
| int4b_t | QF322S4_PRE / VQF322S4_PRE | +inf | 7 (MAX) | 7 (MAX) |
| int4b_t | QF322S4_PRE / VQF322S4_PRE | -inf | -8 (MIN) | -8 (MIN) |
| int4b_t | QF322S4_PRE / VQF322S4_PRE | nan | 0 | 0 |

Note:

- For float data, inf/nan are output as-is in both saturation mode and non-saturation mode, because CTRL[48] does not support the float data type.
- The fp8_e4m3fn_t type has no encoding format for inf. In non-saturation mode, if the absolute value of the result is inf or greater than the maximum normalized value of fp8_e4m3fn_t (448), the output is nan (0x7F).
- For the hifloat8_t type, 111 (0b01101111) is the HiF8+inf encoding, 239 (0b11101111) is the HiF8-inf encoding, and 128 (0b10000000) is the HiF8NAN encoding. In saturation mode, inf remains as the corresponding inf encoding, and nan is saturated to 0.
- The overflow threshold of hifloat8_t is 1.25×2^15=40960. When the absolute value of the quantized value is greater than or equal to 40960, the output uses the inf encoding (111/239) instead of saturating to the maximum value 240.
- Integer types support saturation mode only, and the output results in saturation mode and non-saturation mode are identical.
- VQF322HIF8_PRE uses the Half to Away rounding mode, while VQF322HIF8_PRE_HYBRID uses the Hybrid rounding mode (Half to Away when |exp|<4, and SSR (Stochastic Rounding) when |exp|≥4). The two modes handle inf/nan identically.
- For the same output type, the scalar mode (such as QF322F16_PRE) and the tensor mode (such as VQF322F16_PRE) use the same type conversion algorithm, and their handling of inf/nan boundary values is completely identical. The only difference lies in how quantization parameters are passed (a single scalar vs. a per-column tensor). Therefore, the table lists the scalar mode and the tensor mode together.

## Moving int32_t Data from L0C Buffer to Each Output Type

The int32_t data stored in L0C Buffer comes from the result of matrix multiplication, which does not produce inf/nan, but overflow may occur after quantization.

**Table 3**  Calculation results when quantization overflow occurs for int32_t data in L0C Buffer

| Output Data Type | Real-time Quantization Mode | Element Value in L0C Buffer | Output Element Value |
| --- | --- | --- | --- |
| half | DEQF16 / VDEQF16 | Exceeds the half representable range after quantization | 65504.0 (MAX) / -65504.0 (MIN) |
| bfloat16_t | QS322BF16_PRE / VQS322BF16_PRE | Exceeds the bfloat16 representable range after quantization | 0x7F7F (MAX) / 0xFF7F (MIN) |
| int8_t | REQ8 / VREQ8 | Greater than 127 after quantization | 127 (MAX) |
| int8_t | REQ8 / VREQ8 | Less than -128 after quantization | -128 (MIN) |
| uint8_t | REQ8 / VREQ8 | Greater than 255 after quantization | 255 (MAX) |
| uint8_t | REQ8 / VREQ8 | Less than 0 after quantization | 0 (MIN) |
| int4b_t | REQ4 / VREQ4 | Greater than 7 after quantization | 7 (MAX) |
| int4b_t | REQ4 / VREQ4 | Less than -8 after quantization | -8 (MIN) |

Note: Integer types support saturation mode only, and overflow is saturated to the maximum or minimum value of the target type. For half and bfloat16_t, overflow values are saturated in both saturation mode and non-saturation mode.

## NoQuant (No Quantization) Scenario

In NoQuant mode, no type conversion or quantization is performed, and the float or int32_t data in L0C Buffer is moved to the target buffer as is. CTRL[48] does not support these two data types, so the results are identical in saturation mode and non-saturation mode. float data may contain inf/-inf/nan (from matrix multiplication inputs containing special values), and are output as is. int32_t is the integer result of matrix multiplication and does not contain inf/nan. The NoQuant scenario also covers output formats such as NZ2NZ, NZ2ND, dual target split by M dimension, and dual target split by N dimension, verifying that inf/-inf/nan in the float scenario are correctly transferred across various format conversions and dual-target splits.

**Table 4** Output results when inf/-inf/nan exist in float-type L0C Buffer in NoQuant mode

| Scenario | Output Format | Dual Target | Element Value in L0C Buffer | Output Element Value | Description |
| --- | --- | --- | --- | --- | --- |
| 1 | Nz | No | +inf/-inf/nan | +inf/-inf/nan | Preserved as is |
| 2 | ND | No | +inf/-inf/nan | +inf/-inf/nan | Preserved as is |
| 3 | ND | Yes (split by M) | +inf/-inf/nan | +inf/-inf/nan | Correctly transferred to both targets |
| 4 | ND | Yes (split by N) | +inf/-inf/nan | +inf/-inf/nan | Correctly transferred to both targets |

Note: Table 4 applies only to the float type. int32_t does not contain inf/nan and therefore requires no separate verification. In scenario 3, when splitting by the M dimension, M must be a multiple of 2; in scenario 4, when splitting by the N dimension, N must be a multiple of 32.
