# AscendAntiQuant

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:23:48.356Z pushedAt=2026-10-29T02:14:34.486Z nl=2 -->

## Applicable Products

<!-- npu="950" id11 -->
- Ascend 950PR&950DT products: Supported
<!-- end id11 -->
<!-- npu="A3" id12 -->
- Atlas A3 products: Supported
<!-- end id12 -->
<!-- npu="910b" id13 -->
- Atlas A2 products: Supported
<!-- end id13 -->
<!-- npu="310b" id14 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id14 -->
<!-- npu="310p" id15 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id15 -->
<!-- npu="910" id16 -->
- Atlas training products: Not supported
<!-- end id16 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/AscendAntiQuant_res.md#id1 -->

## Description

Performs pseudo-quantization computation element-wise. For example, it pseudo-quantizes the int8\_t data type to the half data type. The computation formula is as follows:

-   PER\_CHANNEL scenario (per-channel quantization)
    -   Input transpose disabled

        groupSize = src.shape\[0\] / offset.shape\[0\]

        **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* \(src\[i\]\[j\] + offset\[i / groupSize\]\[j\]\)**

    -   Input transpose enabled

        groupSize = src.shape\[1\] / offset.shape\[1\]

        **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* \(src\[i\]\[j\] + offset\[i\]\[j / groupSize\]\)**

-   PER\_TENSOR scenario (per-tensor quantization)

    **dst\[i\]\[j\] = scale \* \(src\[i\]\[j\] + offset\)**

-   PER\_TOKEN scenario (per-token quantization)

    ![](../../../figures/zh-cn_formulaimage_0000002069677288.png)

-   PER\_GROUP scenario (per-group quantization)

    Depending on the input data type, the current PER\_GROUP is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the float4 scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the b8 scenario).

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (float4 scenario)
        -   API with configurable groupSize

            The computation direction of a group is defined as the k direction. In the k direction, every groupSize elements of src share one set of scales. When the shape of src is \[m, n\], if kDim=0, k is the m direction, and the shape of scale is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, k is the n direction, and the shape of scale is \[m, \(n + groupSize - 1\) / groupSize\]. isTranspose=True indicates that src, scale, and dst are all transposed matrices.

            -   k is the m direction, that is, the i axis in the formula is the computation direction of the group: (kDim=0 and isTranspose=False) or (kDim=1 and isTranspose=True)

                **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* src\[i\]\[j\]**

            -   k is the n direction, that is, the j axis in the formula is the computation direction of the group: (kDim=0 and isTranspose=True) or (kDim=1 and isTranspose=False)

                **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* src\[i\]\[j\]**

        -   groupSize fixed to 32

            isTranspose=True indicates that src, scale, and dst are all transposed matrices.

            -   Input transpose disabled (isTranspose=False)

                **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* src\[i\]\[j\]**

            -   Enable input transpose (isTranspose=True)

                **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* src\[i\]\[j\]**

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (b8 scenario)

        Define the computation direction of the group as the k direction. In the k direction, every groupSize elements of src share one set of scale and offset. When the shape of src is \[m, n\], if kDim=0, it indicates that k is the m direction, and the shape of scale and offset is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, it indicates that k is the n direction, and the shape of scale and offset is \[m, \(n + groupSize - 1\) / groupSize\]. offset is an optional input. isTranspose=True indicates that src, scale, and dst are all transposed matrices.

        -   k is the m direction, that is, the i axis in the formula is the computation direction of the group: (kDim=0 and isTranspose=False) or (kDim=1 and isTranspose=True)

            ![](../../../figures/zh-cn_formulaimage_0000002106594713.png)

        -   k is the n direction, that is, the j axis in the formula is the computation direction of the group: (kDim=0 and isTranspose=True) or (kDim=1 and isTranspose=False)

            ![](../../../figures/zh-cn_formulaimage_0000002106474749.png)

## Implementation Principles

**Figure 1**  AscendAntiQuant algorithm block diagram  
![](../../../figures/ascendantiquant_diagram.png "AscendAntiQuant algorithm block diagram")

As shown in the preceding figure, this is the algorithm block diagram of a typical AscendAntiQuant scenario. The computation process consists of the following steps, all performed on the Vector:

1.  Precision conversion: Convert the input src to the half type.
2.  Compute offset: Perform an Add computation when offset is a vector, or an Adds computation when offset is a scalar.
3.  Compute scale: Perform a Mul computation when scale is a vector, or a Muls computation when scale is a scalar.

**Figure 2**  AscendAntiQuant algorithm block diagram when isTranspose is False and the output is bfloat16  
![](../../../figures/ascendantiquant_no_transpose_bfloat16.png "AscendAntiQuant algorithm block diagram when isTranspose is False and the output is bfloat16")

