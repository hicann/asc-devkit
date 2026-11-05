# AntiQuantize

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:21:57.960Z pushedAt=2026-10-29T02:14:34.482Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/AntiQuantize_res.md#id1 -->

## Description

Performs pseudo-quantization computation element-wise, for example, pseudo-quantizing the int8\_t data type to the half data type. **This API supports at most two-dimensional input and does not support higher-dimensional input.**

AntiQuantize is similar in functionality to [AscendAntiQuant](AscendAntiQuant.md). This API provides a more unified form across different quantization scenarios, so it is recommended.

The pseudo-quantization strategies of this API include PER\_TENSOR, PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP. The pseudo-quantization coefficients scale and offset are scalars in the PER\_TENSOR scenario and vectors in the other scenarios. The computation formulas are as follows:

-   PER\_TENSOR scenario (per-tensor quantization): The shape of scale and offset is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000002268883076.png)

-   PER\_CHANNEL scenario (per-channel quantization): When the shape of srcTensor is \[m, n\], each channel dimension corresponds to one quantization parameter, and the shape of scale and offset is \[1, n\].

    ![](../../../figures/zh-cn_formulaimage_0000002295275093.png)

-   PER\_TOKEN scenario (per-token quantization): The elements in each token group of srcTensor (token is the n direction, with m token groups in total) share one quantization parameter. When the shape of srcTensor is \[m, n\], the shape of scale and offset is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002295277697.png)

