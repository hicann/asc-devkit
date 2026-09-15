# Min

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:17:21.817Z -->

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

Returns the minimum finite value of the specified data type.

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

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| count | Input | Number of input data elements. |

## Data Types

<!-- npu="950" id1 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id1 -->

## Return Value

The scalar API returns the minimum finite value of the corresponding data type.

The vector API has no return value.

## Constraints

None

## Example

- Scalar API:

    ```cpp
    int32_t value = AscendC::NumericLimits<int32_t>::Min();
    ```

- Vector API:

    ```cpp
    AscendC::NumericLimits<int32_t>::Min(dstLocal, 256); // Returns the minimum finite value of 256 int32_t elements.
    ```
