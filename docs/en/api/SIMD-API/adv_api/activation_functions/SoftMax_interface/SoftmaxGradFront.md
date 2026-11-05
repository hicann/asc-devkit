# SoftmaxGradFront

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:59:54.090Z pushedAt=2026-10-29T02:14:34.115Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/SoftmaxGradFront_res.md#id1 -->

## Description

Let m be the product of the non-last-axis lengths of the input tensor\[m<sub>0</sub>, m<sub>1</sub>, ...m<sub>t</sub>, n\] (where t is greater than or equal to 0). The shape of the input tensor is then regarded as \[m, n\]. The gradfront backward computation is performed row by row on the input tensor\[m,n\], using the following formula:

![](../../../../figures/zh-cn_formulaimage_0000001902028081.png)

When the input shape is in ND format, the internal reduce process is performed along the last axis. When the input shape is in NZ format, the internal reduce process is performed along the last axis and the first axis. For details about the reduce process, see the illustration in [SoftMax](SoftMax.md).

For ease of understanding, the computation formula is expressed through a Python script implementation as follows, where dx and y are the source operands (inputs) and d is the destination operand (output).

```
def softmax_grad_front(dx, y, is_fp16=False):
    dx = dx.astype(np.float32)
    y = y.astype(np.float32)

    d = (dx * y).sum(axis=-1, keepdims=True)  ###[1024,1]
    if is_fp16:
        d = d.astype(np.float16)
    return d
```

## Implementation Principles

Using a float-type input tensor in ND format with shape \[m, k\] as an example, the following figure shows the internal algorithm diagram of the SoftmaxGradFront high-order API.

**Figure 1**  SoftmaxGradFront algorithm block diagram  
![](../../../../figures/softmaxgradfront_diagram.png "SoftmaxGradFront algorithm block diagram")

The computation process consists of the following steps, all performed on the Vector:

1.  mul step: Multiply all data of inputs x and y. The computation result is saved to a temporary space temp.
2.  reducesum step: Sum the data in each row of \(\[m, k\]\) in temp to obtain \[m, 1\]. The computation result is saved to the temporary space.
3.  broadcast step: Fill \[m, 1\] in units of datablock. For example, for the float type, expand \[m, 1\] to \[m, 8\], and output the result z.

## Prototype

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGradFront(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
    __aicore__ inline void SoftmaxGradFront(const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
    ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space supports two methods: **allocation by the API framework** and **input parameter passing through sharedTmpBuffer** by you.

-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the method of allocation by the API framework, you need to reserve temporary space. For the case of passing through sharedTmpBuffer, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: obtain the required minimum and maximum temporary space sizes through the GetSoftMaxGradMaxTmpSize/GetSoftMaxGradMinTmpSize APIs provided in the [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md). The minimum space ensures functional correctness, and the maximum space is used to improve performance.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isBasicBlock | When the shape information of srcTensor and gradTensor and the Tiling split policy meet the basic block requirements, this parameter can be set to true to improve performance. The default value is false, indicating that it is not enabled. Whether the basic block requirements are met can be determined in either of the following two ways:<br>The shape information [m,n] of srcTensor and dstTensor must meet the following conditions: the last axis length n is less than 2048 and greater than or equal to 256/sizeof(T) (that is, n is at least 128 in the half scenario and at least 64 in the float scenario), and n is a multiple of 64; the product m of the non-last axis lengths is a multiple of 8.<br><br>In the Tiling implementation, call [IsBasicBlockInSoftMax](IsBasicBlockInSoftMax.md) to determine whether the Tiling split policy meets the basic block split requirements.<!-- npu="310b" id7 --><br><br>For the Atlas 200I/500 A2 inference product, this parameter is reserved and not yet enabled. It is retained for future function extension. Keep the default value.<!-- end id7 --> |
| isDataFormatNZ | Whether the current input and output data format is NZ. The default data format is ND, that is, the default value is false.<!-- npu="310b" id8 --><br><br>For the Atlas 200I/500 A2 inference product, configuring the NZ format is not supported.<!-- end id8 --> |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The last axis length is fixed at 32 bytes, that is, one datablock length, and all data in this datablock is the same value. For example, for the half data type, the 16 values in this datablock are all the same value. The non-last axis length must be consistent with srcTensor. |
| gradTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned, and the shape of gradTensor must be consistent with the shape of srcTensor. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The last axis length must be 32-byte aligned, and the shape of srcTensor must be consistent with the shape of gradTensor. |
| sharedTmpBuffer | Input | Temporary space.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of this operand is fixed to uint8_t.<br><br>Used to store intermediate variables during complex computation inside the API, and provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md). |
| tiling | Input | Tiling information required for softmaxgradfront computation. For how to obtain the Tiling information, see [SoftmaxGrad Tiling API](SoftmaxGrad_Tiling_interface.md). |
| softmaxShapeInfo | Input | Shape information of srcTensor. It is of the SoftMaxShapeInfo type, defined in the following code, where the parameters mean:<br>srcM: Product of the non-last axis lengths.<br>srcK: Last axis length, which must be 32-byte aligned.<br>oriSrcM: Product of the original non-last axis lengths.<br>oriSrcK: Original last axis length.<br><br>Note that when the input and output data format is NZ, the last axis length is the reduce axis length, that is, W0\*W1 in [Figure 2](SoftMax.md#fig0172155842215), and the non-last axis is H0\*H1. |

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

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
<!-- npu="A3,910b,310p" id9 -->
-   When srcM != oriSrcM or srcK != oriSrcK in the softmaxShapeInfo parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the computation result of the API overwrites the padded original data in srcTensor. In the input/output non-reuse scenario, the computation result of the API overwrites the data in dstTensor at the position corresponding to the padded position of srcTensor.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the SoftmaxGradFront computation result.
// gradLocal: Input tensor for the SoftmaxGradFront computation.
// srcLocal: Input tensor for the SoftmaxGradFront computation.
// sharedTmpBuffer: Tensor that stores the temporary space during the SoftmaxGradFront computation.
// softmaxTiling: Stores the Tiling information required for the SoftmaxGradFront computation, which can be obtained through the SoftMaxGradTilingFunc API.

AscendC::SoftMaxShapeInfo softmaxInfo(
    /* Product of the non-last axis lengths          */ srcM,
    /* Last axis length, which must be 32-byte aligned */ srcK,
    /* Product of the original non-last axis lengths      */ oriSrcM,
    /* Original last axis length              */ oriSrcK);

// Pass temporary space through the sharedTmpBuffer input parameter.
AscendC::SoftmaxGradFront<T>(dstLocal, gradLocal, srcLocal, sharedTmpBuffer, softmaxTiling, softmaxInfo);
// The API framework allocates temporary space.
AscendC::SoftmaxGradFront<T>(dstLocal, gradLocal, srcLocal, softmaxTiling, softmaxInfo);
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
[[-730.   ]
 [ -88.   ]
 [ -11.2  ]
 [  -2.122]
 [   1.522]
 [   8.8  ]
 [  73.   ]
 [ 548.   ]]
```
