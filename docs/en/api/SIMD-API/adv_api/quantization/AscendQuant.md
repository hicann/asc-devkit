# AscendQuant

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:28:22.960Z pushedAt=2026-10-29T02:14:34.498Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/AscendQuant_res.md#id1 -->

## Description

Performs element-wise quantization, for example, quantizing half/float data types to the int8\_t data type. The computation formula is as follows, where round indicates round-half-to-even rounding and cast indicates the rounding mode:

-   PER\_TENSOR quantization: The entire srcTensor corresponds to one quantization parameter, whose shape is \[1\].

    ![](../../../figures/zh-cn_formulaimage_0000001666705128.png)

-   PER\_CHANNEL quantization: The shape of srcTensor is \[m, n\]. Each channel dimension corresponds to one quantization parameter, whose shape is \[n\].

    ![](../../../figures/zh-cn_formulaimage_0000001738671878.png)

-   PER\_TOKEN quantization: The elements in each token group of srcTensor (a token is in the n direction, and there are m token groups in total) share a set of scale and offset parameters. When the shape of srcTensor is \[m, n\], the shapes of scale and offset are \[m, 1\]. offset is an optional input.

    ![](../../../figures/zh-cn_formulaimage_0000002292403429.png)
-   PER\_GROUP quantization: The computation direction of a group is defined as the k direction. In srcTensor, every groupSize elements along the k direction share a set of scale and offset. When the shape of srcTensor is \[m, n\], if kDim=0, indicating that k is the m direction, the shapes of scale and offset are \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, indicating that k is the n direction, the shapes of scale and offset are \[m, \(n + groupSize - 1\) / groupSize\]. offset is an optional input.

    Depending on the output data type, the current PER\_GROUP is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the float4 scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the b8 scenario).

    -   fp4x2\_e2m1\_t/float4\_e1m2 scenario (float4 scenario)
        -   kDim = 0:

            ![](../../../figures/zh-cn_formulaimage_0000002292493829.png)

        -   kDim = 1:

            ![](../../../figures/zh-cn_formulaimage_0000002292493825.png)

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (b8 scenario)
        -   kDim=0:

            ![](../../../figures/zh-cn_formulaimage_0000002292493833.png)

        -   kDim=1:

            ![](../../../figures/zh-cn_formulaimage_0000002068601638.png)

## Implementation Principles

**Figure 1**  AscendQuant algorithm block diagram where scale and offset are both scalars  
![](../../../figures/ascendquant_scale_offset_scalar.png "AscendQuant algorithm block diagram where scale and offset are both scalars")

**Figure 2**  AscendQuant algorithm block diagram where scale and offset are both tensors  
![](../../../figures/ascendquant_scale_offset_tensor.png "AscendQuant algorithm block diagram where scale and offset are both tensors")

**Figure 3**  AscendQuant algorithm block diagram where scale is a tensor and offset is a scalar  
![](../../../figures/ascendquant_scale_tensor_offset_scalar.png "AscendQuant algorithm block diagram where scale is a tensor and offset is a scalar")

The preceding figures show the internal algorithm block diagram of AscendQuant. The computation process is roughly described in the following steps, all of which are performed on the Vector:

1.  Precision conversion: When the input src, scale, or offset is of the float type, convert it to the half type;
2.  Broadcast: When the input scale or offset is a vector, broadcast it to the same dimensions as src;
3.  Compute scale: When src and scale are vectors, perform a Mul computation; when scale is a scalar, perform a Muls computation to obtain Tmp1;
4.  Compute offset: When Tmp1 and offset are vectors, perform an Add computation; when offset is a scalar, perform an Adds computation to obtain Tmp2;
5.  Precision conversion: Convert Tmp2 from half to the int8\_t type to obtain output.

**Figure 4**  AscendQuant algorithm block diagram PER\_TOKEN/PER\_GROUP scenario where both scale and offset are tensors  
![](../../../figures/ascendquant_per_token_per_group_scale_offset_tensor.png "AscendQuant algorithm block diagram PER_TOKEN-PER_GROUP scenario where both scale and offset are tensors")

