# QuietNaN

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:20:06.432Z -->

## Applicable Products

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Not supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: Not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: Not supported
<!-- end id15 -->

## Description

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

Returns the quiet NaN value of the specified data type (with the most significant bit of the floating-point mantissa set to 1).

## Prototype

- Scalar API, which returns a scalar value:

    ```cpp
    constexpr __aicore__ static inline T QuietNaN()
    ```

- Vector API, which assigns a quiet NaN value to the first **count** elements of **dstLocal**:

    ```cpp
    __aicore__ static inline void QuietNaN(const LocalTensor<T>& dstLocal, uint32_t count)
    ```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **dstLocal** | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported **TPosition** values are **VECIN**/**VECCALC**/**VECOUT**.<br><br>The start address of the **LocalTensor** must be 32-byte aligned. |
| **count** | Input | Number of input data elements. |

## Data Types

<!-- npu="950" id1 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: half, bfloat16_t, float.
<!-- end id1 -->

## Return Value

The scalar API returns a quiet NaN value of the corresponding data type.

The vector API has no return value.

## Constraints

None

## Example

- Scalar API:

    ```cpp
    float value = AscendC::NumericLimits<float>::QuietNaN();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<float>::QuietNaN(dstLocal, 256); // Returns 256 NaN values of the float type.
    ```
