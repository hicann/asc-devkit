# AscendDequant

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:25:14.467Z pushedAt=2026-10-29T02:14:34.489Z nl=2 -->

## Applicable Products

<!-- npu="950" id7 -->
- Ascend 950PR&950DT products: Supported
<!-- end id7 -->
<!-- npu="A3" id8 -->
- Atlas A3 products: Supported
<!-- end id8 -->
<!-- npu="910b" id9 -->
- Atlas A2 products: Supported
<!-- end id9 -->
<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id11 -->
<!-- npu="910" id12 -->
- Atlas training products: Not supported
<!-- end id12 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/quantization/AscendDequant_res.md#id1 -->

## Description

Performs element-wise dequantization, for example, dequantizing the int32\_t data type to data types such as half/float. **This API supports input of up to two dimensions and does not support higher-dimensional input.**

-   Assume that the shape of the input srcTensor is **(m, n)**. The number of bytes occupied by each row of data (that is, n input data elements) must be **32-byte aligned**, and the number of elements to be dequantized in each row is **calCount**.
-   The dequantization coefficient deqScale can be a scalar or a vector. If it is a vector, calCount must be less than or equal to the number of elements in deqScale, and only the first calCount dequantization coefficients take effect.
-   The shape of the output dstTensor is **(m, n\_dst)**. When n \* sizeof\(dstT\) is not 32-byte aligned, it must be **padded upward to 32 bytes**, and n\_dst is the number of columns after upward padding.

The following two examples explain the parameter configuration and computation logic (in the following text, the DequantParams type is a structure that stores shape information \{m, n, calCount\}):

-   In the example shown in the following figure, the data type of srcTensor is int32\_t, m = 4, n = 8, and calCount = 4, indicating that the number of elements to be dequantized in each row of srcTensor is 4, the first 4 values in deqScale take effect, and the remaining 12 values do not participate in the dequantization computation. The data type of dstTensor is bfloat16\_t, m = 4, n\_dst = 16 \(16 \* sizeof\(bfloat16\_t\) % 32 = 0\). The computation logic is as follows: every n elements of srcTensor form a row. For the first calCount elements in each row, the i-th element of srcTensor in that row is multiplied by the i-th element of deqScale and written to the i-th element of the corresponding row in dstTensor. The (calCount + 1)-th element \~ the n\_dst-th element of the corresponding row in dstTensor are all indeterminate values.

    ![](../../../figures/zh-cn_image_0000002155016964.png)

-   In the following example, the data type of srcTensor is int32\_t, m = 4, n = 8, and calCount = 4, indicating that the number of elements to be dequantized in each row of srcTensor is 4. The data type of dstTensor is float, m = 4, n\_dst = 8 \(8 \* sizeof\(float\) % 32 = 0\). For the first 4 elements in each row of srcTensor, each is multiplied by the scalar deqScale and written to the corresponding position in each row of dstTensor.

    ![](../../../figures/zh-cn_image_0000001819864122.png)

When you set mode in the template parameters to **DEQUANT\_WITH\_SINGLE\_ROW**:

For DequantParams \{m, n, calCount\}, if all of the following three conditions are met:

1.  m = 1
2.  calCount is a multiple of 32 / sizeof\(dstT\)
3.  n % calCount = 0

In this case, \{1, n, calCount\} is treated as ** \{n / calCount, calCount, calCount\}**  for the dequantization computation.

The following figure shows the specific effect. The DequantParams passed are \{1, 16, 8\}. Because dstT is float, calCount is a multiple of 8. In **DEQUANT\_WITH\_SINGLE\_ROW** mode, \{1, 2 \* 8, 8\} is converted to \{2, 8, 8\} for computation.

![](../../../figures/zh-cn_image_0000001820178290.png)

![](../../../figures/zh-cn_image_0000001866976705.png)

-   PER\_TOKEN dequantization: The elements in each token group of srcTensor (a token is in the n direction, with m token groups in total) share one set of deqscale parameters. When srcTensor is \[m, n\], deqscale is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002105578933.png)

