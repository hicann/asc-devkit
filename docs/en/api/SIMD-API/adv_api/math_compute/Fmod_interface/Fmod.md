# Fmod

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:59:50.931Z pushedAt=2026-10-29T02:15:02.734Z nl=2 -->

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
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Fmod_interface/Fmod_res.md#id1 -->

## Description

Computes the remainder of two floating-point numbers a and b after division, element by element. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002348879736.png)

![](../../../../figures/zh-cn_formulaimage_0000002188778220.png)

Here, Trunc denotes truncation toward zero. Examples are as follows:

Fmod\(2.0, 1.5\) = 0.5

Fmod\(-3.0, 1.1\) = -0.8

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **allocation by the API framework** and **passing through the sharedTmpBuffer input parameter by the developer**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization.

When the API framework allocates temporary space, you need to reserve the temporary space. When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: obtain the size of the space to be reserved through the API provided in [GetFmodMaxMinTmpSize](GetFmodMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| config | <!-- npu="950" id9 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id9 -->Fmod computation configuration. This parameter is optional and of the FmodConfig type. Its definition is shown in the following code, where the parameters are described as follows:<br>algo: specifies the Fmod algorithm. The supported values of this parameter are as follows: NORMAL: the default value of algo, which uses the simulated normal mode. Supported data types: half and float. ITERATION_COMPENSATION: the high-precision mode with iteration compensation. Supported data type: float.<br><br>iterationNum: the number of iteration compensation rounds in the high-precision mode with iteration compensation. This parameter takes effect only when algo is set to ITERATION_COMPENSATION. The number of rounds ranges from 1 to 11, and the default value is 11. The more iteration rounds, the higher the result precision, but the lower the performance. When using this parameter, you can select the number of iteration rounds based on the exponent bit difference between the two floating-point numbers. The float type has 8 exponent bits. The exponent bit difference between src0Tensor and src1Tensor should not exceed 24*iterationNum. |

```
constexpr uint32_t FMOD_ITERATION_NUM_MAX = 11;
enum class FmodAlgo {
    NORMAL = 0,
    ITERATION_COMPENSATION = 1,
};
struct FmodConfig {
    FmodAlgo algo = FmodAlgo::NORMAL;
    uint32_t iterationNum = FMOD_ITERATION_NUM_MAX;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are VECIN/VECCALC/VECOUT. |
| src0Tensor, src1Tensor | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operands must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). The supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Fmod. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetFmodMaxMinTmpSize](GetFmodMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

<!-- npu="310p" id8 -->
-   For the Atlas inference products AI Core, the input data is limited to the range of \[-2147483647.0, 2147483647.0\].
<!-- end id8 -->
-   The data lengths of the source operands src0Tensor and src1Tensor must be consistent.
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the addresses of the source and destination operands is not supported.
<!-- npu="950" id10 -->
-   For Ascend 950PR&950DT products, when algo in the template parameter config is in ITERATION\_COMPENSATION iteration compensation mode, the operand data type supports only float.
<!-- end id10 -->
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id11 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with preserving Subnormal values.
<!-- end id11 -->

## Examples

For the complete call example, see [Fmod Operator Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/fmod).

```
// dstLocal: Output tensor that stores the Fmod computation result.
// src0Local: Input tensor that stores the divisor for the Fmod computation.
// src1Local: Input tensor that stores the dividend for Fmod computation.
// sharedTmpBuffer: Tensor that stores the temporary space during Fmod computation.

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::Fmod(dstLocal, src0Local, src1Local);
// The API framework allocates temporary space, and only some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Fmod(dstLocal, src0Local, src1Local, 512);

// Temporary space is passed through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Fmod(dstLocal, src0Local, src1Local, sharedTmpBuffer);
// Temporary space is passed through the sharedTmpBuffer input parameter, and only some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Fmod(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

```
__aicore__ constexpr AscendC::FmodConfig GetConfig()
{
    return {.algo = AscendC::FmodAlgo::ITERATION_COMPENSATION, .iterationNum = 11};
}
static constexpr AscendC::FmodConfig config = GetConfig();
AscendC::Fmod<float, false, config>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (src0Local): [-2.56 -2.55 -2.54 ... -0.01 0. 0.01 ... 2.53  2.54  2.55]
Input data (src1Local): [2.    2.    2.    ... 2.    2. 2.   ... 2.    2.    2.]
Output data (dstLocal):  [-0.56 -0.55 -0.54 ... -0.01 0. 0.01 ... 0.53  0.54  0.55]
```