**Figure 5**  AscendQuant algorithm block diagram PER\_TOKEN/PER\_GROUP scenario where scale is a tensor and offset is a scalar  
![](../../../figures/ascendquant_per_token_per_group_scale_tensor_offset_scalar.png "AscendQuant algorithm block diagram PER_TOKEN-PER_GROUP scenario where scale is a tensor and offset is a scalar")

The computation logic for the PER\_TOKEN/PER\_GROUP scenario is as follows:

1.  Read data: Continuously read the input src. Depending on the scenario, read the input scale and offset in different ways. For example, the PER\_TOKEN scenario performs Broadcast processing, and the PER\_GROUP scenario performs Gather processing.
2.  Precision conversion: Convert the data types of src/scale/offset according to the data type combinations of different inputs.
3.  Computation: Perform multiply-add operations on the type-converted data.
4.  Precision conversion: Convert the computation result obtained from the multiply-add operations into the dstT type to obtain the final output.

## Prototype

-   dstTensor is of the int8\_t data type
    -   PER\_TENSOR quantization:
        -   Pass the temporary space through the sharedTmpBuffer input parameter
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset)
                ```

        -   The API framework allocates the temporary space
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const float scale, const float offset, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const float scale, const float offset)
                ```

    -   PER\_CHANNEL quantization:
        -   Pass the temporary space through the sharedTmpBuffer input parameter
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const T offset, const uint32_t scaleCount, const uint32_t calCount)
                ```

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const T offset)
                ```

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor)
                ```

        -   The API framework allocates the temporary space
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& scaleTensor, const T offset, const uint32_t scaleCount, const uint32_t calCount)
                ```

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& scaleTensor, const T offset)
                ```

                ```
                template <typename T, bool isReuseSource = false, const AscendQuantConfig& config = ASCEND_QUANT_DEFAULT_CFG>
                __aicore__ inline void AscendQuant(const LocalTensor<int8_t>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& scaleTensor, const LocalTensor<T>& offsetTensor)
                ```

-   dstTensor is not a fixed data type.

    <!-- npu="950" id9 -->Supported only on Ascend 950PR&950DT products.<!-- end id9 -->

    -   PER\_TENSOR Quantization:
        -   Pass the temporary space through the sharedTmpBuffer input parameter
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const float scale, const float offset)
                ```

        -   The API framework allocates the temporary space
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const float scale, const float offset, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const float scale, const float offset)
                ```

    -   PER\_CHANNEL Quantization:
        -   Pass the temporary space through the sharedTmpBuffer input parameter
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<srcT>& scaleTensor, const srcT offset, const uint32_t scaleCount, const uint32_t calCount)
                ```

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<srcT>& scaleTensor, const LocalTensor<srcT>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<srcT>& scaleTensor, const srcT offset)
                ```

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<srcT>& scaleTensor, const LocalTensor<srcT>& offsetTensor)
                ```

        -   The API framework allocates the temporary space
            -   All or part of the source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor, const srcT offset, const uint32_t scaleCount, const uint32_t calCount)
                ```

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor, const LocalTensor<srcT>& offsetTensor, const uint32_t scaleCount, const uint32_t offsetCount, const uint32_t calCount)
                ```

            -   The entire source operand tensor participates in the computation

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor, const srcT offset)
                ```

                ```
                template <typename dstT, typename srcT, bool isReuseSource = false>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<srcT>& scaleTensor, const LocalTensor<srcT>& offsetTensor)
                ```

    -   PER\_TOKEN/PER\_GROUP Quantization:
        -   Pass the temporary space through the sharedTmpBuffer input parameter
            -   The offset operand is of the Tensor type.

                ```
                template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config, const AscendQuantPolicy& policy>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
                ```

            -   The offset operand is of the scalar type.

                ```
                template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config, const AscendQuantPolicy& policy>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const LocalTensor<scaleT>& scaleTensor,const scaleT offset, const AscendQuantParam& para)
                ```

        -   The API framework allocates the temporary space
            -   The offset operand is of the Tensor type.

                ```
                template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config, const AscendQuantPolicy& policy>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendQuantParam& para)
                ```

            -   The offset operand is of the scalar type.

                ```
                template <typename dstT, typename srcT, typename scaleT, bool isReuseSource = false, const AscendQuantConfig& config, const AscendQuantPolicy& policy>
                __aicore__ inline void AscendQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const scaleT offset, const AscendQuantParam& para)
                ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be obtained in two ways: **allocated by the API framework** or **passed in by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the framework allocates the space, you need to reserve temporary space; when the space is passed through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: call the GetAscendQuantMaxMinTmpSize API provided in [GetAscendQuantMaxMinTmpSize](GetAscendQuantMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

Note that in the PER\_TOKEN/PER\_GROUP quantization scenario, the internal implementation does not require the temporary space Buffer, and sharedTmpBuffer in the corresponding API is a reserved parameter.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| config | Struct template parameter. This parameter is optional and of the AscendQuantConfig type, defined in the following code. The meanings of its parameters are as follows.<br>calcCount: Number of data elements actually computed. calcCount∈[0, srcTensor.GetSize()]. When calling an API with the scaleCount input parameter, if calcCount is set to a non-zero value, it must be an integer multiple of scaleCount.<br>offsetCount: Number of actual quantization parameter elements. offsetCount∈[0, offsetTensor.GetSize()]. offsetCount and scaleCount must be equal and must be an integer multiple of 32. If the called API does not include the offsetCount input parameter, set it to 0.<br>scaleCount: Number of actual quantization parameter elements. scaleCount∈[0, scaleTensor.GetSize()] and must be an integer multiple of 32. If the called API does not include the scaleCount input parameter, set it to 0.<br>workLocalSize: Size of the temporary buffer sharedTmpBuffer. For how to obtain the size of sharedTmpBuffer/workLocalSize, see [GetAscendQuantMaxMinTmpSize](GetAscendQuantMaxMinTmpSize.md). The value of this parameter cannot exceed the size of sharedTmpBuffer. If the called API does not include the sharedTmpBuffer input parameter, set it to 0.<br><br>When the values of the preceding parameters meet any of the following scenarios, parameter constantization is enabled, that is, the constantized parameters are used during compilation to reduce Scalar computation.<br>If the called API does not include the scaleCount input parameter, parameter constantization is enabled when calcCount and workLocalSize are set to non-zero values.<br>If the called API includes the scaleCount input parameter, parameter constantization is enabled when scaleCount, calcCount, and workLocalSize are set to non-zero values. |

```
struct AscendQuantConfig {
    uint32_t calcCount = 0;
    uint32_t offsetCount = 0;
    uint32_t scaleCount = 0;
    uint32_t workLocalSize = 0;
};
```

The following shows an example of configuring the default parameters.

```
constexpr AscendQuantConfig ASCEND_QUANT_DEFAULT_CFG = {0, 0, 0, 0};
```

**Table 2** Template parameter description for dstTensor with a non-fixed data type

| Parameter | Description |
| --- | --- |
| dstT | Data type of the destination operand. Supported data types: int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e1m2_t, and fp4x2_e2m1_t. Note that the fp4x2_e1m2_t and fp4x2_e2m1_t data types are supported only in the PER_GROUP scenario. |
| srcT | Data type of the source operand. Supported data types: half, bfloat16_t, and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 3** PER\_TOKEN/PER\_GROUP Scenario-specific template parameter description

| Parameter | Description |
| --- | --- |
| scaleT | Data type of the quantization parameters scale and offset. Supported data types: half, bfloat16_t, and float. |
| config | Quantization API configuration parameter, of the AscendQuantConfig type, defined in the following code. The meanings of its parameters are as follows.<br>hasOffset: Whether the quantization parameter offset participates in the computation. True: the offset parameter participates in the computation. False: the offset parameter does not participate in the computation.<br>kDim: Computation direction of the group, that is, the k direction. This parameter is valid only in the PER_GROUP scenario. Supported values: 0: the k axis is axis 0, that is, the m direction is the computation direction of the group; 1: the k axis is axis 1, that is, the n direction is the computation direction of the group.<br>roundMode: Rounding mode used when data is converted from a high-precision data type to a low-precision data type during quantization. Supported values: CAST_NONE, CAST_RINT, CAST_ROUND, CAST_FLOOR, CAST_CEIL, CAST_TRUNC, and CAST_HYBRID. For details about each rounding mode, see [precision conversion rules](../../../SIMD-API/basic_api/data_structures/precision_conversion.md#tab1). Different data types support different rounding modes during quantization. When an unsupported rounding mode is used during quantization, the default rounding mode is used instead. For example, when the bfloat16_t data type is quantized to the hifloat8_t data type, if roundMode is set to the unsupported CAST_RINT, the default roundMode (CAST_ROUND) is used during actual quantization. See Table 4 for the rounding modes supported by different data types. |
| policy | Quantization policy configuration parameter, of the AscendQuantPolicy enum type. Supported values:<br>PER_TENSOR: Reserved parameter, not supported yet.<br>PER_CHANNEL: Reserved parameter, not supported yet.<br>PER_TOKEN: Configures the PER_TOKEN scenario.<br>PER_GROUP: Configures the PER_GROUP scenario.<br>PER_CHANNEL_PER_GROUP: Reserved parameter, not supported yet.<br>PER_TOKEN_PER_GROUP: Reserved parameter, not supported yet. |

```
struct AscendQuantConfig {
    bool hasOffset;
    int32_t kDim = 1;
    RoundMode roundMode = RoundMode::CAST_RINT;
};
```

**Table 4** Data type combinations supported in PER\_TOKEN/PER\_GROUP scenarios

| srcDtype | scaleDtype/offsetDtype | dstDtype | roundMode |
| --- | --- | --- | --- |
| half | half | fp8_e5m2_t/fp8_e4m3fn_t | CAST_RINT (default) |
| bfloat16_t | bfloat16_t | fp8_e5m2_t/fp8_e4m3fn_t | CAST_RINT (default) |
| float | float | fp8_e5m2_t/fp8_e4m3fn_t | CAST_RINT (default) |
| half | float | fp8_e5m2_t/fp8_e4m3fn_t | CAST_RINT (default) |
| bfloat16_t | float | fp8_e5m2_t/fp8_e4m3fn_t | CAST_RINT (default) |
| half | half | hifloat8_t | CAST_ROUND (default)<br>CAST_HYBRID |
| bfloat16_t | bfloat16_t | hifloat8_t | CAST_ROUND (default)<br>CAST_HYBRID |
| float | float | hifloat8_t | CAST_ROUND (default)<br>CAST_HYBRID |
| half | float | hifloat8_t | CAST_ROUND (default)<br>CAST_HYBRID |
| bfloat16_t | float | hifloat8_t | CAST_ROUND (default)<br>CAST_HYBRID |
| half | half | int8_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | int8_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| float | float | int8_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | int8_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | int8_t | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | half | fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently both support only the PER_GROUP scenario) | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| half | float | fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently both support only the PER_GROUP scenario) | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | bfloat16_t | fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently both support only the PER_GROUP scenario) | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |
| bfloat16_t | float | fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently both support only the PER_GROUP scenario) | CAST_RINT (default)<br>CAST_ROUND<br>CAST_FLOOR<br>CAST_CEIL<br>CAST_TRUNC |