-   PER\_GROUP Dequantization: Here the computation direction of the group is defined as the k direction. In srcTensor, every groupSize elements along the k direction share one set of deqscale parameters. When srcTensor is \[m, n\], if kDim=0, it indicates that k is the m direction, and deqscale is \[\(m + groupSize - 1\) / groupSize, n\]; if kDim=1, it indicates that k is the n direction, and the shape of deqscale is \[m, \(n + groupSize - 1\) / groupSize\].
    -   kDim=0:

        ![](../../../figures/zh-cn_formulaimage_0000002069815092.png)

    -   kDim=1:

        ![](../../../figures/zh-cn_formulaimage_0000002105655005.png)

## Implementation Principles

Taking the input srcTensor with data type int32\_t and shape \[m, n\], the input deqScale with data type scaleT and shape \[n\], and the output dstTensor with data type dstT and shape \[m, n\] as an example, the following figure shows the internal algorithm block diagram of the AscendDequant high-level API.

**Figure 1**  Internal algorithm block diagram of AscendDequant  
![](../../../figures/ascenddequant_diagram.png "Internal algorithm block diagram of AscendDequant")

The computation process consists of the following steps, all performed on the Vector:

1.  Precision conversion: Convert both srcTensor and deqScale to FP32 tensors, obtaining srcFP32 and deqScaleFP32, respectively.
2.  Mul computation: srcFP32 has m rows in total, each of length n. Through m iterations, multiply each row of srcFP32 by deqScaleFP32, and use a mask to perform the mul computation only on the first dequantParams.calcount elements. In the figure, the value range of index is \[0, m\), corresponding to each row of srcFP32. The computation result is mulRes, with shape \[m, n\].
3.  Result precision conversion: Convert mulRes from FP32 to a tensor of type dstT, obtaining dstTensor with shape \[m, n\].

In the PER\_TOKEN/PER\_GROUP scenario, the input srcTensor has data type int32\_t/float, and the internal algorithm block diagram is as follows.

**Figure 2**  Internal algorithm block diagram of AscendDequant PER\_TOKEN/PER\_GROUP  
![](../../../figures/ascenddequant_per_token_per_group.png "Internal algorithm block diagram of AscendDequant-PER_TOKEN-PER_GROUP")

The computation logic of the PER\_TOKEN/PER\_GROUP scenario is as follows:

1.  Read data: Continuously read the input srcTensor. Depending on the scenario, read the input deqscale in different ways. For example, the PER\_TOKEN scenario performs Broadcast processing, and the PER\_GROUP scenario performs Gather processing.
2.  Precision conversion: Convert the data types of srcTensor/deqscale accordingly based on different input data type combinations.
3.  Computation: Multiply the type-converted srcTensor and deqscale data.
4.  Precision conversion: Convert the result obtained from the above computation to the dstT type to obtain the final output.

## Prototype

-   The dequantization parameter deqScale is a vector.
    -   Pass the temporary space through the sharedTmpBuffer input parameter

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams params)
        ```

    -   The API framework allocates the temporary space

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, DequantParams params)
        ```

    -   PER\_TOKEN/PER\_GROUP quantization

        <!-- npu="950" id21 -->
        Only Ascend 950PR&950DT products are supported.
        <!-- end id21 -->

        -   Pass the temporary space through the sharedTmpBuffer input parameter

            ```
            template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config, const AscendDeQuantPolicy& policy>
            __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const AscendDeQuantParam& para)
            ```

        -   The API framework allocates the temporary space

            ```
            template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config, const AscendDeQuantPolicy& policy>
            __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
            ```

