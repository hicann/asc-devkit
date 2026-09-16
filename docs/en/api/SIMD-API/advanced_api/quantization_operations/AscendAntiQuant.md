# AscendAntiQuant

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T11:21:35.846Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs pseudo-quantization computation element by element. For example, pseudo-quantizes the int8\_t data type to the half data type. The computation formula is as follows:

-   PER\_CHANNEL scenario (Per-Channel Quantization)
    -   Disable input transpose

        groupSize = src.shape\[0\] / offset.shape\[0\]

        **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* \(src\[i\]\[j\] + offset\[i / groupSize\]\[j\]\)**

    -   Enable input transpose

        groupSize = src.shape\[1\] / offset.shape\[1\]

        **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* \(src\[i\]\[j\] + offset\[i\]\[j / groupSize\]\)**

-   PER\_TENSOR scenario (Per-Tensor Quantization)

    **dst\[i\]\[j\] = scale \* \(src\[i\]\[j\] + offset\)**

-   PER\_TOKEN scenario (Per-Token Quantization)

    ![](../../../figures/zh-cn_formulaimage_0000002069677288.png)

-   PER\_GROUP scenario (Per-Group Quantization)

    Depending on the input data type, the current PER\_GROUP is divided into two scenarios: the fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (hereinafter referred to as the float4 scenario) and the int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (hereinafter referred to as the b8 scenario).

    -   fp4x2\_e2m1\_t/fp4x2\_e1m2\_t scenario (float4 scenario)
        -   Configurable **groupSize** API

            The computation direction of a group is defined as the k direction. In **src**, every **groupSize** elements along the k direction share one set of **scale**. When the shape of **src** is \[m, n\], if **kDim**=0, indicating that k is the m direction, the shape of **scale** is \[\(m + **groupSize** - 1\) / **groupSize**, n\]; if **kDim**=1, indicating that k is the n direction, the shape of **scale** is \[m, \(n + **groupSize** - 1\) / groupSize\]. **isTranspose** being **True** indicates that **src**, **scale**, and **dst** are all transposed matrices.

            -   k is the m direction, that is, the i axis in the formula is the computation direction of the group: (**kDim**=0 and **isTranspose**=**False**) or (**kDim**=1 and **isTranspose**=**True**)

                **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* src\[i\]\[j\]**

            -   k is the n direction, that is, the j axis in the formula is the computation direction of the group: (**kDim**=0 and **isTranspose**=**True**) or (**kDim**=1 and **isTranspose**=**False**)

                **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* src\[i\]\[j\]**

        -   **groupSize** fixed at 32

            **isTranspose** being **True** indicates that **src**, **scale**, and **dst** are all transposed matrices.

            -   Input transpose not enabled (**isTranspose**=**False**)

                **dst\[i\]\[j\] = scale\[i / groupSize\]\[j\] \* src\[i\]\[j\]**

            -   Enable input transpose (isTranspose=True)

                **dst\[i\]\[j\] = scale\[i\]\[j / groupSize\] \* src\[i\]\[j\]**

    -   int8\_t/hifloat8\_t/fp8\_e5m2\_t/fp8\_e4m3fn\_t scenario (b8 scenario)

        The computation direction of a group is defined as the k direction. In the k direction, every groupSize elements of src share one set of scale and offset. When the shape of src is \[m, n\], if kDim=0, it indicates that k is the m direction, and the shapes of scale and offset are \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, it indicates that k is the n direction, and the shapes of scale and offset are \[m, \(n + groupSize - 1\) / groupSize\]. offset is an optional input. isTranspose being True indicates that src, scale, and dst are all transposed matrices.

        -   k is the m direction, that is, the i axis in the formula is the computation direction of the group: (kDim=0 and isTranspose=False) or (kDim=1 and isTranspose=True)

            ![](../../../figures/zh-cn_formulaimage_0000002106594713.png)

        -   k is the n direction, that is, the j axis in the formula is the computation direction of the group: (kDim=0 and isTranspose=True) or (kDim=1 and isTranspose=False)

            ![](../../../figures/zh-cn_formulaimage_0000002106474749.png)

## Implementation Principle

**Figure 1**  AscendAntiQuant algorithm block diagram  
![](../../../figures/AscendAntiQuant_algorithm_block_diagram.png "AscendAntiQuant algorithm block diagram")

As shown in the preceding figure, this is the algorithm block diagram of a typical AscendAntiQuant scenario. The computation process is divided into the following steps, all of which are performed on the Vector:

