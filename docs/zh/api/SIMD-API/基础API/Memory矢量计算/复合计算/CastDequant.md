# CastDequant<a name="ZH-CN_TOPIC_0000001788011400"></a>

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
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品 AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_vconv_intf.h"`。

对输入做量化并进行精度转换，对输出结果的溢出采用饱和处理。本接口需要与[SetDeqScale](../类型转换辅助配置接口/SetDeqScale.md)接口配合使用。

- 输入数据类型为int16_t，关闭向量量化模式。

  使用`SetDeqScale`接口设置的scale、offset、signMode参数，对输入整个张量做Per-Tensor量化，输出b8数据类型。计算公式如下：

  $$
  dst_i = Cast((src_i * scale) + offset)
  $$

- 输入数据类型为int16_t，开启向量量化模式。

  从`SetDeqScale`接口设置16组量化参数，每组包含一个scale、offset、signMode，对输入张量做Per-Block量化，一次repeat处理8个DataBlock，每个DataBlock为32B大小，对每个DataBlock上的16个输入数据做量化并进行精度转换，每个数的计算使用一组独立的量化参数，输出为b8数据类型。计算公式如下：

  $$
  dst_i = Cast((src_i * scale_j) + offset_j), j \in [0,15]
  $$

- 输入类型为int32_t，输出half数据类型。

  使用`SetDeqScale`接口设置的scale参数，采用RINT精度舍入模式。计算公式如下：

  $$
  dst_i = Cast(src_i * scale)
  $$

## 函数原型<a name="section620mcpsimp"></a>

- tensor前n个数据计算

  ```cpp
  template <typename T, typename U, bool isVecDeq = true, bool halfBlock = true>
  __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint32_t count)
  ```

