# AntiQuantize

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T11:15:53.116Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs pseudo-quantization computation element-wise, for example, pseudo-quantizing the int8\_t data type to the half data type. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

**AntiQuantize** is similar in functionality to [AscendAntiQuant](AscendAntiQuant.md). This API provides a more unified form across different quantization scenarios, so it is recommended.

The pseudo-quantization strategies of this API include PER\_TENSOR, PER\_CHANNEL, PER\_TOKEN, and PER\_GROUP. The dequantization coefficients **scale** and **offset** are scalars in the PER\_TENSOR scenario and vectors in the other scenarios. The calculation formulas are as follows:

-   PER\_TENSOR scenario (per-tensor quantization): The shape of **scale** and **offset** is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000002268883076.png)

-   PER\_CHANNEL scenario (per-channel quantization): The shape of **srcTensor** is \[m, n\], each channel dimension corresponds to one quantization parameter, and the shape of **scale** and **offset** is \[1, n\].

    ![](../../../figures/zh-cn_formulaimage_0000002295275093.png)

-   PER\_TOKEN scenario (per-token quantization): The elements in each group of tokens of **srcTensor** (tokens are in the n direction, with m groups of tokens in total) share one quantization parameter. When the shape of **srcTensor** is \[m, n\], the shape of **scale** and **offset** is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002295277697.png)