1.  Precision conversion: convert the input src to the half type;
2.  Compute **offset**: when **offset** is a vector, perform an **Add** operation; when **offset** is a scalar, perform an **Adds** operation;
3.  Compute **scale**: when **scale** is a vector, perform a **Mul** operation; when **scale** is a scalar, perform a **Muls** operation.

**Figure 2**  AscendAntiQuant algorithm block diagram when **isTranspose** is **False** and the output is bfloat16  
![](../../../figures/AscendAntiQuant_algorithm_block_diagram_output_bfloat16.png "AscendAntiQuant algorithm block diagram with the output of bfloat16")

On Atlas A2 training products/Atlas A2 inference products, when the output is bfloat16, the computation process is divided into the following steps:

1.  **src** precision conversion: convert the input **src** to the **half** type, then to the **float** type, and store the result in **tmp1**;
2.  **offset** precision conversion: when the input **offset** is a vector, convert it to the **float** type and store the result in **tmp2**; when it is a scalar, perform a **Cast** to convert it to the **float** type;
3.  Compute **offset**: when the input **offset** is a vector, perform an **Add** operation with **tmp2**; when it is a scalar, perform an **Adds** operation;
4.  **scale** precision conversion: when the input **scale** is a vector, convert it to the **float** type and store the result in **tmp2**; when it is a scalar, perform a **Cast** to convert it to the **float** type;
5.  Compute **scale**: when the input **scale** is a vector, perform a **Mul** operation with **tmp2**; when it is a scalar, perform a **Muls** operation;
6.  **dst** precision conversion: convert **tmp1** to the **bf16** type.

**Figure 3**  AscendAntiQuant PER\_TOKEN/PER\_GROUP algorithm block diagram  
![](../../../figures/AscendAntiQuant_PER_TOKEN_PER_GROUP_algorithm_block_diagram.png "AscendAntiQuant PER_TOKEN-PER_GROUP algorithm block diagram")

The computation logic of the PER\_TOKEN/PER\_GROUP b8/float4 scenario is as follows:

1.  Read data: Read the input src. Depending on the scenario, different read methods are used for the input scale and offset. For example, the PER\_TOKEN scenario performs Broadcast processing, and the PER\_GROUP scenario performs Gather processing.
2.  Precision conversion: Convert the data types of src/scale/offset accordingly based on the combination of different input data types.
3.  Computation: Perform addition and multiplication operations on the type-converted data.
4.  Precision conversion: Convert the computation result obtained from the above addition and multiplication operations to the dstT type to obtain the final output.

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   PER\_CHANNEL scenario (Per-Channel Quantization)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_CHANNEL scenario (Per-Channel Quantization, without offset)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR scenario (Per-Tensor Quantization)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset, const OutputDataType scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR scenario (Per-Tensor Quantization, without offset)

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType> &dst, const LocalTensor<InputDataType> &src, const OutputDataType scale, const LocalTensor<uint8_t> &sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_GROUP float4 scenario (Per-Group Quantization)

        Only supported on Ascend 950PR/Ascend 950DT.

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TOKEN/PER\_GROUP b8/float4 scenario (Per-Token Quantization)/(Per-Group Quantization)

        Only supported on Ascend 950PR/Ascend 950DT.

        ```
        template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config, const AscendAntiQuantPolicy& policy>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor,const LocalTensor<uint8_t>& sharedTmpBuffer, const AscendAntiQuantParam& para)
        ```