**Table 5**  PER\_TENSOR API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendQuantMaxMinTmpSize](GetAscendQuantMaxMinTmpSize.md). |
| scale | Input | Quantization parameter.<br><br>The type is Scalar, and the supported data type is float. |
| offset | Input | Quantization parameter.<br><br>The type is Scalar, and the supported data type is float. |
| calCount | Input | Number of elements involved in the computation. |

**Table 6**  PER\_CHANNEL API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendQuantMaxMinTmpSize](GetAscendQuantMaxMinTmpSize.md). |
| scaleTensor | Input | Quantization parameter.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| offsetTensor | Input | Quantization parameter.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| scaleCount | Input | Actual number of quantization parameter elements, where scaleCount ∈ [0, min(scaleTensor.GetSize(), dstTensor.GetSize())], and it must be a multiple of 32. |
| offsetCount | Input | Actual number of quantization parameter elements, where offsetCount ∈ [0, min(offsetTensor.GetSize(), dstTensor.GetSize())], and it must be equal to scaleCount and a multiple of 32. |
| calCount | Input | Number of elements involved in the computation. calCount must be a multiple of scaleCount. |

**Table 7**  PER\_TOKEN/PER\_GROUP API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendQuantMaxMinTmpSize](GetAscendQuantMaxMinTmpSize.md). |
| scaleTensor | Input | Quantization parameter scale.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| offsetTensor/offset | Input | Quantization parameter offset.<br>offsetTensor: The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>offset: The type is Scalar.<br><br>The data type is consistent with scaleTensor. For the float4 scenario, offsetTensor/offset does not take effect. |
| para | Input | Parameters of the quantization API, of the AscendQuantParam type, defined as shown in the following code, where the meanings of the parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must meet the 32B alignment requirement, that is, both the input and output whose last dimension of the shape is n must meet the 32B alignment requirement on that dimension.<br>calCount: Number of elements involved in the computation. calCount must be a multiple of n.<br>groupSize: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and a multiple of 32. |

