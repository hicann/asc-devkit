# Min

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:15:46.658Z pushedAt=2026-10-26T07:41:44.848Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/Min_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the minimum finite value of the specified data type. For floating-point types, it returns the minimum normalized positive number.

## Prototype

- Scalar API, which returns a scalar value:

    ```cpp
    constexpr __aicore__ static inline T Min()
    ```

- Vector API, which assigns the minimum finite value to the first **count** elements of **dst**:

    ```cpp
    __aicore__ static inline void Min(const LocalTensor<T>& dst, uint32_t count)
    ```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Types

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id8 -->

## Return Value

The scalar API returns the minimum finite value of the corresponding data type.

The vector API has no return value.

**Table 2** Output of each data type of Min

| Data Type | Decimal Number | Binary Bit Positions |
| --- | --- | --- |
| uint8_t | 0 | 0000 0000 |
| int8_t | -128 | 1000 0000 |
| uint16_t | 0 | 0000 0000 0000 0000 |
| int16_t | -32768 | 1000 0000 0000 0000 |
| half | 6.1e-5 | 0 00001 0000000000 |
| bfloat16_t | 1.18e-38 | 0 00000001 0000000 |
| uint32_t | 0 | 0000 0000 0000 0000 0000 0000 0000 0000 |
| int32_t | -2147483648 | 1000 0000 0000 0000 0000 0000 0000 0000 |
| float | 1.18e-38 | 0 00000001 00000000000000000000000 |

## Constraints

The scalar API has no constraints. The vector API uses [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first **count** elements with the minimum finite value. For details, see the constraints of the data padding API **duplicate** under Memory vector computation.

## Example

- Scalar API:

    ```cpp
    int32_t value = AscendC::NumericLimits<int32_t>::Min();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<int32_t>::Min(dstLocal, 256); // Return the minimum finite value of 256 int32_t elements.
    ```
