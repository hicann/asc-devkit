# asc_set_ctrl

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T13:22:20.103Z pushedAt=2026-09-08T03:45:05.435Z -->

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

Sets specific bits of the CTRL register (control register).

## Prototype

```cpp
__aicore__ inline void asc_set_ctrl(uint64_t config)
```

## Parameters

Table 1 Parameter description

|Parameter|Input/Output|Description|
| :------ | :---  | :------------ |
|config   |Input   |Register value to be set. Refer to Table 2 for common CTRL register bit descriptions.|

Table 2 Description of common CTRL register bits

|CTRL Bit    |Function|
| :-------     | :---- |
|CTRL[48]      |Controls the saturation mode of integer computation instructions. <br>- 1'b0: Saturation mode. INF output is saturated to ±MAX, and NAN output is saturated to 0. <br> - 1'b1: Non-saturation mode. INF/NAN retains the original output. <br> This control bit supports only the following data types: <br>- Floating-point computation supports the half data type. <br>- Floating-point precision conversion supports the following data types: hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t.|
|CTRL[49]      |Controls whether the scalar unit reads data through datacache. datacache reads faster than ub. Therefore, when a large amount of data is read from ub and the data in ub is not written by the vector unit, this mode can be enabled. <br>- 1'b0: The scalar unit reads data directly from ub. <br> - 1'b1: The scalar unit reads data from datacache.|
|CTRL[53]      |Controls the saturation mode of integer computation instructions. <br>- 1'b0: Truncation mode. The overflow value is truncated to the bit width of the target data type, retaining the low bits and discarding the high bits. <br>- 1'b1: Saturation mode. The overflow value is saturated to ±MAX.|
|CTRL[56]      |MASK mode control bit. MASK assists SIMD instruction execution. CTRL[56] indicates two modes: <br>- 1'b0: Mask register in vector operations. Each bit corresponds to one element in the vector, indicating whether the element participates in the computation. '1' indicates that the corresponding element is computed; '0' indicates that the corresponding element is not computed. <br>- 1'b1: MASK[31:0] indicates the number of elements actually operated by the SIMD instruction. The actual number of repetitions is automatically inferred by VECTOR, and the repeat time parameter (Xt[63:56]) is ignored.|
|CTRL[59]      |Controls the precision conversion saturation mode for float-to-integer or integer-to-integer conversion. It takes effect only when CTRL[60] is enabled. <br>- 1'b0: Saturation mode. The overflow value is saturated to ±MAX. <br>- 1'b1: Truncation mode. The overflow value is truncated to the bit width of the target data type, retaining the low bits and discarding the high bits.|

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Before use, obtain the current value with [asc_get_ctrl()](asc_get_ctrl.md) and modify only the bits that need to be changed.
- Only the CTRL[48], CTRL[53], CTRL[56], and CTRL[59] bits are supported for setting.

## Example

```cpp
// Set CTRL[48] to non-saturation mode.
uint64_t ori_config = asc_get_ctrl();
uint64_t mask = (static_cast<uint64_t>(1) << 48);
uint64_t config = ori_config | mask;
asc_set_ctrl(config);
```