-   The API framework applies for temporary space.
    -   PER\_CHANNEL scenario

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<OutputDataType>& offset, const LocalTensor<OutputDataType>& scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TENSOR scenario

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const OutputDataType offset, const OutputDataType scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_GROUP float4 scenario (groupSize fixed at 32)

        Only supported on Ascend 950PR/Ascend 950DT.

        ```
        template <typename InputDataType, typename OutputDataType, bool isTranspose>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale, const uint32_t k, const AntiQuantShapeInfo& shapeInfo = {})
        ```

    -   PER\_TOKEN/PER\_GROUP b8/float4 scenario (groupSize configurable)

        Only supported on Ascend 950PR/Ascend 950DT.

        ```
        template <typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config, const AscendAntiQuantPolicy& policy>
        __aicore__ inline void AscendAntiQuant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor,const AscendAntiQuantParam& para)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **application by the API framework** and **passing in through the sharedTmpBuffer input parameter**.

-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

For the method of allocation by the API framework, you need to reserve temporary space; for the case of passing temporary space through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the API provided in [GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| InputDataType | Input data type. |
| OutputDataType | Output data type. |
| isTranspose | Whether to enable input data transpose. |

**Table 2**  Template parameter description for the PER_TOKEN/PER_GROUP b8/float4 scenario

| Parameter | Description |
| --- | --- |
| dstT | Data type of the destination operand. |
| srcT | Data type of the source operand. |
| scaleT | Data type of the scale factor parameter. |
| config | Configuration parameter of the quantization API, defined as shown in the code below, where the meanings of the parameters are as follows.<br>hasOffset: Whether the quantization parameter offset participates in the computation. True: indicates that the offset parameter participates in the computation. False: indicates that the offset parameter does not participate in the computation.<br>isTranspose: Indicates whether to enable input src transpose. True: indicates that input src is transposed. False: indicates that input src is not transposed.<br>kDim: Computation direction of group, that is, the k direction. It is valid only in the PER_GROUP scenario. The supported values are as follows. 0: the k axis is axis 0, that is, the m direction is the computation direction of group; 1: the k axis is axis 1, that is, the n direction is the computation direction of group. |
| policy | Quantization policy configuration parameter, of enumeration type. The possible values are as follows:<br>PER_TENSOR: Reserved parameter, not supported yet.<br>PER_CHANNEL: Reserved parameter, not supported yet.<br>PER_TOKEN: Configured for the PER_TOKEN scenario.<br>PER_GROUP: Configured for the PER_GROUP scenario.<br>PER_CHANNEL_PER_GROUP: Reserved parameter, not supported yet.<br>PER_TOKEN_PER_GROUP: Reserved parameter, not supported yet. |

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
| fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently both support only the PER_GROUP scenario) | fp8_e8m0_t | half |
| fp4x2_e1m2_t/fp4x2_e2m1_t<br><br>(Currently both support only the PER_GROUP scenario) | fp8_e8m0_t | bfloat16_t |

**Table 4**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand. For the data types supported by different models, see [Data types supported by dst](#li117871685514).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src | Input | Source operand. For the data types supported by different models, see [Data types supported by src](#li13336025165520).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| offset | Input | Offset of the input data during dequantization. For the data types supported by different models, see [Data types supported by offset](#li5382122225718).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| scale | Input | Scale factor of the input data during dequantization. For the data types supported by different models, see [Data types supported by scale](#li103341450906).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendAntiQuantMaxMinTmpSize](GetAscendAntiQuantMaxMinTmpSize.md). |
| k | Input | When isTranspose is true, the shape of src is [N,K]; when isTranspose is false, the shape of src is [K,N].<br><br>The parameter k corresponds to the K value therein. |
| shapeInfo | Input | Sets the shape information of the offset and scale parameters. It needs to be configured only in the PER_CHANNEL scenario (Per-Channel Quantization).<br><br>Optional parameter. In the PER_CHANNEL scenario, if this parameter is not passed in or the data in the structure is set to 0, the shape information of offset and scale is obtained from the [ShapeInfo](../../basic_api/aux_data_structures/ShapeInfo.md) of offset and scale.<br><br>The type is AntiQuantShapeInfo, defined as shown in the code below. |

```
struct AntiQuantShapeInfo {
    uint32_t offsetHeight{0};  // Height of offset.
    uint32_t offsetWidth{0};  // Width of offset.
    uint32_t scaleHeight{0};  // Height of scale.
    uint32_t scaleWidth{0};  // Width of scale.
};
```

**Table 5**   API parameter description for the PER\_TOKEN/PER\_GROUP b8/float4 scenario

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. Supported data types: half, bfloat16_t, float.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand. Supported data types: int8_t, fp8_e4m3fn_t, fp8_e5m2_t, hifloat8_t, fp4x2_e1m2_t, fp4x2_e2m1_t. Note that the fp4x2_e1m2_t and fp4x2_e2m1_t data types are supported only in the PER_GROUP scenario.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer. Supported data type: uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendQuantMaxMinTmpSize](GetAscendQuantMaxMinTmpSize.md). |
| scaleTensor | Input | Quantization parameter scale. Supported data types: half, float, bfloat16_t, fp8_e8m0_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| offsetTensor | Input | Quantization parameter offset. The supported data types are the same as those of scaleTensor. For the float4 scenario, offsetTensor does not take effect.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| para | Input | Parameters of the quantization API, of the AscendAntiQuantParam type. The specific definition is shown in the following code, and the meanings of the parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the value of n must meet the 32B alignment requirement, that is, both the input and output whose last shape dimension is n must meet the 32B alignment requirement on that dimension.<br>calCount: Number of elements involved in the computation. calCount must be an integer multiple of n.<br>groupSize: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct AscendAntiQuantParam {
        uint32_t m;
        uint32_t n;
        uint32_t calCount;
        uint32_t groupSize = 0;
}
```

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The data length of the input and output operands involved in the computation must meet the 32B alignment requirement.
-   For the input transpose scenario, k must be 32B aligned.
-   Before calling the API, ensure that the size of the input data is correct and that the sizes and shapes of offset and scale are correct.
-   The PER\_TOKEN/PER\_GROUP b8/float4 scenario is supported only on Ascend 950PR/Ascend 950DT.
-   In the PER\_TOKEN/PER\_GROUP b8/float4 scenario, the data volume in the continuous computation direction (that is, the n direction) must be 32B-aligned.
-   In non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios, the data types supported by **dst** are as follows:<a id="li117871685514"></a>

    Ascend 950PR/Ascend 950DT: The supported data types are half and bfloat16\_t.

    Atlas A3 training products/Atlas A3 inference products: The supported data types are half and bfloat16\_t.

    Atlas A2 training products/Atlas A2 inference products: The supported data types are half and bfloat16\_t.

    AI Core of Atlas inference products: The supported data type is half.