On Atlas A2 products, when the output is bfloat16, the computation process consists of the following steps:

1.  src precision conversion: Convert the input src to the half type, then to the float type, and store the result to tmp1.
2.  offset precision conversion: When the input offset is a vector, convert it to the float type and store the result to tmp2; when it is a scalar, perform a Cast to convert it to the float type.
3.  Compute the offset: when the input offset is a vector, perform an Add operation with tmp2; when it is a scalar, perform an Adds operation.
4.  Convert the scale precision: when the input scale is a vector, convert it to the float type and store it in tmp2; when it is a scalar, perform a Cast to convert it to the float type.
5.  Compute the scale: when the input scale is a vector, perform a Mul operation using tmp2; when it is a scalar, perform a Muls operation.
6.  Convert the dst precision: convert tmp1 to the bf16 type.

**Figure 3**  AscendAntiQuant PER\_TOKEN/PER\_GROUP algorithm block diagram  
![](../../../figures/ascendantiquant_per_token_per_group.png "AscendAntiQuant PER\_TOKEN/PER\_GROUP algorithm block diagram ")

The computation logic for the PER\_TOKEN/PER\_GROUP b8/float4 scenario is as follows:

1.  Read data: continuously read the input src; read the input scale and offset in different ways depending on the scenario; for example, the PER\_TOKEN scenario performs Broadcast processing, and the PER\_GROUP scenario performs Gather processing.
2.  Convert precision: convert the data types of src/scale/offset accordingly based on the data type combinations of different inputs.
3.  Compute: perform addition and multiplication operations on the type-converted data.
4.  Convert precision: convert the result obtained from the above addition and multiplication operations into the dstT type to obtain the final output.

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   PER\_CHANNEL Scenario (per-channel quantization)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_CHANNEL Scenario (per-channel quantization, without offset)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR Scenario (per-tensor quantization)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset, const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR Scenario (per-tensor quantization, without offset)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_GROUP float4 scenario (per-group quantization)

        <!-- npu="950" id34 -->
        Only Ascend 950PR&950DT products are supported.
        <!-- end id34 -->

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TOKEN/PER\_GROUP b8/float4 scenario (per-token quantization)/(per-group quantization)

        <!-- npu="950" id35 -->
        Only Ascend 950PR&950DT products are supported.
        <!-- end id35 -->

        ```
        template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config, const AscendAntiQuantPolicy& policy>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor,const LocalTensor<uint8_t>& sharedTmpBuffer, const AscendAntiQuantParam& para)
        ```

