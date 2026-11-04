# DeNormMin

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T19:12:42.852Z pushedAt=2026-10-26T07:41:44.840Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/DeNormMin_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the minimum positive denormalized value of the specified data type.

## Prototype

- Scalar API, with a scalar return value:

    ```cpp
    constexpr __aicore__ static inline T DeNormMin()
    ```

- Vector API, which assigns the minimum positive denormalized value to the first **count** elements of **dstLocal**:

    ```cpp
    __aicore__ static inline void DeNormMin(const LocalTensor<T>& dstLocal, uint32_t count)
    ```

## Parameter Description

**Table 1** Parameter description

| Parameter Name | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Type

<!-- npu="950" id8 -->
For the Ascend 950PR&950DT products, the supported data types are half, bfloat16_t, and float.
<!-- end id8 -->

## Return Value

The scalar API returns the minimum positive denormalized value of the corresponding data type.

The vector API has no return value.

**Table 2**  DeNormMin output for each data type

| Data Type | Decimal Number | Binary Bit Positions |
| --- | --- | --- |
| half | 5.96e-8 | 0 00000 0000000001 |
| bfloat16_t | 9.2e-41 | 0 00000000 0000001 |
| float | 1.4e-45 | 0 00000000 00000000000000000000001 |

## Constraints

The scalar API has no constraints. The vector API uses [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first count elements with the minimum positive denormalized value. Refer to the constraints of the duplicate data padding API under Memory vector computation.

## Example

- Scalar API

    ```cpp
    float value = AscendC::NumericLimits<float>::DeNormMin();
    ```

- Vector API

    ```cpp
    AscendC::NumericLimits<float>::DeNormMin(dstLocal, 256); // Returns 256 minimum positive denormalized values of the float type.
    ```
