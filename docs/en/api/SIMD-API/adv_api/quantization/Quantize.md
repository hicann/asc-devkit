# Quantize

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:48:54.749Z pushedAt=2026-10-29T02:14:34.548Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/Quantize_res.md#id1 -->

## Description

Performs element-wise quantization to convert high-precision data to low-precision data. This API supports four quantization strategies: PER\_TENSOR, PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP. Each quantization strategy supports configuring a rounding mode. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

Quantize is similar to [AscendQuant](AscendQuant.md) in functionality. Quantize extends the capability to configure rounding modes in the PER\_TENSOR and PER\_CHANNEL quantization scenarios, so this API is recommended.

-   PER\_TENSOR quantization: The entire srcTensor corresponds to one quantization parameter, and the shape of scale and offset is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000002292405593.png)

-   PER\_CHANNEL quantization: The shape of srcTensor is \[m, n\], and each channel dimension corresponds to one quantization parameter. The shape of scale and offset is \[1, n\].

    ![](../../../figures/zh-cn_formulaimage_0000002257766684.png)

-   PER\_TOKEN quantization: Elements in each token group of srcTensor (tokens are in the n direction, with m token groups in total) share one quantization parameter. When the shape of srcTensor is \[m, n\], the shape of scale and offset is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002257767428.png)

