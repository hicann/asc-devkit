# AdjustSoftMaxRes

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:45:21.034Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

This API adjusts the SoftMax computation result to a specified value. It is mainly used for post-processing the SoftMax-related computation results. When the input max contains a specified value, the corresponding result in softmaxres is adjusted to the input custom value. The adjustment is performed row by row. That is, when the value of a row in max equals a certain value, all values in the corresponding row of the current softmaxres are adjusted to the input value.

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
| stepSizeMode | Step length mode for taking elements from maxTensor. The parameter values are as follows:<br>0: Default value. Within each BlockSize (32 bytes), the value of the first element is compared with the value of the input from. That is, when the data type of maxTensor is float, one value is taken from every 8 values according to the input shape (m, 8); when the data type of maxTensor is half, one value is taken from every 16 values according to the input shape (m, 16).<br>Non-zero: The value of each element in maxTensor is compared with the value of the input from. That is, according to the input shape (m, 1), the value of one element is taken each time and compared with the value of the input from. When this parameter is non-zero, only the ND format is supported for maxTensor. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| softMaxRes | Input/Output | Both the source operand and the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For the definition of the LocalTensor data structure, see [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The last axis length must be 32-byte-aligned.<br><br>Generally, this is the output result of the softmax computation. |
| maxTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The result of reducemax during the softmax computation.<br>The last axis length of maxTensor is fixed at 32 bytes, that is, the length of one datablock. All data in this datablock is the same value. For example, under the half data type, all 16 values in this datablock are the same reducemax value.<br>The lengths of the non-last axes are consistent with those of softMaxRes.|
| from | Input | Source operand, of the uint32_t type.<br><br>The value in maxTensor to be checked. Note that because all values in maxTensor are of the floating-point type, the value to be filled here must be the hexadecimal value corresponding to the floating-point value. For example, to check whether maxTensor contains the value 1.0, the from value must be the hexadecimal value 0x3f800000 corresponding to 1.0. |
| to | Input | Source operand, whose type is consistent with the data type of softMaxRes.<br><br>The value to be filled into softMaxRes. |
| softmaxShapeInfo | Input | Shape information of **softMaxRes**. The specific definition is shown in the following code, where the parameters mean:<br>**srcM**: Product of the non-last axis lengths.<br>**srcK**: Tail axis length, which must be 32-byte-aligned.<br>**oriSrcM**: Product of the original non-last axis lengths.<br>**oriSrcK**: Original last axis length.<br><br>Note that currently only ND input is supported. |

```
struct SoftMaxShapeInfo {
  uint32_t srcM;
  uint32_t srcK;
  uint32_t oriSrcM;
  uint32_t oriSrcK;
};
```

## Return Value

bool type. When true is returned, it indicates that maxTensor contains the value to be checked; when false is returned, it indicates that maxTensor does not contain the value to be checked.

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   When **srcM** != **oriSrcM** or **srcK** != **oriSrcK** in the **softmaxShapeInfo** parameter, you need to pad the original input \(oriSrcM, oriSrcK\) on GM in the M or K direction to \(srcM, srcK\). The padded data participates in part of the computation. In the input/output reuse scenario, the API computation result overwrites the padded original data in **srcTensor**. In the non-reuse scenario, the API computation result overwrites the data at the corresponding padded positions in **dstTensor**.

## Examples

In this sample, post-processing is required for the SoftMax computation result to check whether maxTensor contains 0xFF7FFFFF, and if so, update the corresponding result to 0. This sample implements the AdjustSoftMaxResCustom operator with a fixed shape of input x\[32, 8\] and output y\[32, 32\]. The shape size of the input softMaxRes is \[32, 8\], the shape size of maxTensor is \[32,8\], and the data type is float.

```
// srcLocal: softmax computation result
AscendC::SoftMax(srcLocal, ...)
// maxLocal: intermediate result of softmax, the result of reducemax
// FROM: Check whether maxLocal contains an element whose value equals FROM.
// TO: When maxLocal contains an element equal to FROM, the elements in the corresponding row of srcLocal are replaced with TO.
// srcShape: Describes the shape information of srcLocal.

AscendC::SoftMaxShapeInfo srcShape = {height, width, height, width};
AscendC::AdjustSoftMaxRes<float, float>(srcLocal, maxLocal, FROM, TO, srcShape);
```

The results are as follows:

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
