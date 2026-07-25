# MmadWithSparse<a name="ZH-CN_TOPIC_000000253823118110"></a>

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

MmadWithSparse接口负责完成特殊稀疏矩阵乘加操作。稀疏矩阵是一种特殊类型的矩阵，即矩阵中包含较多的零元素。4：2结构化稀疏要求一个连续的4个权重或激活值的组（通常是张量中的一行或一列）中，最多只有2个值为非零，其余2个强制为零。

<a name="section19443175724215"></a><a name="section618mcpsimp"></a>MmadWithSparse接口传入的左矩阵A为稀疏矩阵，右矩阵B为稠密矩阵。矩阵A是个全尺寸矩阵，在MmadWithSparse计算时完成稠密化；矩阵B是经过4：2结构化稀疏过滤掉零值之后的稠密矩阵，需要在计算执行前的输入数据准备时自行完成稠密化（按照下文中介绍的稠密算法进行稠密化）。B稠密矩阵需要通过调用LoadDataWithSparse载入，同时加载索引矩阵，索引矩阵在矩阵B稠密化的过程中生成，再用于A矩阵的稠密化。索引矩阵存储在内部缓冲区，该索引矩阵的布局和布局大小与矩阵B相同，用于在进行矩阵乘加操作之前进一步将矩阵A压缩。

跟Mmad接口实现昇腾NPU矩阵乘计算能力类似，MmadWithSparse接口的数学表达式为：

$$
C = A \times B + C
$$

完整示例请参考：[MmadWithSparse样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_with_sparse)。

**表1** Sparse矩阵计算矩阵A、B、C解释说明

| 矩阵计算逻辑 | 矩阵计算物理位置 | 维度 | 输入/输出数据格式 | 数据类型 |
| --- | --- | --- | --- | --- |
| A | L0A Buffer | M x K | Zz | int8_t |
| B | L0B Buffer | K/2 x N | Zn | int8_t |
| C | L0C Buffer | M x N | Nz | int32_t |

下面的图展示了Cube如何计算出其中一行和一列的内积：

**图1** MmadWithSparse接口计算流程示意图
![MmadWithSparse接口计算流程示意图](../../../../figures/mmadwithsparse_workflow_demo.png "MmadWithSparse接口计算流程示意图")