-   PER\_GROUP quantization: The computation direction of a group is defined as the k direction, and every groupSize elements of srcTensor in the k direction share one set of scale and offset. When the shape of srcTensor is \[m, n\], if kDim=0, k is the m direction, and the shape of scale and offset is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, k is the n direction, and the shape of scale and offset is \[m, \(n + groupSize - 1\) / groupSize\].

    Depending on the output data type, PER\_GROUP quantize is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the float4 scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the b8 scenario).

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (float4 scenario)
        -   kDim = 0:

            ![](../../../figures/zh-cn_formulaimage_0000002257871838.png)

        -   kDim = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292420797.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (b8 scenario)
        -   kDim = 0:

            ![](../../../figures/zh-cn_formulaimage_0000002292412281.png)

        -   kDim = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292420789.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
    ```

-   The API framework allocates the temporary space

    ```
    template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor,const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. The temporary space can be obtained in two ways: **allocated by the API framework** or **passed as the sharedTmpBuffer input parameter** by you.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the temporary space is allocated by the API framework, you need to reserve the temporary space. When it is passed through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the API provided in [GetQuantizeMaxMinTmpSize](GetQuantizeMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Used to configure quantization-related information. The type is QuantizeConfig, and its definition is shown in the following code. The meanings of its parameters are as follows.<br>policy: Used to configure the quantization strategy. It is an enumeration type, and its definition is shown in the following code.<br>hasOffset: Used to configure whether offset participates in the computation. true: indicates that offset participates in the computation. false: indicates that offset does not participate in the computation.<br>roundMode: Rounding mode used when data is converted from a high-precision data type to a low-precision data type during quantization. Supported values: CAST_RINT, CAST_ROUND, CAST_FLOOR, CAST_CEIL, CAST_TRUNC, and CAST_HYBRID. For details about each rounding mode, see [precision conversion rules](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1). Different data types support different rounding modes during quantization. When an unsupported rounding mode is used during quantization, the default rounding mode is used instead. For example, when the bfloat16_t data type is quantized to the hifloat8_t data type, if the configured roundMode is the unsupported CAST_RINT, the default roundMode (CAST_ROUND) is used during actual quantization. For the rounding modes supported by different data types, see the following table.<br>kDim: Computation direction of the group, that is, the k direction. It is valid only in the PER_GROUP scenario. Supported values: 0: The k axis is axis 0, that is, the m direction is the computation direction of the group. 1: The k axis is axis 1, that is, the n direction is the computation direction of the group. |
| DstT | Data type of the destination operand. The API automatically infers the data type based on the dstTensor input parameter. You do not need to configure this parameter. Ensure that dstTensor conforms to the supported input/output data type combinations. |
| SrcT | Data type of the source operand. The API automatically infers the data type based on the srcTensor input parameter. You do not need to configure this parameter. Ensure that srcTensor conforms to the supported input/output data type combinations. |
| ScaleT | Data type of the scaling factor scale. The API automatically infers the data type based on the scale input parameter. You do not need to configure this parameter. ScaleT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, scale is a scalar, and ScaleT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, scale is a vector, and ScaleT can only be a LocalTensor type. |
| OffsetT | Data type of offset. The API automatically infers the data type based on the offset input parameter. You do not need to configure this parameter. OffsetT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, offset is a scalar, and OffsetT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, offset can be a scalar or a vector, and OffsetT can be a scalar data type or a LocalTensor type. |

```
struct QuantizeConfig {
    QuantizePolicy policy;
    bool hasOffset;
    RoundMode roundMode = RoundMode::CAST_RINT;
    int32_t kDim = 1;
};
enum class QuantizePolicy : int32_t { PER_TENSOR, PER_CHANNEL, PER_TOKEN, PER_GROUP };
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| scale | Input | Scaling factor used when quantizing the input data. |
| offset | Input | Offset used when quantizing the input data. For the float4 scenario of PER_GROUP quantization, offset does not take effect. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetQuantizeMaxMinTmpSize](GetQuantizeMaxMinTmpSize.md). |
| params | Input | Parameters of the quantization API. The type is QuantizeParams, and its definition is shown in the following code. The meanings of its parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must meet the 32-byte alignment requirement, that is, both the input and output whose last dimension of the shape is n must meet the 32-byte alignment requirement on this dimension.<br>groupSize: Valid in the PER_GROUP scenario. Indicates that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct QuantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
```

The supported input/output data type combinations are as follows:

**Table 3**  Data type combinations supported when DstT is fp8_e5m2_t/fp8_e4m3fn_t

| SrcT | ScaleT/OffsetT |  roundMode |
| --- | --- | --- |
| half | half | CAST_RINT (default) |
| bfloat16_t | bfloat16_t | CAST_RINT (default) |
| float | float | CAST_RINT (default) |
| half | float | CAST_RINT (default) |
| bfloat16_t | float | CAST_RINT (default) |

**Table 4**  Data type combinations supported when DstT is hifloat8_t

| SrcT | ScaleT/OffsetT |  roundMode |
| --- | --- | --- |
| half | half | CAST_ROUND (default)<br>CAST_HYBRID |
| bfloat16_t | bfloat16_t | CAST_ROUND (default)<br>CAST_HYBRID |
| float | float | CAST_ROUND (default)<br>CAST_HYBRID |
| half | float | CAST_ROUND (default)<br>CAST_HYBRID |
| bfloat16_t | float | CAST_ROUND (default)<br>CAST_HYBRID |

**Table 5** Data type combinations supported when DstT is int8_t

| SrcT | ScaleT/OffsetT | roundMode |
| --- | --- | --- |
| half | half | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| float | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |

**Table 6** Data type combinations supported when DstT is fp4x2_e1m2_t/fp4x2_e2m1_t (currently only the PER_GROUP scenario is supported)

| SrcT | ScaleT/OffsetT | roundMode |
| --- | --- | --- |
| half | half | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| float | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |

## Return Value

None

## Constraints

-   **Address overlap between the source operands and the destination operand is not supported.**
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands involved in computation must be 32-byte aligned.
-   The data volume in the continuous computation direction (that is, the n direction) must be 32-byte aligned.
-   PER\_GROUP Offset is not supported in the float4 quantization scenario. In this scenario, the hasOffset parameter in the template parameter config must be set to false.

## Examples

-   PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP modes

    ```
    constexpr static QuantizePolicy tokenPolicy = QuantizePolicy::PER_TOKEN;
    constexpr static QuantizePolicy channelPolicy = QuantizePolicy::PER_CHANNEL;
    constexpr static QuantizePolicy groupPolicy = QuantizePolicy::PER_GROUP;
    // Here, the PER_TOKEN mode is used as an example, with offset enabled and the rounding mode set to CAST_ROUND. kDim is valid only in the PER_GROUP scenario and indicates that the group computation direction is the n direction.
    constexpr static QuantizeConfig config = {tokenPolicy, true, RoundMode::CAST_ROUND, 1};
    QuantizeParams params;
    // m and n are external input parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = n; // Valid only in the PER_GROUP scenario. Here it indicates that all elements in the n direction share one set of scale and offset.
    // dstLocal is a LocalTensor of the int8_t type, and srcLocal, scale, and offset are LocalTensors of the half type.
    Quantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

-   PER\_TENSOR mode

    ```
    constexpr static QuantizePolicy tensorPolicy = QuantizePolicy::PER_TENSOR;
    // Enable offset, with the rounding mode set to CAST_ROUND.
    constexpr static QuantizeConfig config = {tensorPolicy, true, RoundMode::CAST_ROUND, -1};
    QuantizeParams params;
    // m and n are external input parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = 0; // Valid only in the PER_GROUP scenario.
    // dstLocal is a LocalTensor of the int8_t type, srcLocal is a LocalTensor of the half type, and scale and offset are scalars of the half type.
    Quantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

The result is as follows:

```
Input data (srcLocal):
[-4.4, 2.5, -2.9, -3.1, -1.5, -4.8, 1.8, 3.5, 4.5, 1.1, -2.7, 0.5, ... 1.6]
Input data (scale vector):
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, ... 1]
Input data (scale scalar):
[1]
Input data (offset vector):
[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ... 0]
Input data (offset scalar):
[0]
Output data (dstLocal):
[-4, 3, -3, -3, -1, -5, 2, 4, 5, 1, -3, 1, ... 2]
```