-   The dequantization parameter deqScale is a scalar.
    -   Pass the temporary space through the sharedTmpBuffer input parameter

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams params)
        ```

    -   The API framework allocates the temporary space

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale, DequantParams params)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be obtained in two ways: **allocated by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the API framework allocates the space, you need to reserve temporary space. When the space is passed through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: call the GetAscendDequantMaxMinTmpSize API provided in [GetAscendDequantMaxMinTmpSize](GetAscendDequantMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

The following APIs are not recommended. Do not use them in new development:

```
template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
__aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
```

```
template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
__aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer)
```

```
template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
__aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, const uint32_t calCount)
```

```
template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
__aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| dstT | Data type of the destination operand. |
| scaleT | Data type of deqScale. |
| mode | Determines the computation logic when DequantParams is {1, n, calCount}. Pass enum DeQuantMode. The following two configurations are supported:<br>DEQUANT_WITH_SINGLE_ROW: When DequantParams {m, n, calCount} satisfies all of the following conditions: 1. m = 1; 2. calCount is a multiple of 32 / sizeof(dstT); 3. n % calCount = 0, {1, n, calCount} is computed as {n / calCount, calCount, calCount}.<br>DEQUANT_WITH_MULTI_ROW: Even if all the above conditions are met, {1, n, calCount} is still computed only as {1, n, calCount}, that is, a total of n values, with the first calCount values undergoing dequantization. |

**Table 2**  PER\_TOKEN/PER\_GROUP Scenario template parameter description

| Parameter | Description |
| --- | --- |
| srcT | Data type of the source operand. |
| config | Quantization API configuration parameter, of the AscendDeQuantConfig type, defined in the following code. The meanings of its parameters are as follows.<br>hasOffset: Whether the quantization parameter offset participates in the computation. True: offset participates in the computation. False: offset does not participate in the computation.<br>kDim: Computation direction of group, that is, the k direction. Valid only in the PER_GROUP scenario. Supported values are as follows. 0: The k axis is axis 0, that is, the m direction is the computation direction of group. 1: The k axis is axis 1, that is, the n direction is the computation direction of group. |
| policy | Quantization policy configuration parameter, of the AscendDeQuantPolicy enum type. Possible values are as follows:<br>PER_TOKEN: Configured as PER_TOKEN mode.<br>PER_GROUP: Configured as PER_GROUP mode.<br>PER_CHANNEL_PER_GROUP: Reserved parameter. Not supported yet.<br>PER_TOKEN_PER_GROUP: Reserved parameter. Not supported yet. |

```
struct AscendDeQuantConfig {
    bool hasOffset;
    int32_t kDim = 1;
};
```

**Table 3**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. For the data types supported by different models, see [data types supported by dstTensor](#li17926145114504).<br>The number of rows of dstTensor is consistent with that of srcTensor.<br>When n * sizeof(dstT) does not meet 32-byte alignment, it needs to be padded upward to 32 bytes, and n_dst is the number of columns after upward padding. For example, if the data type of srcTensor is int32_t with shape (4, 8), and dstTensor is bfloat16_t, n_dst should be padded from 8 to 16, and the shape of dstTensor is (4, 16). The padding computation process is: n_dst = (8 * sizeof(bfloat16_t) + 32 - 1) / 32 * 32 / sizeof(bfloat16_t). |
| srcTensor | Input | Source operand. Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. Supported data type: int32_t.<br><br>The shape is [m, n]. The bytes occupied by the n input data must be 32-byte aligned. |
| deqScale | Input | Source operand. Type: scalar or [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). When the type is LocalTensor, the supported TPosition is VECIN/VECCALC/VECOUT. For the data types supported by different models, see [data types supported by deqScale](#li189021550175211).<br><br>For the supported data type combinations of dstTensor, srcTensor, and deqScale, see Table 5 and Table 6. |
| sharedTmpBuffer | Input | Temporary buffer. Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. Supported data type: uint8_t.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendDequantMaxMinTmpSize](GetAscendDequantMaxMinTmpSize.md). |
| params | Input | Shape information of srcTensor. Type: DequantParams, defined in the following code. The meanings of its parameters are as follows.<br>m: Number of rows of srcTensor.<br>n: Number of columns of srcTensor.<br>calCount: For each row of srcTensor, the first calCount values are valid data, which are multiplied with the first calCount values of deqScale or the deqScale scalar.<br><br>Note:<br>DequantParams.n * sizeof(T) must be an integer multiple of 32 bytes, where T is the data type of the elements in srcTensor.<br>Because the multiplication is performed on the first calCount values of every n values, DequantParams.n and calCount must satisfy the following relationship: 1 <= DequantParams.calCount <= DequantParams.n.<br>When deqScale is a vector, DequantParams.calCount <= the number of elements of deqScale. |

```
struct DequantParams {
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
};
```

**Table 4**  PER\_TOKEN/PER\_GROUP Scenario API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. Supported data types: half, bfloat16_t, float.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand. Supported data types: int32_t, float.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer. Supported data type: uint8_t.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendQuantMaxMinTmpSize](GetAscendDequantMaxMinTmpSize.md). |
| scaleTensor | Input | Quantization parameter scale. Supported data types: half, bfloat16_t, float.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| offsetTensor | Input | Quantization parameter offset. The supported data type is consistent with that of scaleTensor. Reserved parameter. Not supported yet.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| para | Input | Parameter of the dequantization API, defined in the following code. The meanings of its parameters are as follows.<br>m: Number of elements in the m direction.<br>n: Number of elements in the n direction. The data size corresponding to the n value must meet the 32B alignment requirement, that is, both the input and output whose last dimension of the shape is n must meet the 32B alignment requirement on that dimension.<br>calCount: Number of elements participating in the computation. calCount must be an integer multiple of n.<br>groupSize: Valid in the PER_GROUP scenario, indicating that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and an integer multiple of 32. |

