# TransDataTo5HD<a name="ZH-CN_TOPIC_0000001473462212"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T14:12:39.384Z -->

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
- Atlas training products: Supported
<!-- end id13 -->


## Description<a name="zh-cn_topic_0000001521260417_section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_transpose_intf.h"`.

The TransDataTo5HD API performs data format conversion, generally used to convert the NCHW format to the [NC1HWC0 format](../../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_detailed_explanation.md#nc1hwc0-format). In particular, it can also be used to transpose two-dimensional matrix data blocks. When performing transposition, compared with the Transpose API, Transpose supports only matrix transposition of 16\*16 size; this API can process 512 bytes of data (16 DataBlocks) within a single repeat. Depending on the data type, it supports matrix transposition of different shapes, and it also supports multiple repeat operations.

To help developers understand, for ground truth computation, see [TransDataTo5HD ground truth computation](https://gitcode.com/cann/asc-devkit/blob/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose/scripts/gen_data.py).

## Prototype<a name="zh-cn_topic_0000001521260417_section620mcpsimp"></a>

- dstList and srcList are arrays of type uint64\_t, and the array elements correspond to the address values of LocalTensor. Developers can obtain the address values through the GetPhyAddr API of LocalTensor. If the address values of dstList and srcList are known, it is recommended to use this API.

  ```cpp
  template <typename T>
  __aicore__ inline void TransDataTo5HD(uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE], uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE], const TransDataTo5HDParams& nchwconvParams) 
  ```

- dst and src are LocalTensors of type uint64\_t, and the address values of the corresponding LocalTensors are stored contiguously. Developers can obtain the address values through the GetPhyAddr API of LocalTensor.

  ```cpp
  template <typename T>
  __aicore__ inline void TransDataTo5HD(const LocalTensor<uint64_t>& dst, const LocalTensor<uint64_t>& src, const TransDataTo5HDParams& nchwconvParams) 
  ```

- dstList and srcList are arrays of type LocalTensor.

  ```cpp
  template <typename T>
  __aicore__ inline void TransDataTo5HD(const LocalTensor<T> (&dstList)[NCHW_CONV_ADDR_LIST_SIZE], const LocalTensor<T> (&srcList)[NCHW_CONV_ADDR_LIST_SIZE], const TransDataTo5HDParams& nchwconvParams) 
  ```

## Parameters<a name="zh-cn_topic_0000001521260417_section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Meaning |
| :------- | :--- |
| T | Data type of the operand. |

**Table 2**  Parameter list

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dstList | Output | Destination operand address sequence.<br><br>NCHW_CONV_ADDR_LIST_SIZE has a fixed length of 16.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md) or the address value of LocalTensor. The supported TPosition is VECIN/VECCALC/VECOUT (the storage location is Unified Buffer).<br>The start address of LocalTensor must be 32-byte aligned. |
| srcList | Input | Source operand address sequence.<br><br>NCHW_CONV_ADDR_LIST_SIZE has a fixed length of 16.<br>The type is LocalTensor or the address value of LocalTensor. The supported TPosition is VECIN/VECCALC/VECOUT (the storage location is Unified Buffer).<br>The start address of LocalTensor must be 32-byte aligned. |
| dst | Output | Destination operand.<br><br>The type is LocalTensor, and the contiguous storage corresponds to the address value of LocalTensor. The supported TPosition of LocalTensor is VECIN/VECCALC/VECOUT. The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand.<br><br>The type is LocalTensor, and the contiguous storage corresponds to the address value of LocalTensor. The supported TPosition of LocalTensor is VECIN/VECCALC/VECOUT. The start address of LocalTensor must be 32-byte aligned. |
| nchwconvParams | Input | Related parameters used to control the data structure and instruction iteration of TransdataTo5HD.<br>For details about the nchwconvParams parameter, see [TransDataTo5HDParams Parameters](#table3). |

**Table 3**  TransDataTo5HDParams Parameters<a id="table3"></a>

| Parameter | Type | Description |
| :------- | :--- | :--- |
| dstHighHalf | Input | Specifies whether the data at each dstList address is stored in the high half or the low half of the DataBlock. This configuration supports only the int8_t and uint8_t data types.<br><br>The supported data type is bool, with the following two values:<br>•true: indicates storage in the high half of the DataBlock<br>•false: indicates storage in the low half of the DataBlock |
| srcHighHalf | Input | Specifies whether the data at each srcList address is read from the high half or the low half of the DataBlock. This configuration supports only the int8_t and uint8_t data types.<br><br>The supported data type is bool, with the following two values:<br>•true: indicates reading from the high half of the DataBlock<br>•false: indicates reading from the low half of the DataBlock |
| repeatTimes | Input | Number of repeated iterations, repeatTimes∈[0,255].<br><br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dimension_slicing.md).<br><br>Note:<br>•repeatTimes=0 indicates that no conversion operation is performed, no write is performed to the destination operand, and the API is treated as a NOP (no operation).<br>•When repeatTimes is 1, the valid start position of the destination operand/source operand is the start position of the dstList/srcList sequence input plus dstRepStride/srcRepStride. When repeatTimes is 1, to make the valid start position of the destination operand/source operand the start position of the dstList/srcList sequence input, set dstRepStride/srcRepStride to 0.<br>•When repeatTimes is greater than 1, in the first repeat, the valid start position of the destination operand/source operand is the start position of the dstList/srcList sequence input; in the second repeat, dstRepStride/srcRepStride is added. And so on. |
| dstRepStride | Input | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlock. |
| srcRepStride | Input | Address stride of the same DataBlock of the vector source operand between adjacent iterations, in DataBlock. |

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
Atlas inference products (AI Core) support the following data types: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, and float.
<!-- end id5 -->

<!-- npu="910" id6 -->
Atlas training products support the following data types: int8_t, uint8_t, int16_t, uint16_t, and half.
<!-- end id6 -->

## Constraints<a name="zh-cn_topic_0000001521260417_section633mcpsimp"></a>

- For the operand address alignment requirements, see [Unified Buffer Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- For the operand address overlap constraints, see [Unified Buffer Address Overlap Constraints](../../../general_description_and_constraints.md#section668772811100).
- The addresses in dst and src must be stored contiguously.
- To implement the transpose function through single or multiple repeat operations, you only need to configure srcList and dstList once. Subsequent LocalTensor addresses are automatically offset through the nchwconvParams parameter.

## Return Value<a name="zh-cn_topic_0000001521260417_section640mcpsimp"></a>

None

## Key Features Description<a name="zh-cn_topic_0000002557628265_section16509954114712"></a>

### Processing 16 DataBlocks of Data in a Single Repeat<a name="zh-cn_topic_0000002557628265_section166723295485"></a>

- When this API is used to perform contiguous matrix transposition of 16 DataBlocks, the address sequence configuration rules are as follows:
  - srcList: The interval between adjacent addresses in the array is blockSize elements (blockSize = 32 / sizeof\(T\)), that is, source data is read from contiguous DataBlocks;
  - dstList: The interval between adjacent addresses in the array is blockSize elements (blockSize = 32 / sizeof\(T\)), that is, target data is written to contiguous DataBlocks.

- When this API is used to perform matrix transposition of 16 DataBlocks, the nchwconvParams configuration rules are as follows:
  - repeatTimes: The number of loops is 1;
  - srcRepStride: Set to 0 so that the valid start position of the source operand is the start position of the srcList sequence;
  - dstRepStride: Set to 0 so that the valid start position of the destination operand is the start position of dstList.

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // Valid only for int8_t and uint8_t input parameter types. Read data from the high half of srcLocalList.
  transDataParams.srcHighHalf = true; // Valid only for int8_t and uint8_t input parameter types. Write data to the high half of dstLocalList.
  transDataParams.repeatTimes = 1;    // Number of repeated iterations. Each repeat processes 16 DataBlocks.
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // Number of elements stored in each DataBlock, which is 32 here.
  uint64_t dstLocalList[16];
  for (int i = 0; i < 16; i++) {   // dstLocal is a LocalTensor of the int8_t type.
    dstLocalList[i] = (uint64_t)(dstLocal[width * i].GetPhyAddr());
  }
  uint64_t srcLocalList[16];
  for (int i = 0; i < 16; i++) {   // srcLocal is a LocalTensor of the int8_t type.
    srcLocalList[i] = (uint64_t)(srcLocal[width * i].GetPhyAddr());
  }
  AscendC::TransDataTo5HD<int8_t>(dstLocalList, srcLocalList, transDataParams);
  ```

- When the data type bit width is 8 bits, each DataBlock contains 32 numbers. Values are taken from the corresponding positions in the specified 16 DataBlocks to form half a DataBlock, which is then placed into the destination address. Whether the read and storage occur in the high half or the low half of the DataBlock is determined by the srcHighHalf and dstHighHalf parameters.

  **Figure 1** b8 type matrix transpose
  ![b8](../../../../figures/trans_data_to_5hd_b8.png)

- When the data type bit width is 16 bits, each DataBlock contains 16 numbers. Values are taken from the corresponding positions in the specified 16 DataBlocks to form one new DataBlock, which is then placed into the destination address.

  **Figure 2** b16 type matrix transpose
  ![b16](../../../../figures/trans_data_to_5hd_b16.png)

- When the data type bit width is 32 bits, each DataBlock contains 8 numbers. Values are taken from the corresponding positions in the specified 16 DataBlocks to form two new DataBlocks, which are then placed into the destination address.

  **Figure 3** b32 type matrix transpose
  ![b32](../../../../figures/trans_data_to_5hd_b32.png)

- The 16 DataBlocks may be stored non-contiguously. It is recommended that you use GetPhyAddr to obtain the address values of the LocalTensor and store them in an address array.

  **Figure 4**  Transposing 16 DataBlocks in a single repeat<a name="zh-cn_topic_0000002557628265_fig12595141012208"></a>
  ![Transposing 16 DataBlocks in a single repeat](../../../../figures/trans_data_to_5hd.png)

### Converting NCHW Format to NC1HWC0 Format<a name="zh-cn_topic_0000002557628265_section3122131914516"></a>

- When converting NCHW format to NC1HWC0 format, if the bit width of the data type is 32 bits or 16 bits, C0 = 16; if the bit width of the data type is 8 bits, C0 = 32.

- When contiguous data is used for matrix transposition or fractalization (NCHW-\>NC1HWC0) through this API, the address sequence configuration rules are as follows:
  - srcList: The interval between adjacent addresses in the array is H \* W elements, that is, the start position of each HW plane.
  - dstList: The interval between adjacent addresses in the array is blockSize elements (blockSize = 32 / sizeof\(T\)), that is, the destination data is written into contiguous DataBlocks.

- When contiguous data is used for matrix transposition or fractalization (NCHW-\>NC1HWC0) through this API, the nchwconvParams configuration rules are as follows:
  - repeatTimes: The number of iterations is H \* W / blockSize (blockSize = 32 / sizeof\(T\)), that is, repeated iterations cover the entire HW plane.
  - srcRepStride: Set to 1, that is, between adjacent iterations, the vector source operand stride is 1 DataBlock.
  - dstRepStride: Set to 16, that is, between adjacent iterations, the vector destination operand stride is 16 DataBlocks.

  ```cpp
  constexpr uint32_t N = 2;
  constexpr uint32_t C = 32;
  constexpr uint32_t H = 16;
  constexpr uint32_t W = 16;
  constexpr uint32_t C0 = 16;
  constexpr uint32_t C1 = C / C0; // 2
  // Total data volume
  constexpr uint32_t totalElements = N * C * H * W; // 16384
  // Number of outer loops.
  constexpr uint32_t loopCount = N * C /NCHW_CONV_ADDR_LIST_SIZE; // 4
  // Set conversion parameters: NCHW → NC1HWC0
  AscendC::TransDataTo5HDParams transParams;
  transParams.repeatTimes = 16;  // Number of repeats required to process the entire HW plane with hSize * wSize / elems_per_block
  transParams.dstRepStride = 16; // dstList is spaced NCHW_CONV_ADDR_LIST_SIZE DataBlocks between loops
  transParams.srcRepStride = 1;  // srcList is contiguous on the same HW plane between loops
  // Process data in a loop.
  for (uint32_t loop = 0; loop < loopCount; loop++) {
    // Set the address list.
    uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE];
    for (int i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
      srcList[i] = (uint64_t)srcLocal[loop * NCHW_CONV_ADDR_LIST_SIZE * H * W + i * H * W].GetPhyAddr();
      dstList[i] = (uint64_t)dstLocal[loop * NCHW_CONV_ADDR_LIST_SIZE * H * W + i * C0].GetPhyAddr();
    }
    // Perform data layout conversion.
    AscendC::TransDataTo5HD<T>(dstList, srcList, transParams);
  }
  ```

  ![nchw2nc1hwc0](../../../../figures/trans_data_to_5hd_nchw_nc1hwc0.png)

## Example<a name="zh-cn_topic_0000001521260417_section19372434133520"></a>

This sample shows only some key code. To run the sample code, copy this code segment and replace the code in the Compute function of the complete sample template for scenario 3 of the [Transpose class sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose).

- Calling method when the input parameter type is LocalTensor:

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // Valid only for int8_t/uint8_t input parameters. Read data from the high half of srcLocalList.
  transDataParams.srcHighHalf = true; // Valid only for int8_t/uint8_t input parameters. Write data to the high half of dstLocalList.
  transDataParams.repeatTimes = 1; // Number of repeated iterations. Each repeat processes 16 DataBlocks.
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // Number of elements stored in each DataBlock, which is 32 here.
  AscendC::LocalTensor<int8_t> dstLocalList[16];
  for (int i = 0; i < 16; i++) { // dstLocal is a LocalTensor of the int8_t type.
      dstLocalList[i] = dstLocal[width * i];
  }
  AscendC::LocalTensor<int8_t> srcLocalList[16];
  for (int i = 0; i < 16; i++) { // srcLocal is a LocalTensor of the int8_t type.
      srcLocalList[i] = srcLocal[width * i];
  }
  AscendC::TransDataTo5HD<int8_t>(dstLocalList, srcLocalList, transDataParams);
  ```

- Calling method when the input parameter type is the address value of a LocalTensor (recommended):

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // Valid only for int8_t/uint8_t input parameters. Read data from the high half of srcLocalList.
  transDataParams.srcHighHalf = true; // Valid only for int8_t/uint8_t input parameters. Write data to the high half of dstLocalList.
  transDataParams.repeatTimes = 1; // Number of repeated iterations. Each repeat processes 16 DataBlocks.
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // Number of elements stored in each DataBlock, which is 32 here.
  uint64_t dstLocalList[16];
  for (int i = 0; i < 16; i++) { // dstLocal is a LocalTensor of the int8_t type.
      dstLocalList[i] = (uint64_t)(dstLocal[width * i].GetPhyAddr());
  }
  uint64_t srcLocalList[16];
  for (int i = 0; i < 16; i++) { // srcLocal is a LocalTensor of the int8_t type.
      srcLocalList[i] = (uint64_t)(srcLocal[width * i].GetPhyAddr());
  }
  AscendC::TransDataTo5HD<int8_t>(dstLocalList, srcLocalList, transDataParams);
  ```

- Calling method when the input parameter type is the address LocalTensor:

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // Valid only for int8_t/uint8_t input parameters. Read data from the high half of srcLocalList.
  transDataParams.srcHighHalf = true; // Valid only for int8_t/uint8_t input parameters. Write data to the high half of dstLocalList.
  transDataParams.repeatTimes = 1; // Number of repeated iterations. Each repeat processes 16 DataBlocks.
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // Number of elements stored in each DataBlock, which is 32 here.
  // Use TQue to allocate a uint64_t address LocalTensor for storing the addresses of dstLocal and srcLocal.
  AscendC::LocalTensor<uint64_t> dst = workQueueSrc1.AllocTensor<uint64_t>();
  for (int i = 0; i < 16; i++) { // dstLocal is a LocalTensor of the int8_t type.
      dst.SetValue(i, (uint64_t)(dstLocal[width * i].GetPhyAddr()));
  }
  AscendC::LocalTensor<uint64_t> src = workQueueSrc2.AllocTensor<uint64_t>();
  for (int i = 0; i < 16; i++) { // srcLocal is a LocalTensor of the int8_t type.
      src.SetValue(i, (uint64_t)(srcLocal[width * i].GetPhyAddr()));
  }
  AscendC::TransDataTo5HD<int8_t>(dst, src, transDataParams);
  // Release the address LocalTensor.
  workQueueSrc1.FreeTensor(dst);
  workQueueSrc2.FreeTensor(src);
  ```

  When the input/output is of the int8\_t type, the result example is as follows:

  ```plain
  Input data (src):
  [[  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
     18  19  20  21  22  23  24  25  26  27  28  29  30  31]
   [ 32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49
     50  51  52  53  54  55  56  57  58  59  60  61  62  63]
   [ 64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81
     82  83  84  85  86  87  88  89  90  91  92  93  94  95]
   [ 96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111 112 113
    114 115 116 117 118 119 120 121 122 123 124 125 126 127]
   [  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
     18  19  20  21  22  23  24  25  26  27  28  29  30  31]
   [ 32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49
     50  51  52  53  54  55  56  57  58  59  60  61  62  63]
   [ 64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81
     82  83  84  85  86  87  88  89  90  91  92  93  94  95]
   [ 96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111 112 113
    114 115 116 117 118 119 120 121 122 123 124 125 126 127]
   [  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
     18  19  20  21  22  23  24  25  26  27  28  29  30  31]
   [ 32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49
     50  51  52  53  54  55  56  57  58  59  60  61  62  63]
   [ 64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81
     82  83  84  85  86  87  88  89  90  91  92  93  94  95]
   [ 96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111 112 113
    114 115 116 117 118 119 120 121 122 123 124 125 126 127]
   [  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
     18  19  20  21  22  23  24  25  26  27  28  29  30  31]
   [ 32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49
     50  51  52  53  54  55  56  57  58  59  60  61  62  63]
   [ 64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81
     82  83  84  85  86  87  88  89  90  91  92  93  94  95]
   [ 96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111 112 113
    114 115 116 117 118 119 120 121 122 123 124 125 126 127]]
  Output data (dstGm):
  // Read data from the high half of the input data and write it to the high half of the output data.
  [[0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16 48 80 112 16 48 80 112 16 48 80 112 16 48 80 112 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 17 49 81 113 17 49 81 113 17 49 81 113 17 49 81 113 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 18 50 82 114 18 50 82 114 18 50 82 114 18 50 82 114 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 19 51 83 115 19 51 83 115 19 51 83 115 19 51 83 115 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 20 52 84 116 20 52 84 116 20 52 84 116 20 52 84 116 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 21 53 85 117 21 53 85 117 21 53 85 117 21 53 85 117 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 22 54 86 118 22 54 86 118 22 54 86 118 22 54 86 118 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 23 55 87 119 23 55 87 119 23 55 87 119 23 55 87 119 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 24 56 88 120 24 56 88 120 24 56 88 120 24 56 88 120 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 25 57 89 121 25 57 89 121 25 57 89 121 25 57 89 121 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 26 58 90 122 26 58 90 122 26 58 90 122 26 58 90 122 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 27 59 91 123 27 59 91 123 27 59 91 123 27 59 91 123 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 28 60 92 124 28 60 92 124 28 60 92 124 28 60 92 124 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 29 61 93 125 29 61 93 125 29 61 93 125 29 61 93 125 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 30 62 94 126 30 62 94 126 30 62 94 126 30 62 94 126 ]
  [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 31 63 95 127 31 63 95 127 31 63 95 127 31 63 95 127 ]]
  ```

  When the input/output is of the half type, the result example is as follows:

  ```plain
  Input data (src):
  [[  0.   1.   2.   3.   4.   5.   6.   7.   8.   9.  10.  11.  12.  13.
     14.  15.]
   [ 16.  17.  18.  19.  20.  21.  22.  23.  24.  25.  26.  27.  28.  29.
     30.  31.]
   [ 32.  33.  34.  35.  36.  37.  38.  39.  40.  41.  42.  43.  44.  45.
     46.  47.]
   [ 48.  49.  50.  51.  52.  53.  54.  55.  56.  57.  58.  59.  60.  61.
     62.  63.]
   [ 64.  65.  66.  67.  68.  69.  70.  71.  72.  73.  74.  75.  76.  77.
     78.  79.]
   [ 80.  81.  82.  83.  84.  85.  86.  87.  88.  89.  90.  91.  92.  93.
     94.  95.]
   [ 96.  97.  98.  99. 100. 101. 102. 103. 104. 105. 106. 107. 108. 109.
    110. 111.]
   [112. 113. 114. 115. 116. 117. 118. 119. 120. 121. 122. 123. 124. 125.
    126. 127.]
   [128. 129. 130. 131. 132. 133. 134. 135. 136. 137. 138. 139. 140. 141.
    142. 143.]
   [144. 145. 146. 147. 148. 149. 150. 151. 152. 153. 154. 155. 156. 157.
    158. 159.]
   [160. 161. 162. 163. 164. 165. 166. 167. 168. 169. 170. 171. 172. 173.
    174. 175.]
   [176. 177. 178. 179. 180. 181. 182. 183. 184. 185. 186. 187. 188. 189.
    190. 191.]
   [192. 193. 194. 195. 196. 197. 198. 199. 200. 201. 202. 203. 204. 205.
    206. 207.]
   [208. 209. 210. 211. 212. 213. 214. 215. 216. 217. 218. 219. 220. 221.
    222. 223.]
   [224. 225. 226. 227. 228. 229. 230. 231. 232. 233. 234. 235. 236. 237.
    238. 239.]
   [240. 241. 242. 243. 244. 245. 246. 247. 248. 249. 250. 251. 252. 253.
    254. 255.]]
  
  Output data (dstGm):
  [[  0.  16.  32.  48.  64.  80.  96. 112. 128. 144. 160. 176. 192. 208.
    224. 240.]
   [  1.  17.  33.  49.  65.  81.  97. 113. 129. 145. 161. 177. 193. 209.
    225. 241.]
   [  2.  18.  34.  50.  66.  82.  98. 114. 130. 146. 162. 178. 194. 210.
    226. 242.]
   [  3.  19.  35.  51.  67.  83.  99. 115. 131. 147. 163. 179. 195. 211.
    227. 243.]
   [  4.  20.  36.  52.  68.  84. 100. 116. 132. 148. 164. 180. 196. 212.
    228. 244.]
   [  5.  21.  37.  53.  69.  85. 101. 117. 133. 149. 165. 181. 197. 213.
    229. 245.]
   [  6.  22.  38.  54.  70.  86. 102. 118. 134. 150. 166. 182. 198. 214.
    230. 246.]
   [  7.  23.  39.  55.  71.  87. 103. 119. 135. 151. 167. 183. 199. 215.
    231. 247.]
   [  8.  24.  40.  56.  72.  88. 104. 120. 136. 152. 168. 184. 200. 216.
    232. 248.]
   [  9.  25.  41.  57.  73.  89. 105. 121. 137. 153. 169. 185. 201. 217.
    233. 249.]
   [ 10.  26.  42.  58.  74.  90. 106. 122. 138. 154. 170. 186. 202. 218.
    234. 250.]
   [ 11.  27.  43.  59.  75.  91. 107. 123. 139. 155. 171. 187. 203. 219.
    235. 251.]
   [ 12.  28.  44.  60.  76.  92. 108. 124. 140. 156. 172. 188. 204. 220.
    236. 252.]
   [ 13.  29.  45.  61.  77.  93. 109. 125. 141. 157. 173. 189. 205. 221.
    237. 253.]
   [ 14.  30.  46.  62.  78.  94. 110. 126. 142. 158. 174. 190. 206. 222.
    238. 254.]
   [ 15.  31.  47.  63.  79.  95. 111. 127. 143. 159. 175. 191. 207. 223.
    239. 255.]]
  ```

  When the input and output are of the int32\_t type, the result example is as follows:

  ```plain
  Input data (src):
  [[  0   1   2   3   4   5   6   7  ]
   [  8   9  10  11  12  13  14  15  ]
   [ 16  17  18  19  20  21  22  23  ]
   [ 24  25  26  27  28  29  30  31  ]
   [ 32  33  34  35  36  37  38  39  ]
   [ 40  41  42  43  44  45  46  47  ]
   [ 48  49  50  51  52  53  54  55  ]
   [ 56  57  58  59  60  61  62  63  ]
   [ 64  65  66  67  68  69  70  71  ]
   [ 72  73  74  75  76  77  78  79  ]
   [ 80  81  82  83  84  85  86  87  ]
   [ 88  89  90  91  92  93  94  95  ]
   [ 96  97  98  99  100 101 102 103 ]
   [ 104 105 106 107 108 109 110 111 ]
   [ 112 113 114 115 116 117 118 119 ]
   [ 120 121 122 123 124 125  126 127]]
  Output data (dstGm):
  [[ 0 8 16 24 32 40 48 56 64 72 80 88 96 104 112 120   ]
   [ 1 9 17 25 33 41 49 57 65 73 81 89 97 105 113 121   ] 
   [ 2 10 18 26 34 42 50 58 66 74 82 90 98 106 114 122  ]
   [ 3 11 19 27 35 43 51 59 67 75 83 91 99 107 115 123  ]
   [ 4 12 20 28 36 44 52 60 68 76 84 92 100 108 116 124 ] 
   [ 5 13 21 29 37 45 53 61 69 77 85 93 101 109 117 125 ]
   [ 6 14 22 30 38 46 54 62 70 78 86 94 102 110 118 126 ]
   [ 7 15 23 31 39 47 55 63 71 79 87 95 103 111 119 127 ]]
  ```
