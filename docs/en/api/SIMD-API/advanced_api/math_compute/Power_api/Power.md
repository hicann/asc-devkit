# Power

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T09:14:30.015Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: supported
- Atlas A3 training series products/Atlas A3 inference series products: supported
- Atlas A2 training series products/Atlas A2 inference series products: supported
- Atlas 200I/500 A2 inference products: not supported
- Atlas inference series products AI Core: supported
- Atlas inference series products Vector Core: not supported
- Atlas training series products: not supported

## Description

Implements element-wise exponentiation and provides three types of APIs. The processing logic is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218328733.png)

![](../../../../figures/zh-cn_formulaimage_0000002192030526.png)

## Prototype

-   Power\(dstTensor, src0Tensor, src1Tensor\)
    -   Temporary space is passed through the **sharedTmpBuffer** input parameter.
        -   The source operand **Tensor** participates in the computation fully or partially.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The source operand **Tensor** participates in the computation fully.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework allocates temporary space.
        -   The source operand **Tensor** participates in the computation fully or partially.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, uint32_t calCount)
            ```

        -   The source operand **Tensor** participates in the computation fully.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
            ```

-   Power\(dstTensor, src0Tensor, src1Scalar\)
    -   Temporary space is passed through the **sharedTmpBuffer** input parameter.
        -   The source operand tensor fully or partially participates in computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The source operand Tensor fully participates in computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework allocates temporary space.
        -   The source operand tensor fully or partially participates in computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, uint32_t calCount)
            ```

        -   The source operand tensor fully participates in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar)
            ```

-   Power\(dstTensor, **src0Scalar**, src1Tensor\)
    -   Temporary space is passed through the **sharedTmpBuffer** input parameter.
        -   The source operand tensor fully or partially participates in computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The source operand tensor fully participates in computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework allocates temporary space.
        -   The source operand tensor fully or partially participates in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, uint32_t calCount)
            ```

        -   The source operand fully participates in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor)
            ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be allocated in two ways: **allocated by the API framework** and **passed through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. In this way, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.

When the API framework allocates the temporary space, you need to reserve the temporary space. When the temporary space is passed through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size **BufferSize** is obtained as follows: use the **GetPowerMaxMinTmpSize** API provided in [GetPowerMaxMinTmpSize](GetPowerMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different models, see [Supported data types](#li559613463410). |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Configuration related to the **Power** computation. This parameter is optional and of the **PowerConfig** type. Its definition is shown in the following code, where the parameters have the following meanings:<br>**algo**: Different Power algorithms supported for different data types. The supported values of this parameter are as follows: **INTRINSIC**: Default value. If the data type is an integer type, the INTRINSIC algorithm uses the fast exponentiation algorithm to implement the Power computation, and the supported data types are uint8_t, int8_t, uint16_t, int16_t, uint32_t, and int32_t. If the data type is a floating-point type, the INTRINSIC algorithm performs the Power computation according to the formula Power(x, y) = exp(y * ln(x)), and the supported data types are half and float. **DOUBLE_FLOAT_TECH**: The DOUBLE_FLOAT_TECH algorithm is a high-precision floating-point algorithm. It promotes the precision of the source operand and then performs the Power computation according to the formula Power(x, y) = exp(y * ln(x)), reducing precision loss during the computation. The supported data types are bfloat16_t, half, and float. |

```
enum class PowerAlgo {
    INTRINSIC = 0,
    DOUBLE_FLOAT_TECH,
};

struct PowerConfig {
    PowerAlgo algo = PowerAlgo::INTRINSIC;
};
```

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: **VECIN**/**VECCALC**/**VECOUT**. |
| src0Tensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: **VECIN**/**VECCALC**/**VECOUT**.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src1Tensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: **VECIN**/**VECCALC**/**VECOUT**.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src0Scalar/src1Scalar | Input | Source operand of the **Scalar** type. The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary memory space.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: **VECIN**/**VECCALC**/**VECOUT**.<br><br>For the three **power** APIs, for how to obtain the temporary space size BufferSize under different input data types, see [GetPowerMaxMinTmpSize](GetPowerMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

- **Overlapping between the source operand and destination operand addresses is not supported.**
-   For the AI Core of Atlas inference products, the exponent of the exponentiation must be less than 2<sup>31</sup>-1.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Supported data types<a id="li559613463410"></a>

    Ascend 950PR/Ascend 950DT, the supported data types are: uint8\_t, int8\_t, uint16\_t, int16\_t, uint32\_t, int32\_t, half, bfloat16\_t, and float.

    Atlas A3 training products/Atlas A3 inference products, the supported data types are: half, float, and int32\_t.

    Atlas A2 training products/Atlas A2 inference products, the supported data types are: half, float, and int32\_t.

    For the AI Core of Atlas inference products, the supported data types are: half, float, and int32_t.

## Examples

For the complete call example, see the [Power example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/power).

```
// dstLocal: Tensor that stores the computation result.
// srcLocalExp: Tensor used as the exponent in the Power computation.
// srcLocalBase: Tensor used as the base in the Power computation.

// Use srcLocalBase as the base to perform exponentiation on all elements in srcLocalExp.
AscendC::Power<T, false>(dstLocal, srcLocalBase, srcLocalExp);

// scalarValueBase: Base used in the Power computation.
T scalarValueBase = srcLocalBase.GetValue(0);
// Use the same base scalarValueBase to perform exponentiation on all elements in srcLocalExp.
AscendC::Power<T, false>(dstLocal, scalarValueBase, srcLocalExp);

// scalarValueExp: Exponent used in the Power computation.
T scalarValueExp = srcLocalExp.GetValue(0);
// Use the same exponent scalarValueExp to perform exponentiation on all elements in srcLocalBase.
AscendC::Power<T, false>(dstLocal, srcLocalBase, scalarValueExp);

// static constexpr AscendC::PowerConfig config = { AscendC::PowerAlgo::DOUBLE_FLOAT_TECH };
// AscendC::Power<srcType, false, config>(dstLocal, scalarValue, srcLocal2);
```

AscendC::Power<T, false\>\(dstLocal, srcLocalBase, srcLocalExp\) Sample data is as follows:

```
Input data (srcLocalBase): [2 3 4 5 6 7 8 9]
Input data (srcLocalExp): [4 3 2 1 4 3 2 1]
Output data (dstLocal): [16 27 16 5 1296 343 64 9]
```

AscendC::Power<T, false\>\(dstLocal, scalarValueBase, srcLocalExp\) Sample data is as follows:

```
Input data (scalarValueBase): 2
Input data (srcLocalExp): [4 3 2 1 4 3 2 1]
Output data (dstLocal): [16 8 4 2 16 8 4 2]
```

AscendC::Power<T, false\>\(dstLocal, srcLocalBase, scalarValueExp\) Sample data is as follows:

```
Input data (srcLocalBase): [2 3 4 5 6 7 8 9]
Input data (scalarValueExp): 4
Output data (dstLocal): [16 81 256 625 1296 2401 4096 6561]
```
