# Dequantize

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T11:49:49.972Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs dequantization element by element, for example, dequantizing the int32\_t data type to data types such as half/float. **This API supports at most two-dimensional input and does not support higher-dimensional input.**

Dequantize is functionally similar to [AscendDequant](AscendDequant.md). This API provides a more unified form across different quantization scenarios, and therefore is recommended.

The dequantization strategies of this API include PER\_TENSOR, PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP. The dequantization coefficient scale is a scalar in the PER\_TENSOR scenario and a vector in the other scenarios. The specific calculation formulas are as follows:

-   PER\_TENSOR scenario (per-tensor dequantization): The shape of scale and offset is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000002295238441.png)

-   PER\_CHANNEL scenario (per-channel dequantization): The shape of srcTensor is \[m, n\], each channel dimension corresponds to one quantization parameter, and the shape of scale and offset is \[1, n\].

    ![](../../../figures/zh-cn_formulaimage_0000002260632216.png)

-   PER\_TOKEN scenario (per-token dequantization): The elements in each token group of srcTensor (a token is in the n direction, and there are m token groups in total) share one quantization parameter. When the shape of srcTensor is \[m, n\], the shape of scale and offset is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002260737926.png)

-   PER\_GROUP scenario (dequantization by group): The computation direction of a group is defined as the k direction. In the k direction of srcTensor, every groupSize elements share one set of scale and offset. When the shape of srcTensor is \[m, n\], if kDim=0, it indicates that k is the m direction, and the shape of scale and offset is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, it indicates that k is the n direction, and the shape of scale and offset is \[m, \(n + groupSize - 1\) / groupSize\].
    -   k is the m direction, that is, the i axis in the formula is the computation direction of the group (kDim=0):

        ![](../../../figures/zh-cn_formulaimage_0000002295268473.png)

    -   k is the n direction, that is, the j axis in the formula is the computation direction of the group (kDim=1):

        ![](../../../figures/zh-cn_formulaimage_0000002295155733.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Dequantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
    ```

-   The API framework applies for temporary space.

    ```
    template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Dequantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const DequantizeParams& params)
    ```

    Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

    -   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

    -   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

    For the method of allocation by the API framework, you need to reserve temporary space. For the case of passing temporary space via **sharedTmpBuffer**, you need to apply for space for sharedTmpBuffer. The method for obtaining the temporary space size **BufferSize** is as follows: use the GetDequantizeMaxMinTmpSize API provided in [GetDequantizeMaxMinTmpSize](GetDequantizeMaxMinTmpSize.md) to obtain the range size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Used to configure dequantization-related information. The type is DequantizeConfig, defined as shown in the following code, where the meanings of the parameters are as follows.<br>policy: Used to configure the quantization policy. It is an enumeration type, defined as shown in the following code.<br>hasOffset: Reserved parameter. Currently, only false is supported.<br>kDim: Computation direction of the group, that is, the k direction. It is valid only in the PER_GROUP scenario. The supported values are as follows. 0: The k axis is axis 0, that is, the m direction is the computation direction of the group. 1: The k axis is axis 1, that is, the n direction is the computation direction of the group. |
| DstT | Data type of the destination operand. The API automatically deduces the data type based on the input parameter dstTensor. You do not need to configure this parameter, but only need to ensure that dstTensor satisfies the supported data type combinations of input and output in Table 3. |
| SrcT | Data type of the source operand. The API automatically deduces the data type based on the input parameter srcTensor. You do not need to configure this parameter, but only need to ensure that srcTensor satisfies the supported data type combinations of input and output in Table 3. |
| ScaleT | Data type of scale. The API automatically deduces the data type based on the input parameter scale. You do not need to configure this parameter. ScaleT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR scenario, scale is a scalar, and ScaleT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP scenarios, scale is a vector, and ScaleT can only be a LocalTensor type. |
| OffsetT | Data type of offset. The API automatically deduces the data type based on the input parameter offset. You do not need to configure this parameter. OffsetT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization policy, offset is a scalar, and OffsetT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, offset is a vector, and OffsetT can only be a LocalTensor type. |

```
struct DequantizeConfig {
    DequantizePolicy policy;
    bool hasOffset = false;
    int32_t kDim = 1;
};
enum class DequantizePolicy : int32_t {
    PER_TENSOR,
    PER_CHANNEL,
    PER_TOKEN,
    PER_GROUP
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Assume that the shape of srcTensor is [m, n]. The number of bytes occupied by each row of data (that is, n input data) must be 32-byte aligned. |
| scale | Input | Scaling factor used when dequantizing the input data. |
| offset | Input | Offset used when dequantizing the input data. Currently a reserved parameter, which can be configured as 0 or an empty Tensor. |
| sharedTmpBuffer | Input | Temporary buffer. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetDequantizeMaxMinTmpSize](GetDequantizeMaxMinTmpSize.md). |
| params | Input | Parameters of the dequantization API. The type is DequantizeParams, defined as shown in the following code, where the meanings of the parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must satisfy the 32-byte alignment requirement, that is, the input or output whose last dimension of the shape is n must satisfy the 32-byte alignment requirement in this dimension.<br>groupSize: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct DequantizeParams {
        uint32_t m;
        uint32_t n;
        uint32_t groupSize = 0;
};
```

**Table 3** Supported data type combinations for input and output

| Quantization Policy | dstTensor | srcTensor | scale/offset |
| --- | --- | --- | --- |
| PER_TENSOR | bfloat16_t | int32_t | bfloat16_t |
| PER_TENSOR | bfloat16_t | int32_t | float |
| PER_TENSOR | float | int32_t | bfloat16_t |
| PER_TENSOR | float | int32_t | float |
| PER_CHANNEL | half | int32_t | uint64_t<br><br>Note: When the data type of scale is uint64_t, the lower 32 bits are float data that participate in the computation, and the upper 32 bits are not used by this API. |
| PER_CHANNEL | float | int32_t | float |
| PER_CHANNEL | float | int32_t | bfloat16_t |
| PER_CHANNEL | bfloat16_t | int32_t | bfloat16_t |
| PER_CHANNEL | bfloat16_t | int32_t | float |
| PER_TOKEN/PER_GROUP | half | int32_t | half |
| PER_TOKEN/PER_GROUP | bfloat16_t | int32_t | bfloat16_t |
| PER_TOKEN/PER_GROUP | float | int32_t | float |
| PER_TOKEN/PER_GROUP | half | int32_t | float |
| PER_TOKEN/PER_GROUP | bfloat16_t | int32_t | float |
| PER_TOKEN/PER_GROUP | half | float | half |
| PER_TOKEN/PER_GROUP | bfloat16_t | float | bfloat16_t |
| PER_TOKEN/PER_GROUP | float | float | float |
| PER_TOKEN/PER_GROUP | half | float | float |
| PER_TOKEN/PER_GROUP | bfloat16_t | float | float |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data volume in the continuous computation direction (that is, the n direction) must be 32-byte aligned.

## Examples

-   PER\_CHANNEL, PER\_TOKEN, PER\_GROUP modes

    ```
    constexpr static DequantizePolicy tokenPolicy = DequantizePolicy::PER_TOKEN;
    constexpr static DequantizePolicy channelPolicy = DequantizePolicy::PER_CHANNEL;
    constexpr static DequantizePolicy groupPolicy = DequantizePolicy::PER_GROUP;
    // Here, PER_CHANNEL mode is used as an example, with offset disabled. kDim is valid only in the PER_GROUP scenario and indicates that the group computation direction is the n direction.
    constexpr static DequantizeConfig config = {channelPolicy, false, 1};
    DequantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = n;  // Takes effect only in the PER_GROUP scenario. Here, it indicates that all elements in the n direction share one set of scale and offset.
    // srcLocal is a LocalTensor of type int32_t, and dstLocal, scaleLocal, and offsetLocal are LocalTensors of type float.
    Dequantize<config>(dstLocal, srcLocal, scaleLocal, offsetLocal, params);  // offsetLocal is a reserved parameter and can be configured as an empty Tensor.
    ```

-   PER\_TENSOR mode

    ```
    constexpr static DequantizePolicy tensorPolicy = DequantizePolicy::PER_TENSOR;
    // Do not enable offset.
    constexpr static DequantizeConfig config = {tensorPolicy, false, -1};
    DequantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = 0;  // Takes effect only in the PER_GROUP scenario.
    // srcLocal is a LocalTensor of the int32_t type, dstLocal is a LocalTensor of the float type, and scale and offset are scalars of the float type.
    Dequantize<config>(dstLocal, srcLocal, scale, offset, params);  // offset is a reserved parameter and can be configured to 0.
    ```

The results are as follows:

```
Input data (srcLocal):
[-4, 2, -2, -3, -1, -4, 1, 3, 4, 1, -2, 0, ... 1]
Input data (scale vector):
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, ... 1]
Input data (scale scalar):
[1]
Output data (dstLocal), where dstLocal = srcLocal:
[-4, 2, -2, -3, -1, -4, 1, 3, 4, 1, -2, 0, ... 1]
```
