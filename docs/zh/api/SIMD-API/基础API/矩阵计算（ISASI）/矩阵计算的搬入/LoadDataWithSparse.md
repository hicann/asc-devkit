# LoadDataWithSparse<a name="ZH-CN_TOPIC_0000002568950935"></a>

## 产品支持情况<a name="zh-cn_topic_0000002512171654_section796754519912"></a>

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
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->

## 功能说明<a name="zh-cn_topic_0000002512171654_section106841136114319"></a>

头文件路径为：`"basic_api/kernel_operator_mm_intf.h"`。

用于从L1 Buffer中搬运以512字节为单位存放的稠密权重矩阵到L0B Buffer里，同时搬运以128字节为单位的索引矩阵到内置的专用buffer空间（用于后续MmadWithSparse接口进行读取）。

索引矩阵的数据类型为uint2，需要拼成uint8的数据类型，再传入接口。索引矩阵在一个uint8的地址中的排布是逆序排布的，例如：索引矩阵在地址中的排布为1 0 2 1 0 1 2 0，其中1 0 2 1（对应索引矩阵前四位1 2 0 1）为一个uint8，0 1 2 0（对应索引矩阵后四位0 2 1 0）为一个uint8。

仅支持如下数据通路的搬运：L1 Buffer->L0B Buffer。

**图1** L1 Buffer->L0B Buffer LoadDataWithSparse示意图，LoadData2dParams参数配置startIndex = 1，repeatTimes = 5，表示需要从源操作数src中第1块数据分形开始搬运5块连续的数据分形，从源操作数idx中第1块index分形开始搬运5块连续的index分形。<a name="zh-cn_topic_0000002512171654_fig71111314164414"></a>  

![](../../../../figures/loaddatawithsparse_l12l0b.png)

## 函数原型<a name="zh-cn_topic_0000002512171654_section82039854412"></a>

```cpp
template <typename T = int8_t, typename U = uint8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type = true, typename Std::enable_if<Std::is_same<PrimT<U>, uint8_t>::value, bool>::type = true>
__aicore__ inline void LoadDataWithSparse(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx, const LoadData2dParams& loadDataParam)
```

## 参数说明<a name="zh-cn_topic_0000002512171654_section16128134420472"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| -------- | ------ |
| T | dst、src的数据类型。 |
| U | idx的数据类型。 |
| Std::enable_if\<Std::is_same\<PrimT\<T\>, int8_t\>::value, bool\>::type | 用于T的数据类型检查，用户无需关注。 |
| Std::enable_if\<Std::is_same\<PrimT\<U\>, uint8_t\>::value, bool\>::type | 用于U的数据类型检查，用户无需关注。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ----------- | ------ |
| dst | 输出 | 目的操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>支持的数据类型为int8_t。<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的物理存储位置为L0B Buffer(TPosition: B2)。<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的物理存储位置为L0B Buffer(TPosition: B2)。 |
| src | 输入 | 源操作数，类型为LocalTensor。<br>分形约束参考[矩阵计算输入搬运约束](矩阵计算输入搬运约束.md)。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>支持的数据类型为int8_t。<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的物理存储位置为L1 Buffer(TPosition: B1)。<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的物理存储位置为L1 Buffer(TPosition: B1)。 |
| idx | 输入 | 源操作数，类型为LocalTensor。<br>数据分形大小为128字节，每个数据分形shape为16 \* 32 \* 2bit。<br>起始地址对齐约束参考[对齐约束](矩阵计算输入搬运约束.md)。<br>支持的数据类型为uint8_t。<br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的物理存储位置为L1 Buffer(TPosition: B1)。<br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的物理存储位置为L1 Buffer(TPosition: B1)。 |
| loadDataParam | 输入 | LoadData参数结构体，类型为：<br>&nbsp;&nbsp;&bull; LoadData2dParams，具体参考[LoadData2dParams结构体内参数说明](LoadData_2D.md#table_load2d_params)。<br>需要注意的是，本接口仅支持连续的数据分形搬运，不支持跳stride，因此仅支持配置loadDataParam中的startIndex和repeatTimes参数，其余参数未使用，无需配置。 |

## 数据类型<a name="zh-cn_topic_0000002512171654_section4219135304818"></a>

支持数据类型为：src和dst支持int8_t，idx支持uint8_t。

## 返回值说明<a name="zh-cn_topic_0000002512171654_section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000002512171654_section2045914466492"></a>

- 当配置loadDataParam中参数repeatTimes=0时，表示不执行搬运，该接口将被视为NOP（空操作）。最大支持repeatTimes=255。
- 不支持转置功能，仅支持连续的数据分形搬运，不支持跳stride，仅支持配置loadDataParam中的startIndex和repeatTimes参数，其余参数未使用。
- 存放索引矩阵的专用buffer空间大小为L0B Buffer大小的四分之一，开发者无需配置地址，MmadWithSparse接口会自动从该buffer中读取数据。
- 仅支持L1 Buffer->L0B Buffer通路，且L1 Buffer上的分形为Zn。
- 每次迭代中的startIndex不能小于零。

## 调用示例<a name="zh-cn_topic_0000002512171654_section088124295117"></a>

本接口用于配合Sparse Mmad使用，完整示例请参考：[MmadWithSparse调用样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_with_sparse)。
