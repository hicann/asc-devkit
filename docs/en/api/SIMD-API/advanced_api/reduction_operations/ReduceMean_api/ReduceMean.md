# ReduceMean

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T12:52:04.331Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the mean of a multidimensional vector along the specified dimension.

Define the specified computation dimension as the R-axis (Reduce axis) and the non-specified dimension as the A-axis (Normal axis). As shown in the figure below, for a two-dimensional matrix of shape \(2, 3\), when the mean is computed along the first dimension, the output is \[2.5, 3.5, 4.5\]; when the mean is computed along the second dimension, the output is \[2, 5\].

**Figure 1**  Example of ReduceMean computation along the first dimension  
![](../../../../figures/ReduceMean_computation_along_the_first_dimension.png "Example of ReduceMean computation along the first dimension")

**Figure 2**  Example of ReduceMean computation along the last dimension  
![](../../../../figures/ReduceMean_computation_along_the_last_dimension.png "Example of ReduceMean computation along the last dimension")

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceMean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t srcShape[], bool srcInnerPad)
    ```

-   The API framework applies for temporary space.

    ```
    template <class T, class pattern, bool isReuseSource = false>
    __aicore__ inline void ReduceMean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t srcShape[], bool srcInnerPad)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. When the API framework applies for it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetReduceMeanMaxMinTmpSize](GetReduceMeanMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data type is float. |
| pattern | Specifies the ReduceMean computation axes, including the Reduce axis and the Normal axis. **pattern** is formed by a combination of the letters A and R, the number of which equals the number of vector dimensions. The letter A indicates the Normal axis, and R indicates the Reduce axis. For example, AR indicates that ReduceMean is computed on a two-dimensional vector: the first dimension is the Normal axis and the second dimension is the Reduce axis, that is, the mean is computed along the second dimension.<br><br>**pattern** is a structure defined in the **AscendC::Pattern::Reduce** namespace, and you do not need to care about its member variables.<br><br>Currently, **pattern** supports only the values AR and RA. You need to explicitly specify **pattern** as **AscendC::Pattern::Reduce::AR** or **AscendC::Pattern::Reduce::RA**. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is **false**. If you allow the source operand to be overwritten, set this parameter to **true** to enable it, which saves some memory space.<br><br>If set to **true**, the memory space of **src** is reused during the internal computation of this API, saving memory space. If set to **false**, the memory space of **src** is not reused during the internal computation of this API.<br><br>For an example of using **isReuseSource**, see [More Samples](../../math_compute/more_samples_83.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside ReduceMean, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReduceMeanMaxMinTmpSize](GetReduceMeanMaxMinTmpSize.md). |
| srcShape | Input | Array of the uint32_t type, indicating the shape information of the source operand. The dimension of this shape must be consistent with the dimension of the template parameter **pattern**. Currently, only a two-dimensional shape is supported. For example, if **pattern** is AR, the dimension of this shape can only be two-dimensional. |
| srcInnerPad | Input | Indicates whether the data of the innermost axis to be actually computed is 32-byte aligned. Currently, only **true** is supported. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   The internal algorithm does not handle data overflow during accumulation, and API precision is not guaranteed in overflow scenarios.

## Examples

```
uint32_t shape[] = { 2, 8 };
constexpr bool isReuse = true;
AscendC::ReduceMean<T, AscendC::Pattern::Reduce::AR, isReuse>(dstLocal, srcLocal, tmp, shape, true); // tmp is the size of the passed temporary space, shape is the shape of the srcLocal input, and true indicates whether the address is 32B aligned.
```

The results are as follows:

```
The input and output data type is float.
Input data (src):
[[ 0.0 4.0 2.0 0.0 -1.0 2.0 -1.0 7.0],
 [ 0.0 1.0 -9.0 2.0 2.0 2.0 8.0 3.0]]
Input pattern: AR
Input shape: (2,8)
Output data (dst): [1.625 1.125]
```
