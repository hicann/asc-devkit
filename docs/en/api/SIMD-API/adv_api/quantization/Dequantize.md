# Dequantize

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:28:37.103Z pushedAt=2026-10-29T02:14:34.492Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/Dequantize_res.md#id1 -->

## Description

Performs dequantization element by element, for example, dequantizing the int32\_t data type to data types such as half/float. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

Dequantize is similar in functionality to [AscendDequant](AscendDequant.md). This API provides a more unified form across different quantization scenarios, so it is recommended.

This API supports four dequantization strategies: PER\_TENSOR, PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP. The dequantization coefficient scale is a scalar in the PER\_TENSOR scenario and a vector in the other scenarios. The specific computation formulas are as follows:

-   PER\_TENSOR scenario (per-tensor dequantization): The shape of scale and offset is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000002295238441.png)

-   PER\_CHANNEL scenario (per-channel dequantization): When the shape of srcTensor is \[m, n\], each channel dimension corresponds to one quantization parameter, and the shape of scale and offset is \[1, n\].

    ![](../../../figures/zh-cn_formulaimage_0000002260632216.png)

-   PER\_TOKEN scenario (per-token dequantization): The elements in each token group of srcTensor (token is the n direction, with m token groups in total) share one quantization parameter. When the shape of srcTensor is \[m, n\], the shape of scale and offset is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002260737926.png)

