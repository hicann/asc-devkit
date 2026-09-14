# asc_set_va_reg

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-09-05T15:19:02.974Z pushedAt=2026-09-08T03:45:05.582Z -->

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

Sets the 16 DataBlock addresses for the transpose operation and associates the operand address sequences with address registers. The API requires the first 8 and the last 8 address sequences to be associated with address registers separately.

## Prototype

```cpp
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int8_t** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint8_t** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int16_t** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint16_t** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ half** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int32_t** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint32_t** src_array)
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ float** src_array)
```

## Parameters

|Parameter|Input/Output|Description|
| ------------ | ------------ | ------------ |
|addr|Input|Address register, of type `ub_addr8_t`. Possible values are:<br>&bull; VA0<br>&bull; VA1<br>&bull; VA2<br>&bull; VA3<br>&bull; VA4<br>&bull; VA5<br>&bull; VA6<br>&bull; VA7<br>The number indicates the register order. Each address register can be associated with only 8 addresses. For usage, see the example.|
|src_array|Input|Operand address sequence.|

## Return Value

None

## Pipeline Type

PIPE_V

## Constraints

- For operand address alignment constraints, see [General Address Alignment Constraints](../general_description_and_constraints.md#general-address-alignment-constraints).

## Example

```cpp
constexpr uint64_t total_length = 256;    // total_length indicates the data length involved in the computation.
__ubuf__ half src[total_length];
__ubuf__ half dst[total_length];
const uint32_t STEP = 16;
// src_list and dst_list are arrays of 16 DataBlock addresses.
__ubuf__ half* src_list[16] = {
    (__ubuf__ half*)(src + 0 * STEP),
    (__ubuf__ half*)(src + 1 * STEP),
    (__ubuf__ half*)(src + 2 * STEP),
    (__ubuf__ half*)(src + 3 * STEP),
    (__ubuf__ half*)(src + 4 * STEP),
    (__ubuf__ half*)(src + 5 * STEP),
    (__ubuf__ half*)(src + 6 * STEP),
    (__ubuf__ half*)(src + 7 * STEP),
    (__ubuf__ half*)(src + 8 * STEP),
    (__ubuf__ half*)(src + 9 * STEP),
    (__ubuf__ half*)(src + 10 * STEP),
    (__ubuf__ half*)(src + 11 * STEP),
    (__ubuf__ half*)(src + 12 * STEP),
    (__ubuf__ half*)(src + 13 * STEP),
    (__ubuf__ half*)(src + 14 * STEP),
    (__ubuf__ half*)(src + 15 * STEP)
};
__ubuf__ half* dst_list[16] = {
    (__ubuf__ half*)(dst + 0 * STEP),
    (__ubuf__ half*)(dst + 1 * STEP),
    (__ubuf__ half*)(dst + 2 * STEP),
    (__ubuf__ half*)(dst + 3 * STEP),
    (__ubuf__ half*)(dst + 4 * STEP),
    (__ubuf__ half*)(dst + 5 * STEP),
    (__ubuf__ half*)(dst + 6 * STEP),
    (__ubuf__ half*)(dst + 7 * STEP),
    (__ubuf__ half*)(dst + 8 * STEP),
    (__ubuf__ half*)(dst + 9 * STEP),
    (__ubuf__ half*)(dst + 10 * STEP),
    (__ubuf__ half*)(dst + 11 * STEP),
    (__ubuf__ half*)(dst + 12 * STEP),
    (__ubuf__ half*)(dst + 13 * STEP),
    (__ubuf__ half*)(dst + 14 * STEP),
    (__ubuf__ half*)(dst + 15 * STEP)
};
const int32_t VA_REG_ARRAY_LEN = 8;

// The API requires the first 8 and the last 8 address sequences to be associated with address registers separately.
asc_set_va_reg(VA0, dst_list);
asc_set_va_reg(VA1, dst_list + VA_REG_ARRAY_LEN);
asc_set_va_reg(VA2, src_list);
asc_set_va_reg(VA3, src_list + VA_REG_ARRAY_LEN);
```