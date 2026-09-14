# AscendDequant

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T11:30:04.662Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise dequantization, for example, dequantizing the int32\_t data type to data types such as half/float. **This API supports at most two-dimensional input and does not support higher-dimensional input.**

-   Assume that the shape of the input **srcTensor** is **(m, n)**. The number of bytes occupied by each row of data (that is, n input data elements) must be **32-byte aligned**, and the number of elements to be dequantized in each row is **calCount**.
-   The dequantization scale **deqScale** can be a scalar or a vector. When it is a vector, calCount must be less than or equal to the element count of **deqScale**, and only the first calCount dequantization scales take effect.
-   The shape of the output **dstTensor** is **(m, n\_dst)**. When n \* sizeof\(dstT\) is not 32-byte aligned, it must be **padded upward to 32 bytes**, and n\_dst is the number of columns after upward padding.

The following two specific examples explain the parameter configuration and computation logic (in the following text, the DequantParams type is a structure that stores shape information \{m, n, calCount\}):

-   In the example shown in the following figure, the data type of **srcTensor** is int32\_t, m = 4, n = 8, and calCount = 4, indicating that the number of elements to be dequantized in each row of **srcTensor** is 4. The first 4 values in **deqScale** take effect, and the remaining 12 values do not participate in the dequantization computation. The data type of **dstTensor** is bfloat16\_t, m = 4, and n\_dst = 16 \(16 \* sizeof\(bfloat16\_t\) % 32 = 0\). The computation logic is as follows: every n elements of **srcTensor** form one row. For the first calCount elements in each row, the i-th element of **srcTensor** in that row is multiplied by the i-th element of **deqScale**, and the result is written to the i-th element of the corresponding row of **dstTensor**. The elements from the (calCount + 1)-th to the n\_dst-th element of the corresponding row of **dstTensor** are all indeterminate values.

    ![](../../../figures/zh-cn_image_0000002155016964.png)

-   In the example shown in the following figure, the data type of **srcTensor** is int32\_t, m = 4, n = 8, and calCount = 4, indicating that the number of elements to be dequantized in each row of **srcTensor** is 4. The data type of **dstTensor** is float, m = 4, and n\_dst = 8 \(8 \* sizeof\(float\) % 32 = 0\). For the first 4 elements in each row of **srcTensor**, each is multiplied by the scalar **deqScale** and written to the corresponding position in each row of **dstTensor**.

    ![](../../../figures/zh-cn_image_0000001819864122.png)

When the user configures **mode** in the template parameters as **DEQUANT\_WITH\_SINGLE\_ROW**:

For **DequantParams** \{m, n, calCount\}, if the following three conditions are all met:

1. m = 1
2. **calCount** is a multiple of 32 / sizeof\(dstT\)
3. n % calCount = 0

In this case, \{1, n, calCount\} is treated as **\{n / calCount, calCount, calCount\}**** for dequantization computation.

The specific effect is shown in the following figure, where the passed **DequantParams** is \{1, 16, 8\}. Because **dstT** is float, **calCount** is a multiple of 8. In **DEQUANT\_WITH\_SINGLE\_ROW** mode, \{1, 2 \* 8, 8\} is converted to \{2, 8, 8\} for computation.

![](../../../figures/zh-cn_image_0000001820178290.png)

![](../../../figures/zh-cn_image_0000001866976705.png)

- PER\_TOKEN dequantization: The elements in each group of tokens of **srcTensor** (tokens are in the n direction, with m groups of tokens in total) share one group of **deqscale** parameters. When **srcTensor** is \[m, n\], **deqscale** is \[m, 1\].

    ![](../../../figures/zh-cn_formulaimage_0000002105578933.png)

