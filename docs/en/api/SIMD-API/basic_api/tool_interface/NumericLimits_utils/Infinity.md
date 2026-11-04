# Infinity

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:13:26.427Z pushedAt=2026-10-26T07:41:44.842Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/Infinity_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the positive infinity value of the specified data type.

## Prototype

- Scalar API, which returns a scalar value:

    ```cpp
    constexpr __aicore__ static inline T Infinity()
    ```

- Vector API, which assigns a positive infinity value to the first **count** elements of **dstLocal**:

    ```cpp
    __aicore__ static inline void Infinity(const LocalTensor<T>& dstLocal, uint32_t count)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Type

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are: half, bfloat16_t, float.
<!-- end id8 -->

## Return Value

The scalar API returns the [positive infinity value](../../data_structures/builtin_data_types.md) of the corresponding data type.

The vector API has no return value.

**Table 2** Infinity output for each data type

| Data Type | Decimal Number | Binary Bits |
| --- | --- | --- |
| half | inf | 0 11111 0000000000 |
| bfloat16_t | inf | 0 11111111 0000000 |
| float | inf | 0 11111111 00000000000000000000000 |

## Constraints

The scalar API has no constraints. The vector API uses [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first count elements with positive infinity values. For details, see the constraints of the duplicate data padding API under Memory vector computation.

## Example

- Scalar API:

    ```cpp
    float value = AscendC::NumericLimits<float>::Infinity();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<float>::Infinity(dstLocal, 256); // Return 256 positive infinity values of the float type.
    ```