```
struct AscendQuantParam {
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
    uint32_t groupSize = 0;
};
```

## Return Value

None

## Constraints

-   The source operand and the destination operand can be reused.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands involved in the computation must meet the 32B alignment requirement.
-   When Scale is of the float type, its value range is still the value range of the half type.
-   The support status of the function prototypes with non-fixed data types for dstTensor is as follows:
    <!-- npu="950" id12 -->
    - Ascend 950PR&950DT products: Supported.
    <!-- end id12 -->
    <!-- npu="A3" id13 -->
    - Atlas A3 products: Not supported.
    <!-- end id13 -->
    <!-- npu="910b" id14 -->
    - Atlas A2 products: Not supported.
    <!-- end id14 -->
    <!-- npu="310p" id15 -->
    - Atlas inference products AI Core: Not supported.
    <!-- end id15 -->
    <!-- npu="910" id16 -->
    - Atlas training products: Not supported.
    <!-- end id16 -->
<!-- npu="910" id10 -->
-   Atlas training products support only PER\_TENSOR quantization, and do not support PER\_CHANNEL quantization, PER\_TOKEN quantization, or PER\_GROUP quantization.
<!-- end id10 -->

-   The support status of the PER\_TOKEN/PER\_GROUP quantization scenario is as follows:

    <!-- npu="950" id11 -->
    - Ascend 950PR&950DT products: Supported.
    <!-- end id11 -->

    <!-- npu="A3" id17 -->
    - Atlas A3 products: Not supported.
    <!-- end id17 -->

    <!-- npu="910b" id18 -->
    - Atlas A2 products: Not supported.
    <!-- end id18 -->

    <!-- npu="310p" id19 -->
    - Atlas inference products AI Core: Not supported.
    <!-- end id19 -->

    <!-- npu="910" id20 -->
    - Atlas training products: Not supported.
    <!-- end id20 -->