-   PER\_GROUP dequantization: Here, the computation direction of the group is defined as the k direction, and in the k direction of **srcTensor**, every **groupSize** elements share one set of **deqscale** parameters. When **srcTensor** is \[m, n\], if **kDim**=0, it indicates that k is the m direction, and **deqscale** is \[\(m + groupSize - 1\) / groupSize, n\]; if **kDim**=1, it indicates that k is the n direction, and the shape of **deqscale** is \[m, \(n + groupSize - 1\) / groupSize\].
    -   kDim=0:

        ![](../../../figures/zh-cn_formulaimage_0000002069815092.png)

    -   kDim=1:

        ![](../../../figures/zh-cn_formulaimage_0000002105655005.png)

## Implementation Principle

Taking the input **srcTensor** with data type int32\_t and shape \[m, n\], the input **deqScale** with data type **scaleT** and shape \[n\], and the output **dstTensor** with data type **dstT** and shape \[m, n\] as an example, the internal algorithm block diagram of the **AscendDequant** high-level API is described as follows.

**Figure 1** AscendDequant internal algorithm block diagram  
![](../../../figures/AscendDequant_internal algorithm_block_diagram.png "AscendDequant internal algorithm block diagram")

The computation process consists of the following steps, all performed on the vector:

1.  Precision conversion: Convert both **srcTensor** and **deqScale** to FP32-precision tensors, obtaining **srcFP32** and **deqScaleFP32** respectively;
2.  Mul computation: **srcFP32** has m rows in total, each with a length of n. Through m iterations, each row of **srcFP32** is multiplied by **deqScaleFP32**, and the mask ensures that the mul computation is performed only on the first **dequantParams.calcount** elements. In the figure, the value range of index is \[0, m\), corresponding to each row of **srcFP32**. The result is **mulRes**, with shape \[m, n\];
3.  Result data precision conversion: mulRes is converted from FP32 to a tensor of type dstT, and the result is dstTensor with the shape \[m, n\].

In the PER\_TOKEN/PER\_GROUP scenario, the input srcTensor data type is int32\_t/float, and the internal algorithm block diagram is as follows.

**Figure 2**  AscendDequant PER\_TOKEN/PER\_GROUP internal algorithm block diagram  
![](../../../figures/AscendDequant_PER_TOKEN_PER_GROUP_internal_algorithm_block_diagram.png "AscendDequant PER\_TOKEN/PER\_GROUP internal algorithm block diagram")

The computation logic for the PER\_TOKEN/PER\_GROUP scenario is as follows:

1.  Read data: Continuously read the input srcTensor. Depending on the scenario, different read methods are used for the input deqscale. For example, broadcast processing is performed in the PER\_TOKEN scenario, and gather processing is performed in the PER\_GROUP scenario.
2.  Precision conversion: Perform the corresponding data type conversion on srcTensor/deqscale according to the data type combination of different inputs.
3.  Computation: Perform multiplication on the type-converted srcTensor and deqscale data.
4.  Precision conversion: Convert the result obtained from the above computation to the dstT type to obtain the final output.

## Prototype

-   The dequantization parameter deqScale is a vector.
    -   Pass the temporary space through the **sharedTmpBuffer** input parameter.

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams params)
        ```

    -   The API framework applies for temporary space.

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale, DequantParams params)
        ```

    -   PER\_TOKEN/PER\_GROUP quantization

        Only supported on Ascend 950PR/Ascend 950DT.

        -   Pass the temporary space through the **sharedTmpBuffer** input parameter.

            ```
            template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config, const AscendDeQuantPolicy& policy>
            __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const AscendDeQuantParam& para)
            ```

        -   The API framework applies for temporary space.

            ```
            template <typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config, const AscendDeQuantPolicy& policy>
            __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor, const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
            ```

