# Quantize

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T12:12:57.098Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise quantization to convert high-precision data to low-precision data. This API supports four quantization strategies: PER\_TENSOR, PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP, each of which supports configuring a rounding mode. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

Quantize is functionally similar to [AscendQuant](AscendQuant.md). Quantize extends the capability of configuring the rounding mode in the PER\_TENSOR and PER\_CHANNEL quantization scenarios, so this API is recommended.

-   PER\_TENSOR quantization: The entire srcTensor corresponds to one quantization parameter, and the shape of scale and offset is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000002292405593.png)

-   PER\_CHANNEL quantization: The shape of srcTensor is \[m, n\], each channel dimension corresponds to one quantization parameter, and the shape of scale and offset is \[1, n\].

    ![](../../../figures/zh-cn_formulaimage_0000002257766684.png)

-   PER\_TOKEN quantization: The elements in each token group of srcTensor (token is the n direction, with m token groups in total) share one quantization parameter. When the shape of srcTensor is \[m, n\], the shape of scale and offset is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002257767428.png)

-   PER\_GROUP Quantize: Defines the computation direction of the group as the **k** direction, where every **groupSize** elements of **srcTensor** in the **k** direction share one set of **scale** and **offset**. When the shape of **srcTensor** is \[m, n\], if **kDim**=0, **k** is the **m** direction, and the shape of **scale** and **offset** is \[\(m + **groupSize** - 1\) / **groupSize**, n\]; if **kDim**=1, **k** is the **n** direction, and the shape of **scale** and **offset** is \[m, \(n + **groupSize** - 1\) / groupSize\].

    Depending on the output data type, PER\_GROUP quantization is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the **float4** scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the **b8** scenario).

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (**float4** scenario)
        -   **kDim** = 0:

            ![](../../../figures/zh-cn_formulaimage_0000002257871838.png)

        -   **kDim** = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292420797.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (**b8** scenario)
        -   **kDim** = 0:

            ![](../../../figures/zh-cn_formulaimage_0000002292412281.png)

        -   kDim = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292420789.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
    ```

-   The API framework allocates temporary space.

    ```
    template <const QuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void Quantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor,const ScaleT& scale, const OffsetT& offset, const QuantizeParams& params)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the method of space allocation by the API framework, you need to reserve temporary space. For the case of passing the space through **sharedTmpBuffer**, you need to allocate space for **sharedTmpBuffer**. The temporary space size **BufferSize** is obtained as follows: obtain the size range of the space to be reserved through the API provided in [GetQuantizeMaxMinTmpSize](GetQuantizeMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Used to configure information related to quantize computation. The type is QuantizeConfig, and its specific definition is shown in the following code, where the meanings of the parameters are as follows.<br>policy: Used to configure the quantization strategy. It is an enumeration type, and its specific definition is shown in the following code.<br>hasOffset: Used to configure whether offset participates in the computation. true: Indicates that offset participates in the computation. false: Indicates that offset does not participate in the computation.<br>roundMode: The rounding mode used when data is converted from a high-precision data type to a low-precision data type during quantization. Supported values are CAST_RINT, CAST_ROUND, CAST_FLOOR, CAST_CEIL, CAST_TRUNC, and CAST_HYBRID. For details about each rounding mode, see [precision conversion rules](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1). Quantization of different data types supports different rounding modes. When an unsupported rounding mode is used during quantization, it falls back to the default rounding mode. For example, when the bfloat16_t data type is quantized to the hifloat8_t data type, if the configured roundMode is the unsupported CAST_RINT, the actual quantization falls back to the default roundMode (CAST_ROUND). For the rounding modes supported by different data types, see the following table.<br>kDim: The computation direction of group, that is, the k direction. It is valid only in the PER_GROUP scenario. Supported values are as follows: 0: The k axis is the 0th axis, that is, the m direction is the computation direction of group. 1: The k axis is the 1st axis, that is, the n direction is the computation direction of group. |
| DstT | Data type of the destination operand. The API automatically derives the data type based on the input parameter dstTensor. You do not need to configure this parameter, but only need to ensure that dstTensor conforms to the supported data type combinations for input and output. |
| SrcT | Data type of the source operand. The API automatically derives the data type based on the input parameter srcTensor. You do not need to configure this parameter, but only need to ensure that srcTensor conforms to the supported data type combinations for input and output. |
| ScaleT | Data type of the scaling factor scale. The API automatically derives the data type based on the input parameter scale. You do not need to configure this parameter. ScaleT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, scale is a scalar, and ScaleT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, scale is a vector, and ScaleT can only be a LocalTensor type. |
| OffsetT | Data type of offset. The API automatically derives the data type based on the input parameter offset. You do not need to configure this parameter. OffsetT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, offset is a scalar, and OffsetT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, offset can be a scalar or a vector, and OffsetT can be a scalar data type or a LocalTensor type. |

```
struct QuantizeConfig {
    QuantizePolicy policy;
    bool hasOffset;
    RoundMode roundMode = RoundMode::CAST_RINT;
    int32_t kDim = 1;
};
enum class QuantizePolicy : int32_t {
    PER_TENSOR,
    PER_CHANNEL,
    PER_TOKEN,
    PER_GROUP
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| scale | Input | Scaling factor used when quantizing the input data. |
| offset | Input | Offset used when quantizing the input data. For the float4 scenario of PER_GROUP quantization, offset does not take effect. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetQuantizeMaxMinTmpSize](GetQuantizeMaxMinTmpSize.md). |
| params | Input | Parameters of the quantize API. The type is QuantizeParams, and its specific definition is shown in the following code, where the meanings of the parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must meet the 32-byte alignment requirement, that is, the input or output whose last dimension of the shape is n must meet the 32-byte alignment requirement on that dimension.<br>groupSize: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct QuantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
```

The supported data type combinations for input and output are as follows:

**Table 3**  Data type combinations supported when DstT is fp8_e5m2_t/fp8_e4m3fn_t

| SrcT | ScaleT/OffsetT | roundMode |
| --- | --- | --- |
| half | half | CAST_RINT (default) |
| bfloat16_t | bfloat16_t | CAST_RINT (default) |
| float | float | CAST_RINT (default) |
| half | float | CAST_RINT (default) |
| bfloat16_t | float | CAST_RINT (default) |

**Table 4**  Data type combinations supported when DstT is hifloat8_t

| SrcT | ScaleT/OffsetT | roundMode |
| --- | --- | --- |
| half | half | CAST_ROUND (default)<br>CAST_HYBRID |
| bfloat16_t | bfloat16_t | CAST_ROUND (default)<br>CAST_HYBRID |
| float | float | CAST_ROUND (default)<br>CAST_HYBRID |
| half | float | CAST_ROUND (default)<br>CAST_HYBRID |
| bfloat16_t | float | CAST_ROUND (default)<br>CAST_HYBRID |

**Table 5** Supported data type combinations when DstT is int8_t
| SrcT | ScaleT/OffsetT | roundMode |
| --- | --- | --- |
| half | half | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| float | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |

**Table 6** Supported data type combinations when DstT is fp4x2_e1m2_t/fp4x2_e2m1_t (currently only the PER_GROUP scenario is supported)
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

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands involved in the computation must be 32-byte aligned.
-   The data volume in the continuous computation direction (that is, the n direction) must be 32-byte aligned.
-   The float4 scenario of PER_GROUP quantization does not support offset. In this scenario, the hasOffset parameter in the config template parameter must be set to false.

## Examples

-   PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP modes

    ```
    constexpr static QuantizePolicy tokenPolicy = QuantizePolicy::PER_TOKEN;
    constexpr static QuantizePolicy channelPolicy = QuantizePolicy::PER_CHANNEL;
    constexpr static QuantizePolicy groupPolicy = QuantizePolicy::PER_GROUP;
    // Take the PER_TOKEN mode as an example here, with offset enabled and the rounding mode set to CAST_ROUND. kDim is valid only in the PER_GROUP scenario and indicates that the group computation direction is the n direction.
    constexpr static QuantizeConfig config = {tokenPolicy, true, RoundMode::CAST_ROUND, 1};
    QuantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
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
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = 0; // Valid only in the PER_GROUP scenario.
    // dstLocal is a LocalTensor of the int8_t type, srcLocal is a LocalTensor of the half type, and scale and offset are scalars of the half type.
    Quantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

The results are as follows:

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
