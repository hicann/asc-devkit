# SetDeqScale<a name="ZH-CN_TOPIC_0000002573904763"></a>

## 产品支持情况<a name="zh-cn_topic_0000002563051145_section815614567413"></a>

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
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->


## 功能说明<a name="zh-cn_topic_0000002563051145_section748346451"></a>

头文件路径为：`"kernel_operator_vec_vconv_intf.h"`。

本接口用于设置DEQSCALE寄存器的值，DEQSCALE寄存器位宽为64bit，用于Vector计算单元上的量化计算，寄存器中存放的参数在不同场景下的含义不同：

> [!NOTE]说明
> 本文中类似`s322fp16`、`s162b8`的场景简称采用“源数据类型简写+数字2+目的数据类型简写”的形式，数字2表示转换到。例如，`s322fp16`可拆分为`s32`、`2`和`fp16`，表示将`int32_t`转换为`half`的场景；`s162b8`表示将`int16_t`转换为8bit位宽数据类型的场景。数据类型简写规则请参见[内置数据类型](../../数据结构/内置数据类型.md#datatype-abbreviation)。

**表1**  DEQSCALE寄存器比特位含义映射表

| 模式 | 比特位数 | 变量名 | 含义 |
| :--- | :------- | :----- | :--- |
| [AddDeqRelu](../复合计算/AddDeqRelu.md)、[Cast](../类型转换/Cast.md)、[CastDequant](../复合计算/CastDequant.md)的s322fp16场景 | 0~15 | scale | 一个half类型数据。 |
| CastDequant不开启向量量化的s162b8场景 | 0~31 | scale | 一个float类型数据M（硬件在计算时将其视为(1,8,10)格式，即1个符号位、8个指数位和10个尾数位）。 |
| CastDequant不开启向量量化的s162b8场景 | 37~45 | offset | 一个有符号的9位整数。 |
| CastDequant不开启向量量化的s162b8场景 | 46 | signMode | 用于指示量化结果是否有符号（其中0表示无符号，1表示有符号）。 |
| CastDequant开启向量量化的s162b8场景 | 0~13 | vdeqAddr | 被硬件视为一个地址，指向UB中的一块空间，该空间大小为128B。空间被切分为16个64bit，每64bit可用于1次量化计算。 |

## 函数原型<a name="zh-cn_topic_0000002563051145_section21861260618"></a>

- 用于AddDeqRelu、Cast、CastDequant的s322fp16场景。<a id="func1"></a>

  ```cpp
  __aicore__ inline void SetDeqScale(half scale)
  ```

- 用于CastDequant不开启向量量化的s162b8场景。

  ```cpp
  __aicore__ inline void SetDeqScale(float scale, int16_t offset, bool signMode)
  ```

- 用于CastDequant开启向量量化的s162b8场景。

  ```cpp
  template <typename T>
  __aicore__ inline void SetDeqScale(const LocalTensor<T>& vdeq, const VdeqInfo& vdeqInfo)
  ```

## 参数说明<a name="zh-cn_topic_0000002563051145_section138547361666"></a>

**表2**  模板参数说明

| 参数名 | 描述 |
| :-- | :-- |
| T | 输入量化Tensor的数据类型。支持的数据类型为uint64_t。 |

**表3**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :--- | :--- |
| scale | 输入 | scale量化参数。<br>AddDeqRelu、Cast、CastDequant的s322fp16场景下数据类型为half。<br>CastDequant（isVecDeq=false）场景设置下数据类型为float。 |
| offset | 输入 | offset量化参数，int16_t类型，只有前9位有效。<br>用于CastDequant（isVecDeq=false）的场景，设置offset。 |
| signMode | 输入 | bool类型，表示量化结果是否带符号。<br>用于CastDequant（isVecDeq=false）的场景，设置signMode。 |
| vdeq | 输入 | 类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br>用于CastDequant（isVecDeq=true）的场景，输入量化tensor，大小为128Byte。 |
| vdeqInfo | 输入 | [VdeqInfo](#table4)类型，用于CastDequant（isVecDeq=true）的场景，结构体内包含量化tensor中的16组量化参数，接口会读取结构体内的参数，填充到vdeq。 |

**表4**  VdeqInfo结构体内参数说明<a id="table4"></a>

| 参数名 | 描述 |
| :--- | :--- |
| vdeqScale | float类型的数组，用于存储量化tensor中的scale参数scale<sub>0</sub>-scale<sub>15</sub>。 |
| vdeqOffset | int16_t类型的数组，用于存储量化tensor中的offset参数offset<sub>0</sub>-offset<sub>15</sub>。 |
| vdeqSignMode | bool类型的数组，用于存储量化tensor中的signMode参数signMode<sub>0</sub>-signMode<sub>15</sub>。 |

## 约束说明<a name="zh-cn_topic_0000002563051145_section1323412155712"></a>

- offset量化参数，int16\_t类型，只有前9位有效。
- vdeq地址对齐约束参考[地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="zh-cn_topic_0000002563051145_section5349145316712"></a>

- SetDeqScale(half scale)。

  ```cpp
  // 配合Cast的s322fp16场景使用
  // dstLocal为half类型的LocalTensor，srcLocal为int32_t类型的LocalTensor
  uint32_t srcSize = 256; // 参与计算的元素个数
  half scale = 1.0; // 量化参数为1
  AscendC::SetDeqScale(scale);
  // dst = src
  AscendC::Cast(dstLocal, srcLocal, AscendC::RoundMode::CAST_NONE, srcSize);
  ```

- SetDeqScale\(float scale, int16\_t offset, bool signMode\)。

  ```cpp
  // 配合CastDequant（isVecDeq=false）场景使用
  // dstLocal为int8_t类型的LocalTensor，srcLocal为int16_t类型的LocalTensor
  uint32_t srcSize = 256; // 参与计算的元素个数
  float scale = 1.0; // 量化参数为1
  int16_t offset = 0; // 不带偏移
  bool signMode = true; // dstLocal为int8_t类型，为有符号数
  AscendC::SetDeqScale(scale, offset, signMode);
  // dst = src
  AscendC::CastDequant<int8_t, int16_t, false, false>(dstLocal, srcLocal, srcSize);
  ```

- SetDeqScale\(const LocalTensor<T\>& vdeq, const VdeqInfo& vdeqInfo\)。

  ```cpp
  // 配合CastDequant（isVecDeq=true）场景使用
  // dstLocal为int8_t类型的LocalTensor，srcLocal为int16_t类型的LocalTensor
  uint32_t srcSize = 256; // 参与计算的元素个数
  float vdeqScale[16] = { 0 };
  int16_t vdeqOffset[16] = { 0 };
  bool vdeqSignMode[16] = { 0 };
  for (int i = 0; i < 16; i++) {
      vdeqScale[i] = 1.0; // 量化参数为1
      vdeqOffset[i] = 0; // 不带偏移
      vdeqSignMode[i] = true; // dstLocal为int8_t类型，为有符号数
  }
  AscendC::VdeqInfo vdeqInfo(vdeqScale, vdeqOffset, vdeqSignMode);
  AscendC::SetDeqScale<uint64_t>(tmpBuffer, vdeqInfo);
  // dst = src
  AscendC::CastDequant<int8_t, int16_t, true, true>(dstLocal, srcLocal, srcSize);
  ```
