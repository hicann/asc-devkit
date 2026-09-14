# SoftmaxFlashV2

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:52:01.856Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

SoftmaxFlashV2 is an enhanced version of SoftmaxFlash, corresponding to the FlashAttention-2 algorithm. The product of the non-last-axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0) is regarded as m, and the shape of the input tensor is regarded as \[m, n\]. The following computation is performed row by row on the input tensor\[m,n\], where different update values correspond to different formulas. Here, x, inmax, and insum are inputs, and M, S, and E are outputs.

-   When update is false:

    ![](../../../../figures/zh-cn_formulaimage_0000002023909837.png)

-   When update is true:

    ![](../../../../figures/zh-cn_formulaimage_0000002023912661.png)

When the input shape is in ND format, the internal reduce process is performed along the last axis. When the input shape is in NZ format, the internal reduce process is performed along the last axis and the first axis. For details about the reduce process, see the illustration in [SoftMax](SoftMax.md).

For ease of understanding, the calculation formulas are expressed through a Python script implementation as follows, where src, inmax, insum, and update are inputs, and dst, x\_sum, x\_max, and exp\_max are outputs.

```
def softmax_flash_2(src, inmax=None, insum=None, update=None):
    if update is None:
        x_max = np.max(src, axis=-1, keepdims=True)
        x_sub = src - x_max
        dst = np.exp(x_sub)
        x_sum = np.sum(dst, axis=-1, keepdims=True)
        exp_max = None
        return dst, x_max, x_sum, exp_max
    else:
        x_max = np.max(np.concatenate((inmax, src), axis=-1), axis=-1, keepdims=True)
        dst = np.exp(src - x_max)
        exp_max = np.exp(inmax - x_max)
        x_sum = np.sum(dst, axis=-1, keepdims=True)
        x_sum = exp_max * insum +  x_sum
        return dst, x_max, x_sum, exp_max
```

## Implementation Principle

Taking an input tensor of the float type in ND format with the shape \[m, k\] as an example, the internal algorithm block diagram of the SoftmaxFlashV2 high-level API is described as follows.

**Figure 1**  SoftmaxFlashV2 algorithm block diagram  
![](../../../../figures/softmaxflashv2_algorithm_block_diagram.png "SoftmaxFlashV2 algorithm block diagram")

The computation process is divided into two branches based on whether **isUpdate** is enabled, both of which are performed on the Vector.

-   When **isUpdate** is False, the process is divided into the following steps:
    1.  reducemax step: Compute the maximum value of each row of the input x to obtain \[m, 1\]. The computation result is saved to a temporary space temp.
    2.  broadcast step: Fill the data \[m, 1\] in temp in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output max at the same time;
    3.  sub step: Subtract max from all data of the input x row by row.
    4.  exp step: Compute the exp of all data after the sub operation, and output y;
    5.  reducesum step: Compute the sum of each row of the exp result to obtain \[m, 1\]. The computation result is saved to the temporary space temp;
    6.  broadcast step: Fill temp\[m, 1\] in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output sum at the same time.

-   When **isUpdate** is True, the process is divided into the following steps:
    1.  reducemax step: Compute the maximum value of each row of the input x to obtain \[m, 1\]. The computation result is saved to a temporary space temp.
    2.  broadcast step: Fill the data \[m, 1\] in temp in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and save it as max;
    3.  max step: Perform the max operation on the input inmax and the max computed in the previous step to obtain a new max and output it;
    4.  sub step: Subtract the new max from the input inmax, then perform the exp operation to compute and output expmax.
    5.  sub step: Subtract the new max from the input x row by row.
    6.  exp step: Compute the exp of all data after the sub operation, and output y.
    7.  reducesum step: Compute the sum of each row of the exp result to obtain \[m, 1\]. The computation result is saved to the temporary space temp.
    8.  broadcast step: Fill the temp data \[m, 1\] in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and save the result to sum.
    9.  mul step: Multiply the input insum by the expmax result.
    10. add step: Add the multiplication result to sum, save the result to sum, and output it.

## Prototype<a name="section620mcpsimp"></a>

