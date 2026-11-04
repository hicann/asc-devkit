# Max

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:15:30.643Z pushedAt=2026-10-26T07:41:44.847Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/Max_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the maximum finite value of the specified data type.

## Prototype

- Scalar API, returning a scalar value:

    ```cpp
    constexpr __aicore__ static inline T Max()
    ```

- Vector API, assigning the maximum finite value to the first count elements of dst:

    ```cpp
    __aicore__ static inline void Max(const LocalTensor<T>& dst, uint32_t count)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Type

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id8 -->

## Return Value

The scalar API returns the maximum finite value of the corresponding data type.

The vector API has no return value.

**Table 2** Max output for each data type

| Data Type | Decimal Number | Binary Bit Positions |
| --- | --- | --- |
| uint8_t | 255 | 1111 1111 |
| int8_t | 127 | 0111 1111 |
| uint16_t | 65535 | 1111 1111 1111 1111 |
| int16_t | 32767 | 0111 1111 1111 1111 |
| half | 65504 | 0 11110 1111111111 |
| bfloat16_t | 3.4e+38 | 0 11111110 1111111 |
| uint32_t | 4294967295 | 1111 1111 1111 1111 1111 1111 1111 1111 |
| int32_t | 2147483647 | 0111 1111 1111 1111 1111 1111 1111 1111 |
| float | 3.4e+38 | 0 11111110 11111111111111111111111 |

## Constraints

The scalar API has no constraints. For the vector API, use [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first count elements with the maximum finite value. For details, see the constraints of the duplicate data padding API under Memory vector computation.

## Example

- Scalar API:

    ```cpp
    int32_t value = AscendC::NumericLimits<int32_t>::Max();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<int32_t>::Max(dstLocal, 256); // Returns the maximum value of 256 int32_t elements.
    ```
