# LoadData（GMToL1-2D矩阵搬运）<a id="ZH-CN_TOPIC_0000002538231138"></a>

## 产品支持情况<a id="zh-cn_topic_0000002567745223_section796754519912"></a>

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

## 功能说明<a id="zh-cn_topic_0000002567745223_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

负责完成普通矩阵计算所需的2D格式数据的搬运，以大小为512字节的数据分形为单位从Global Memory搬运至L1 Buffer（TPosition为A1/B1）。

## 函数原型<a id="zh-cn_topic_0000002567745223_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const LoadData2DParams& loadDataParams)
```

## 参数说明<a id="zh-cn_topic_0000002567745223_section16128134420472"></a>

**表1** 通用参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考矩阵计算输入搬运约束中的[对齐约束](矩阵计算输入搬运约束.md)。<br>数据类型和src的数据类型保持一致。<br>支持的物理存储位置为L1 Buffer（TPosition为A1/B1）。 |
| src | 输入 | 源操作数，类型为GlobalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考矩阵计算输入搬运约束中的[对齐约束](矩阵计算输入搬运约束.md)。<br>数据类型和dst的数据类型保持一致。<br>支持的物理存储位置为Global Memory（TPosition为GM）。 |
| loadDataParams | 输入 | LoadData参数结构体，类型为LoadData2DParams，具体参考[表2](#zh-cn_topic_0000002567745223_table8955841508)。 |

**表2** LoadData2DParams结构体内参数说明<a id="zh-cn_topic_0000002567745223_table8955841508"></a>

| 参数名称 | 含义 |
| ---------- | ---------- |
| startIndex | 分形矩阵ID，说明搬运起始位置为源操作数中第几个分形（0为源操作数中第1个分形矩阵）。取值范围：startIndex∈[0, 65535]。单位：512字节。默认为0。 |
| repeatTimes | 迭代次数，每个迭代可以处理512字节数据。取值范围：repeatTimes∈[0, 255]。<br>**注：repeatTimes = 0表示不执行搬运，该接口将被视为NOP（空操作）。** |
| srcStride | 相邻迭代间，源操作数前一个分形与后一个分形**起始地址**的间隔，单位：512字节。取值范围：srcStride∈[0, 65535]。默认为0。<br>**注：srcStride = 0表示在连续的重复执行周期之间，重复获取相同的分形矩阵。** |
| sid | 此参数用户无需关注，设置为0即可。 |
| dstGap | 相邻迭代间，目的操作数前一个分形**结束地址**与后一个分形**起始地址**的间隔，单位：512字节。取值范围：dstGap∈[0, 65535]。默认为0。<br>**注：dstGap = 0表示相邻repeat目的操作数起始地址间隔1个数据分形即连续存放。** |
| ifTranspose | 是否启用转置功能，对每个分形矩阵进行转置，默认为false：<br>&nbsp;&nbsp;&bull; true：启用。<br>&nbsp;&nbsp;&bull; false：不启用。<br>**注：本通路场景下不支持转置，参数无意义，保持默认即可。** |
| addrMode | 用于控制多次迭代场景下，源操作数中每一次迭代的分形矩阵索引ID是递增还是递减，0表示递增，下一次repeat index = startIndex + srcStride \* repeatTimes。1表示递减，下一次repeat index = startIndex - srcStride \* repeatTimes。 |

## 数据类型<a id="zh-cn_topic_0000002567745223_section4219135304818"></a>

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float。
<!-- end id10 -->

<!-- npu="A3" id11 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：b8、b16、b32。
<!-- end id11 -->

<!-- npu="910b" id12 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：b8、b16、b32。
<!-- end id12 -->

<!-- npu="310b" id13 -->
针对Atlas 200I/500 A2 推理产品，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half、bfloat16_t、uint32_t、int32_t、float。
<!-- end id13 -->

<!-- npu="310p" id14 -->
针对Atlas 推理系列产品AI Core，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half。
<!-- end id14 -->

<!-- npu="910" id15 -->
针对Atlas 训练系列产品，支持数据类型为：uint8_t、int8_t、uint16_t、int16_t、half。
<!-- end id15 -->

<!-- npu="x90" id16 -->
针对Kirin X90，支持数据类型为：int8_t、half。
<!-- end id16 -->

<!-- npu="9030" id17 -->
针对Kirin 9030，支持数据类型为：half。
<!-- end id17 -->

## 返回值说明

无

## 约束说明<a id="zh-cn_topic_0000002567745223_section2045914466492"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
- 本通路搬运过程中不支持转置。
- 目的地址必须32字节对齐。源地址必须1字节对齐，指令执行占用的流水为PIPE_MTE2。
- 当srcStride=0时，表示连续的repeat之间读取的源操作数中的同一块数据分形。
<!-- npu="910" id18 -->
- 针对Atlas 训练系列产品不支持dstGap设置参数。
<!-- end id18 -->

## 调用示例<a id="zh-cn_topic_0000002567745223_section088124295117"></a>

如下示例中：在数据类型为half的场景下，通过调用LoadData接口完成GM->L1 Buffer的Nz2Nz数据搬运。

搬运过程的数据排布变化示意图如下：

![](../../../../figures/load2d_gm2l1_demo.png)

示例代码片段如下：

```cpp
// m=32, k=32, fractalShape[0] = 16,fractalShape[1] = 16, fractalSize = 256;
uint32_t dstOffset = fractalSize;
uint32_t srcOffset = fractalSize;
AscendC::LoadData2DParams loadDataParams;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1]);
loadDataParams.srcStride = CeilDivision(m, fractalShape[0]);
loadDataParams.dstGap = CeilDivision(m, fractalShape[0]) - 1;
for (uint16_t i = 0; i < CeilDivision(m, fractalShape[0]); ++i) {
    AscendC::LoadData(a1Local[i * dstOffset], aGM[i * srcOffset], loadDataParams);
}
```