-   PER\_TOKEN/PER\_GROUP scenario, the data volume in the continuous computation direction (the n direction) must be 32B-aligned.

## Examples

-   The following is a call example in the PER\_TENSOR quantization scenario:

    ```
    // dstLocal: Tensor that stores the quantization computation result, with a shape of 1024
    // srcLocal: Tensor that stores the quantization computation input, with a shape of 1024 and a type of float/half
    // sharedTmpBuffer: Tensor that stores the temporary buffer during quantization computation

    const float scale = 0.02; // Quantization parameter
    const float offset = 0.9; // Quantization parameter, dstLocal[i] = srcLocal[i] * scale + offset
    uint32_t calCount = 1022; // The first calCount elements of srcTensor participate in the computation.

    // dstTensor is of the int8_t data type. Pass temporary space through the sharedTmpBuffer input parameter.
    AscendC::AscendQuant<srcType>(dstLocal, srcLocal, sharedTmpBuffer, scale, offset, calCount);

    // dstTensor is not of a fixed data type.
    AscendC::AscendQuant<dstType, srcType>(dstLocal, srcLocal, scale, offset, calCount);
    ```

    The result is as follows:

    ```
    Input data (srcLocal): [-512. -511. -510. ...  509.  510.  511.]
    Input quantization parameter (scale): 0.02
    Input quantization parameter (offset): 0.9
    Output data (dstLocal): [-9 -9 -9 ... 11 51.   51.1]
    ```

