# NegativeInfinity

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T05:34:19.927Z pushedAt=2026-11-18T01:25:31.194Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/NegativeInfinity_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the negative infinity value of the specified data type.

## Function Prototype

- Scalar API, which returns a scalar value:

    ```cpp
    constexpr __aicore__ static inline T NegativeInfinity()
    ```

- Vector API, which assigns negative infinity to the first **count** elements of **dstLocal**:

    ```cpp
    __aicore__ static inline void NegativeInfinity(const LocalTensor<T>& dstLocal, uint32_t count)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Type

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are: half, bfloat16_t, float.
<!-- end id8 -->

## Return Value

The scalar API returns the [negative infinity value](../../data_structures/builtin_data_types.md) of the corresponding data type.

The vector API has no return value.

**Table 2**  NegativeInfinity output for each data type

| Data Type | Decimal Number | Binary Bit Positions |
| --- | --- | --- |
| half | -inf | 1 11111 0000000000 |
| bfloat16_t | -inf | 1 11111111 0000000 |
| float | -inf | 1 11111111 00000000000000000000000 |

## Constraints

The scalar API has no constraints. The vector API uses [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first count elements with negative infinity, following the constraints of the duplicate data padding API under Memory vector computation.

## Example

- Scalar API:

    ```cpp
    float value = AscendC::NumericLimits<float>::NegativeInfinity();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<float>::NegativeInfinity(dstLocal, 256);// Returns 256 float values of negative infinity.
    ```
