# SoftmaxGrad

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:55:59.953Z pushedAt=2026-10-29T02:14:34.109Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftmaxGrad_res.md#id1 -->

## Description

Take the product of the non-last-axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0) as m, and regard the shape of the input tensor as \[m, n\]. Perform grad backward computation on the input tensor\[m,n\] row by row. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001855883134.png)

When the input shape is in ND format, the internal reduce process is performed along the last axis. When the input shape is in NZ format, the internal reduce process is performed along the last axis and the first axis. For details about the reduce process, see the illustration in [SoftMax](SoftMax.md).

For ease of understanding, the computation formula is expressed through a Python script implementation as follows, where src, grad, and isFront are source operands (inputs), and dst is the destination operand (output).

```
def softmax_grad(grad, src, isFront = None):
    dst = grad * src
    dst = np.sum(dst, axis=-1, keepdims=True)
    if isFront :
         return dst
    dst = (grad - dst) * src
    return dst
```

## Implementation Principles

Using an input tensor of the float type in ND format with the shape \[m,k\] as an example, the following figure shows the internal algorithm diagram of the SoftmaxGrad high-level API.

**Figure 1**  SoftmaxGrad algorithm diagram  
![](../../../../figures/softmaxgrad_algorithm_diagram.png "SoftmaxGrad algorithm diagram")

The computation process consists of the following steps, all performed on the Vector:

1.  mul step: Multiply all data of the input x and y. The computation result is saved to a temporary space temp.
2.  reducesum step: Sum each row of the temp data \(\[m, k\]\) to obtain \[m, 1\]. The computation result is saved to the temporary space.
3.  broadcast step: Fill the data of the reducesum result \[m, 1\] in units of data blocks. For example, for the float type, expand \[m, 1\] to \[m, 8\].
4.  Determine whether the isFront mode is used. If yes, output the broadcast result and end the computation. If no, continue with the subsequent steps.
5.  broadcast step: Expand the dimension of \[m, 8\] to \[m, k\]. The computation result is saved to the temporary space.
6.  sub step: Subtract the broadcast result of the previous step from all data of the input x.
7.  mul step: Multiply all data after the sub step by the input y, and output the result z.

## Prototype

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGrad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGrad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **allocated by the API framework** and **passed by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

When the temporary space is allocated by the API framework, you need to reserve the temporary space. When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. Obtain the temporary space size BufferSize as follows: use the GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize APIs provided in the [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, and the maximum space improves performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | This parameter is reserved. Pass the default value false. |
| isDataFormatNZ | Whether the current input and output data format is NZ. The default data format is ND, that is, the default value is false. <!-- npu="310b" id7 --><br><br>For the Atlas 200I/500 A2 inference product, the NZ format is not supported. <!-- end id7 --> |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. The shape of dstTensor is consistent with the shapes of gradTensor and srcTensor. |
| gradTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. The shape of gradTensor is consistent with the shapes of dstTensor and srcTensor. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned. The shape of srcTensor is consistent with the shapes of dstTensor and gradTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md). |
| softmaxShapeInfo | Input | Shape information of srcTensor. The type is SoftMaxShapeInfo, defined in the following code. The parameters are described as follows:<br>srcM: product of the non-last-axis lengths.<br>srcK: last axis length, which must be 32-byte aligned.<br>oriSrcM: product of the original non-last-axis lengths.<br>oriSrcK: original last axis length.<br><br>Note that when the input and output data format is NZ, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |
| tiling | Input | Tiling information required for the softmaxgrad computation. For how to obtain the tiling information, see [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md). |
| isFront | Input | Whether to enable the isFront computation. If True, the last axis length of dstTensor must be fixed to 32 bytes. |

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

-   The tensor space of srcTensor and dstTensor can be reused.
-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
<!-- npu="A3,910b,310p" id8 -->
-   When srcM != oriSrcM or srcK != oriSrcK in the softmaxShapeInfo parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on the GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. When the input and output are reused, the API's computation result overwrites the padded original data in srcTensor. When the input and output are not reused, the API's computation result overwrites the data in dstTensor at the positions corresponding to the padded positions of srcTensor.
<!-- end id8 -->

## Examples

```
// dstLocal: Tensor that stores the SoftmaxGrad computation result.
// gradLocal: Input tensor for the SoftmaxGrad computation.
// srcLocal: Input tensor for the SoftmaxGrad computation.
// sharedTmpBuffer: Tensor that stores the temporary buffer during SoftmaxGrad computation.
// softmaxTiling: Stores the Tiling information required for SoftmaxGrad computation, which can be obtained through the SoftMaxGradTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* Product of non-last axis lengths          */ srcM,
    /* Last axis length, which must be 32-byte aligned */ srcK,
    /* Original product of non-last axis lengths */ oriSrcM,
    /* Original last axis length                 */ oriSrcK);
bool isFront = false; // Disable isFront computation.

// Pass the temporary space through the sharedTmpBuffer input parameter.
AscendC::SoftmaxGrad<T>(dstLocal, gradLocal, srcLocal, sharedTmpBuffer, softmaxTiling, isFront, softmaxInfo);
// The API framework allocates the temporary space.
AscendC::SoftmaxGrad<T>(dstLocal, gradLocal, srcLocal, softmaxTiling, isFront, softmaxInfo);
```

The result is as follows:

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