-   The API framework allocates the temporary space
    -   PER\_CHANNEL Scenario

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR Scenario

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset, const OutputDataType scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_GROUP float4 scenario (groupSize fixed at 32)

        <!-- npu="950" id36 -->
        Only Ascend 950PR&950DT products are supported.
        <!-- end id36 -->

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TOKEN/PER\_GROUP b8/float4 scenario (groupSize configurable)

        <!-- npu="950" id37 -->
        Only Ascend 950PR&950DT products are supported.
        <!-- end id37 -->

        ```
        template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config, const AscendAntiQuantPolicy& policy>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor,const AscendAntiQuantParam& para)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **allocated by the API framework** and **passed by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the workspace is allocated by the API framework, you need to reserve temporary space. When the workspace is passed in through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the API provided in [GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| InputDataType | Input data type. |
| OutputDataType | Output data type. |
| isTranspose | Whether to enable input data transpose. |

**Table 2**  Template parameter description for the PER\_TOKEN/PER\_GROUP b8/float4 scenario

| Parameter | Description |
| --- | --- |
| dstT | Data type of the destination operand. |
| srcT | Data type of the source operand. |
| scaleT | Data type of the scale factor parameter scale. |
| config | Quantization API configuration parameter, defined as shown in the following code, where the parameters have the following meanings.<br>hasOffset: Whether the quantization parameter offset participates in the computation. True: Indicates that the offset parameter participates in the computation. False: Indicates that the offset parameter does not participate in the computation.<br>isTranspose: Indicates whether to enable input src transpose. True: Indicates that input src is transposed. False: Indicates that input src is not transposed.<br>kDim: Computation direction of group, that is, the k direction. Valid only in the PER_GROUP scenario. Supported values are as follows. 0: The k axis is axis 0, that is, the m direction is the computation direction of group. 1: The k axis is axis 1, that is, the n direction is the computation direction of group. |
| policy | Quantization policy configuration parameter, of enumeration type. Supported values are as follows:<br>PER_TENSOR: Reserved parameter, not currently supported.<br>PER_CHANNEL: Reserved parameter, not currently supported.<br>PER_TOKEN: Configured as the PER_TOKEN scenario.<br>PER_GROUP: Configured as the PER_GROUP scenario.<br>PER_CHANNEL_PER_GROUP: Reserved parameter, not currently supported.<br>PER_TOKEN_PER_GROUP: Reserved parameter, not currently supported. |

```
struct AscendAntiQuantConfig {
    bool hasOffset;
    bool isTranspose;
    int32_t kDim = 1;
};
```

**Table 3**  PER\_TOKEN/PER\_GROUP Supported data type combinations for the b8/float4 scenario

| srcDtype | scaleDtype/offsetDtype | dstDtype |
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
| fp8_e5m2_t/fp8_e4m3fn_t | bfloat16_t | bfloat16_t |
| fp8_e5m2_t/fp8_e4m3fn_t | float | float |
| fp8_e5m2_t/fp8_e4m3fn_t | float | half |
| fp8_e5m2_t/fp8_e4m3fn_t | float | bfloat16_t |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently, both support only the PER_GROUP scenario.) | fp8_e8m0_t | half |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently, both support only the PER_GROUP scenario.) | fp8_e8m0_t | bfloat16_t |

**Table 4**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand. For the data types supported by different models, see [data types supported by dst](#li117871685514).<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| src | Input | Source operand. For the data types supported by different models, see [data types supported by src](#li13336025165520).<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| offset | Input | Offset used when pseudo-quantization the input data. For the data types supported by different models, see [data types supported by offset](#li5382122225718).<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| scale | Input | Scale factor used when pseudo-quantization the input data. For the data types supported by different models, see [data types supported by scale](#li103341450906).<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md). |
| k | Input | When isTranspose is true, the shape of src is [N,K]; when isTranspose is false, the shape of src is [K,N].<br><br>The parameter k corresponds to the K value. |
| shapeInfo | Input | Sets the shape information of the offset and scale parameters. It needs to be configured only in the PER_CHANNEL scenario (per-channel quantization).<br><br>Optional parameter. In the PER_CHANNEL scenario, if this parameter is not passed or the data in the structure is set to 0, the shape information of offset and scale is obtained from the [ShapeInfo](../../basic_api/aux_data_structures/ShapeInfo.md) of offset and scale.<br><br>Type: AntiQuantShapeInfo, defined as shown in the following code. |

```
struct AntiQuantShapeInfo {
    uint32_t offsetHeight{0}; // Height of offset.
    uint32_t offsetWidth{0};  // Width of offset.
    uint32_t scaleHeight{0};  // Height of scale.
    uint32_t scaleWidth{0};   // Width of scale.
};
```

**Table 5**  PER\_TOKEN/PER\_GROUP API parameter description for the b8/float4 scenario

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. Supported data types: half, bfloat16_t, float.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand. Supported data types: int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e1m2_t, fp4x2_e2m1_t. Note that the fp4x2_e1m2_t and fp4x2_e2m1_t data types are supported only in the PER_GROUP scenario.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer. Supported data type: uint8_t.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md). |
| scaleTensor | Input | Quantization parameter scale. Supported data types: half, float, bfloat16_t, fp8_e8m0_t.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| offsetTensor | Input | Quantization parameter offset. The supported data types are the same as those of scaleTensor. For the float4 scenario, offsetTensor does not take effect.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| para | Input | Parameters of the quantization API, of the AscendAntiQuantParam type, defined as shown in the following code. The meanings of the parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must meet the 32B alignment requirement, that is, both the input and output whose last shape dimension is n must meet the 32B alignment requirement on that dimension.<br>calCount: Number of elements involved in the computation. calCount must be an integer multiple of n.<br>groupSize: Valid in the PER_GROUP scenario. Indicates that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct AscendAntiQuantParam {
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
    uint32_t groupSize = 0;
};
```

## Return Value

None

## Constraints

-   **Address overlap between the source operands and the destination operand is not supported.**
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands involved in computation must be 32B-aligned.
-   When the input is transposed, k must be 32B-aligned.
-   Before calling the API, ensure that the size of the input data is correct and that the sizes and shapes of offset and scale are correct.

-   PER\_TOKEN/PER\_GROUP The support for the b8/float4 scenario is as follows:

    <!-- npu="950" id33 -->
    - Ascend 950PR&950DT products: Supported.
    <!-- end id33 -->

    <!-- npu="A3" id38 -->
    - Atlas A3 products: Not supported.
    <!-- end id38 -->

    <!-- npu="910b" id39 -->
    - Atlas A2 products: Not supported.
    <!-- end id39 -->

    <!-- npu="310p" id40 -->
    - Atlas inference products AI Core: Not supported.
    <!-- end id40 -->

