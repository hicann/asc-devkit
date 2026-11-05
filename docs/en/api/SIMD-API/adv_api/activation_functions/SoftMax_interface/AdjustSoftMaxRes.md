# AdjustSoftMaxRes

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:40:35.939Z pushedAt=2026-10-29T02:14:34.061Z nl=2 -->

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
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SoftMax_interface/AdjustSoftMaxRes_res.md#id1 -->

## Description

This API adjusts the SoftMax computation result to a specified value. It is mainly used for post-processing the SoftMax-related computation result. When the input max contains a specified value, the corresponding value in softmaxres is adjusted to the input custom value. The adjustment is performed row by row. That is, when the value of a row in max equals a certain value, all values in the corresponding row of the current softmaxres are adjusted to the input value.

For ease of understanding, the computation formula is expressed through a Python script implementation as follows, where res is both the input and the output, and max\\from\\to\\res\_shape are all inputs.

```
def adjust_softmax_res(res, max, from, to, res_shape):
    for i in range(res_shape[0]):
        if max[i] == from:
            for j in res_shape[1]:
                res[i][j] = to
    return
```

## Prototype

```
template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
__aicore__ inline bool AdjustSoftMaxRes(const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to, const SoftMaxShapeInfo& softmaxShapeInfo)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T1 | Data type of softMaxRes. Supported data types: half and float. |
| T2 | Data type of maxTensor. Supported data types: half and float. |
| isDataFormatNZ | Whether the current input and output data format is NZ. The default data format is ND, that is, the default value is false. |
| stepSizeMode | Mode of the step length for taking elements from maxTensor. The parameter values are as follows:<br>0: Default value. Within each BlockSize (32 bytes), the value of the first element is compared with the value of the input from. That is, when the data type of maxTensor is float, one value is taken from every 8 values according to the input shape (m, 8); when the data type of maxTensor is half, one value is taken from every 16 values according to the input shape (m, 16).<br>Non-zero: The value of each element of maxTensor is compared with the value of the input from. That is, according to the input shape (m, 1), the value of one element is taken each time and compared with the value of the input from. When this parameter is non-zero, only the ND format of maxTensor is supported. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| softMaxRes | Input/Output | Both the source operand and the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For the definition of the LocalTensor data structure, see [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The last axis length must be 32-byte aligned.<br><br>Generally, it is the output result of the softmax computation. |
| maxTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The result of reducemax during the softmax computation.<br>The last axis length of maxTensor is fixed to 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, in the half data type, the 16 values in this datablock are all the same reducemax value.<br>The length of the non-last axis is consistent with that of softMaxRes. |
| from | Input | Source operand, of the uint32_t type.<br><br>The value in maxTensor to be determined. Note that because the values in maxTensor are all floating-point numbers, the value to be filled here is the hexadecimal value corresponding to the floating-point number. For example, when you need to determine whether maxTensor contains the value 1.0, the from value should be the hexadecimal value 0x3f800000 corresponding to 1.0. |
| to | Input | Source operand, whose type is consistent with the data type of softMaxRes.<br><br>The value to be filled into softMaxRes. |
| softmaxShapeInfo | Input | Shape information of softMaxRes. The specific definition is shown in the following code, where the parameters are as follows:<br>srcM: Product of the non-last axis lengths.<br>srcK: Last axis length, which must be 32-byte aligned.<br>oriSrcM: Product of the original non-last axis lengths.<br>oriSrcK: Original last axis length.<br><br>Note that currently only ND input is supported. |

```
struct SoftMaxShapeInfo {
    uint32_t srcM;
    uint32_t srcK;
    uint32_t oriSrcM;
    uint32_t oriSrcK;
};
```

## Return Value

bool type. When true is returned, it indicates that maxTensor contains the value to be determined. When false is returned, it indicates that maxTensor does not contain the value to be determined.

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
<!-- npu="A3,910b,310p" id9 -->
-   When srcM != oriSrcM or srcK != oriSrcK in the softmaxShapeInfo parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in some computations. In the input/output reuse scenario, the API computation result overwrites the padded original data in srcTensor. In the input/output non-reuse scenario, the API computation result overwrites the data at the position in dstTensor corresponding to the padded position in srcTensor.
<!-- end id9 -->

## Examples

In this sample, post-processing is required for the SoftMax computation result to determine whether maxTensor contains 0xFF7FFFFF. If so, the corresponding result is set to 0. This sample implements the AdjustSoftMaxResCustom operator with a fixed input shape of x\[32, 8\] and output shape of y\[32, 32\]. The shape of the input softMaxRes is \[32, 8\], the shape of maxTensor is \[32, 8\], and the data type is float.

```
// srcLocal: softmax computation result.
AscendC::SoftMax(srcLocal, ...)
// maxLocal: softmax intermediate result, that is, the reducemax result.
// FROM: Determines whether maxLocal contains an element whose value equals FROM.
// TO: When maxLocal contains an element whose value equals FROM, the elements in the corresponding row of srcLocal are replaced with TO.
// srcShape: Describes the shape information of srcLocal.

AscendC::SoftMaxShapeInfo srcShape = {height, width, height, width};
AscendC::AdjustSoftMaxRes<float, float>(srcLocal, maxLocal, FROM, TO, srcShape);
```

The result is as follows:

```
Input data (srcLocal):
[ 0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
  0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274 ]
Input data (maxLocal):
[ 7.   7.   7.   7.   7.   7.   7.   7.
 15.  15.  15.  15.  15.  15.  15.  15.
 23.  23.  23.  23.  23.  23.  23.  23.
 31.  31.  31.  31.  31.  31.  31.  31.
 39.  39.  39.  39.  39.  39.  39.  39.
 47.  47.  47.  47.  47.  47.  47.  47.
 55.  55.  55.  55.  55.  55.  55.  55.
 63.  63.  63.  63.  63.  63.  63.  63.
 71.  71.  71.  71.  71.  71.  71.  71.
 79.  79.  79.  79.  79.  79.  79.  79.
 87.  87.  87.  87.  87.  87.  87.  87.
 95.  95.  95.  95.  95.  95.  95.  95.
103. 103. 103. 103. 103. 103. 103. 103.
111. 111. 111. 111. 111. 111. 111. 111.
119. 119. 119. 119. 119. 119. 119. 119.
127. 127. 127. 127. 127. 127. 127. 127.
135. 135. 135. 135. 135. 135. 135. 135.
143. 143. 143. 143. 143. 143. 143. 143.
151. 151. 151. 151. 151. 151. 151. 151.
159. 159. 159. 159. 159. 159. 159. 159.
167. 167. 167. 167. 167. 167. 167. 167.
175. 175. 175. 175. 175. 175. 175. 175.
183. 183. 183. 183. 183. 183. 183. 183.
191. 191. 191. 191. 191. 191. 191. 191.
199. 199. 199. 199. 199. 199. 199. 199.
207. 207. 207. 207. 207. 207. 207. 207.
215. 215. 215. 215. 215. 215. 215. 215.
223. 223. 223. 223. 223. 223. 223. 223.
231. 231. 231. 231. 231. 231. 231. 231.
239. 239. 239. 239. 239. 239. 239. 239.
247. 247. 247. 247. 247. 247. 247. 247.
255. 255. 255. 255. 255. 255. 255. 255. ]
Output data (srcLocal):
[
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274
0.0005766128 0.001567396 0.004260624 0.011581578 0.031481992 0.08557693 0.23262219 0.63233274 ]
```