其中矩阵A原始分形为\(16, 2\*C0\)，索引矩阵Index分形为\(C0，16\)，每一行矩阵A的数据会基于索引矩阵Index中对应的一列数据进行4选2，索引矩阵分形格式及生成方式请参考[4选2稀疏索引矩阵](../矩阵计算分形介绍/辅助矩阵分形格式详解.md#4选2稀疏索引矩阵)，选择算法参考[矩阵A稀疏选择算法说明](#zh-cn_topic_0000002535726174_li1829819426378)；经过选择处理后的矩阵A分形变成\(16, C0\)，矩阵B原始分形为\(C0, 16\)，接下来会执行普通Mmad运算，即矩阵A中一行和矩阵B中一列完成内积运算得到结果矩阵C中对应一个元素。

- <a name="zh-cn_topic_0000002535726174_li1829819426378"></a>**矩阵A稀疏选择算法说明**

  索引矩阵经过LoadDataWithSparse指令后存储于Cube上内置的专用buffer空间，数据类型为uint8，分形格式为小n大Z，对应上图中的分形大小为（32，16）。每一个uint8类型的索引元素由4个uint2的原始数据组成，每两个2位索引数据可对应4位原始矩阵A。针对每一组2个索引数据，A矩阵的4个元素的选择过滤规则示例如下表：

  - 第一个索引数据0用于指示前3个元素中第1个非零元素的相对位置。
  - 第二个索引数据1用于指示第2个非零元素在后3个元素中的相对位置。
  - 其中，“-”表示不关心该位置上的值，即会被过滤。

  **表2** 矩阵A选择过滤规则表

  | 索引数据0 | 索引数据1 | 元素0 | 元素1 | 元素2 | 元素3 |
  | --------- | --------- | ----- | ----- | ----- | ----- |
  | 2’b10 | 2’b10 | - | - | X | Y |
  | 2’b01 | 2’b10 | - | X | - | Y |
  | 2’b00 | 2’b10 | X | - | - | Y |
  | 2’b01 | 2’b01 | - | X | Y | - |
  | 2’b00 | 2’b01 | X | - | Y | - |
  | 2’b00 | 2’b00 | X | Y | - | - |
  | 2’b10 | 2’b00 | - | - | X/X | - |
  | 2’b01 | 2’b00 | - | X/X | - | - |

  **图2** 矩阵A 4：2选择算法模型<a name="zh-cn_topic_0000002535726174_fig14223210123816"></a>
  ![矩阵A-4-2选择算法模型](../../../../figures/sparse_mmad_4select2.png "矩阵A-4-2选择算法模型")

  [图2](#zh-cn_topic_0000002535726174_fig14223210123816)展示了一个uint8类型的索引元素对应选择8个原始矩阵A元素的算法模型，最后输出4个选择后的矩阵A元素。

  1. 在正常使用情况下，软件应确保最多存在两个非零元素。如果发生错误，即存在三个或更多非零元素时，只会使用最低有效位（LSB）位置的前两个非零元素。
  2. 上表中使用的“-”表示“不关心该位置上的值”，即暗示可能存在三个或更多非零元素的情况。

## 函数原型

```cpp
template <typename T = int32_t, typename U = int8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int32_t>::value, bool>::type = true, typename Std::enable_if<Std::is_same<PrimT<U>, int8_t>::value, bool>::type = true>
__aicore__ inline void MmadWithSparse(const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<U>& filter, const MmadParams& mmadParams)
```

## 参数说明

**表3** 模板参数说明

| 参数名 | 描述 |
| ------ | ------------------------------------------------------------------------------------------------------------------ |
| T | dst的数据类型。 |
| U | fm、filter的数据类型。<br>当dst、fm、filter为基础数据类型时，T必须为int32_t类型，U必须为int8_t类型，否则编译失败。 |

**表4** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | --------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| dst | 输出 | 目的操作数，结果矩阵C，类型为LocalTensor，支持的物理存储位置为L0C Buffer（TPosition:CO1）。<br>LocalTensor的起始地址需要256个元素（1024字节）对齐。 |
| fm | 输入 | 源操作数，左矩阵A，类型为LocalTensor，支持的物理存储位置为L0A Buffer（TPosition: A2）。<br>LocalTensor的起始地址需要512字节对齐。 |
| filter | 输入 | 源操作数，右矩阵B，类型为LocalTensor，支持的物理存储位置为L0B Buffer（TPosition:B2）。<br>LocalTensor的起始地址需要512字节对齐。 |
| mmadParams | 输入 | 矩阵乘相关参数，类型为MmadParams。<br>具体定义请参考`${INSTALL_DIR}/asc/include/basic_api/interface/kernel_struct_mm.h`，`${INSTALL_DIR}`请替换为CANN软件安装后文件存储路径。<br>参数说明请参考[表5](#zh_cn_topic_mmadsparse_section5_table5)。 |

**表5** MmadParams结构体内参数说明（Sparse场景）<a id="zh_cn_topic_mmadsparse_section5_table5"></a>

| 参数名称 | 含义 |
| --------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| m | 左矩阵Height，取值范围：m∈[0, 4095]。默认值为0。 |
| n | 右矩阵Width，取值范围：n∈[0, 4095]。默认值为0。 |
| k | 左矩阵Width、右矩阵Height，取值范围：k∈[0, 4095]。默认值为0。 |
| cmatrixInitVal | 是否开启C矩阵默认初始化清零操作。默认值为 true。<br>&nbsp;&nbsp;&bull; true：C矩阵默认初始化为0；<br>&nbsp;&nbsp;&bull; false：C矩阵不进行默认操作，通过设置cmatrixSource参数进行初始化。 |
| cmatrixSource | 配置C矩阵初始值是否来源于BT Buffer。默认值为false。<br>&nbsp;&nbsp;&bull; false：不对L0C Buffer进行初始化操作；<br>&nbsp;&nbsp;&bull; true：使用BT Buffer（TPosition:C2）的数据对L0C Buffer进行初始化操作。<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持配置为true/false。<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持配置为true/false。<br>Atlas 200I/500 A2 推理产品，支持配置为true/false。<br>注意：带Bias输入的接口配置该参数无效，会根据Bias输入的位置来判断C矩阵初始值是否来源于BT Buffer。 |
| isBias | 该参数废弃，新开发内容不要使用该参数。如果需要累加初始矩阵，请使用带Bias的接口来实现；也可以通过cmatrixInitVal和cmatrixSource参数配置C矩阵的初始值来源来实现。推荐使用带Bias的接口，相比于配置cmatrixInitVal和cmatrixSource参数更加简单方便。<br>配置是否需要累加初始矩阵，默认值为false，取值说明如下：<br>&nbsp;&nbsp;&bull; false：矩阵乘，无需累加初始矩阵，C = A \* B。<br>&nbsp;&nbsp;&bull; true：矩阵乘加，需要累加初始矩阵，C += A \* B。 |
| unitFlag | unitFlag是一种Mmad指令和Fixpipe指令细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出。取值说明如下：<br>&nbsp;&nbsp;&bull; 0（2'b00）：不开启unitFlag；<br>&nbsp;&nbsp;&bull; 2（2'b10）：开启unitFlag，硬件执行完指令之后，不复位单元标记位；<br>&nbsp;&nbsp;&bull; 3（2'b11）：开启unitFlag，硬件执行完指令之后，复位单元标记位。<br>开启该功能时，须将Mmad指令和Fixpipe指令的unitFlag值设置为2或3。<br>该参数仅支持如下型号：<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品；<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品。<br>参数设置方案和特性细节可参考：[UnitFlag](../../矩阵计算（ISASI）/Mmad计算关键特性说明/UnitFlag.md#ZH-CN_TOPIC_00000025690709788)。 |
| kDirectionAlign | Sparse场景本开关默认为false，不支持配置为true。K方向对齐的核心功能是通过`kDirectionAlign`参数控制在使用float数据类型时，L0A Buffer和L0B Buffer矩阵在K方向上的对齐方式。 |
| fmOffset | 左矩阵offset（整个左矩阵对应一个值），支持Scalar（应与src_fm.dtype一致）/立即数，默认0。<br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |
| enSsparse | 开启结构化稀疏特性，默认false；<br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |
| enWinogradA | 指示矩阵a是否通过winograd_feature_map_transform()生成，用于支持winograd特性，bool类型，默认false；<br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |
| enWinogradB | 指示矩阵b是否通过winograd_weight_transform()生成，用于支持winograd特性，bool类型，默认false；<br>注：未使用，兼容旧款产品接口传入，Atlas A2 训练系列产品/Atlas A2 推理系列产品及往后产品不做处理。 |

## 数据类型<a id="zh_cn_topic_mmadsparse_section_datatype"></a>

**表6** A、B、C支持的精度类型组合（Atlas 200I/500 A2 推理产品）（Atlas A2 训练系列产品/Atlas A2 推理系列产品）（Atlas A3 训练系列产品/Atlas A3 推理系列产品）

| 左矩阵A | 右矩阵B | 结果矩阵C |
| ------- | ------- | --------- |
| int8_t | int8_t | int32_t |

## 返回值说明

无

## 约束说明

- 不同矩阵对于存储位置的约束：

  - 结果矩阵C只支持位于物理存储位置为L0C Buffer（TPosition:CO1）
  - 左矩阵A只支持位于物理存储位置为L0A Buffer（TPosition:A2）
  - 右矩阵B只支持位于物理存储位置为L0B Buffer（TPosition:B2）
- 原始稀疏矩阵B每4个元素中应保证最多2个非零元素，如果存在3个或更多非零元素，则仅使用前2个非零元素。
- 当M、K、N中的任意一个值为0时，表示指令不会执行，该接口将被视为NOP（空操作）。
- MmadWithSparse接口不支持Gemv模式。
- 其他特殊场景约束可参考[Mmad接口约束说明](Mmad.md#约束说明)。

## 调用示例

完整使用样例请参见[MmadWithSparse样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_with_sparse)。

```cpp
AscendC::LocalTensor<int8_t> a1Local(AscendC::TPosition::A1, a1Addr, aSize);
AscendC::LocalTensor<int8_t> a2Local(AscendC::TPosition::A2, a2Addr, aSize);
AscendC::LocalTensor<int8_t> b1Local(AscendC::TPosition::B1, b1Addr, bSize);
AscendC::LocalTensor<uint8_t> idxB1Local(AscendC::TPosition::B1, idxB1Addr, bSize / 4);
AscendC::LocalTensor<int8_t> b2Local(AscendC::TPosition::B2, b2Addr, bSize);
AscendC::LocalTensor<int32_t> cLocal(AscendC::TPosition::CO1, cAddr, cSize);

// GM->L1 Buffer，将原始矩阵a，稠密化矩阵b与对应idx矩阵搬运至L1 Buffer
CopyIn(a1Local, b1Local, idxB1Local);
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(EVENT_ID0);

// L1 Buffer->L0A Buffer/L0B Buffer，将原始矩阵a，稠密化矩阵b与对应idx矩阵搬运至L0A Buffer/L0B Buffer
SplitA(a1Local, a2Local);
SplitB(b2Local, b1Local, idxB1Local);
AscendC::SetFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE1_M>(EVENT_ID0);

// mmad需要指定矩阵的维度进行计算
uint32_t m = 128;
uint32_t k = 64;
uint32_t n = 128;
AscendC::MmadWithSparse(c1Local, a2Local, b2Local, { m, n, k, false, 0, false, false, false });
```