-   In non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios, the data types supported by **src** are as follows:<a id="li13336025165520"></a>

    Ascend 950PR/Ascend 950DT: In the PER\_CHANNEL and PER\_TENSOR scenarios, the supported data types are int8\_t, fp8\_e4m3fn\_t, fp8\_e5m2\_t, and hifloat8\_t; in the PER\_GROUP float4 scenario, the supported data types are fp4x2\_e2m1\_t and fp4x2\_e1m2\_t.

    Atlas A3 training products/Atlas A3 inference products: The supported data types are int8\_t and int4b\_t.

    Atlas A2 training products/Atlas A2 inference products: The supported data types are int8\_t and int4b\_t.

    AI Core of Atlas inference products: The supported data type is int8\_t.

-   Data types supported by offset in non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios<a id="li5382122225718"></a>

    Ascend 950PR/Ascend 950DT, the supported data types are: half, bfloat16\_t.

    Atlas A3 training products/Atlas A3 inference products, the supported data types are: half, bfloat16\_t.

    Atlas A2 training products/Atlas A2 inference products, the supported data types are: half, bfloat16\_t.

    The AI Core of Atlas inference products supports the following data type: half.

-   Data types supported by scale in non-PER\_TOKEN/PER\_GROUP b8/float4 scenarios<a id="li103341450906"></a>

    Ascend 950PR/Ascend 950DT, the supported data types in the PER\_CHANNEL and PER\_TENSOR scenarios are: half, bfloat16\_t. The supported data types in the PER\_GROUP float4 scenario are: fp8\_e8m0\_t.

    Atlas A3 training products/Atlas A3 inference products, the supported data types are: half, bfloat16\_t.

    Atlas A2 training products/Atlas A2 inference products, the supported data types are: half, bfloat16\_t.

    Atlas inference products AI Core supports the following data type: half.

## Examples

```
// dstLocal: result tensor.
// srcLocal: quantized input.
// offsetLocal: offset parameter.
// scaleLocal: scale parameter.
// sharedTmpBuffer: temporary buffer managed by you, used to store intermediate variables in internal computation.
// k: length of the k axis.
// shapeInfo: shape information of the offsetLocal and scaleLocal tensors.
AscendC::AntiQuantShapeInfo shapeInfo = {1, elementCountOfOffset, 1, elementCountOfOffset};
AscendC::AscendAntiQuant<InputType, OutType, false>(dstLocal, srcLocal, offsetLocal, scaleLocal, sharedTmpBuffer, k, shapeInfo);
```

The results are as follows:

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

The PER\_TOKEN/PER\_GROUP b8 scenario call example is as follows.

```
// Note that m and n must be passed in externally.
constexpr static bool isReuseSource = false;
constexpr static AscendAntiQuantConfig config = {has_offset, has_transpose, -1};
constexpr static AscendAntiQuantPolicy policy = AscendAntiQuantPolicy::PER_TOKEN;
AscendAntiQuantParam para;
para.m = m;
para.n = n;
para.calCount = calCount;
AscendAntiQuant<dstType, srcType, scaleType, config, policy>(dstLocal, srcLocal, scaleLocal, offsetLocal, para);
```