- tensor高维切分计算
  - mask逐bit模式

    ```cpp
    template <typename T, typename U, bool isSetMask = true, bool isVecDeq = true, bool halfBlock = true>
    __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

  - mask连续模式

    ```cpp
    template <typename T, typename U, bool isSetMask = true, bool isVecDeq = true, bool halfBlock = true>
    __aicore__ inline void CastDequant(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
    ```

## 参数说明<a name="section622mcpsimp"></a>

**表**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
|T|目的操作数数据类型。<br>当类型为b8时，和`SetDeqScale`接口的signMode入参配合使用，当signMode=true时输出数据类型int8_t；signMode=false时输出数据类型uint8_t。|
|U|源操作数数据类型。<br> |
|isSetMask|是否在接口内部设置mask。<br>&bull; true，表示在接口内部设置mask。<br>&bull; false，表示在接口外部设置mask，开发者需要使用[SetVectorMask](../掩码操作/SetVectorMask.md)接口设置mask值。这种模式下，接口入参中的mask值设置为占位符`MASK_PLACEHOLDER`，用于占位，无实际含义。|
|isVecDeq|控制是否选择向量量化模式。<br>和`SetDeqScale`接口配合使用，当`SetDeqScale`接口传入Tensor时，isVecDeq必须为true。|
|halfBlock|输出数据类型为b8时有效，用于指示输出元素存放在上半还是下半DataBlock。<br>halfBlock=true时，结果存放在下半DataBlock；halfBlock=false时，结果存放在上半DataBlock。<br>如下图所示，一个DataBlock大小的输入得到8个b8类型输出，当halfBlock为false时，这8个输出数据存放在上半DataBlock。<br>![](../../../../figures/halfBlock说明.png)<br>若需要得到连续的目的操作数数据，可以通过将DataBlock分为奇偶块，调用两次接口分别处理奇偶块，奇数块的输出存放在下半DataBlock，偶数块输出存放在上半DataBlock。<br>![](../../../../figures/castdequant_continous.png) |

**表**  参数说明

|参数名称|输入/输出|说明|
|---|---|---|
|dst|输出|目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。|
|src|输入|源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。|
|count|输入|参与计算的元素个数。<br>参数取值范围和操作数的数据类型有关，数据类型不同，能够处理的元素个数最大值不同，最大处理的数据量不能超过UB大小限制。|
|mask/mask[]|输入|mask用于控制每次迭代内参与计算的元素。<br>注意：数据类型转换的mask会按照输入和输出类型中sizeof(dtype)较大的来筛选。<br>设置详见[掩码操作](../掩码操作/掩码操作.md)。|
|repeatTime|输入|重复迭代次数。<br>矢量计算单元，每次读取连续的256Bytes数据进行计算，为完成对输入数据的处理，必须通过多次迭代（repeat）才能完成所有数据的读取与计算。repeatTime表示迭代的次数。<br>关于该参数的具体描述请参考[高维切分](../SIMD计算说明/高维切分.md)。|
|repeatParams|输入|控制操作数地址步长的参数。[UnaryRepeatParams](../../辅助数据结构//UnaryRepeatParams.md)类型，包含操作数相邻迭代间相同DataBlock的地址步长，操作数同一迭代内不同DataBlock的地址步长等参数。<br>相邻迭代间的地址步长参数说明请参考[repeatStride](../SIMD计算说明/高维切分.md)；同一迭代内DataBlock的地址步长参数说明请参考[dataBlockStride](../SIMD计算说明/高维切分.md)。|

## 数据类型

<!-- npu="950" id8 -->

**表**  Ascend 950PR/Ascend 950DT支持的数据类型组合情况

| src数据类型 | dst数据类型 |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id8 -->

<!-- npu="A3" id9 -->

**表**  Atlas A3 训练系列产品/Atlas A3 推理系列产品支持的数据类型组合情况

| src数据类型 | dst数据类型 |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id9 -->

<!-- npu="910b" id10 -->

**表**  Atlas A2 训练系列产品/Atlas A2 推理系列产品支持的数据类型组合情况

| src数据类型 | dst数据类型 |
| :---------- | :---------- |
| int32_t     | half        |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id10 -->

<!-- npu="310p" id11 -->

**表**  Atlas 推理系列产品AI Core支持的数据类型组合情况

| src数据类型 | dst数据类型 |
| :---------- | :---------- |
| int16_t     | int8_t      |
| int16_t     | uint8_t     |

<!-- end id11 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#section668772811100)。
- 当输出类型为b8时，目的操作数数据类型需要和`SetDeqScale`接口的signMode入参配合，当signMode=true时输出数据类型int8_t；signMode=false时输出数据类型uint8_t。
- halfBlock参数仅在输出数据为b8类型时有效。
<!-- npu="A3,910b,950" id12 -->
- 当参数count或repeatTime取值为0时，该接口的行为如下：
  <!-- npu="A3,910b" id13 -->
  - 针对如下型号，该接口不会执行计算操作，不会对目的操作数进行写入，该接口将被视为NOP（空操作）。
    <!-- npu="A3" id14 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品
    <!-- end id14 -->
    <!-- npu="910b" id15 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品
    <!-- end id15 -->
  <!-- end id13 -->
  <!-- npu="950" id16 -->
  - 针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count或repeatTime取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id16 -->
<!-- end id12 -->
<!-- npu="950" id17 -->
- 对UB空间的占用说明。针对Ascend 950PR/Ascend 950DT：
  - tensor高维切分计算接口占用8KB Unified Buffer临时空间。
  - tensor前n个数据连续计算接口不涉及8KB Unified Buffer临时空间的占用。
<!-- end id17 -->
- 当输入类型为int16_t，开启向量量化模式时，`SetDeqScale`接口需要传入至少128B大小的UB空间，若空间不足128B，可能导致精度错误。
- 该接口需要与`SetDeqScale`配合使用，使用关系映射表如下：

  **表**  CastDequant与SetDeqScale使用关系映射表
  |CastDequant使用场景|对应使用的SetDeqScale函数原型|
  |---|---|
  |输入类型为int16_t，关闭向量量化模式|<pre>\_\_aicore\_\_ inline void SetDeqScale(float scale, int16_t offset, bool signMode)</pre>|
  |输入类型为int16_t，开启向量量化模式|<pre>template &lt;typename T&gt;<br>\_\_aicore\_\_ inline void SetDeqScale(const LocalTensor&lt;T&gt;& vdeq, const VdeqInfo& vdeqInfo)</pre>|
  |输入类型为int32_t，输出数据类型为half类型|<pre>\_\_aicore\_\_ inline void SetDeqScale(half scale)</pre>|

## 关键特性

### 溢出保护

该接口在执行int32_t到half数据类型的反量化时，会进行防溢出保护处理：
$$dst_i = Cast(src_i / 2^{17} \times scale \times 2^{17}) = Cast(src_i \times scale)$$
中间缩小步骤将数值控制在安全范围内，有效避免了浮点溢出。

## 调用示例<a name="section642mcpsimp"></a>

完整的调用样例可参考[复合计算样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/element_wise_compound_compute)。

- 高维切分计算接口样例-mask连续模式。

  ```cpp
  int32_t mask = 256 / sizeof(int16_t);
  // repeatTime = 2, 128 elements one repeat, 256 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::CastDequant<uint8_t, int16_t, true, true, true>(dstLocal, srcLocal, mask, 2, { 1, 1, 8, 8 });
  ```

- 高维切分计算接口样例-mask逐bit模式。

  ```cpp
  uint64_t mask[2] = { UINT64_MAX, UINT64_MAX };
  // repeatTime = 2, 128 elements one repeat, 256 elements total
  // dstBlkStride, srcBlkStride = 1, no gap between blocks in one repeat
  // dstRepStride, srcRepStride = 8, no gap between repeats
  AscendC::CastDequant<uint8_t, int16_t, true, true, true>(dstLocal, srcLocal, mask, 2, { 1, 1, 8, 8 });
  ```

- 前n个数计算接口样例。

  ```cpp
  AscendC::CastDequant<uint8_t, int16_t, true, true>(dstLocal, srcLocal, 256);
  ```

结果示例如下：

```plain
输入数据srcLocal: 
[20 53 26 12 36  6 20 93 66 30 56 99 59 92  7 37 22 47 98 10 85 29 14 46
 17 34 45 17 25 45 82 17 66 94 68 23 67  8 89  8 92  6 10 80 87 20  9 81
 70 62 11 58 38 83 32 14 38 47 41 63 94 26 96 89 88 35 86 55 60 82 15 65
 92 67 83 23 63 25 85 93 50 91 75 60 80 10 55 20 71 14 67 23 31 63  7 93
 69 45 61 23 43 86 11 81 81 36 76 58 53 25 23 51 59 78 82 10 39 40 24 50
 68 49 79 40  4 53 22 38 45 17 29 54  9 66 98 47 12 47 47 20 98  0 59 77
  1 21 39 70 66 20 68  8 77 77 54  0  3 33 37 37 48 60 83 88 27 70 31 49
 75 21 59  3 99 84 92 84 14 44 26 56 72 56 37 52 39 11  2 59 59 65 71 64
 10 65 62 48 42 79 69 69 27 99  8 38 36 77 34 34 60 50 52 50 41 31 95 68
 27 16 42 64 19 47  0 10 36 36 33 62 98 64 32 81 49 53 27 70 35  9 63  7
 10 89  3 39 94 23 89 16 23 60 71 42 46 58 65 90]
输出数据dstLocal: 
[ 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 20 53 26 12 36  6 20 93
 66 30 56 99 59 92  7 37  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 22 47 98 10 85 29 14 46 17 34 45 17 25 45 82 17  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 66 94 68 23 67  8 89  8 92  6 10 80 87 20  9 81
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 70 62 11 58 38 83 32 14
 38 47 41 63 94 26 96 89  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 88 35 86 55 60 82 15 65 92 67 83 23 63 25 85 93  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 50 91 75 60 80 10 55 20 71 14 67 23 31 63  7 93
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 69 45 61 23 43 86 11 81
 81 36 76 58 53 25 23 51  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 59 78 82 10 39 40 24 50 68 49 79 40  4 53 22 38  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 45 17 29 54  9 66 98 47 12 47 47 20 98  0 59 77
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1 21 39 70 66 20 68  8
 77 77 54  0  3 33 37 37  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 48 60 83 88 27 70 31 49 75 21 59  3 99 84 92 84  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 14 44 26 56 72 56 37 52 39 11  2 59 59 65 71 64
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 10 65 62 48 42 79 69 69
 27 99  8 38 36 77 34 34  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 60 50 52 50 41 31 95 68 27 16 42 64 19 47  0 10  0  0  0  0  0  0  0  0
   0  0  0  0  0  0  0  0 36 36 33 62 98 64 32 81 49 53 27 70 35  9 63  7
   0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 10 89  3 39 94 23 89 16
 23 60 71 42 46 58 65 90]
```