-   The dequantization parameter **deqScale** is a scalar.
    -   Pass the temporary space through the **sharedTmpBuffer** input parameter.

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale, const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams params)
        ```

    -   The API framework applies for temporary space.

        ```
        template <typename dstT, typename scaleT, DeQuantMode mode = DeQuantMode::DEQUANT_WITH_SINGLE_ROW>
        __aicore__ inline void AscendDequant(const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale, DequantParams params)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When the temporary space is allocated by the API framework, you need to reserve the temporary space. When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for **sharedTmpBuffer**. The temporary space size **BufferSize** is obtained as follows: use the **GetAscendDequantMaxMinTmpSize** API provided in [GetAscendDequantMaxMinTmpSize](GetAscendDequantMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

The following APIs are not recommended. Do not use the following APIs in new development:

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
| scaleT | Data type of **deqScale**. |
| mode | Determines the computation logic when **DequantParams** is {1, n, calCount}. Pass in the **DeQuantMode** enum, which supports the following two configurations:<br>**DEQUANT_WITH_SINGLE_ROW**: When **DequantParams** {m, n, calCount} satisfies all of the following conditions: 1. m = 1; 2. calCount is a multiple of 32 / sizeof(dstT); 3. n % calCount = 0, {1, n, calCount} is treated as {n / calCount, calCount, calCount} for computation.<br>**DEQUANT_WITH_MULTI_ROW**: Even if all the above conditions are satisfied, {1, n, calCount} is still treated as {1, n, calCount} for computation, that is, a total of n numbers, with the first calCount numbers undergoing dequantization computation. |

**Table 2**  PER\_TOKEN/PER\_GROUP Scenario template parameter description

| Parameter | Description |
| --- | --- |
| srcT | Data type of the source operand. |
| config | Quantization API configuration parameter, of the **AscendDeQuantConfig** type, defined as shown in the following code, where the meaning of each parameter is as follows.<br>**hasOffset**: Whether the quantization parameter **offset** participates in the computation. **True**: Indicates that the **offset** parameter participates in the computation. **False**: Indicates that the **offset** parameter does not participate in the computation.<br>**kDim**: Computation direction of the group, that is, the k direction. Valid only in the PER_GROUP scenario. Supported values are as follows. 0: The k axis is axis 0, that is, the m direction is the computation direction of the group. 1: The k axis is axis 1, that is, the n direction is the computation direction of the group. |
| policy | Quantization policy configuration parameter, of the **AscendDeQuantPolicy** enum type. Possible values are as follows:<br>**PER_TOKEN**: Configured as PER_TOKEN mode.<br>**PER_GROUP**: Configured as PER_GROUP mode.<br>**PER_CHANNEL_PER_GROUP**: Reserved parameter, not supported yet.<br>**PER_TOKEN_PER_GROUP**: Reserved parameter, not supported yet. |

```
struct AscendDeQuantConfig {
        bool hasOffset;
        int32_t kDim = 1;
}
```

**Table 3**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. For the data types supported by different models, see [Data types supported by dstTensor](#li17926145114504).<br>The number of rows of **dstTensor** is the same as the number of rows of **srcTensor**.<br>When n * sizeof(dstT) is not 32-byte aligned, it needs to be padded upward to 32 bytes, and n_dst is the number of columns after upward padding. For example, if the data type of **srcTensor** is int32_t with shape (4, 8), and **dstTensor** is bfloat16_t, then n_dst should be padded from 8 to 16, and the shape of **dstTensor** is (4, 16). The padding computation process is: n_dst = (8 * sizeof(bfloat16_t) + 32 - 1) / 32 * 32 / sizeof(bfloat16_t). |
| srcTensor | Input | Source operand. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. The supported data type is: int32_t.<br><br>The shape is [m, n], and the number of bytes occupied by the n input data must be 32-byte aligned. |
| deqScale | Input | Source operand. The type is a scalar or [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). When the type is **LocalTensor**, the supported TPosition values are VECIN/VECCALC/VECOUT. For the data types supported by different models, see [Data types supported by deqScale](#li189021550175211).<br><br>For the supported data type combinations of **dstTensor**, **srcTensor**, and **deqScale**, see Table 5 and Table 6. |
| sharedTmpBuffer | Input | Temporary buffer. The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. The supported data type is: uint8_t.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetAscendDequantMaxMinTmpSize](GetAscendDequantMaxMinTmpSize.md). |
| params | Input | Shape information of **srcTensor**. The type is **DequantParams**, defined as shown in the following code, where the meaning of each parameter is as follows.<br>**m**: Number of rows of **srcTensor**.<br>**n**: Number of columns of **srcTensor**.<br>**calCount**: For each row of **srcTensor**, the first calCount numbers are valid data, which are multiplied with the first calCount numbers of **deqScale** or with the **deqScale** scalar.<br><br>Note:<br>**DequantParams.n** * sizeof(T) must be an integer multiple of 32 bytes, where T is the data type of the elements in **srcTensor**.<br>Because the multiplication is performed on the first calCount numbers in each group of n numbers, **DequantParams.n** and calCount must satisfy the following relationship: 1 <= **DequantParams.calCount** <= **DequantParams.n**.<br>When **deqScale** is a vector, **DequantParams.calCount** <= the element count of **deqScale**. |

```
struct DequantParams
{
    uint32_t m;
    uint32_t n;
    uint32_t calCount;
};
```

**Table 4**  PER\_TOKEN/PER\_GROUP scenario API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. Supported data types: half, bfloat16_t, float.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand. Supported data types: int32_t, float.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer. Supported data type: uint8_t.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAscendQuantMaxMinTmpSize](GetAscendDequantMaxMinTmpSize.md). |
| scaleTensor | Input | Quantization parameter scale. Supported data types: half, bfloat16_t, float.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| offsetTensor | Input | Quantization parameter offset. The supported data type is consistent with scaleTensor. Reserved parameter, currently not supported.<br><br>Type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| para | Input | Parameters of the dequantization API, defined in the code below, where the meanings of the parameters are as follows.<br>m: Element count in the m direction.<br>n: Element count in the n direction. The data size corresponding to the n value must satisfy the 32B alignment requirement, that is, both the input and output whose last shape dimension is n must satisfy the 32B alignment requirement on that dimension.<br>calCount: Element count participating in the computation. calCount must be a multiple of n.<br>groupSize: Valid in the PER_GROUP scenario, indicates that groupSize rows/columns of data share one scale/offset. The value of groupSize must be greater than 0 and a multiple of 32. |

```
struct AscendDeQuantParam {
        uint32_t m;
        uint32_t n;
        uint32_t calCount;
        uint32_t groupSize = 0;
}
```

**Table 5**  Supported data type combinations (deqScale is LocalTensor)

| dstTensor | srcTensor | deqScale |
| --- | --- | --- |
| half | int32_t | uint64_t<br><br>Note: When the data type of deqScale is uint64_t, the lower 32 bits of the value are the data participating in the computation, with the data type float, and the upper 32 bits are some control parameters, which are not used by this API. |
| float | int32_t | float |
| float | int32_t | bfloat16_t |
| bfloat16_t | int32_t | bfloat16_t |
| bfloat16_t | int32_t | float |

**Table 6**  Supported data type combinations (deqScale is a scalar)

| dstTensor | srcTensor | deqScale |
| --- | --- | --- |
| bfloat16_t | int32_t | bfloat16_t |
| bfloat16_t | int32_t | float |
| float | int32_t | bfloat16_t |
| float | int32_t | float |

**Table 7**  PER\_TOKEN/PER\_GROUP scenario supported data type combinations

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

-   **Address overlap between the source operand and the destination operand is not supported.**
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
-   PER\_TOKEN/PER\_GROUP scenario, the data volume in the continuous computation direction (that is, the n direction) must satisfy 32B alignment.
-   In non-PER\_TOKEN/PER\_GROUP scenarios, the data types supported by **dstTensor** are as follows:<a id="li17926145114504"></a>

    For Ascend 950PR/Ascend 950DT, the supported data types are: half, bfloat16\_t, and float.

    For Atlas A3 training products/Atlas A3 inference products, the supported data types are: half, bfloat16\_t, and float.

    For Atlas A2 training products/Atlas A2 inference products, the supported data types are: half, bfloat16\_t, and float.

    For Atlas inference products AI Core, the supported data types are: half and float.

-   In non-PER\_TOKEN/PER\_GROUP scenarios, the data types supported by **deqScale** are as follows:<a id="li189021550175211"></a>

    For Ascend 950PR/Ascend 950DT, when **deqScale** is a vector, the supported data types are: uint64\_t, float, and bfloat16\_t; when **deqScale** is a scalar, the supported data types are bfloat16\_t and float.

    For Atlas A3 training products/Atlas A3 inference products, when **deqScale** is a vector, the supported data types are: uint64\_t, float, and bfloat16\_t; when **deqScale** is a scalar, the supported data types are bfloat16\_t and float.

    For Atlas A2 training products/Atlas A2 inference products, when **deqScale** is a vector, the supported data types are: uint64\_t, float, and bfloat16\_t; when **deqScale** is a scalar, the supported data types are bfloat16\_t and float.

    For Atlas inference products AI Core, when **deqScale** is a vector, the supported data types are: uint64\_t and float; when **deqScale** is a scalar, the supported data type is float.

## Examples

```
// dstLocal: Stores the result tensor of the dequantization computation.
// srcLocal: Stores the input tensor of the dequantization computation.
// deqScaleLocal: Stores the input tensor of the dequantization scale for the dequantization computation.
// sharedTmpBuffer: Stores the tensor used as a temporary buffer during the dequantization computation.

uint32_t m = 4;  // Number of rows of srcTensor.
uint32_t n = 8;  // Number of columns of srcTensor.
uint32_t calCount = 6;  // The first calCount elements of each row of srcTensor participate in the dequantization computation.

// The dequantization parameter deqScale is a vector, and the temporary space is passed through the sharedTmpBuffer input parameter.
AscendC::AscendDequant(dstLocal, srcLocal, deqScaleLocal, sharedTmpBuffer, {m, n, calCount});

// The dequantization parameter deqScale is a vector, and the temporary space is allocated through the API framework.
AscendC::AscendDequant(dstLocal, srcLocal, deqScaleLocal, {m, n, calCount});

// The dequantization parameter deqScale is a scalar, and the temporary space is passed through the sharedTmpBuffer input parameter.
AscendC::AscendDequant(dstLocal, srcLocal, static_cast<float>(2.2), sharedTmpBuffer, {m, n, calCount});
// The dequantization parameter deqScale is a scalar. Allocate temporary space through the API framework.
AscendC::AscendDequant(dstLocal, srcLocal, static_cast<float>(2.2), {m, n, calCount});
```

The results are as follows:

```
Input data (srcLocal) of int32_t data type:
[[-16 -15 -14 -13 -12 -11 -10  -9]
 [ -8  -7  -6  -5  -4  -3  -2  -1]
 [  0   1   2   3   4   5   6   7]
 [  8   9  10  11  12  13  14  15]]

Dequantization parameter deqScaleLocal of float data type:
[2.2  -2.2  2.2  -2.2  2.2  -2.2  0.  0.]

Output data (dstLocal) of float data type:
[[-35.2  33.  -30.8  28.6 -26.4  24.2 -10.   -9. ]
 [-17.6  15.4 -13.2  11.   -8.8   6.6  -2.   -1. ]
 [  0.   -2.2   4.4  -6.6   8.8 -11.    6.    7. ]
 [ 17.6 -19.8  22.  -24.2  26.4 -28.6  14.   15. ]]
```

The call example for PER\_TOKEN/PER\_GROUP scenarios is as follows.

```
// Note that m and n must be passed in externally.
constexpr static bool isReuseSource = false;
constexpr static AscendDeQuantConfig config = {has_offset, -1};
constexpr static AscendDeQuantPolicy policy = AscendDeQuantPolicy::PER_TOKEN; // Modify the enumeration value to enable PER_GROUP.
AscendDeQuantParam para;
para.m = m;
para.n = n;
para.calCount = calCount;
AscendDequant<dstType, srcType, scaleType, config, policy>(dstLocal, srcLocal, scaleLocal, offsetLocal, para);
```
