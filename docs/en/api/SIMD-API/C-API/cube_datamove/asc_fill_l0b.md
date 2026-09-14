# asc_fill_l0b

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:49:26.268Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
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

Assigns the binary value of **value** to **dst** to initialize the Local Memory of the L0B Buffer.

## Prototype

- Conventional computation

    ```cpp
    __aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ half* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ float* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config)
    ```

- Synchronous computation

    ```cpp
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, half value, const asc_fill_value_config& config)
    __aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config)
    ```

## Parameters

| Parameter | Input/Output | Description |
| :----- | :------- | :------- |
| dst | Output | Start address of the destination operand (matrix). |
| value | Input | Source operand (scalar). |
| config | Input | Initialization-related parameters used. For details, see [asc_fill_value_config](../struct/asc_fill_value_config.md). |

## Return Value

None

## Pipeline Type

PIPE_MTE1

## Constraints

- The start address of **dst** must be 512-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../general_description_and_constraints.md#general-address-overlap-constraints).
- If the data types of **dst** and **value** are different, convert the value of **value** according to the data type of **dst**. For details, see the example.

## Example

```cpp
constexpr uint64_t total_length = 256;    // total_length indicates the total length of data involved in the transfer.
asc_fill_value_config config;

// dst and value have different data types.
__cb__ float dst0[total_length];
uint32_t value0 = 0x3F800000;  // The binary representation of the float value 1 is 0x3F800000.
config.repeat = 1;
config.blk_num = total_length * sizeof(float) / 512;
config.dst_gap = 0;
asc_fill_l0b(dst0, value0, config);    // Initialize the 256 float elements in dst0 to 1.

// dst and value have the same data type.
__cb__ half dst1[total_length];
half value1 = 1;  // No binary conversion is needed; directly fill with 1.
config.repeat = 1;
config.blk_num = total_length * sizeof(half) / 512;
config.dst_gap = 0;
asc_fill_l0b(dst1, value1, config);    // Initialize the 256 half elements in dst1 to 1.
```