-   In the PER\_TOKEN/PER\_GROUP b8/float4 scenario, the data volume in the continuous computation direction (that is, the n direction) must be 32B-aligned.
-   Data types supported by dst in non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios<a id="li117871685514"></a>

    <!-- npu="950" id17 -->
    - Ascend 950PR&950DT products support the following data types: half, bfloat16\_t.
    <!-- end id17 -->

    <!-- npu="A3" id18 -->
    - Atlas A3 products support the following data types: half, bfloat16\_t.
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    - Atlas A2 products support the following data types: half, bfloat16\_t.
    <!-- end id19 -->

    <!-- npu="310p" id20 -->
    - The Atlas inference products AI Core supports the following data types: half.
    <!-- end id20 -->

-   Data types supported by src in non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios<a id="li13336025165520"></a>

    <!-- npu="950" id21 -->
    - Ascend 950PR&950DT products support the following data types in PER\_CHANNEL and PER\_TENSOR scenarios: int8\_t, fp8\_e4m3fn\_t, fp8\_e5m2\_t, hifloat8\_t;  support the following data types in PER\_GROUP float4 scenarios: fp4x2\_e2m1\_t, fp4x2\_e1m2\_t.
    <!-- end id21 -->

    <!-- npu="A3" id22 -->
    - Atlas A3 products support the following data types: int8\_t, int4b\_t.
    <!-- end id22 -->

    <!-- npu="910b" id23 -->
    - Atlas A2 products support the following data types: int8\_t, int4b\_t.
    <!-- end id23 -->

    <!-- npu="310p" id24 -->
    - Atlas inference products AI Core support the following data types: int8\_t.
    <!-- end id24 -->

-   Data types supported by offset in non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios<a id="li5382122225718"></a>

    <!-- npu="950" id25 -->
    - Ascend 950PR&950DT products support the following data types: half, bfloat16\_t.
    <!-- end id25 -->

    <!-- npu="A3" id26 -->
    - Atlas A3 products support the following data types: half, bfloat16\_t.
    <!-- end id26 -->

    <!-- npu="910b" id27 -->
    - Atlas A2 products support the following data types: half, bfloat16\_t.
    <!-- end id27 -->

    <!-- npu="310p" id28 -->
    - Atlas inference products AI Core support the following data type: half.
    <!-- end id28 -->

-   Data types supported by scale in non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios<a id="li103341450906"></a>

    <!-- npu="950" id29 -->
    - Ascend 950PR&950DT products support the following data types in PER\_CHANNEL and PER\_TENSOR scenarios: half, bfloat16\_t; the data type supported in the PER\_GROUP float4 scenario is: fp8\_e8m0\_t.
    <!-- end id29 -->

    <!-- npu="A3" id30 -->
    - Atlas A3 products support the following data types: half, bfloat16\_t.
    <!-- end id30 -->

    <!-- npu="910b" id31 -->
    - Atlas A2 products support the following data types: half, bfloat16\_t.
    <!-- end id31 -->

    <!-- npu="310p" id32 -->
    - The Atlas inference products AI Core supports the following data type: half.
    <!-- end id32 -->

## Examples

```
// dstLocal: result tensor
// srcLocal: Quantized input
// offsetLocal: Offset parameter
// scaleLocal: Scale parameter
// sharedTmpBuffer: Temporary buffer managed by you, used to store intermediate variables during internal computation
// k: Length of the k axis
// shapeInfo: Shape information of the offsetLocal and scaleLocal tensors
AscendC::AntiQuantShapeInfo shapeInfo = {1, elementCountOfOffset, 1, elementCountOfOffset};
AscendC::AscendAntiQuant<InputType, OutType, false>(
    dstLocal, srcLocal, offsetLocal, scaleLocal, sharedTmpBuffer, k, shapeInfo);
```

The result is as follows:

```
Input data src (shape [2,64], non-transpose scenario):
[1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
offset (shape [1,64]):
[2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.
 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.
 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.]
scale (shape [1,64]):
[3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3.
 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3.
 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3. 3.]
Output data dstLocal (shape [2,64]):
[9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9. 9.
 9. 9. 9. 9. 9. 9. 9. 9.]
```

The following shows a call example for the b8 scenario in PER\_TOKEN/PER\_GROUP.

```
// Note that m and n must be passed in from outside.
constexpr static bool isReuseSource = false;
constexpr static AscendAntiQuantConfig config = {has_offset, has_transpose, -1};
constexpr static AscendAntiQuantPolicy policy = AscendAntiQuantPolicy::PER_TOKEN;
AscendAntiQuantParam para;
para.m = m;
para.n = n;
para.calCount = calCount;
AscendAntiQuant<dstType, srcType, scaleType, config, policy>(dstLocal, srcLocal, scaleLocal, offsetLocal, para);
```
