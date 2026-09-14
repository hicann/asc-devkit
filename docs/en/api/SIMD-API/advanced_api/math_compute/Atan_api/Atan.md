# Atan

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:48:16.315Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs the arctangent trigonometric operation on each element. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002183047940.png)

![](../../../../figures/zh-cn_formulaimage_0000002227268389.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Configuration of the Atan algorithm. This parameter is optional and of the AtanConfig type. Its definition is shown in the following code, where the parameters are described as follows:<br>**algo**: Algorithm used in the internal implementation of Atan. It is of the AtanAlgo type and supports the following values: **TAYLOR_EXPANSION**: Default value. This algorithm implements Atan using a 6th-order Taylor expansion and supports the half and float data types. **POLYNOMIAL_APPROXIMATION**: This algorithm is a 17th-degree polynomial approximation algorithm and supports the float data type. |

```
enum class AtanAlgo {
    TAYLOR_EXPANSION = 0,
    POLYNOMIAL_APPROXIMATION,
};

struct AtanConfig {
    AtanAlgo algo = AtanAlgo::TAYLOR_EXPANSION;
};
```

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the Atan computation result.
// srcLocal: Tensor that stores the Atan computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Atan computation.

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::Atan(dstLocal, srcLocal);
// The API framework allocates temporary space, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Atan(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer, 512);

// Specify the input algorithm as POLYNOMIAL_APPROXIMATION, the input data type as float, and the actual number of computed elements as 512.
static constexpr AscendC::AtanConfig atanConfig = { AscendC::AtanAlgo::POLYNOMIAL_APPROXIMATION};
AscendC::Atan<float, false, atanConfig>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The sample data is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[-1.19847027, -1.19717361, -1.19560622 ... 0. ... 1.19429046, 1.19560622, 1.19717361]
```
