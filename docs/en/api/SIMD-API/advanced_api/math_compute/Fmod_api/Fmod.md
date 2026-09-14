# Fmod

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T15:17:17.355Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the remainder after dividing two floating-point numbers a and b element by element. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002348879736.png)

![](../../../../figures/zh-cn_formulaimage_0000002188778220.png)

Here, **Trunc** denotes the truncation-toward-zero operation. Examples are as follows:

Fmod\(2.0, 1.5\) = 0.5

Fmod\(-3.0, 1.1\) = -0.8

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the API framework allocation method, developers need to reserve temporary space. For the case of passing through sharedTmpBuffer, developers need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the API provided in [GetFmodMaxMinTmpSize](GetFmodMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Fmod-related configuration. This parameter is optional and of the FmodConfig type. Its definition is shown in the following code, where the parameters are described as follows:<br>algo: Specifies the Fmod algorithm. Supported values of this parameter are as follows: NORMAL: the default value of algo, which uses the simulated normal mode. Supported data types: half and float. ITERATION_COMPENSATION: the high-precision mode with iteration compensation. Supported data type: float.<br><br>iterationNum: the number of iteration compensation rounds in the high-precision mode with iteration compensation. This parameter takes effect only when algo is set to ITERATION_COMPENSATION. The number of rounds ranges from 1 to 11, with a default value of 11. The more iteration rounds, the higher the result precision, but the lower the performance. When using it, you can select the number of iteration rounds based on the exponent bit difference between the two floating-point numbers. The float type has 8 exponent bits in total. The exponent bit difference between src0Tensor and src1Tensor should not exceed 24*iterationNum. |

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
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0Tensor, src1Tensor | Input | Source operands.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Fmod, provided by yourself.<br><br>For how to obtain the temporary space size BufferSize, see [GetFmodMaxMinTmpSize](GetFmodMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For the AI Core of Atlas inference products, the input data is limited to the range of \[-2147483647.0, 2147483647.0\].
-   The data lengths of the source operands src0Tensor and src1Tensor must be consistent.
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For Ascend 950PR/Ascend 950DT, when the algo in the template parameter config is ITERATION\_COMPENSATION iteration compensation mode, the operand data type supports only float.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete calling example, see the [fmod operator example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/fmod).

```
// dstLocal: Output Tensor that stores the Fmod computation result.
// src0Local: Input Tensor that stores the divisor for the Fmod computation.
// src1Local: Input Tensor that stores the dividend for the Fmod computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Fmod computation.

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::Fmod(dstLocal, src0Local, src1Local);
// The API framework allocates temporary space, with partial participation in computation. The number of elements to be computed is 512.
AscendC::Fmod(dstLocal, src0Local, src1Local, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, with all elements participating in computation.
AscendC::Fmod(dstLocal, src0Local, src1Local, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, with partial participation in computation. The number of elements to be computed is 512.
AscendC::Fmod(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

```
__aicore__ constexpr AscendC::FmodConfig GetConfig() {
    return { .algo = AscendC::FmodAlgo::ITERATION_COMPENSATION, .iterationNum = 11 };
}
static constexpr AscendC::FmodConfig config = GetConfig();
AscendC::Fmod<float, false, config>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (src0Local): [-2.56 -2.55 -2.54 ... -0.01 0. 0.01 ... 2.53  2.54  2.55]
Input data (src1Local): [2.    2.    2.    ... 2.    2. 2.   ... 2.    2.    2.]
Output data (dstLocal):  [-0.56 -0.55 -0.54 ... -0.01 0. 0.01 ... 0.53  0.54  0.55]
```
