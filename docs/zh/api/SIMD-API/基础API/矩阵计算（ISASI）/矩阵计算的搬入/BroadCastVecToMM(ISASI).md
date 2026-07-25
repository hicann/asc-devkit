# BroadCastVecToMM\(ISASI\)<a name="ZH-CN_TOPIC_0000001860143713"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：不支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
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

## 功能说明<a name="section618mcpsimp"></a>

将矢量数据广播到矩阵中，每个数据块中的每16个元素会被连续复制16次；当前支持的数据传输通路：Unified Buffer-\>L0C Buffer（VECIN/VECCALC/VECOUT-\>CO1）。

**图1** 功能示例<a name="fig1730933122314"></a>  

![](../../../../figures/功能示例.png "功能示例")

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void BroadCastVecToMM(const LocalTensor<T> &dst, const LocalTensor<U> &src, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap, const uint8_t dstGap)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1** 模板参数说明<a name="table4835205712588"></a>

| 参数名 | 描述 |
| ---------- | ---------- |
| T | dst的数据类型。 |
| U | src的数据类型。 |

**表2** 参数说明<a name="table284254116544"></a>

| 参数名称 | 类型 | 说明 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，结果矩阵，类型为LocalTensor，支持的物理地址为L0C Buffer（TPosition：CO1）。<br>LocalTensor的起始地址需要256个元素对齐。<br>支持的数据类型为：half、int32_t、float。 |
| src | 输入 | 源操作数，输入矢量，类型为LocalTensor，支持的物理地址为Unified Buffer（TPosition：VECIN/VECCALC/VECOUT）。<br>支持的数据类型需要与dst一致。 |
| blockCount | 输入 | 指定该指令包含的连续广播数据块个数，取值范围：blockCount∈[1, 255]。 |
| blockLen | 输入 | 指定该指令每个连续广播数据块长度，单位为16个元素。取值范围：blockLen∈[1, 255]。 |
| srcGap | 输入 | 源操作数，相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为datablock（32字节）。 |
| dstGap | 输入 | 目的操作数，相邻连续数据块间的间隔（前面一个数据块的尾与后面数据块的头的间隔），单位为256个元素。 |

## 数据类型

Atlas 推理系列产品AI Core，支持的数据类型为：half、int32_t、float。

## 约束说明<a name="section633mcpsimp"></a>

操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

## 调用示例<a name="section642mcpsimp"></a>

本示例中，输入bias形状为\[1, 32\]，输出c的形状为\[32, 32\]，格式为Nz，调用示例图如下：

**图2** 调用示例图<a name="fig496292825418"></a>  

![](../../../../figures/调用示例图.png "调用示例图")

本示例仅展示样例中的部分代码。如需运行，请参考[BroadCastVecToMM样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/broadcast_ub2l0c)实现完整的代码。

```cpp
// brcLocal为TPosition::CO1上的float类型的LocalTensor，srcLocal为TPosition::VECIN上的float类型的LocalTensor
// blockCount = 1，blockLen = 1，连续广播的数据块个数为1，每个数据块包含16个元素，共输出256个元素
// srcGap = 0，dstGap = 1，源操作数与目的操作数之间连续
AscendC::BroadCastVecToMM(brcLocal, srcLocal, 1, 1, 0, 1)
```
