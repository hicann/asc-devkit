# FusedMulAdd<a name="ZH-CN_TOPIC_0000001834610937"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T08:55:49.161Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id7 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id7 -->
<!-- npu="A3" id8 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id8 -->
<!-- npu="910b" id9 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id9 -->
<!-- npu="310b" id10 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id10 -->
<!-- npu="310p" id11 -->
- Atlas inference products AI Core: Supported
<!-- end id11 -->
<!-- npu="310p" id12 -->
- Atlas inference products Vector Core: Not supported
<!-- end id12 -->
<!-- npu="910" id13 -->
- Atlas training products: Not supported
<!-- end id13 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_ternary_scalar_intf.h"`

Multiplies src0 and dst element by element and adds src1, and stores the final result in dst. The calculation formula is as follows:

$$
dst_j = (src0_i \times dst_i) + src1_i
$$

## Prototype<a name="section620mcpsimp"></a>

- Computes the first n data elements of the tensor

  ```cpp
  template <typename T>
  __aicore__ inline void FusedMulAdd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t& count)
  ```

- Computes the tensor by high-dimensional slicing
  - Bitwise mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void FusedMulAdd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask[], const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

  - Continuous mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void FusedMulAdd(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Input/Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| src0, src1 | Input | Source operands.<br>The type is LocalTensor, and the supported TPosition is VECIN/VECCALC/VECOUT. |
| count | Input | Number of elements involved in the computation.<br>**Note: The value range of this parameter is related to the data type of the operand. Different data types support different maximum numbers of elements that can be processed. The maximum amount of data to be processed cannot exceed the UB size limit.** |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration.<br>For details about the settings, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations.<br>The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same datablock between adjacent iterations of the operands and the address stride of different datablocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT support the following data types: half, bfloat16_t, float, int64_t, uint64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products support the following data types: half, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products support the following data types: half, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 inference products support the following data types: half, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
Atlas inference products AI Core support the following data types: half, float.
<!-- end id5 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For address alignment constraints, see [Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- The actual use of the FusedMulAdd instruction is affected by bank conflicts.

    In the non-overlapping address scenario, data at three different addresses (dst, src0, and src1) cannot be read in a single cycle, so only half of the theoretical parallelism can be achieved, and the theoretical parallelism is halved from the original value. In the overlapping address scenario, the original theoretical parallelism is maintained.
<!-- npu="950" id6 -->
- For Ascend 950PR/Ascend 950DT, uint64\_t/int64\_t data types support only the API for computing the first n data of a tensor.
<!-- end id6 -->

## Example<a name="section837496171220"></a>

- Example of the high-dimensional slicing computation API in continuous mask mode (half type input).

  ```cpp
  uint64_t mask = 128;
  // repeatTime = 2, calculate 128 numbers per iteration, 256 numbers in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::FusedMulAdd(dstLocal, src0Local, src1Local, mask, 2, { 1, 1, 1, 8, 8, 8 });
  ```

- Example of the high-dimensional slicing computation API in bitwise mask mode (half type input).

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 2, calculate 128 numbers per iteration, 256 numbers in total.
  // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
  // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between adjacent iterations.
  AscendC::FusedMulAdd(dstLocal, src0Local, src1Local, mask, 2, { 1, 1, 1, 8, 8, 8 });
  ```

- Example of computing the first n data of a tensor (half type input).

  ```cpp
  AscendC::FusedMulAdd(dstLocal, src0Local, src1Local, 256);
  ```

The result is as follows:

```plain
Input data src0Local:
[ 51.4     25.92    28.3     26.62    -4.523  -93.6      8.71    21.02
 -49.8    -80.4     95.94     4.74    70.25   -60.38    79.5    -18.19
  79.1    -66.7     62.5     83.6    -73.1     72.56   -63.03    81.3
 -62.28   -49.94   -78.7     56.2    -52.97    66.2     43.94    13.43
  43.06    51.16    34.16   -64.56   -73.1    -94.7    -68.94    73.06
 -93.4    -46.62   -83.75    15.46    32.88   -76.4     53.84    70.6
  -3.455  -88.4    -65.75   -16.1     88.9    -70.56   -69.44   -11.91
 -77.06    76.06    22.73    91.25   -96.06    64.4     13.1     30.56
 -99.2    -98.56    58.1     31.92   -56.47   -72.7    -42.94    49.1
  98.44    83.75    -4.336   30.03    33.2    -54.78   -21.19   -57.22
 -61.34   -39.8    -29.44   -16.12    38.4    -71.6    -28.52    63.62
  36.      61.38    26.69   -16.34    92.2    -67.7    -92.75   -41.16
 -85.44   -91.2    -22.31   -47.38   -27.28   -77.44    64.     -78.56
  52.22   -61.8    -84.94   -64.7     91.3     64.56    67.25    65.44
  58.7     64.7    -75.06   -44.7    -22.05    71.7     78.9     34.7
 -26.88    39.7    -83.3      0.6274 -34.56   -94.7     -7.027   86.1
 -15.18    63.47    39.     -53.1     54.53    75.44    32.53   -78.3
 -22.34    74.7     -4.312  -33.2      2.19    98.25   -14.66    34.88
   6.746   88.5    -55.03    88.25   -79.56   -61.62    82.3     47.47
 -17.19    45.72   -71.4    -93.5    -32.84   -40.      49.88    27.98
 -70.56   -47.6     76.25    54.62   -62.06   -13.484   86.94    21.81
 -54.53     2.236  -25.16    86.75   -45.97   -44.5    -54.     -53.6
 -23.33    58.62   -48.16   -17.52    87.75   -60.7    -80.6     74.5
  66.4     70.7    -34.28    -3.43   -88.5    -43.56   -22.9    -93.5
 -95.75   -90.6     57.97   -60.06    75.94   -92.3    -15.87    38.5
  37.34   -80.56    71.25    69.      36.25    55.53    74.75    31.1
  -8.445    2.152   95.75    -4.777    9.41    97.8    -64.75    90.94
  38.84    16.8    -17.44    87.     -11.336   98.1    -94.6    -76.2
 -20.14    18.1    -90.4     51.84   -22.88    20.33    45.38    96.06
 -68.56   -57.66    61.78   -78.3     76.     -26.23    27.36   -52.5
 -90.4     23.78   -47.7    -36.      68.4    -59.2    -59.28    32.12
 -44.84    -2.428   -9.266   57.44    66.25   -62.8     92.8     50.75  ]
Input data src1Local:
[-43.1     -90.       -0.7295   49.28    -52.12    -55.53     99.6
  94.4      62.56     20.67    -25.4     -70.6      43.44     57.97
   5.355    38.66    -67.3     -26.72     43.7     -81.06     54.47
  -71.3      84.8      92.9      49.88    -49.94     79.75    -71.8
   6.5      42.3      22.44      6.64    -83.6     -24.3     -97.06
  43.47    -31.06     -9.55     -7.734   -30.27     70.3      10.91
  55.72     60.03     85.8     -21.86    -34.28     -1.962   -11.18
 -20.4      53.34    -44.72      9.28    -40.44     19.42     62.66
 -84.75     39.1      45.9     -89.56     70.94     99.5      16.62
 -36.7     -26.2      87.06    -87.94     19.      -30.12     16.94
 -85.44    -17.06     33.28    -49.84    -24.78    -58.25     27.81
 -23.48     81.06     82.94    -35.88     -4.47    -74.7      85.
 -18.22    -67.5      76.5      96.1     -32.4     -45.56     21.53
 -28.5      88.       -0.1978  -20.34    -44.53    -13.27     -7.93
  33.3      89.8      49.12    -19.84     48.38     83.9      53.
 -65.6      62.97     76.75    -74.4     -23.19     73.1      -9.38
 -31.86     69.44    -52.47    -75.94     54.78     78.94    -74.9
  -0.01271  21.88    -82.9     -34.44     20.56     64.25      7.57
 -63.6     -78.44     19.06     67.25     34.62     82.4      15.6
  84.06     75.06    -97.94    -41.12    -77.75     35.3      88.
  -2.758    -4.36     34.8      73.94    -33.28    -24.5      53.38
 -54.3      19.14     57.1      43.4     -39.4      16.36     24.94
 -42.94    -26.25     27.92    -35.44     79.94     42.12    -62.72
  90.       98.75     -8.22     79.      -96.94    -91.1     -32.94
  64.4     -78.56    -49.56     10.23     82.9     -27.        7.023
 -42.7     -25.67    -42.34     22.72     98.56     69.2     -72.9
  60.28    -43.94     73.06    -28.31     37.5      84.8      -1.514
  81.7     -68.3      46.3      66.       86.44    -26.78    -52.72
  -3.766   -17.95     87.6     -93.5       9.13     58.25     44.62
   2.64     90.25    -42.16    -50.62     18.48     -3.156    36.16
  82.4      26.44     69.8     -47.56    -54.72     58.88    -16.77
  58.44     -5.62    -38.88     40.44    -87.94     33.25     68.94
 -73.2       3.64     45.3     -59.1     -69.9     -94.5      16.02
  12.11     91.56    -30.4     -47.56     56.84    -17.06     60.
 -31.33     50.      -40.44      9.17     31.9     -51.2     -34.72
 -11.43     19.58    -89.6     -61.53    -98.25     94.94     43.8
  -6.848   -99.       99.9     -28.66   ]
Input data dstLocal:
[-18.36     44.94     32.34    -50.      -99.5      89.4       1.568
  61.4     -36.      -46.28     54.88     14.92     61.2     -16.14
  42.72     87.25     -2.787   -66.8      58.3      23.94     89.
 -78.56    -38.94    -78.25     52.6      77.94     84.4     -63.94
  90.7      51.97     84.1      99.3     -70.1      -3.691     4.16
  24.98    -91.94     91.2      65.3     -47.28     68.1     -60.2
 -90.4       0.1636    9.32     51.4      10.45     46.9      42.78
 -38.2     -39.25    -79.1      52.2      56.03    -20.72    -25.81
  27.5     -42.94    -71.6     -73.2     -16.45     99.      -16.
 -75.94    -18.44      6.92    -54.66     12.016    35.53     65.6
  84.4      55.28     34.16    -69.5      -0.4287  -83.1     -30.69
  86.06    -67.56    -97.56    -23.44     73.56    -29.84     -0.49
 -78.44    -17.45    -19.47     57.12     28.31    -86.8      95.44
  82.4      40.53     92.7      20.36    -77.75    -44.62    -21.
 -63.44     71.      -32.2      66.7      46.94    -95.3     -71.8
  -1.351    20.95    -84.44     33.28    -55.2      10.17     22.27
 -26.42    -76.06     90.       44.2     -80.4     -94.25     -9.055
  15.44     75.94    -40.47    -43.78     18.31     -5.586   -55.1
 -95.9      82.5      75.6     -76.25    -83.6     -70.      -54.16
  56.62     64.56    -97.06     93.5     -58.8       7.746   -12.164
  53.06    -56.72     97.7     -11.07     68.2     -77.3      77.8
 -41.44    -14.21    -48.56    -40.5      88.44     85.5     -92.25
  -8.39    -41.78     27.44     85.44     40.8     -80.2      38.94
  -7.598     3.83     74.      -97.06    -84.3      74.9     -88.6
 -92.3     -34.97     65.8     -60.8      39.06    -19.64     65.4
   5.336    -0.07324 -52.16     70.44     11.75      4.72     77.8
 -62.9      80.3     -24.66     62.84    -98.75    -58.88    -90.4
  31.53     70.44    -97.7      64.8      -7.203   -28.3      30.5
 -48.75    -89.06     25.94    -81.06    -62.72    -65.4      79.4
 -90.7      65.6      79.56    -21.22     -0.1489   83.44    -29.86
 -75.9      29.75     53.34     47.66     18.02     92.7      90.56
   8.2     -98.6     -40.53     15.484    28.47    -49.28     24.05
 -23.       -0.0836   50.5     -70.1     -96.9     -10.23    -19.34
 -86.6      63.38    -88.4      92.5     -16.52    -94.7     -92.1
  82.2     -35.94    -67.5     -46.03    -70.44    -54.44    -85.5
  48.72    -61.25     41.      -51.25    -50.4     -50.66     51.84
 -52.9      11.086   -74.1      50.66   ]
Output data dstLocal:
[ -987.      1075.       914.5    -1282.       398.     -8424.
   113.3     1384.      1856.      3740.      5240.         0.0625
  4344.      1032.      3402.     -1548.      -287.8     4428.
  3688.      1921.     -6452.     -5772.      2538.     -6272.
 -3226.     -3942.     -6560.     -3664.     -4796.      3482.
  3718.      1341.     -3104.      -213.1       45.06   -1570.
  6692.     -8640.     -4512.     -3484.     -6288.      2816.
  7624.        62.56     392.2    -3948.       528.5     3310.
  -159.      3354.      2634.      1228.      4648.     -3994.
  1458.       370.     -2204.     -3226.     -1582.     -6768.
  1652.      6472.      -193.     -2356.      1803.      -595.
 -3264.       402.5    -2036.     -4756.     -3708.      2696.
  3396.     -5868.       -22.92   -2554.      -990.5    -4740.
  1512.      5668.      1402.     -2932.       804.        92.9
 -3030.      1182.       631.5     3730.       986.5    -5372.
  2568.     -1374.      3824.     -6272.     -1908.      3156.
  3798.      1907.      1448.     -3274.       927.     -5184.
  3052.      7572.     -3696.        17.88   -1717.      5540.
  2964.     -3588.       757.      1448.     -1583.     -4852.
 -6808.     -2050.      1827.     -6676.      -789.       535.5
 -2019.     -1689.      3614.        32.06     257.5     5224.
   610.      7024.     -1129.     -4772.     -3228.      3798.
 -2938.      4356.      2176.      7504.     -2132.     -4468.
     1.906    491.8      113.44   -5576.     -1397.      -312.
   426.8    -6868.     -4232.     -3710.      1150.      3050.
 -3290.      4160.     -1454.     -4192.       556.      3880.
  -873.     -3454.      2116.     -2202.     -2810.       451.5
   390.8     4034.      6104.      1040.      6416.     -1966.
  5096.      -156.8    -1706.     -5264.     -1713.       847.
 -3522.      -328.8      -23.97   -3100.     -3370.      -107.2
   483.2    -4796.      5132.      5940.     -1564.      4416.
  3424.       286.8     8000.     -1292.     -1681.      9184.
 -6136.       739.5    -1667.     -1885.     -3706.      8208.
  -324.     -3214.     -2332.      5328.      5700.     -6252.
  2468.      4376.     -1637.        13.86    -707.5      -28.1
 -7180.      -115.7      571.5     4612.     -1222.      8488.
  3502.       196.2     1714.     -3564.      -135.      2706.
  4696.     -1763.       390.         2.129  -4520.     -3696.
  2146.      -302.5     -861.5    -8304.     -4252.      5064.
  5668.      1350.     -7212.      2476.      2216.      1937.
  6060.     -1086.      3390.      1909.     -5884.     -2896.
  3650.      1227.      2236.        24.12     564.      3022.
 -3512.      -795.5    -6780.      2542.    ]
```
