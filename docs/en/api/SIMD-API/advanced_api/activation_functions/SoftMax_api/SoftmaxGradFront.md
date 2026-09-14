# SoftmaxGradFront

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:56:43.586Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The product of the non-last axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0) is regarded as m, and the shape of the input tensor is regarded as \[m, n\]. The gradfront backward computation is performed on the input tensor\[m,n\] row by row, and the calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001902028081.png)

When the input shape is in ND format, the internal reduce process is performed along the last axis; when the input shape is in NZ format, the internal reduce process is performed along the last axis and the first axis. For details about the reduce process, see the illustration in [SoftMax](SoftMax.md).

For ease of understanding, the calculation formula is expressed by means of a Python script implementation as follows, where dx and y are the source operands (inputs), and d is the destination operand (output).

```
def softmax_grad_front(dx, y, is_fp16=False):
    dx = dx.astype(np.float32)
    y = y.astype(np.float32)

    d = (dx * y).sum(axis=-1, keepdims=True)  ###[1024,1]
    if is_fp16:
        d = d.astype(np.float16)
    return d
```

## Implementation Principle

Taking an input tensor of the float type in ND format with the shape \[m, k\] as an example, the internal algorithm block diagram of the SoftmaxGradFront high-level API is described as follows.

**Figure 1**  SoftmaxGradFront algorithm block diagram  
![](../../../../figures/softmaxgradfront_algorithm_block_diagram.png "SoftmaxGradFront algorithm block diagram")

The computation process consists of the following steps, all performed on the vector:

1.  mul step: Multiply all data of the inputs x and y, and save the computation result to a temporary space temp.
2.  reducesum step: Sum the data in each row of \(\[m, k\]\) in temp to obtain \[m, 1\], and save the computation result to the temporary space.
3. broadcast step: Fill \[m, 1\] in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output the result z.

## Prototype

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGradFront(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGradFront(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated for and released, offering high flexibility and high memory utilization.

When the temporary space is allocated by the API framework, the developer needs to reserve the temporary space. When it is passed through sharedTmpBuffer, the developer needs to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: obtain the required minimum and maximum temporary space sizes through the GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize APIs provided in [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md). The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isBasicBlock | When the shape information of srcTensor and gradTensor and the Tiling splitting strategy meet the basic block requirements, this parameter can be set to true to improve performance. The default value is false, indicating that it is not enabled. Whether the basic block requirements are met can be determined in either of the following two ways:<br>The shape information [m,n] of srcTensor and dstTensor must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, n is at least 128 in the half scenario and at least 64 in the float scenario), and n is a multiple of 64. The product m of the non-last axis lengths is a multiple of 8.<br><br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling splitting strategy meets the basic block splitting requirements.<br><br>For the Atlas 200I/500 A2 inference product, this parameter is reserved and not yet enabled. It is retained for future function extension. Keep the default value. |
| isDataFormatNZ | Whether the current input and output data format is the NZ format. The default data format is ND, that is, the default value is false.<br><br>For the Atlas 200I/500 A2 inference product, configuring the NZ format is not supported. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length is fixed at 32 bytes, that is, one datablock length, and all data in this datablock is the same value. For example, for the half data type, the 16 values in this datablock are all the same value. The non-last axis length must be consistent with srcTensor. |
| gradTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. The shape of gradTensor is consistent with the shape of srcTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. The shape of srcTensor is consistent with the shape of gradTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>It is used to store intermediate variables during complex computation inside the API and is provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md). |
| tiling | Input | Tiling information required for softmaxgradfront computation. For how to obtain the Tiling information, see [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md). |
| softmaxShapeInfo | Input | Shape information of srcTensor. The type is SoftMaxShapeInfo, whose definition is shown in the following code. The meanings of the parameters are as follows:<br>srcM: product of the non-last axis lengths.<br>srcK: last axis length, which must be 32-byte aligned.<br>oriSrcM: product of the original non-last axis lengths.<br>oriSrcK: original last axis length.<br><br>Note that when the input and output data format is the NZ format, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |

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

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   When **srcM** != **oriSrcM** or **srcK** != **oriSrcK** in the **softmaxShapeInfo** parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the API computation result overwrites the padded original data in **srcTensor**. In the non-reuse scenario, the API computation result overwrites the data at the corresponding padded positions in **dstTensor**.

## Examples

```
// dstLocal: Tensor that stores the SoftmaxGradFront computation result.
// gradLocal: Stores the input tensor for the SoftmaxGradFront computation.
// srcLocal: Stores the input tensor for the SoftmaxGradFront computation.
// sharedTmpBuffer: Stores the tensor temporarily buffered during the SoftmaxGradFront computation.
// softmaxTiling: Stores the Tiling information required for the SoftmaxGradFront computation, which can be obtained through the SoftMaxGradTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /*Product of the non-last axis lengths.*/ srcM,
    /*Last axis length, which must be 32-byte aligned.*/ srcK,
    /*Product of the original non-last axis lengths.*/ oriSrcM,
    /*Original last axis length.*/ oriSrcK
);

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::SoftmaxGradFront<T>(dstLocal, gradLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// The API framework allocates temporary space.
AscendC::SoftmaxGradFront<T>(dstLocal, gradLocal, srcLocal, softmaxTiling, softmaxInfo);
```

The results are as follows:

```
Input data (gradLocal):
[[-100.     -80.     -60.     -50.     -30.     -20.     -15.     -10.   ]
 [  -9.      -8.      -7.      -6.      -5.      -4.      -3.      -2.   ]
 [  -1.5     -1.      -0.8     -0.6     -0.5     -0.45    -0.4     -0.35 ]
 [  -0.3     -0.25    -0.2     -0.15    -0.1     -0.05    -0.01    -0.001]
 [   0.       0.001    0.01     0.05     0.1      0.15     0.2      0.25 ]
 [   0.3      0.35     0.4      0.45     0.5      0.6      0.8      1.   ]
 [   1.5      2.       3.       4.       5.       6.       7.       8.   ]
 [   9.      10.      15.      20.      30.      50.      60.      80.   ]]
Input data (srcLocal):
[[2. 2. 2. 2. 2. 2. 2. 2.]
 ...
 [2. 2. 2. 2. 2. 2. 2. 2.]]
Output data (dstLocal):
[[-730.   ]
 [ -88.   ]
 [ -11.2  ]
 [  -2.122]
 [   1.522]
 [   8.8  ]
 [  73.   ]
 [ 548.   ]]
```
