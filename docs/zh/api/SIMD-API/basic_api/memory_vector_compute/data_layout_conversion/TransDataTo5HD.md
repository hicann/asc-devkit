# TransDataTo5HD<a name="ZH-CN_TOPIC_0000001473462212"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_layout_conversion/TransDataTo5HD_res.md#id1 -->

## 功能说明<a name="zh-cn_topic_0000001521260417_section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_transpose_intf.h"`。

TransDataTo5HD接口数据格式转换，一般用于将NCHW格式转换成[NC1HWC0格式](../../cube_compute_ISASI/cube_compute_fractal_intro/key_fractal_format_details.md#nc1hwc0格式)。特别的，也可以用于二维矩阵数据块的转置。完成转置功能时，相比于Transpose接口，Transpose仅支持16\*16大小的矩阵转置；本接口单次repeat内可处理512Byte的数据（16个DataBlock），根据数据类型不同，支持不同shape的矩阵转置，同时还可以支持多次repeat操作。

为方便开发者理解，真值计算请参考：[TransDataTo5HD真值计算](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose/scripts/gen_data.py)。

## 函数原型<a name="zh-cn_topic_0000001521260417_section620mcpsimp"></a>

- dstList与srcList类型为uint64\_t的数组，数组元素对应LocalTensor的地址值。开发者可以通过LocalTensor的GetPhyAddr接口获取该地址值。如果已知dstList与srcList的地址值，建议使用该接口。

  ```cpp
  template <typename T>
  __aicore__ inline void TransDataTo5HD(uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE], uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE], const TransDataTo5HDParams& nchwconvParams) 
  ```

- dst与src类型为uint64\_t的LocalTensor，连续存储对应LocalTensor的地址值。开发者可以通过LocalTensor的GetPhyAddr接口获取该地址值。

  ```cpp
  template <typename T>
  __aicore__ inline void TransDataTo5HD(const LocalTensor<uint64_t>& dst, const LocalTensor<uint64_t>& src, const TransDataTo5HDParams& nchwconvParams) 
  ```

- dstList与srcList类型为LocalTensor的数组。

  ```cpp
  template <typename T>
  __aicore__ inline void TransDataTo5HD(const LocalTensor<T> (&dstList)[NCHW_CONV_ADDR_LIST_SIZE], const LocalTensor<T> (&srcList)[NCHW_CONV_ADDR_LIST_SIZE], const TransDataTo5HDParams& nchwconvParams) 
  ```

## 参数说明<a name="zh-cn_topic_0000001521260417_section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名称 | 含义 |
| :------- | :--- |
| T | 操作数数据类型。 |

**表2**  参数列表

| 参数名称 | 输入/输出 | 含义 |
| :------- | :-------- | :--- |
| dstList | 输出 | 目的操作数地址序列。<br><br>NCHW_CONV_ADDR_LIST_SIZE固定长度为16。<br>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)或者LocalTensor的地址值，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要按照32字节对齐。 |
| srcList | 输入 | 源操作数地址序列。<br><br>NCHW_CONV_ADDR_LIST_SIZE固定长度为16。<br>类型为LocalTensor或者LocalTensor的地址值，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要按照32字节对齐。 |
| dst | 输出 | 目的操作数。<br><br>类型为LocalTensor，连续存储对应LocalTensor的地址值。LocalTensor支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要32字节对齐。 |
| src | 输入 | 源操作数。<br><br>类型为LocalTensor，连续存储对应LocalTensor的地址值。LocalTensor支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要32字节对齐。 |
| nchwconvParams | 输入 | 用于控制TransdataTo5HD的数据结构和指令迭代的相关参数。<br>nchwconvParams参数说明请参考[TransDataTo5HDParams参数说明](#table3)。 |

**表3**  TransDataTo5HDParams参数说明<a id="table3"></a>

| 参数名称 | 类型 | 说明 |
| :------- | :--- | :--- |
| dstHighHalf | 输入 | 指定每个dstList地址中的数据存储到Datablock的高半部还是低半部，该配置只支持int8_t、uint8_t的数据类型。<br><br>支持的数据类型为bool，有以下两种取值：<br>•true：表示存储于Datablock的高半部<br>•false：表示存储于Datablock的低半部 |
| srcHighHalf | 输入 | 指定每个srcList地址中的数据从Datablock的高半部还是低半部读取，该配置只支持int8_t、uint8_t的数据类型。<br><br>支持的数据类型为bool，有以下两种取值：<br>•true：表示从Datablock的高半部读取<br>•false：表示从Datablock的低半部读取 |
| repeatTimes | 输入 | 重复迭代次数，repeatTimes∈[0,255]。<br><br>关于该参数的具体描述请参考[高维切分](../SIMD_compute/high_dim_split.md)。<br><br>注：<br>•repeatTimes=0表示不会执行转换操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。<br>•当repeatTimes为1时，目的操作数/源操作数的有效起始位置为dstList/srcList序列输入的起始位置加上dstRepStride/srcRepStride；repeatTimes为1，如果要让目的操作数/源操作数的有效起始位置为dstList/srcList序列输入的起始位置，需要将dstRepStride/srcRepStride置为0。<br>•当repeatTimes大于1时，第一次repeat中目的操作数/源操作数的有效起始位置为dstList/srcList序列输入的起始位置，第二次需要加上dstRepStride/srcRepStride。以此类推。 |
| dstRepStride | 输入 | 相邻迭代间，矢量目的操作数相同DataBlock地址步长，单位为DataBlock。 |
| srcRepStride | 输入 | 相邻迭代间，矢量源操作数相同DataBlock地址步长，单位为DataBlock。 |

## 数据类型

<!-- npu="950" id10 -->
Ascend 950PR/Ascend 950DT，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float。
<!-- end id10 -->

<!-- npu="A3" id11 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id11 -->

<!-- npu="910b" id12 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id12 -->

<!-- npu="310b" id13 -->
Atlas 200I/500 A2 推理产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id13 -->

<!-- npu="310p" id14 -->
Atlas 推理系列产品AI Core，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id14 -->

<!-- npu="910" id15 -->
Atlas 训练系列产品，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half。
<!-- end id15 -->

<!-- npu="x90" id16 -->
Kirin X90，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id16 -->

<!-- npu="9030" id17 -->
Kirin 9030，支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
<!-- end id17 -->

## 约束说明<a name="zh-cn_topic_0000001521260417_section633mcpsimp"></a>

- 操作数地址对齐要求请参见[Unified Buffer地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
- 操作数地址重叠约束请参考[Unified Buffer地址重叠约束](../../../general_description_and_constraints.md#section668772811100)。
- dst与src中的地址需要连续存放。
- 单次和多次repeat操作实现转置功能，都只需要配置一次srcList和dstList，后续LocalTensor地址通过nchwconvParams参数自动偏移。

## 返回值说明<a name="zh-cn_topic_0000001521260417_section640mcpsimp"></a>

无。

## 关键特性说明<a name="zh-cn_topic_0000002557628265_section16509954114712"></a>

### 单次repeat处理16个DataBlock的数据<a name="zh-cn_topic_0000002557628265_section166723295485"></a>

- 通过该接口做16个DataBlock大小的连续矩阵转置时，地址序列配置规则如下：
  - srcList：数组中相邻地址的间隔为blockSize个元素（blockSize = 32 / sizeof\(T\)），即从连续的DataBlock中读入源数据；
  - dstList：数组中相邻地址的间隔为blockSize个元素（blockSize = 32 / sizeof\(T\)），即目标数据写入连续的DataBlock中。

- 通过该接口做16个DataBlock大小的矩阵转置时，nchwconvParams配置规则如下：
  - repeatTimes：循环次数为1次；
  - srcRepStride：设置为0，让源操作数的有效起始位置为srcList序列的起始位置；
  - dstRepStride：设置为0，让目的操作数的有效起始位置为dstList的起始位置。

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // 只对int8_t、uint8_t的入参有效，从srcLocalList的高半位读取数据
  transDataParams.srcHighHalf = true; // 只对int8_t、uint8_t的入参有效，写入dstLocalList的高半位
  transDataParams.repeatTimes = 1;    // 重复迭代次数，每次repeat处理16个DataBlock
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // 每个DataBlock存储的元素个数，此处为32个
  uint64_t dstLocalList[16];
  for (int i = 0; i < 16; i++) {   // dstLocal为int8_t类型的LocalTensor
    dstLocalList[i] = (uint64_t)(dstLocal[width * i].GetPhyAddr());
  }
  uint64_t srcLocalList[16];
  for (int i = 0; i < 16; i++) {   // srcLocal为int8_t类型的LocalTensor
    srcLocalList[i] = (uint64_t)(srcLocal[width * i].GetPhyAddr());
  }
  AscendC::TransDataTo5HD<int8_t>(dstLocalList, srcLocalList, transDataParams);
  ```

- 当数据类型位宽为8位时，每个DataBlock包含32个数，从指定的16个DataBlock中的对应位置取值，组成半个DataBlock放入目的地址中，读取和存放是在DataBlock的高半部分还是低半部分由参数srcHighHalf和dstHighHalf决定。

  **图1** b8类型矩阵转置
  ![b8](../../../../figures/trans_data_to_5hd_b8.png)

- 当数据类型位宽为16位时，每个DataBlock包含16个数，从指定的16个DataBlock中的对应位置取值，组成1个新的DataBlock放入目的地址中。

  **图2** b16类型矩阵转置
  ![b16](../../../../figures/trans_data_to_5hd_b16.png)

- 当数据类型位宽为32位时，每个DataBlock包含8个数，从指定的16个DataBlock中的对应位置取值，组成2个新的DataBlock放入目的地址中。

  **图3** b32类型矩阵转置
  ![b32](../../../../figures/trans_data_to_5hd_b32.png)

- 16个DataBlock数据可以不连续，推荐通过GetPhyAddr获取LocalTensor的地址值存入地址数组。

  **图4**  单次repeat实现16个DataBlock数据转置<a name="zh-cn_topic_0000002557628265_fig12595141012208"></a>
  ![单次repeat实现16个DataBlock数据转置](../../../../figures/trans_data_to_5hd.png)

### 将NCHW格式转换成NC1HWC0格式<a name="zh-cn_topic_0000002557628265_section3122131914516"></a>

- NCHW格式转换成NC1HWC0格式时，如果是数据类型的位宽为32位或者16位，则C0=16；如果数据类型的位宽为8位，则C0=32。

- 连续数据通过该接口做矩阵转置或分形（NCHW-\>NC1HWC0）时，地址序列配置规则如下：
  - srcList：数组中相邻地址的间隔为H \* W个元素，即每个HW平面的起始位置；
  - dstList：数组中相邻地址的间隔为blockSize个元素（blockSize = 32 / sizeof\(T\)），即目标数据写入连续的DataBlock中。

- 连续数据通过该接口做矩阵转置或分形（NCHW-\>NC1HWC0）时，nchwconvParams配置规则如下：
  - repeatTimes：循环次数为H \* W / blockSize次（blockSize = 32 / sizeof\(T\)），即重复迭代覆盖整个HW平面；
  - srcRepStride：设置为1，即相邻迭代间，矢量源操作数步长为1 DataBlock；
  - dstRepStride：设置为16，即相邻迭代间，矢量目的操作数步长为16 DataBlock。

  ```cpp
  constexpr uint32_t N = 2;
  constexpr uint32_t C = 32;
  constexpr uint32_t H = 16;
  constexpr uint32_t W = 16;
  constexpr uint32_t C0 = 16;
  constexpr uint32_t C1 = C / C0; // 2
  // 总数据量
  constexpr uint32_t totalElements = N * C * H * W; // 16384
  // 外循环次数
  constexpr uint32_t loopCount = N * C /NCHW_CONV_ADDR_LIST_SIZE; // 4
  // 设置转换参数：NCHW → NC1HWC0
  AscendC::TransDataTo5HDParams transParams;
  transParams.repeatTimes = 16;  // hSize * wSize / elems_per_block处理整个HW平面需要的repeat次数
  transParams.dstRepStride = 16; // 循环间dstList间隔NCHW_CONV_ADDR_LIST_SIZE个DataBlock
  transParams.srcRepStride = 1;  // 循环间同一HW平面srcList连续
  // 循环处理数据
  for (uint32_t loop = 0; loop < loopCount; loop++) {
    // 设置地址列表
    uint64_t srcList[NCHW_CONV_ADDR_LIST_SIZE];
    uint64_t dstList[NCHW_CONV_ADDR_LIST_SIZE];
    for (int i = 0; i < NCHW_CONV_ADDR_LIST_SIZE; i++) {
      srcList[i] = (uint64_t)srcLocal[loop * NCHW_CONV_ADDR_LIST_SIZE * H * W + i * H * W].GetPhyAddr();
      dstList[i] = (uint64_t)dstLocal[loop * NCHW_CONV_ADDR_LIST_SIZE * H * W + i * C0].GetPhyAddr();
    }
    // 执行数据排布转换
    AscendC::TransDataTo5HD<T>(dstList, srcList, transParams);
  }
  ```

  ![nchw2nc1hwc0](../../../../figures/trans_data_to_5hd_nchw_nc1hwc0.png)

## 调用示例<a name="zh-cn_topic_0000001521260417_section19372434133520"></a>

本样中只展示部分关键代码。如果您需要运行样例代码，请将该代码段拷贝并替换[Transpose类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose)场景三完整样例模板中Compute函数的部分代码即可。

- 入参类型是LocalTensor的调用方式：

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // 只对int8_t/uint8_t的入参有效，从srcLocalList的高半位读取数据
  transDataParams.srcHighHalf = true; // 只对int8_t/uint8_t的入参有效，写入dstLocalList的高半位
  transDataParams.repeatTimes = 1; // 重复迭代次数，每次repeat处理16个DataBlock
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // 每个DataBlock存储的元素个数，此处为32个
  AscendC::LocalTensor<int8_t> dstLocalList[16];
  for (int i = 0; i < 16; i++) { // dstLocal为int8_t类型的LocalTensor
      dstLocalList[i] = dstLocal[width * i];
  }
  AscendC::LocalTensor<int8_t> srcLocalList[16];
  for (int i = 0; i < 16; i++) { // srcLocal为int8_t类型的LocalTensor
      srcLocalList[i] = srcLocal[width * i];
  }
  AscendC::TransDataTo5HD<int8_t>(dstLocalList, srcLocalList, transDataParams);
  ```

- 入参类型是LocalTensor地址值的调用方式，推荐使用：

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // 只对int8_t/uint8_t的入参有效，从srcLocalList的高半位读取数据
  transDataParams.srcHighHalf = true; // 只对int8_t/uint8_t的入参有效，写入dstLocalList的高半位
  transDataParams.repeatTimes = 1; // 重复迭代次数，每次repeat处理16个DataBlock
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // 每个DataBlock存储的元素个数，此处为32个
  uint64_t dstLocalList[16];
  for (int i = 0; i < 16; i++) { // dstLocal为int8_t类型的LocalTensor
      dstLocalList[i] = (uint64_t)(dstLocal[width * i].GetPhyAddr());
  }
  uint64_t srcLocalList[16];
  for (int i = 0; i < 16; i++) { // srcLocal为int8_t类型的LocalTensor
      srcLocalList[i] = (uint64_t)(srcLocal[width * i].GetPhyAddr());
  }
  AscendC::TransDataTo5HD<int8_t>(dstLocalList, srcLocalList, transDataParams);
  ```

- 入参类型是地址LocalTensor的调用方式：

  ```cpp
  AscendC::TransDataTo5HDParams transDataParams;
  transDataParams.dstHighHalf = true; // 只对int8_t/uint8_t的入参有效，从srcLocalList的高半位读取数据
  transDataParams.srcHighHalf = true; // 只对int8_t/uint8_t的入参有效，写入dstLocalList的高半位
  transDataParams.repeatTimes = 1; // 重复迭代次数，每次repeat处理16个DataBlock
  transDataParams.dstRepStride = 0;
  transDataParams.srcRepStride = 0;
  int width = 32 / sizeof(int8_t); // 每个DataBlock存储的元素个数，此处为32个
  // 使用TQue分配uint64_t的地址LocalTensor，用于存储dstLocal与srcLocal的地址
  AscendC::LocalTensor<uint64_t> dst = workQueueSrc1.AllocTensor<uint64_t>();
  for (int i = 0; i < 16; i++) { // dstLocal为int8_t类型的LocalTensor
      dst.SetValue(i, (uint64_t)(dstLocal[width * i].GetPhyAddr()));
  }
  AscendC::LocalTensor<uint64_t> src = workQueueSrc2.AllocTensor<uint64_t>();
  for (int i = 0; i < 16; i++) { // srcLocal为int8_t类型的LocalTensor
      src.SetValue(i, (uint64_t)(srcLocal[width * i].GetPhyAddr()));
  }
  AscendC::TransDataTo5HD<int8_t>(dst, src, transDataParams);
  // 释放地址LocalTensor
  workQueueSrc1.FreeTensor(dst);
  workQueueSrc2.FreeTensor(src);
  ```

  当输入输出为int8\_t类型时，结果示例如下：

  ```plain
  输入数据(src)：
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
  输出数据(dstGm)：
  // 从输入数据的高半位读取数据，写入输出数据的高半位
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

  当输入输出为half类型时，结果示例如下：

  ```plain
  输入数据(src)：
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
  
  输出数据(dstGm)：
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

  当输入输出为int32\_t类型时，结果示例如下：

  ```plain
  输入数据（src）：
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
  输出数据（dstGm）：
  [[ 0 8 16 24 32 40 48 56 64 72 80 88 96 104 112 120   ]
   [ 1 9 17 25 33 41 49 57 65 73 81 89 97 105 113 121   ] 
   [ 2 10 18 26 34 42 50 58 66 74 82 90 98 106 114 122  ]
   [ 3 11 19 27 35 43 51 59 67 75 83 91 99 107 115 123  ]
   [ 4 12 20 28 36 44 52 60 68 76 84 92 100 108 116 124 ] 
   [ 5 13 21 29 37 45 53 61 69 77 85 93 101 109 117 125 ]
   [ 6 14 22 30 38 46 54 62 70 78 86 94 102 110 118 126 ]
   [ 7 15 23 31 39 47 55 63 71 79 87 95 103 111 119 127 ]]
  ```
