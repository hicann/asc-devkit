# QuietNaN

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T05:35:54.578Z pushedAt=2026-11-18T01:25:31.201Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/NumericLimits_utils/QuietNaN_res.md#id1 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the [quiet NaN value](../../data_structures/builtin_data_types.md) of the specified data type (the most significant bit of the floating-point mantissa is 1).

## Function Prototype

- Scalar API, which returns a scalar value:

    ```cpp
    constexpr __aicore__ static inline T QuietNaN()
    ```

- Vector API, which assigns the quiet NaN value to the first count elements of dstLocal:

    ```cpp
    __aicore__ static inline void QuietNaN(const LocalTensor<T>& dstLocal, uint32_t count)
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

The scalar API returns a quiet NaN value of the corresponding data type.

The vector API has no return value.

**Table 2** QuietNaN output for each data type

| Data Type | Decimal Number | Binary Bit Positions |
| --- | --- | --- |
| half | nan | 0 11111 1000000000 |
| bfloat16_t | nan | 0 11111111 1000000 |
| float | nan | 0 11111111 10000000000000000000000 |

## Constraints

The scalar API has no constraints. The vector API uses [duplicate](../../memory_vector_compute/data_padding/Duplicate.md) to fill the first count elements with quiet NaN values. For details, see the constraints of the duplicate data padding API under Memory vector computation.

## Example

- Scalar API:

    ```cpp
    float value = AscendC::NumericLimits<float>::QuietNaN();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<float>::QuietNaN(dstLocal, 256); // Return 256 NaN values of the float type.
    ```