```
struct AscendDeQuantParam {
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
    uint32_t groupSize = 0;
};
```

**Table 5**  Supported data type combinations (deqScale is LocalTensor)

| dstTensor | srcTensor | deqScale |
| --- | --- | --- |
| half | int32_t | uint64_t<br><br>Note: When the data type of deqScale is uint64_t, the lower 32 bits of the value are the data participating in the computation, of the float data type, and the upper 32 bits are some control parameters, which are not used by this API. |
| float | int32_t | float |
| float | int32_t | bfloat16_t |
| bfloat16_t | int32_t | bfloat16_t |
| bfloat16_t | int32_t | float |

**Table 6** Supported data type combinations (deqScale is a scalar)

| dstTensor | srcTensor | deqScale |
| --- | --- | --- |
| bfloat16_t | int32_t | bfloat16_t |
| bfloat16_t | int32_t | float |
| float | int32_t | bfloat16_t |
| float | int32_t | float |

**Table 7** Data type combinations supported in the PER\_TOKEN/PER\_GROUP scenario

| srcDtype | scaleDtype | dstDtype |
| --- | --- | --- |
| int32_t | half | half |
| int32_t | bfloat16_t | bfloat16_t |
| int32_t | float | float |
| int32_t | float | half |
| int32_t | float | bfloat16_t |
| float | half | half |
| float | bfloat16_t | bfloat16_t |
| float | float | float |
| float | float | half |
| float | float | bfloat16_t |

## Return Value

None

## Constraints

-   **Address overlap between the source operands and the destination operand is not supported.**
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   The support for PER\_TOKEN/PER\_GROUP quantization scenarios is as follows:

    <!-- npu="950" id22 -->
    - Ascend 950PR&950DT products: Supported.
    <!-- end id22 -->

    <!-- npu="A3" id23 -->
    - Atlas A3 products: Not supported.
    <!-- end id23 -->

    <!-- npu="910b" id24 -->
    - Atlas A2 products: Not supported.
    <!-- end id24 -->

    <!-- npu="310p" id25 -->
    - Atlas inference products AI Core: Not supported.
    <!-- end id25 -->

-   In the PER\_TOKEN/PER\_GROUP scenario, the data volume in the continuous computation direction (that is, the n direction) must be 32B aligned.
-   Data types supported by dstTensor in the non-PER\_TOKEN/PER\_GROUP scenario<a id="li17926145114504"></a>

    <!-- npu="950" id13 -->
    - Ascend 950PR&950DT products: supported data types are half, bfloat16\_t, and float.
    <!-- end id13 -->

    <!-- npu="A3" id14 -->
    - Atlas A3 products: supported data types are half, bfloat16\_t, and float.
    <!-- end id14 -->

    <!-- npu="910b" id15 -->
    - Atlas A2 products: supported data types are half, bfloat16\_t, and float.
    <!-- end id15 -->

    <!-- npu="310p" id16 -->
    - Atlas inference products AI Core: supported data types are half and float.
    <!-- end id16 -->

