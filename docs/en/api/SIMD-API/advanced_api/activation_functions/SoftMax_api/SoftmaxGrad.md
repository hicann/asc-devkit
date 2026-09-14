# SoftmaxGrad

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:56:27.856Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

The product of the non-last axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0) is regarded as m, and the shape of the input tensor is regarded as \[m, n\]. The grad backward computation is performed row by row on the input tensor\[m,n\], and the calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001855883134.png)

When the input shape is in ND format, the internal reduce process is performed along the last axis. When the input shape is in NZ format, the internal reduce process is performed along the last axis and the first axis. For details about the reduce process, see the illustration in [SoftMax](SoftMax.md).

For ease of understanding, the calculation formula is expressed by means of a Python script implementation as follows, where src, grad, and isFront are the source operands (inputs), and dst is the destination operand (output).

```
def softmax_grad(grad, src, isFront = None):
    dst = grad * src
    dst = np.sum(dst, axis=-1, keepdims=True)
    if isFront :
         return dst
    dst = (grad - dst) * src
    return dst
```

## Implementation Principle

Taking an input tensor of the float type in ND format with the shape \[m,k\] as an example, the internal algorithm block diagram of the SoftmaxGrad high-level API is described as follows.

**Figure 1**  SoftmaxGrad algorithm block diagram  
![](../../../../figures/softmaxgrad_algorithm_block_diagram.png "SoftmaxGrad algorithm block diagram")

The computation process consists of the following steps, all performed on the vector:

1.  mul step: Multiply all data of the inputs x and y. The calculation result is saved to a temporary space temp.
2.  reducesum step: Sum each row of the temp data \(\[m, k\]\) to obtain \[m, 1\]. The calculation result is saved to the temporary space.
3.  broadcast step: Fill the data of the reducesum result \[m, 1\] in units of datablock. For example, for the float type, expand \[m, 1\] into \[m, 8\];
4.  Determine whether the isFront mode is enabled. If yes, output the broadcast result and end the calculation; if no, continue with the subsequent steps;
5.  broadcast step: Expand the dimension of \[m, 8\] into \[m, k\], and save the calculation result to the temporary space;
6.  sub step: Subtract the broadcast result from the previous step from all data of input x;
7.  mul step: Multiply all data after the sub operation by input y, and output the result z.

## Prototype

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGrad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGrad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

For the method of application by the API framework, the developer needs to reserve temporary space; for the case of passing through sharedTmpBuffer, the developer needs to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: obtain the required maximum and minimum temporary space sizes through the GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize APIs provided in the [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md). The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. The supported data types are half and float. |
| isReuseSource | This parameter is reserved. Pass the default value **false**. |
| isDataFormatNZ | Whether the current input and output data format is the NZ format. The default data format is ND, that is, the default value is **false**.<br><br>For the Atlas 200I/500 A2 inference products, configuring the NZ format is not supported. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length requires 32-byte alignment. The shape of dstTensor is consistent with the shapes of gradTensor and srcTensor. |
| gradTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length requires 32-byte alignment. The shape of gradTensor is consistent with the shapes of dstTensor and srcTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length requires 32-byte alignment. The shape of srcTensor is consistent with the shapes of dstTensor and gradTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>It is used to store intermediate variables during complex computation inside the API and is provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md). |
| softmaxShapeInfo | Input | Shape information of srcTensor. The type is SoftMaxShapeInfo, whose definition is shown in the following code. The meanings of the parameters are as follows:<br>srcM: product of the non-last axis lengths.<br>srcK: last axis length, which must be 32-byte aligned.<br>oriSrcM: product of the original non-last axis lengths.<br>oriSrcK: original last axis length.<br><br>Note that when the input and output data format is the NZ format, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |
| tiling | Input | Tiling information required for the softmaxgrad calculation. For how to obtain the tiling information, see [SoftmaxGrad Tiling API](softmaxgrad_tiling_api.md). |
| isFront | Input | Whether to enable the isFront calculation. If set to **True**, the last axis length of dstTensor must be fixed to 32 bytes. |

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

-   The Tensor space of srcTensor and dstTensor can be reused.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   When **srcM** != **oriSrcM** or **srcK** != **oriSrcK** in the **softmaxShapeInfo** parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the API computation result overwrites the padded original data in **srcTensor**. In the non-reuse scenario, the API computation result overwrites the data at the corresponding padded positions in **dstTensor**.

## Examples

```
// dstLocal: Tensor that stores the SoftmaxGrad calculation result.
// gradLocal: Tensor that stores the input for the SoftmaxGrad computation.
// srcLocal: Tensor that stores the input for the SoftmaxGrad computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the SoftmaxGrad calculation.
// softmaxTiling: Stores the tiling information required for the SoftmaxGrad calculation, which can be obtained through the SoftMaxGradTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /*Product of the non-last axis lengths.*/ srcM,
    /*Last axis length, which must be 32-byte aligned.*/ srcK,
    /*Product of the original non-last axis lengths.*/ oriSrcM,
    /*Original last axis length.*/ oriSrcK
);
bool isFront = false;  // Do not enable isFront.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::SoftmaxGrad<T>(dstLocal, gradLocal, srcLocal, sharedTmpBuffer, softmaxTiling, isFront, softmaxInfo);
// The API framework allocates the temporary space.
AscendC::SoftmaxGrad<T>(dstLocal, gradLocal, srcLocal, softmaxTiling, isFront, softmaxInfo);
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
[[ 1260.     1300.     1340.     1360.     1400.     1420.     1430.     1440.   ]
 [  158.      160.      162.      164.      166.      168.      170.      172.   ]
 [   19.4      20.4      20.8      21.2      21.4      21.5      21.6      21.7  ]
 [    3.644     3.744     3.844     3.944     4.044     4.144     4.224     4.242]
 [   -3.044    -3.042    -3.024    -2.944    -2.844    -2.744    -2.644    -2.544]
 [  -17.      -16.9     -16.8     -16.7     -16.6     -16.4     -16.      -15.6  ]
 [ -143.     -142.     -140.     -138.     -136.     -134.     -132.     -130.   ]
 [-1078.    -1076.    -1066.    -1056.    -1036.     -996.     -976.     -936.   ]]
```
