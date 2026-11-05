# Power

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:40:00.979Z pushedAt=2026-10-29T02:15:02.915Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Power_interface/Power_res.md#id1 -->

## Description

Implements element-wise power operation and provides three types of APIs. The processing logic is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218328733.png)

![](../../../../figures/zh-cn_formulaimage_0000002192030526.png)

## Prototype

-   Power\(dstTensor, src0Tensor, src1Tensor\)
    -   Passes temporary space through the sharedTmpBuffer input parameter
        -   Full/partial participation of the source operand Tensor in the computation

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   Full participation of the source operand Tensor in the computation

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework requests temporary space
        -   Full/partial participation of the source operand Tensor in the computation

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, uint32_t calCount)
            ```

        -   Full participation of the source operand Tensor in the computation

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
            ```

-   Power\(dstTensor, src0Tensor, src1Scalar\)
    -   Passes temporary space through the sharedTmpBuffer input parameter
        -   The source operand Tensor participates fully/partially in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The source operand Tensor participates fully in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework requests temporary space.
        -   The source operand Tensor participates fully/partially in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, uint32_t calCount)
            ```

        -   The source operand Tensor participates fully in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar)
            ```

-   Power\(dstTensor, src0Scalar, src1Tensor\)
    -   Passes temporary space through the sharedTmpBuffer input parameter.
        -   The source operand Tensor participates fully/partially in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   The source operand Tensor participates fully in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   The API framework requests temporary space.
        -   The source operand Tensor fully or partially participates in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, uint32_t calCount)
            ```

        -   The source operand Tensor fully participates in the computation.

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor)
            ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **requested by the API framework** and **passed by you through the sharedTmpBuffer input parameter**.

-   When the API framework requests the temporary space, you do not need to request it, but you must reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer requests it. In this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly requested and released, which provides higher flexibility and higher memory utilization.

In the method where the API framework requests the temporary space, you need to reserve the temporary space. In the case where the temporary space is passed through sharedTmpBuffer, you need to request space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the GetPowerMaxMinTmpSize API provided in [GetPowerMaxMinTmpSize](GetPowerMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. For the data types supported by different models, see [supported data types](#li559613463410). |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| config | <!-- npu="950" id7 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id7 -->Power computation configuration. This parameter is optional and of the PowerConfig type. Its definition is shown in the following code, where the parameters have the following meanings:<br>algo: Different Power algorithms supported by different data types. The supported values of this parameter are as follows: INTRINSIC: default value. If the data type is an integer, the INTRINSIC algorithm uses the fast exponentiation algorithm to implement Power computation, and the supported data types are uint8_t, int8_t, uint16_t, int16_t, uint32_t, and int32_t. If the data type is a floating-point type, the INTRINSIC algorithm performs Power computation according to the formula Power(x, y) = exp(y * ln(x)), and the supported data types are half and float. DOUBLE_FLOAT_TECH: The DOUBLE_FLOAT_TECH algorithm is a high-precision floating-point algorithm. It improves the precision of the source operand and then performs Power computation according to the formula Power(x, y) = exp(y * ln(x)), reducing precision loss during the computation process. The supported data types are bfloat16_t, half, and float. |

```
enum class PowerAlgo {
    INTRINSIC = 0,
    DOUBLE_FLOAT_TECH,
};

struct PowerConfig {
    PowerAlgo algo = PowerAlgo::INTRINSIC;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0Tensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src1Tensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| src0Scalar/src1Scalar | Input | Source operand, of the Scalar type. The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary memory space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For the three power APIs, for how to obtain the temporary space size BufferSize under different input data types, see [GetPowerMaxMinTmpSize](GetPowerMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

- **Address overlap between source and destination operands is not supported.**
<!-- npu="310p" id8 -->
-   For the Atlas inference products AI Core, the exponent of the power operation must be less than 2<sup>31</sup>-1.
<!-- end id8 -->
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Supported data types<a id="li559613463410"></a>

    <!-- npu="950" id9 -->
    For Ascend 950PR&950DT products, the supported data types are: uint8\_t, int8\_t, uint16\_t, int16\_t, uint32\_t, int32\_t, half, bfloat16\_t, float.
    <!-- end id9 -->
    <!-- npu="A3" id10 -->
    For Atlas A3 products, the supported data types are: half, float, int32\_t.
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    For Atlas A2 products, the supported data types are: half, float, int32\_t.
    <!-- end id11 -->
    <!-- npu="310p" id12 -->
    For the Atlas inference products AI Core, the supported data types are: half, float, int32\_t.
    <!-- end id12 -->

<!-- npu="950" id13 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal values in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When configured as `true`, FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with the result when Subnormal values are retained.
<!-- end id13 -->

## Examples

For the complete call example, see [Power Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/power).

```
// dstLocal: Tensor storing the computation result.
// srcLocalExp: Tensor used as the exponent in the Power computation.
// srcLocalBase: Tensor used as the base in the Power computation.

// Use srcLocalBase as the base to perform power operation on all elements in srcLocalExp.
AscendC::Power<T, false>(dstLocal, srcLocalBase, srcLocalExp);

// scalarValueBase: Base used in the Power computation.
T scalarValueBase = srcLocalBase.GetValue(0);
// Use the same base scalarValueBase to perform power operation on all elements in srcLocalExp.
AscendC::Power<T, false>(dstLocal, scalarValueBase, srcLocalExp);

// scalarValueExp: Exponent used in the Power computation.
T scalarValueExp = srcLocalExp.GetValue(0);
// Use the same exponent scalarValueExp to perform power operation on all elements in srcLocalBase.
AscendC::Power<T, false>(dstLocal, srcLocalBase, scalarValueExp);

// static constexpr AscendC::PowerConfig config = { AscendC::PowerAlgo::DOUBLE_FLOAT_TECH };
// AscendC::Power<srcType, false, config>(dstLocal, scalarValue, srcLocal2);
```

AscendC::Power<T, false\>\(dstLocal, srcLocalBase, srcLocalExp\) Example data is as follows:

```
Input data (srcLocalBase): [2 3 4 5 6 7 8 9]
Input data (srcLocalExp): [4 3 2 1 4 3 2 1]
Output data (dstLocal): [16 27 16 5 1296 343 64 9]
```

AscendC::Power<T, false\>\(dstLocal, scalarValueBase, srcLocalExp\) Example data is as follows:

```
Input data (scalarValueBase): 2
Input data (srcLocalExp): [4 3 2 1 4 3 2 1]
Output data (dstLocal): [16 8 4 2 16 8 4 2]
```

AscendC::Power<T, false\>\(dstLocal, srcLocalBase, scalarValueExp\) Example data is as follows:

```
Input data (srcLocalBase): [2 3 4 5 6 7 8 9]
Input data (scalarValueExp): 4
Output data (dstLocal): [16 81 256 625 1296 2401 4096 6561]
```