-   The API framework allocates temporary space.
    -   The data types of **LocalTensor** are the same, and **ReduceMax** is not output.

        ```
        template <typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftmaxFlashV2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& expSumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inExpSumTensor, const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The data types of **LocalTensor** are the same, and **ReduceMax** is output.

        ```
        template <typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftmaxFlashV2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& outReduceMax, const LocalTensor<T>& outExpSum, const LocalTensor<T>& outMax, const LocalTensor<T>& srcTensor, const LocalTensor<T>& outExpMax, const LocalTensor<T>& inExpSum, const LocalTensor<T>& inMax, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

        Atlas 200I/500 A2 inference products do not support this API.

        The AI Core of Atlas inference products does not support this API.

    -   The data types of **LocalTensor** are different, and **ReduceMax** is not output.

        ```
        template <typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftmaxFlashV2(const LocalTensor<half>& dstTensor, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inExpSumTensor, const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The data types of **LocalTensor** are the same, and **ReduceMax** is not output.

        ```
        template <typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftmaxFlashV2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& outExpSum, const LocalTensor<T>& outMax, const LocalTensor<T>& srcTensor, const LocalTensor<T>& outExpMax, const LocalTensor<T>& inExpSum, const LocalTensor<T>& inMax, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

    -   The data types of **LocalTensor** are the same, and **ReduceMax** is output.

        ```
        template <typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftmaxFlashV2(const LocalTensor<T>& dstTensor, const LocalTensor<T>& outReduceMax, const LocalTensor<T>& expSumTensor, const LocalTensor<T>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inExpSumTensor, const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

        Atlas 200I/500 A2 inference products do not support this API.

        The AI Core of Atlas inference products does not support this API.

    -   When the data types of the LocalTensors differ, ReduceMax is not output.

        ```
        template <typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
        __aicore__ inline void SoftmaxFlashV2(const LocalTensor<half>& dstTensor, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor, const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inExpSumTensor, const LocalTensor<float>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation. The temporary space can be applied for by the **API framework** or passed in by the developer through the **sharedTmpBuffer** input parameter.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When the temporary space is applied for by the API framework, the developer needs to reserve the temporary space. When it is passed in through **sharedTmpBuffer**, the developer needs to allocate space for the tensor. The temporary space size **BufferSize** is obtained as follows: use the **GetSoftMaxFlashV2MinTmpSize**/**GetSoftMaxFlashV2MaxTmpSize** APIs provided in [SoftmaxFlashV2 Tiling API](softmaxflashv2_tiling_api.md) to obtain the required minimum and maximum temporary space sizes. The minimum space ensures functional correctness, and the maximum space is used to improve performance.

In addition, a kernel-side tiling computation API is provided. When the input shape on the kernel side is inconsistent with the shape passed in through the host-side TilingData, this API can be used to recompute the tiling on the kernel side. For the meaning of the parameters of this API, see [SoftmaxFlashV2 Tiling API](softmaxflashv2_tiling_api.md).

-   **Kernel-side tiling computation API**

    ```
    __aicore__ inline constexpr SoftMaxTiling SoftMaxFlashV2TilingFunc(const SoftMaxShapeInfo& shapeInfo, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2, const uint32_t localWorkSpaceSize, const bool isUpdate = false, const bool isBasicBlock = false, const bool isDataFormatNZ = false, const bool isFlashOutputBrc = false)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isUpdate | Whether to enable the computation in the update part. |
| isReuseSource | This parameter is reserved. Pass the default value **false**. |
| isBasicBlock | When the shape information of **srcTensor** and **dstTensor** and the Tiling splitting strategy meet the basic block requirements, this parameter can be set to **true** to improve performance. The default value is **false**, indicating that it is not enabled. Whether the basic block requirements are met can be determined in either of the following two ways:<br>The shape information [m, n] of **srcTensor** and **dstTensor** must meet the following conditions: the last-axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, n is at least 128 in the half scenario and at least 64 in the float scenario), and n is a multiple of 64. The product m of the non-last-axis lengths is a multiple of 8.<br><br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling splitting strategy meets the basic block splitting requirements.<br><br>For Atlas 200I/500 A2 inference products, this parameter is reserved and not yet enabled. It is reserved for future function extension. Keep the default value. |
| isDataFormatNZ | Whether the current input and output data format is the NZ format. The default data format is ND, that is, the default value is **false**.<br><br>For Atlas 200I/500 A2 inference products, configuring the NZ format is not supported. |
| config | Struct template parameter. This parameter is optional and of the **SoftmaxConfig** type. Its definition is shown in the following code. The meanings of its parameters are as follows:<br>**isCheckTiling**: Whether to check the consistency between the shape and the tiling. If they are inconsistent, the API recomputes the required tiling based on the shape. The default value is **true**: the API checks the consistency internally.<br>**oriSrcM**: Product of the original non-last-axis lengths. After this parameter is set, the shape is made constant, and the constant shape is used during compilation.<br>**oriSrcK**: Original last-axis length. After this parameter is set, the shape is made constant, and the constant shape is used during compilation.<br>**mode**: Processing mode of the output shape. When the input and output data format is the NZ format, configuring the **mode** parameter is not supported. It is of the **SoftmaxMode** type, with the following values:<br>**SOFTMAX_NORMAL**: Default value, the normal mode. Broadcast is performed on the output data so that the output shape is expanded from (m, 1) to (m, 8) (when the output is of the float data type) or (m, 16) (when the output is of the half data type).<br>**SOFTMAX_OUTPUT_WITHOUT_BRC**: Non-expansion mode. Broadcast is not performed on the output data. The output shape is (m, 1), and the corresponding input parameters (for example, **inExpSumTensor** and **inMaxTensor**) also have the shape (m, 1).<br><br>This parameter is generally used together with the kernel-side tiling computation API.<br><br>Note: After **oriSrcM** and **oriSrcK** are set, the template parameter **isBasicBlock** does not take effect. Whether the computed data is a basic block is determined and processed internally by the API.<br><br>For Ascend 950PR/Ascend 950DT, this parameter is supported.<br><br>For Atlas A3 training products/Atlas A3 inference products, this parameter is supported.<br><br>For Atlas A2 training products/Atlas A2 inference products, this parameter is supported.<br><br>For Atlas 200I/500 A2 inference products, this parameter is reserved and not yet enabled. Keep the default value.<br><br>For Atlas inference products AI Core, this parameter is supported, but configuring **mode** is not supported. |

```
struct SoftmaxConfig{
    bool isCheckTiling = true;
    uint32_t oriSrcM = 0;
    uint32_t oriSrcK = 0;
    SoftmaxMode mode = SoftmaxMode::SOFTMAX_NORMAL;
};
```

The following is a configuration example.

```
constexpr SoftmaxConfig SOFTMAX_DEFAULT_CFG = {true, 0, 0, SoftmaxMode::SOFTMAX_NORMAL};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of **dstTensor** is the same as that of the source operand **srcTensor**. |
| outReduceMax | Output | Destination operand. Used to save the result of the first reducemax computation during the softmax computation.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The shape of **outReduceMax** is the same as that of the destination operand **maxTensor**.<br><br>For the API that outputs this result:<br>When the template parameter **isUpdate** is **false**, this result is not output.<br>Only the ND input and output data format is supported. The template parameter **isDataFormatNZ** is reserved. Pass the default value **false**.<br>The template parameter **config.isCheckTiling** is reserved. Pass the default value **false**.<br>The template parameter **config.mode** can only be configured as the non-expansion mode **SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**. |
| expSumTensor, outExpSum | Output | Destination operand. Used to save the result of the reducesum computation during the softmax computation.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>Except in the scenario where the template parameter **config** is configured as the non-expansion mode (**SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**), the last-axis length of **expSumTensor** is fixed to 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same reducesum value.<br>The non-last-axis length is the same as that of **dstTensor**. |
| maxTensor, outMax | Output | Destination operand. Used to save the result of the reducemax computation during the softmax computation.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>Except in the scenario where the template parameter **config** is configured as the non-expansion mode (**SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**), the last-axis length of **maxTensor** is fixed to 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same reducemax value.<br>The non-last-axis length is the same as that of **dstTensor**. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last-axis length must be 32-byte aligned. |
| expMaxTensor, outExpMax | Output | Destination operand. Used to save the result of the exponential power of e raised to the difference between inmax and reducemax.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>Except in the scenario where the template parameter **config** is configured as the non-expansion mode (**SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**), the last-axis length of **expMaxTensor** is fixed to 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last-axis length must be the same as that of **dstTensor**. |
| inExpSumTensor, inExpSum | Input | Source operand. The sum value required for the softmax computation.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>Except in the scenario where the template parameter **config** is configured as the non-expansion mode (**SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**), the last-axis length of **inExpSumTensor** is fixed to 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last-axis length must be the same as that of **dstTensor**. |
| inMaxTensor, inMax | Input | Source operand. The max value required for the softmax computation.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>Except in the scenario where the template parameter **config** is configured as the non-expansion mode (**SoftmaxMode::SOFTMAX_OUTPUT_WITHOUT_BRC**), the last-axis length of **inMaxTensor** is fixed to 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, in the half data type, the 16 numbers in this datablock are all the same value.<br>The non-last-axis length must be the same as that of **dstTensor**. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>It is used to store intermediate variables during the complex computation inside the API and is provided by the developer.<br><br>For how to obtain the temporary space size **BufferSize**, see [SoftmaxFlashV2 Tiling API](softmaxflashv2_tiling_api.md). |
| tiling | Input | Tiling information required for the computation of the softmaxflashv2 API. For how to obtain the Tiling information, see [SoftmaxFlashV2 Tiling API](softmaxflashv2_tiling_api.md). |
| softmaxShapeInfo | Input | Shape information of **srcTensor**. It is of the **SoftMaxShapeInfo** type. Its definition is shown in the following code. The meanings of its parameters are as follows:<br>**srcM**: Product of the non-last-axis lengths.<br>**srcK**: Last-axis length, which must be 32-byte aligned.<br>**oriSrcM**: Product of the original non-last-axis lengths.<br>**oriSrcK**: Original last-axis length.<br><br>Note that when the input and output data format is the NZ format, the last-axis length is the reduce-axis length, that is, W0\*W1 in [Figure 2](SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |

```
struct SoftMaxShapeInfo {
  uint32_t srcM;
  uint32_t srcK;
  uint32_t oriSrcM;
  uint32_t oriSrcK;
};
```

## Return Value

None

## Constraints

-   The tensor space of **srcTensor** and **dstTensor** can be reused, the space of **maxTensor** and **inMaxTensor** can be reused, and the space of **expSumTensor** and **inExpSumTensor** can be reused.
-   Except in the scenario where the template parameter **config** is configured as the non-expansion mode (SoftmaxMode::SOFTMAX\_OUTPUT\_WITHOUT\_BRC), the last-axis length of the tensor space of **expSumTensor**, **maxTensor**, **expMaxTensor**, **inExpSumTensor**, and **inMaxTensor** must be fixed to 32 bytes.
-   For the API that outputs ReduceMax:
    -   The template parameters **isReuseSource**, **isDataFormatNZ**, and **config.isCheckTiling** are reserved parameters.
    -   **config.mode** supports only the non-expansion mode SOFTMAX\_OUTPUT\_WITHOUT\_BRC. When it is configured as the SOFTMAX\_NORMAL mode, the API does not execute and does not save any outputs.
    -   When the template parameter **isUpdate** is **false**, **outReduceMax** is not output.
    -   Except for **outReduceMax**, the computation result of each remaining output is the same as that of the [API that does not output ReduceMax](#section620mcpsimp).

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   When **srcM** != **oriSrcM** or **srcK** != **oriSrcK** in the **softmaxShapeInfo** parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the API computation result overwrites the padded original data in **srcTensor**. In the non-reuse scenario, the API computation result overwrites the data at the corresponding padded positions in **dstTensor**.

## Examples

-   **srcK** alignment

    In this example, the shape size of the input **srcTensor** and the output **dstTensor** is \[320,64\], the shape size of the input **inSumTensor** and **inMaxTensor** is \[320,16\], and the shape size of the output **expMaxTensor** is \[320,16\]. The data type is **half**, the data layout of the inputs and outputs is ND, the **srcTensor** and **dstTensor** spaces are not reused, the basic block is not enabled, and **isUpdate** is **true**.

    ```
    // dstLocal: Tensor that stores the SoftMax computation result.
    // expSumLocal: Stores the reducesum result during the softmax computation.
    // maxLocal: Stores the reducemax result during the softmax computation.
    // srcLocal: Stores the input tensor for the SoftMax computation.
    // expMaxLocal: Stores the result of e raised to the power of the difference between inmax and reducemax.
    // inExpSumLocal: Stores the sum value required for the softmax computation.
    // inMaxLocal: Stores the max value required for the softmax computation.
    // sharedTmpBuffer: Stores the tensor used as a temporary buffer during the SoftMax computation.
    // softmaxTiling: Stores the Tiling information required for the SoftMax computation, which can be obtained through the SoftMaxFlashV2TilingFunc API.

    AscendC::SoftMaxShapeInfo softmaxInfo(
        /* Product of non-last-axis lengths.          */ 320,
        /* Last-axis length, which must be 32-byte aligned. */ 64,
        /* Original product of non-last-axis lengths.      */ 320,
        /* Original last axis length.*/ 64
    );

    // Pass the temporary space through the sharedTmpBuffer input parameter, do not output ReduceMax, and pass template parameters to make the shape constant.
    AscendC::SoftmaxFlashV2<T, true, false, false, false, static_config>(dstLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inExpSumLocal, inMaxLocal, sharedTmpBuffer, tiling, softmaxInfo);
    // Pass the temporary space through the sharedTmpBuffer input parameter and do not output ReduceMax.
    AscendC::SoftmaxFlashV2<T, true>(dstLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inExpSumLocal, inMaxLocal, sharedTmpBuffer, tiling, softmaxInfo);
    // The API framework allocates temporary space, with sumTensor and maxTensor parameters.
    AscendC::SoftmaxFlashV2<T, true>(dstLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inExpSumLocal, inMaxLocal, tiling, softmaxInfo);
    ```

    The results are as follows:

    ```
    Input data (srcLocal):
    [[-10.    -10.    -10.    ...  -9.94   -9.94   -9.94 ]
     [ -9.94   -9.94   -9.94  ...  -9.875  -9.875  -9.875]
     [ -9.875  -9.875  -9.875 ...  -9.81   -9.81   -9.81 ]
     ...
     [  9.81    9.81    9.81  ...   9.875   9.875   9.875]
     [  9.875   9.875   9.875 ...   9.94    9.94    9.94 ]
     [  9.94    9.94    9.94  ...  10.     10.     10.   ]]
  Output data (expSumLocal):
    [[62.03 62.03 62.03 ... 62.03 62.03 62.03]
     [62.03 62.03 62.03 ... 62.03 62.03 62.03]
     [62.03 62.03 62.03 ... 62.03 62.03 62.03]
     ...
     [62.03 62.03 62.03 ... 62.03 62.03 62.03]
     [62.03 62.03 62.03 ... 62.03 62.03 62.03]
     [62.03 62.03 62.03 ... 62.03 62.03 62.03]]
   Output data (maxLocal):
    [[-9.94  -9.94  -9.94  ... -9.94  -9.94  -9.94 ]
     [-9.875 -9.875 -9.875 ... -9.875 -9.875 -9.875]
     [-9.81  -9.81  -9.81  ... -9.81  -9.81  -9.81 ]
     ...
     [ 9.875  9.875  9.875 ...  9.875  9.875  9.875]
     [ 9.94   9.94   9.94  ...  9.94   9.94   9.94 ]
     [10.    10.    10.    ... 10.    10.    10.   ]]
   Output data (dstLocal):
    [[0.015144 0.015144 0.015144 ... 0.01611  0.01611  0.01611 ]
     [0.015144 0.015144 0.015144 ... 0.01611  0.01611  0.01611 ]
     [0.015144 0.015144 0.015144 ... 0.01611  0.01611  0.01611 ]
     ...
     [0.015144 0.015144 0.015144 ... 0.01611  0.01611  0.01611 ]
     [0.015144 0.015144 0.015144 ... 0.01611  0.01611  0.01611 ]
     [0.015144 0.015144 0.015144 ... 0.01611  0.01611  0.01611 ]]
    ```

-   srcK is non-aligned

    In this example, the shape of the input srcTensor and the output dstTensor is \[320,63\], the data type is half, and the input/output data layout is ND. This example demonstrates the copy-in and copy-out operations with non-aligned padding and the API call method.

    ```
    #include "kernel_operator.h"
    // In the init phase, height=320 and width=63.
    padWidth = AlignUp(width * sizeof(T), 32) / sizeof(T);
    // Copy-in phase.
    AscendC::DataCopyExtParams copyParams{static_cast<uint16_t>(height), static_cast<uint32_t>(width * sizeof(T)), 0, 0, 0};
    AscendC::DataCopyPadExtParams<T> padParam = {true, 0, static_cast<uint8_t>(padWidth - width), 0};
    AscendC::DataCopyPad(srcLocal, srcGlobal, copyParams, padParam);

    // Compute phase.
    // Because padding occurs, the shape used in the API call differs from the original shape.
    AscendC::SoftMaxShapeInfo srcShape = {height, padWidth, height, width};
    AscendC::SoftmaxFlashV2<T, true>(dstLocal, expSumLocal, maxLocal, srcLocal, expMaxLocal, inExpSumLocal, inMaxLocal, tiling, srcShape);
    // Copy-out phase.
    AscendC::DataCopyExtParams copyParams{static_cast<uint16_t>(height), static_cast<uint32_t>(width * sizeof(T)), 0, 0, 0};
    AscendC::DataCopyPad(dstGlobal, dstLocal, copyParams);
    ```

    The results are as follows:

    ```
    Input data (srcLocal):
    [[-10.    -10.    -10.    ...  -9.94   -9.94    0.   ]
     [ -9.94   -9.94   -9.94  ...  -9.875  -9.875   0.   ]
     [ -9.875  -9.875  -9.875 ...  -9.81   -9.81    0.   ]
     ...
     [  9.81    9.81    9.81  ...   9.875   9.875   0.   ]
     [  9.875   9.875   9.875 ...   9.94    9.94    0.   ]
     [  9.94    9.94    9.94  ...  10.     10.      0.   ]]
   Output data (expSumLocal):
    [[61.03 61.03 61.03 ... 61.03 61.03 61.03]
     [61.03 61.03 61.03 ... 61.03 61.03 61.03]
     [61.03 61.03 61.03 ... 61.03 61.03 61.03]
     ...
     [61.1  61.1  61.1  ... 61.1  61.1  61.1 ]
     [61.1  61.1  61.1  ... 61.1  61.1  61.1 ]
     [61.1  61.1  61.1  ... 61.1  61.1  61.1 ]]
    Output data (maxLocal):
    [[-9.94  -9.94  -9.94  ... -9.94  -9.94  -9.94 ]
     [-9.875 -9.875 -9.875 ... -9.875 -9.875 -9.875]
     [-9.81  -9.81  -9.81  ... -9.81  -9.81  -9.81 ]
     ...
     [ 9.875  9.875  9.875 ...  9.875  9.875  9.875]
     [ 9.94   9.94   9.94  ...  9.94   9.94   9.94 ]
     [10.    10.    10.    ... 10.    10.    10.   ]]
    Output data (dstLocal):
    [[0.015396 0.015396 0.015396 ... 0.01639  0.01639  0.01639 ]
     [0.015396 0.015396 0.015396 ... 0.01639  0.01639  0.01639 ]
     [0.015396 0.015396 0.015396 ... 0.01639  0.01639  0.01639 ]
     ...
     [0.01538  0.01538  0.01538  ... 0.01637  0.01637  0.01637 ]
     [0.01538  0.01538  0.01538  ... 0.01637  0.01637  0.01637 ]
     [0.01538  0.01538  0.01538  ... 0.01637  0.01637  0.01637 ]]
    ```