-   Data types supported by deqScale in the non-PER\_TOKEN/PER\_GROUP scenario<a id="li189021550175211"></a>

    <!-- npu="950" id17 -->
    - Ascend 950PR&950DT products: when deqScale is a vector, the supported data types are uint64\_t, float, and bfloat16\_t; when deqScale is a scalar, the supported data types are bfloat16\_t and float.
    <!-- end id17 -->

    <!-- npu="A3" id18 -->
    - Atlas A3 products: when deqScale is a vector, the supported data types are uint64\_t, float, and bfloat16\_t; when deqScale is a scalar, the supported data types are bfloat16\_t and float.
    <!-- end id18 -->

    <!-- npu="910b" id19 -->
    - Atlas A2 products: when deqScale is a vector, the supported data types are uint64\_t, float, and bfloat16\_t; when deqScale is a scalar, the supported data types are bfloat16\_t and float.
    <!-- end id19 -->

    <!-- npu="310p" id20 -->
    - For the Atlas inference products AI Core, when deqScale is a vector, the supported data types are: uint64\_t and float; when deqScale is a scalar, the supported data type is float.
    <!-- end id20 -->

## Examples

```
// dstLocal: Tensor that stores the result of the dequantization computation.
// srcLocal: Tensor that stores the input of the dequantization computation.
// deqScaleLocal: Input tensor that stores the dequantization coefficients for the dequantization computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the dequantization computation.

uint32_t m = 4;        // Number of rows in srcTensor.
uint32_t n = 8;        // Number of columns in srcTensor.
uint32_t calCount = 6; // The first calCount elements of each row in srcTensor participate in the dequantization computation.

// The dequantization parameter deqScale is a vector, and the temporary space is passed through the sharedTmpBuffer input parameter.
AscendC::AscendDequant(dstLocal, srcLocal, deqScaleLocal, sharedTmpBuffer, {m, n, calCount});

// The dequantization parameter deqScale is a vector, and the temporary space is allocated through the API framework.
AscendC::AscendDequant(dstLocal, srcLocal, deqScaleLocal, {m, n, calCount});

// The dequantization parameter deqScale is a scalar, and the temporary space is passed through the sharedTmpBuffer input parameter.
AscendC::AscendDequant(dstLocal, srcLocal, static_cast<float>(2.2), sharedTmpBuffer, {m, n, calCount});
// The dequantization parameter deqScale is a scalar, with the temporary space allocated through the API framework.
AscendC::AscendDequant(dstLocal, srcLocal, static_cast<float>(2.2), {m, n, calCount});
```

The result is as follows:

```
Input data (srcLocal) int32_t data type:
[[-16 -15 -14 -13 -12 -11 -10  -9]
 [ -8  -7  -6  -5  -4  -3  -2  -1]
 [  0   1   2   3   4   5   6   7]
 [  8   9  10  11  12  13  14  15]]

Dequantization parameter deqScaleLocal float data type:
[2.2  -2.2  2.2  -2.2  2.2  -2.2  0.  0.]

Output data (dstLocal) float data type:
[[-35.2  33.  -30.8  28.6 -26.4  24.2 -10.   -9. ]
 [-17.6  15.4 -13.2  11.   -8.8   6.6  -2.   -1. ]
 [  0.   -2.2   4.4  -6.6   8.8 -11.    6.    7. ]
 [ 17.6 -19.8  22.  -24.2  26.4 -28.6  14.   15. ]]
```

The call example for the PER\_TOKEN/PER\_GROUP scenario is as follows.

```
// Note that m and n must be passed externally.
constexpr static bool isReuseSource = false;
constexpr static AscendDeQuantConfig config = {has_offset, -1};
constexpr static AscendDeQuantPolicy policy = AscendDeQuantPolicy::PER_TOKEN; // Modify the enum value to enable PER_GROUP.
AscendDeQuantParam para;
para.m = m;
para.n = n;
para.calCount = calCount;
AscendDequant<dstType, srcType, scaleType, config, policy>(dstLocal, srcLocal, scaleLocal, offsetLocal, para);
```
