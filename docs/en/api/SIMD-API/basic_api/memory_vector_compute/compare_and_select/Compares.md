# Compares<a name="ZH-CN_TOPIC_0000001787851728"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:02:43.523Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id12 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id12 -->
<!-- npu="A3" id13 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id13 -->
<!-- npu="910b" id14 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id14 -->
<!-- npu="310b" id15 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id15 -->
<!-- npu="310p" id16 -->
- Atlas inference products AI Core: Supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products Vector Core: Not supported
<!-- end id17 -->
<!-- npu="910" id18 -->
- Atlas training products: Not supported
<!-- end id18 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_cmpsel_intf.h"`

Compares the size of an element in a tensor with that of a Scalar element by element. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0.

Supports multiple comparison modes:

- LT: less than

- GT: greater than

- GE: greater than or equal to

- EQ: equal to

- NE: not equal to

- LE: less than or equal to

## Prototype<a name="section620mcpsimp"></a>

- Computation on the first n data of a tensor

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void Compares(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, uint32_t count)
    ```

- High-dimensional splitting computation of a tensor
    - Bitwise mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compares(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

    - Continuous mask mode

        ```cpp
        template <typename T, typename U, bool isSetMask = true>
        __aicore__ inline void Compares(const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
        ```

## Parameters<a name="section622mcpsimp"></a>

### Template Parameters and API Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand. |
| U | Data type of the destination operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>dst is used to store the comparison result. The uint8_t data in dst is expanded by bit, and from left to right each bit represents the comparison result of src0 and src1Scalar at the corresponding position. If the comparison result is true, the corresponding bit is 1; otherwise, it is 0.<br> |
| src0 | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| src1Scalar | Input | Source operand, a Scalar. The data type is the same as that of src0. |
| cmpMode | Input | CMPMODE type, indicating the comparison mode, including EQ, NE, GE, LE, GT, and LT.<br>&bull; LT: src0 is less than src1Scalar <br>&bull; GT: src0 is greater than src1Scalar <br>&bull; GE: src0 is greater than or equal to src1Scalar <br>&bull; EQ: src0 is equal to src1Scalar <br>&bull; NE: src0 is not equal to src1Scalar <br>&bull; LE: src0 is less than or equal to src1Scalar<br> |
| mask/mask[] | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [UnaryRepeatParams](../../aux_data_structures/UnaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |
| count | Input | Number of elements that participate in the computation. When setting count, ensure that the space occupied by count elements is 256-byte aligned. The unaligned elements do not participate in the computation, and only the complete aligned blocks are valid. |

### mask/mask[] Parameters

<!-- npu="950" id1 -->
- For Ascend 950PR/Ascend 950DT, the setting takes effect.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- For Atlas A3 training products/Atlas A3 inference products, this is a reserved parameter and is ineffective.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- For Atlas A2 training products/Atlas A2 inference products, this is a reserved parameter and is ineffective.
<!-- end id3 -->

<!-- npu="310b" id4 -->
- For Atlas 200I/500 A2 inference product, the setting takes effect.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- For Atlas inference products AI Core, this is a reserved parameter and is ineffective.
<!-- end id5 -->

## Data Type

<!-- npu="950" id6 -->
- For Ascend 950PR/Ascend 950DT
    - The data types supported by T are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t, and double (only CMPMODE::EQ is supported).
    - The data type supported by U is: uint8_t.
<!-- end id6 -->

<!-- npu="A3" id7 -->
- For Atlas A3 training products/Atlas A3 inference products
    - The supported data types of T are half (supported by all CMPMODE), float (supported by all CMPMODE), and int32_t (supported only by CMPMODE::EQ).
    - The supported data type of U is uint8_t.
<!-- end id7 -->

<!-- npu="910b" id8 -->
- For Atlas A2 training products/Atlas A2 inference products
    - The supported data types of T are half (supported by all CMPMODE), float (supported by all CMPMODE), and int32_t (supported only by CMPMODE::EQ).
    - The supported data type of U is uint8_t.
<!-- end id8 -->

<!-- npu="310b" id9 -->
- For Atlas 200I/500 A2 inference products
    - The supported data types of T are int16_t, uint16_t, half, int32_t, uint32_t, and float.
    - The supported data type of U is uint8_t.
<!-- end id9 -->

<!-- npu="310p" id10 -->
- For Atlas inference products AI Core
    - The supported data types of T are half and float.
    - The data type supported by U is uint8_t.
<!-- end id10 -->

## Return Value<a name="section128671456102513"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).

- **dst** is sorted into a binary result in little-endian order, corresponding to the comparison result of the data at the corresponding position in **src0**.

- **For the API that uses the first n data elements of a tensor for computation, when setting count, ensure that the space occupied by count elements is 256-byte aligned.**
<!-- npu="950" id11 -->
- For Ascend 950PR/Ascend 950DT, the int8\_t/uint8\_t/uint64\_t/int64\_t/double data type supports only the API that computes the first n data elements of a tensor, and double supports only CMPMODE::EQ.
<!-- end id11 -->

## Example<a name="section642mcpsimp"></a>

In this example, the source operand src0Local stores 256 float data elements. The function implemented in this example compares the elements in src0Local with the data in src1Local.GetValue\(0\). If an element in src0Local is less than the element in src1Local.GetValue\(0\), the corresponding bit in the dstLocal result is set to 1; otherwise, it is set to 0. The dst result is stored using the uint8\_t data type.

For the complete example, see scenario 3 of the [Compare class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare).

- Computation on the first n elements of a tensor

    ```cpp
    AscendC::Compares(dstLocal, src0Local, src1Scalar, AscendC::CMPMODE::LT, srcDataSize);
    ```

- Computation on high-dimensional tensor slices in continuous mask mode

    ```cpp
    uint64_t mask = 256 / sizeof(float); // 256 is the number of bytes processed per iteration
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::Compares(dstLocal, src0Local, src1Scalar, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```

- Computation on high-dimensional tensor slices in bitwise mask mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0};
    int repeat = 4;
    AscendC::UnaryRepeatParams repeatParams = { 1, 1, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::Compares(dstLocal, src0Local, src1Scalar, AscendC::CMPMODE::LT, mask, repeat, repeatParams);
    ```

The result is as follows:

```
Input data (src0Local):
[ 16.604824    45.069473    65.108345   -59.68792     21.043684
  75.90726    -27.046307   -40.10546     -5.933778    83.56574
  58.87062    -12.77814     28.17882     62.549377   -22.310246
 -67.69001     81.06072     69.988945    69.10082     -6.667376
  96.20256     18.532446   -66.56364    -32.531246    49.980835
  35.668995   -16.847628     1.3236234   10.0143795   43.878166
  26.628105    31.774637    47.9279      79.7291     -54.09651
  95.49459    -18.404795   -86.84594      9.406091   -79.54437
   0.49116692 -48.151714   -12.97062    -99.89055     23.475513
 -27.366564   -69.229675    83.613304    52.14729     40.98426
 -23.422009   -53.386215     1.6576616  -62.36946     54.693733
  66.2058      -4.0042257  -25.351263     1.0000885   -6.458584
  25.447659    71.647316    82.31162     -7.7359715   28.107353
 -79.22045     20.292479    67.7434     -76.054085    -7.754251
  38.632687    -4.8460293  -69.791954   -57.574455   -99.96178
 -73.29611    -68.57477     98.200035   -55.30482    -55.590027
  79.53274     -1.862139   -37.60953    -12.225406   -35.2875
 -24.047668   -66.07609     21.9362      80.603516    28.928387
  26.579298    97.6649      78.94723    -89.86824     73.29788
  18.957182   -73.87053    -23.508097   -51.02931     39.158726
 -96.61422    -41.192455    54.973663    47.58695     -3.9818003
 -81.05088    -67.62415    -17.491713   -34.916042   -95.993744
  -3.4719822  -55.956417     6.223455    12.240832    15.055512
  94.70584    -13.33949    -50.46866     54.612816   -28.521824
 -87.63997     59.53054     41.000504   -31.266075   -31.419422
 -32.940186    53.449913    50.012768   -13.663364    40.931725
 -68.80396    -86.63726     76.866585   -83.76385      3.7227867
  58.443035   -74.333046   -92.52674     24.249512    -7.935491
  24.197245   -34.85033     67.854645    72.65312     13.622443
 -70.94266     15.401667    -9.332295   -86.61463     72.659676
 -83.63352      9.279887    81.037964    46.285606   -12.967846
 -48.72901     69.07614    -40.355286   -94.257034   -45.514374
  24.966864    -9.657219    61.803864   -83.09603     77.769035
 -97.44226    -89.71987    -53.969315    43.892918    73.88798
  67.23104     36.65282    -93.70069    -87.48934    -27.679005
 -36.825226   -30.117033   -41.579655   -97.325325    77.1972
 -49.883194    33.061394   -63.844925    89.74327     64.549416
  80.16943     73.26347    -87.307175   -96.62777     81.8532
   7.5365276   28.357092    59.896378   -15.95738    -77.42723
   0.03529428 -20.263502    45.59324    -90.160835    89.478004
  57.608685    60.71819     45.8125      39.94484    -48.77375
 -56.897358     5.2580256   -6.937905   -49.80309    -42.527523
  72.91772     89.53271    -62.181187    18.490683   -69.40782
   6.141204    13.938042    75.312515    21.766457    -8.157599
  55.53147    -30.789118   -12.087165    82.435684    23.4884
  82.73172     -2.026827    -8.124383   -10.707488   -74.32759
 -54.702602    14.209252    93.73145     98.93554     52.803623
  32.200726    41.823833    90.193756   -34.512424   -85.64022
  97.47763     33.353424    94.84875     23.03139     99.97347
 -72.47978     19.51753    -88.28579    -88.70721    -18.659292
 -79.5277      62.90431     21.837631    45.989056    -9.62086
  11.4855795 ]
Input data (src1Scalar):
[-95.16087   -71.4676     51.817818  -12.358237   96.60704   -12.0067835
 -44.128048    7.5811195  84.61196   -60.303513   21.470125   98.96244
  18.262054   80.014244   48.37233   -75.03457  ]
Output data (dstLocal):
[ 0  0  0  0  0  8  0  0  0  4  0  0 16 32  0  0  0  0  0  0 32  0  4 16
  0  0  0  0  0  0  0  0]
```