-   PER\_GROUP Scenario (per-group dequantization): The computation direction of a group is defined as the k direction. In srcTensor, every groupSize elements along the k direction share one set of scale and offset. When the shape of srcTensor is \[m, n\], if kDim=0, k is the m direction, and the shape of scale and offset is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, k is the n direction, and the shape of scale and offset is \[m, \(n + groupSize - 1\) / groupSize\].
    -   k is the m direction, that is, the i axis in the formula is the computation direction of the group (kDim=0):

        ![](../../../figures/zh-cn_formulaimage_0000002295268473.png)

    -   k is the n direction, that is, the j axis in the formula is the computation direction of the group (kDim=1):

        ![](../../../figures/zh-cn_formulaimage_0000002295155733.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Dequantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const LocalTensor<uint8_t>& sharedTmpBuffer, const DequantizeParams& params)
    ```

-   The API framework allocates the temporary space

    ```
    template <const DequantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Dequantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const DequantizeParams& params)
    ```

    Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space can be obtained in two ways: **allocated by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

    -   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

    -   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

    When the temporary space is allocates by the API framework, you need to reserve the temporary space. When it is passed in through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. To obtain the temporary space size BufferSize, use the GetDequantizeMaxMinTmpSize API provided in [GetDequantizeMaxMinTmpSize](GetDequantizeMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Configures dequantization-related information. The type is DequantizeConfig, defined in the following code, where the parameters are described as follows.<br>policy: Configures the quantization strategy. It is an enumeration type, defined in the following code.<br>hasOffset: Reserved parameter. Currently, only false is supported.<br>kDim: Computation direction of the group, that is, the k direction. It is valid only in the PER_GROUP scenario. Supported values are as follows. 0: The k axis is axis 0, that is, the m direction is the computation direction of the group. 1: The k axis is axis 1, that is, the n direction is the computation direction of the group. |
| DstT | Data type of the destination operand. The API automatically infers the data type based on the dstTensor input parameter. You do not need to configure this parameter. Ensure that dstTensor meets the supported input/output data type combinations in Table 3. |
| SrcT | Data type of the source operand. The API automatically infers the data type based on the srcTensor input parameter. You do not need to configure this parameter. Ensure that srcTensor meets the supported input/output data type combinations in Table 3. |
| ScaleT | Data type of scale. The API automatically infers the data type based on the scale input parameter. You do not need to configure this parameter. ScaleT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>In the PER_TENSOR scenario, scale is a scalar, and ScaleT can only be a scalar data type.<br>In the PER_CHANNEL, PER_TOKEN, and PER_GROUP scenarios, scale is a vector, and ScaleT can only be a LocalTensor type. |
| OffsetT | Data type of offset. The API automatically infers the data type based on the offset input parameter. You do not need to configure this parameter. OffsetT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, offset is a scalar, and OffsetT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, offset is a vector, and OffsetT can only be a LocalTensor type. |

```
struct DequantizeConfig {
    DequantizePolicy policy;
    bool hasOffset = false;
    int32_t kDim = 1;
};
enum class DequantizePolicy : int32_t { PER_TENSOR, PER_CHANNEL, PER_TOKEN, PER_GROUP };
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Assume that the shape of srcTensor is [m, n]. The number of bytes occupied by each row of data (that is, n input data) must be 32-byte aligned. |
| scale | Input | Scaling factor for dequantizing the input data. |
| offset | Input | Offset for dequantizing the input data. This is a reserved parameter and can be configured as 0 or an empty tensor. |
| sharedTmpBuffer | Input | Temporary buffer. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetDequantizeMaxMinTmpSize](GetDequantizeMaxMinTmpSize.md). |
| params | Input | Parameters of the dequantization API. The type is DequantizeParams, defined in the following code, where the parameters are described as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must meet the 32-byte alignment requirement, that is, both the input and output whose last dimension is n must meet the 32-byte alignment requirement on this dimension.<br>groupSize: Valid in the PER_GROUP scenario. Indicates that groupSize rows/columns of data share one scale. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct DequantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
```

**Table 3**  Supported input/output data type combinations

| Quantization Strategy | dstTensor | srcTensor | scale/offset |
| --- | --- | --- | --- |
| PER_TENSOR | bfloat16_t | int32_t | bfloat16_t |
| PER_TENSOR | bfloat16_t | int32_t | float |
| PER_TENSOR | float | int32_t | bfloat16_t |
| PER_TENSOR | float | int32_t | float |
| PER_CHANNEL | half | int32_t | uint64_t<br><br>Note: When the data type of scale is uint64_t, the lower 32 bits are the float data that participates in the computation, and the upper 32 bits are not used by this API. |
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

-   **Address overlap between the source operands and the destination operand is not supported.**
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data volume in the continuous computation direction (that is, the n direction) must be 32-byte aligned.

## Examples

-   PER\_CHANNEL, PER\_TOKEN, PER\_GROUP modes

    ```
    constexpr static DequantizePolicy tokenPolicy = DequantizePolicy::PER_TOKEN;
    constexpr static DequantizePolicy channelPolicy = DequantizePolicy::PER_CHANNEL;
    constexpr static DequantizePolicy groupPolicy = DequantizePolicy::PER_GROUP;
    // The following uses the PER_CHANNEL mode as an example, with offset disabled. kDim is valid only in the PER_GROUP scenario and indicates that the group computation direction is the n direction.
    constexpr static DequantizeConfig config = {channelPolicy, false, 1};
    DequantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = n; // Valid only in the PER_GROUP scenario. Here it indicates that all elements in the n direction share one set of scale and offset.
    // srcLocal is a LocalTensor of the int32_t type, and dstLocal, scaleLocal, and offsetLocal are LocalTensors of the float type.
    Dequantize<config>(dstLocal, srcLocal, scaleLocal, offsetLocal, params); // offsetLocal is a reserved parameter and can be configured as an empty Tensor.
    ```

-   PER\_TENSOR mode

    ```
    constexpr static DequantizePolicy tensorPolicy = DequantizePolicy::PER_TENSOR;
    // Offset is not enabled.
    constexpr static DequantizeConfig config = {tensorPolicy, false, -1};
    DequantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = 0; // Takes effect only in the PER_GROUP scenario.
    // srcLocal is a LocalTensor of the int32_t type, dstLocal is a LocalTensor of the float type, and scale and offset are scalars of the float type.
    Dequantize<config>(dstLocal, srcLocal, scale, offset, params); // offset is a reserved parameter and can be set to 0;
    ```

The result is as follows:

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
