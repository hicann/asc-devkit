# Lowest

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:13:51.013Z pushedAt=2026-10-26T07:41:44.845Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/Lowest_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the lowest finite value of the specified data type, that is, the finite value x for which no other finite value y satisfies y < x.

## Function Prototype

- Scalar API, with a scalar return value:

    ```cpp
    constexpr __aicore__ static inline T Lowest()
    ```

- Vector API, assigns the lowest finite value to the first count elements of dstLocal:

    ```cpp
    __aicore__ static inline void Lowest(const LocalTensor<T>& dstLocal, uint32_t count)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Type

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id8 -->

## Return Value

The scalar API returns the lowest finite value of the corresponding data type.

The vector API has no return value.

**Table 2** Lowest output for each data type

| Data Type | Decimal Value | Binary Bit Positions |
| --- | --- | --- |
| uint8_t | 0 | 0000 0000 |
| int8_t | -128 | 1000 0000 |
| uint16_t | 0 | 0000 0000 0000 0000 |
| int16_t | -32768 | 1000 0000 0000 0000 |
| half | -65504 | 1 11110 1111111111 |
| bfloat16_t | -3.4e+38 | 1 11111110 1111111 |
| uint32_t | 0 | 0000 0000 0000 0000 0000 0000 0000 0000 |
| int32_t | -2147483648 | 1000 0000 0000 0000 0000 0000 0000 0000 |
| float | -3.4e+38 | 1 11111110 11111111111111111111111 |

## Constraints

The scalar API has no constraints. The vector API uses [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first **count** elements with the lowest finite value. For details, see the constraints of the data padding API **duplicate** under Memory vector computation.

## Example

- Scalar API:

    ```cpp
    int32_t value = AscendC::NumericLimits<int32_t>::Lowest();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<int32_t>::Lowest(dstLocal, 256);// Return the lowest finite value of 256 int32_t elements.
    ```