-   PER\_GROUP Scenario (per-group quantization): The computation direction of the group is defined as the k direction, and every groupSize elements of srcTensor in the k direction share one set of scale and offset. When the shape of srcTensor is \[m, n\], if kDim=0, it indicates that k is the m direction, and the shape of scale and offset is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, it indicates that k is the n direction, and the shape of scale and offset is \[m, \(n + groupSize - 1\) / groupSize\].

    Depending on the input data type, PER\_GROUP is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the float4 scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the b8 scenario).

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (float4 scenario)
        -   k is the m direction, that is, the i axis in the formula is the computation direction of the group (kDim=0):

            ![](../../../figures/zh-cn_formulaimage_0000002260652804.png)

        -   k is the n direction, that is, the j axis in the formula is the computation direction of the group (kDim=1):

            ![](../../../figures/zh-cn_formulaimage_0000002260755858.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (b8 scenario)
        -   k is the m direction, that is, the i axis in the formula is the computation direction of the group (kDim=0):

            ![](../../../figures/zh-cn_formulaimage_0000002295285957.png)

        -   k is the n direction, that is, in the formula the j axis is the computation direction of group (kDim=1):

            ![](../../../figures/zh-cn_formulaimage_0000002260755850.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void AntiQuantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
    ```

-   The API framework allocates the temporary space

    ```
    template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void AntiQuantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const AntiQuantizeParams& params)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **allocation by the API framework** and **passing through the sharedTmpBuffer input parameter** by you.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the API framework allocation method, you need to reserve temporary space. For the case of passing through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: obtain the range size of the space to be reserved through the API provided in [GetAntiQuantizeMaxMinTmpSize](GetAntiQuantizeMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Used for configuration of pseudo-quantization related information. The type is AntiQuantizeConfig, and its specific definition is shown in the following code, where the meanings of the parameters are as follows.<br>policy: Used for configuration of the quantization strategy. It is an enumeration type, and its specific definition is shown in the following code.<br>hasOffset: Used for configuration of whether offset participates in the computation. true: indicates that offset participates in the computation. false: indicates that offset does not participate in the computation.<br>kDim: The computation direction of group, that is, the k direction. It is valid only in the PER_GROUP scenario, and the supported values are as follows. 0: The k axis is axis 0, that is, the m direction is the computation direction of group. 1: The k axis is axis 1, that is, the n direction is the computation direction of group. |
| DstT | Data type of the destination operand. The API automatically derives the data type based on the dstTensor input parameter. You do not need to configure this parameter; just ensure that dstTensor satisfies the supported input/output data type combinations in Table 3 and Table 4. |
| SrcT | Data type of the source operand. The API automatically derives the data type based on the srcTensor input parameter. You do not need to configure this parameter; just ensure that srcTensor satisfies the supported input/output data type combinations in Table 3 and Table 4. |
| ScaleT | Data type of scale. The API automatically derives the data type based on the scale input parameter. You do not need to configure this parameter. ScaleT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR scenario, scale is a scalar, and ScaleT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP scenarios, scale is a vector, and ScaleT can only be a LocalTensor type. |
| OffsetT | Data type of offset. The API automatically derives the data type based on the offset input parameter. You do not need to configure this parameter. OffsetT can be a scalar data type or a LocalTensor type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, offset is a scalar, and OffsetT can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, offset is a vector, and OffsetT can only be a LocalTensor type. |

```
struct AntiQuantizeConfig {
    AntiQuantizePolicy policy;
    bool hasOffset;
    int32_t kDim = 1;
};
enum class AntiQuantizePolicy : int32_t { PER_TENSOR, PER_CHANNEL, PER_TOKEN, PER_GROUP };
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| scale | Input | Scaling factor for pseudo-quantization of the input data. |
| offset | Input | Offset for pseudo-quantization of the input data. For the float4 scenario of PER_GROUP quantization, offset does not take effect. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAntiQuantizeMaxMinTmpSize](GetAntiQuantizeMaxMinTmpSize.md). |
| params | Input | Parameters of the quantization API. The type is AntiQuantizeParams, and its specific definition is shown in the following code, where the meanings of the parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must satisfy the 32-byte alignment requirement, that is, the input or output whose last dimension of the shape is n must satisfy the 32-byte alignment requirement on that dimension.<br>groupSize: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct AntiQuantizeParams {
    uint32_t m;
    uint32_t n;
    uint32_t groupSize = 0;
};
```

The supported input/output data type combinations are as follows:

**Table 3**  Data type combinations supported by the PER_TENSOR/PER_CHANNEL quantization strategies

| SrcT | ScaleT/OffsetT |  DstT |
| --- | --- | --- |
| fp8_e4m3fn_t | half | half |
| fp8_e5m2_t | half | half |
| hifloat8_t | half | half |
| int8_t | half | half |
| fp8_e4m3fn_t | bfloat16_t | bfloat16_t |
| fp8_e5m2_t | bfloat16_t | bfloat16_t |
| hifloat8_t | bfloat16_t | bfloat16_t |
| int8_t | bfloat16_t | bfloat16_t |

**Table 4**  Data type combinations supported by the PER_TOKEN/PER_GROUP quantization strategies

| SrcT | ScaleT/OffsetT |  DstT |
| --- | --- | --- |
| int8_t | half | half |
| int8_t | bfloat16_t | bfloat16_t |
| int8_t | float | float |
| int8_t | float | half |
| int8_t | float | bfloat16_t |
| hifloat8_t | half | half |  |
| hifloat8_t | bfloat16_t | bfloat16_t |
| hifloat8_t | float | float |
| hifloat8_t | float | half |
| hifloat8_t | float | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t | half | half |
| fp8_e5m2_t/fp8_e4m3fn_t| bfloat16_t | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t| float | float |
| fp8_e5m2_t/fp8_e4m3fn_t| float | half |
| fp8_e5m2_t/fp8_e4m3fn_t| float | bfloat16_t |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br>(currently both support only the PER_GROUP scenario) | fp8_e8m0_t | half |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br>(currently both support only the PER_GROUP scenario) | fp8_e8m0_t | bfloat16_t |

## Return Value

None

## Constraints

-   **Address overlap between the source operands and the destination operand is not supported.**
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands involved in the computation must be 32-byte aligned.
-   The data volume in the continuous computation direction (that is, the n direction) must be 32-byte aligned.
-   The float4 scenario of PER_GROUP quantization does not support offset. In this scenario, the hasOffset parameter in the template parameter config must be set to false.

## Examples

-   PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP modes

    ```
    constexpr static AntiQuantizePolicy tokenPolicy = AntiQuantizePolicy::PER_TOKEN;
    constexpr static AntiQuantizePolicy channelPolicy = AntiQuantizePolicy::PER_CHANNEL;
    constexpr static AntiQuantizePolicy groupPolicy = AntiQuantizePolicy::PER_GROUP;
    // Here, the PER_TOKEN mode is used as an example, with offset enabled. kDim is valid only in the PER_GROUP scenario and indicates that the group computation direction is the n direction.
    constexpr static AntiQuantizeConfig config = {tokenPolicy, true, 1};
    AntiQuantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = n; // Valid only in the PER_GROUP scenario. Here it indicates that all elements in the n direction share one set of scale and offset.
    // srcLocal is a LocalTensor of the int8_t type, and dstLocal, scale, and offset are LocalTensors of the half type.
    AntiQuantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

-   PER\_TENSOR mode

    ```
    constexpr static AntiQuantizePolicy tensorPolicy = AntiQuantizePolicy::PER_TENSOR;
    // Enable offset.
    constexpr static AntiQuantizeConfig config = {tensorPolicy, true, -1};
    AntiQuantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements in the m and n directions that srcLocal actually participates in.
    params.m = m;
    params.n = n;
    params.groupSize = 0; // Valid only in the PER_GROUP scenario.
    // srcLocal is a LocalTensor of the int8_t type, dstLocal is a LocalTensor of the half type, and scale and offset are scalars of the half type.
    AntiQuantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

The result is as follows:

```
Input data (srcLocal):
[-4, 2, -2, -3, -1, -4, 1, 3, 4, 1, -2, 0, ... 1]
Input data (scale vector):
[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, ... 1]
Input data (scale scalar):
[1]
Input data (offset vector):
[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ... 0]
Input data (offset scalar):
[0]
Output data (dstLocal), where dstLocal = srcLocal:
[-4, 2, -2, -3, -1, -4, 1, 3, 4, 1, -2, 0, ... 1]
```