-   The following is a call example in the PER_CHANNEL quantization scenario:

    ```
    // dstLocal: Tensor that stores the quantization computation result, with a shape of 1024.
    // srcLocal: Tensor that stores the input for the quantization computation, with a shape of 1024 and a type of float/half.
    // scaleLocal: input tensor that stores quantization parameters.
    // offsetLocal: input tensor that stores quantization parameters.
    // sharedTmpBuffer: tensor that stores temporary buffers during quantization computation.

    uint32_t scaleCount = 64;  // Quantization parameter, which must be a multiple of 32.
    uint32_t offsetCount = 64; // Quantization parameter, which must be equal to scaleCount.
    uint32_t calCount = 1022;  // The first calCount elements of srcTensor participate in the computation.

    // dstTensor is of the int8_t data type. Pass the temporary space through the sharedTmpBuffer input parameter.
    AscendC::AscendQuant<srcType>(
        dstLocal, srcLocal, sharedTmpBuffer, scaleLocal, offsetLocal, scaleCount, offsetCount, calCount);

    // dstTensor is of a non-fixed data type.
    AscendC::AscendQuant<dstType, srcType>(dstLocal, srcLocal, scaleLocal, offsetLocal, scaleCount, offsetCount, calCount);
    ```

    The result is as follows:

    ```
    Input data (srcLocal): [-512. -511. -510. ...  509.  510.  511.]
    Input quantization parameter (scale): [0.02 0.02 0.02 ... 0.02]
    Input quantization parameter (offset): [1.01 1.02 1.03 ... 1.32]
    Output data (dstLocal): [-9 -9 -9 ... 11 510.  511.]
    ```

The following is a call example in the PER_TOKEN/PER_GROUP scenario:

-   If the rounding mode `roundMode` of the AscendQuantConfig parameter is not configured, the default configuration `RoundMode::CAST\_RINT` is used.

    ```
    // Note that m and n must be passed in from outside.
    constexpr static bool isReuseSource = false;
    constexpr static AscendQuantConfig config = {has_offset, 1};
    constexpr static AscendQuantPolicy policy = AscendQuantPolicy::PER_TOKEN; // Modify the enum value to enable PER_GROUP.
    LocalTensor<uint8_t> sharedTmpBuffer = inQueue.AllocTensor<uint8_t>();
    AscendQuantParam para;
    para.m = m;
    para.n = n;
    para.calCount = calCount;
    AscendQuant<dstType, srcType, scaleType, isReuseSource, config, policy>(
        dstLocal, srcLocal, sharedTmpBuffer, scaleLocal, offsetLocal, para);
    ```

-   Actively configure the rounding mode `roundMode` of the `AscendQuantConfig` parameter.

    ```
    // Note that m and n must be passed in from outside.
    constexpr static bool isReuseSource = false;
    constexpr static AscendQuantConfig config = {has_offset, 1, RoundMode::CAST_ROUND};
    constexpr static AscendQuantPolicy policy = AscendQuantPolicy::PER_TOKEN; // Modify the enum value to enable PER_GROUP.
    LocalTensor<uint8_t> sharedTmpBuffer = inQueue.AllocTensor<uint8_t>();
    AscendQuantParam para;
    para.m = m;
    para.n = n;
    para.calCount = calCount;
    AscendQuant<dstType, srcType, scaleType, isReuseSource, config, policy>(
        dstLocal, srcLocal, sharedTmpBuffer, scaleLocal, offsetLocal, para);
    ```