-   PER\_GROUP scenario (group-wise quantization): The computation direction of the group is defined as the k direction, and every groupSize elements of srcTensor in the k direction share one set of scale and offset. When the shape of srcTensor is \[m, n\], if kDim=0, indicating that k is the m direction, the shape of scale and offset is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, indicating that k is the n direction, the shape of scale and offset is \[m, \(n + groupSize - 1\) / groupSize\].

    Depending on the input data type, PER\_GROUP is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the float4 scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the b8 scenario).

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (float4 scenario)
        -   k is in the m direction, that is, the i axis in the formula is the computation direction of the group (kDim=0):

            ![](../../../figures/zh-cn_formulaimage_0000002260652804.png)

        -   k is in the n direction, that is, the j axis in the formula is the computation direction of the group (kDim=1):

            ![](../../../figures/zh-cn_formulaimage_0000002260755858.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (b8 scenario)
        -   k is in the m direction, that is, the i axis in the formula is the computation direction of the group (kDim=0):

            ![](../../../figures/zh-cn_formulaimage_0000002295285957.png)

        -   k is the n direction, that is, in the formula the j axis is the computation direction of group (kDim=1):

            ![](../../../figures/zh-cn_formulaimage_0000002260755850.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void AntiQuantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const LocalTensor<uint8_t>& sharedTmpBuffer, const AntiQuantizeParams& params)
    ```

-   The API framework allocates temporary space.

    ```
    template <const AntiQuantizeConfig& config, typename DstT, typename SrcT, typename ScaleT, typename OffsetT>
    __aicore__ inline void AntiQuantize(const LocalTensor<DstT>& dstTensor, const LocalTensor<SrcT>& srcTensor, const ScaleT& scale, const OffsetT& offset, const AntiQuantizeParams& params)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the method of allocation by the API framework, you need to reserve temporary space. For the case of passing temporary space through **sharedTmpBuffer**, you need to allocate space for **sharedTmpBuffer**. The method for obtaining the temporary space size **BufferSize** is as follows: obtain the size of the space that needs to be reserved through the API provided in [GetAntiQuantizeMaxMinTmpSize](GetAntiQuantizeMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Used to configure pseudo-quantization related information. The type is **AntiQuantizeConfig**, and its specific definition is shown in the code below, where the meanings of the parameters are as follows.<br>**policy**: Used to configure the quantization strategy. It is an enumeration type, and its specific definition is shown in the code below.<br>**hasOffset**: Used to configure whether offset participates in the calculation. **true**: Indicates that offset participates in the calculation. **false**: Indicates that offset does not participate in the calculation.<br>**kDim**: The computation direction of group, that is, the k direction. It is valid only in the PER_GROUP scenario, and the supported values are as follows. **0**: The k axis is the 0th axis, that is, the m direction is the computation direction of group. **1**: The k axis is the 1st axis, that is, the n direction is the computation direction of group. |
| DstT | Data type of the destination operand. The data type is automatically derived within the API based on the input parameter **dstTensor**, so you do not need to configure this parameter. Ensure that **dstTensor** satisfies the supported input/output data type combinations in Table 3 and Table 4. |
| SrcT | Data type of the source operand. The data type is automatically derived within the API based on the input parameter **srcTensor**, so you do not need to configure this parameter. Ensure that **srcTensor** satisfies the supported input/output data type combinations in Table 3 and Table 4. |
| ScaleT | Data type of scale. The data type is automatically derived within the API based on the input parameter **scale**, so you do not need to configure this parameter. **ScaleT** can be a scalar data type or a **LocalTensor** type.<br><br>Note:<br>For the PER_TENSOR scenario, scale is a scalar, so **ScaleT** can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP scenarios, scale is a vector, so **ScaleT** can only be a **LocalTensor** type. |
| OffsetT | Data type of offset. The data type is automatically derived within the API based on the input parameter **offset**, so you do not need to configure this parameter. **OffsetT** can be a scalar data type or a **LocalTensor** type.<br><br>Note:<br>For the PER_TENSOR quantization strategy, offset is a scalar, so **OffsetT** can only be a scalar data type.<br>For the PER_CHANNEL, PER_TOKEN, and PER_GROUP quantization strategies, offset is a vector, so **OffsetT** can only be a **LocalTensor** type. |

```
struct AntiQuantizeConfig {
    AntiQuantizePolicy policy;
    bool hasOffset;
    int32_t kDim = 1;
};
enum class AntiQuantizePolicy : int32_t {
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
| scale | Input | Scaling factor for pseudo-quantization of the input data. |
| offset | Input | Offset for pseudo-quantization of the input data. For the float4 scenario of PER_GROUP quantization, offset does not take effect. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For the method of obtaining the temporary space size **BufferSize**, see [GetAntiQuantizeMaxMinTmpSize](GetAntiQuantizeMaxMinTmpSize.md). |
| params | Input | Parameters of the quantization API. The type is **AntiQuantizeParams**, and its specific definition is shown in the code below, where the meanings of the parameters are as follows.<br>**m**: Number of elements in the m direction.<br>**n**: Number of elements in the n direction. The data size corresponding to the n value must meet the 32-byte alignment requirement, that is, both the input and output whose last dimension of shape is n must meet the 32-byte alignment requirement on this dimension.<br>**groupSize**: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

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

**Table 4** Data type combinations supported by the PER_TOKEN/PER_GROUP quantization strategies

| SrcT | ScaleT/OffsetT |  DstT |
| --- | --- | --- |
| int8_t | half | half |
| int8_t | bfloat16_t | bfloat16_t |
| int8_t | float | float |
| int8_t | float | half |
| int8_t | float | bfloat16_t |
| hifloat8_t | half | half |
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

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands participating in the computation must be 32-byte aligned.
-   The data volume in the continuous computation direction (that is, the n direction) must be 32-byte aligned.
-   PER\_GROUP The quantized float4 scenario does not support offset. In this scenario, the **hasOffset** parameter in the template parameter **config** must be set to **false**.

## Examples

-   PER\_CHANNEL, PER\_TOKEN, PER\_GROUP modes

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
    params.groupSize = n; // Valid only in the PER_GROUP scenario. Here, it indicates that all elements in the n direction share one set of scale and offset.
    // srcLocal is a LocalTensor of int8_t type, and dstLocal, scale, and offset are LocalTensors of half type.
    AntiQuantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

-   PER\_TENSOR mode

    ```
    constexpr static AntiQuantizePolicy tensorPolicy = AntiQuantizePolicy::PER_TENSOR;
    // Enable offset.
    constexpr static AntiQuantizeConfig config = {tensorPolicy, true, -1};
    AntiQuantizeParams params;
    // m and n are externally passed parameters, indicating the number of elements of srcLocal that actually participate in the m and n directions.
    params.m = m;
    params.n = n;
    params.groupSize = 0; // Valid only in the PER_GROUP scenario.
    // srcLocal is a LocalTensor of int8_t type, dstLocal is a LocalTensor of half type, and scale and offset are scalars of half type.
    AntiQuantize<config>(dstLocal, srcLocal, scale, offset, params);
    ```

The results are as follows:

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
