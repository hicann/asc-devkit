# SetFixpipeNz2ndFlag<a id="ZH-CN_TOPIC_0000002569070993"></a>

## 产品支持情况<a id="zh-cn_topic_0000002516209232_section18204144912492"></a>

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->

## 功能说明<a id="zh-cn_topic_0000002516209232_section618mcpsimp"></a>

DataCopy数据搬运（[L0C Buffer->GM](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)）过程中进行随路格式转换（NZ格式转换为ND格式）时，通过调用该接口设置格式转换的相关配置。

## 函数原型<a id="zh-cn_topic_0000002516209232_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint16_t dstNdStride)
```

<!-- npu="950" id10 -->
```cpp
// 如下原型仅Ascend 950PR/Ascend 950DT支持
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
```
<!-- end id10 -->

## 参数说明<a id="zh-cn_topic_0000002516209232_section622mcpsimp"></a>

<!-- npu="A3,910b,310b" id11 -->
针对如下产品型号，参数说明见下表：
<!-- npu="A3" id12 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id12 -->
<!-- npu="910b" id13 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id13 -->
<!-- npu="310b" id14 -->
Atlas 200I/500 A2 推理产品
<!-- end id14 -->
**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| ndNum | 输入 | nd矩阵的数量，取值范围∈[1, 65535]。（[具体含义图示参见NZ2ND介绍特性章节](../矩阵搬出关键特性说明/NZ2ND.md)） |
| srcNdStride | 输入 | 以分形大小为单位的源步长，源相邻nz矩阵的偏移（头与头）。当ndNum大于1时，取值范围∈[1, 512]；当ndNum配置为1时，srcNdStride可配置为0，此时不生效。单位为1024B。 |
| dstNdStride | 输入 | 目的相邻nd矩阵的偏移（头与头）。取值范围dstNdStride∈[1, 65535]，单位为element元素。 |
<!-- end id11 -->

<!-- npu="950" id15 -->
针对Ascend 950PR/Ascend 950DT：

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| ndNum | 输入 | nd矩阵的数量，取值范围∈[1, 65535]。（[具体含义图示参见NZ2ND介绍特性章节](../矩阵搬出关键特性说明/NZ2ND.md)） |
| srcNdStride | 输入 | 以分形大小为单位的源步长，源相邻nz矩阵的偏移（头与头）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。取值范围∈[0, 65535]，单位为C0_SIZE。 |
| dstNdStride | 输入 | 目的相邻nd矩阵的偏移（头与头）。取值范围dstNdStride∈[1, 2^32-1]，单位为element元素。 |
<!-- end id15 -->

<!-- npu="x90,9030" id16 -->
针对如下产品型号，参数说明见下表：
<!-- npu="x90" id17 -->
Kirin X90 处理器系列产品
<!-- end id17 -->
<!-- npu="9030" id18 -->
Kirin 9030 处理器系列产品
<!-- end id18 -->
**表3** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| ndNum | 输入 | nd矩阵的数量，取值范围∈[1, 65535]。（[具体含义图示参见NZ2ND介绍特性章节](../矩阵搬出关键特性说明/NZ2ND.md)） |
| srcNdStride | 输入 | 以分形大小为单位的源步长，源相邻nz矩阵的偏移（头与头）。当ndNum配置为1时，srcNdStride配置为0即可，不生效。取值范围∈[0, 65535]，单位为C0_SIZE。 |
| dstNdStride | 输入 | 目的相邻nd矩阵的偏移（头与头）。取值范围dstNdStride∈[0, 65535]，单位为element元素。 |
<!-- end id16 -->

## 数据类型

参数的数据类型见[函数原型](#zh-cn_topic_0000002516209232_section620mcpsimp)。

## 返回值说明<a id="section640mcpsimp"></a>

无

## 约束说明<a id="zh-cn_topic_0000002516209232_section633mcpsimp"></a>

- 调用DataCopy接口并且需要进行随路NZ格式转换为ND格式之前，必须先调用SetFixpipeNz2ndFlag设置格式转换配置。

## 调用示例<a id="section6461234123118"></a>

完整示例可参考[data_copy_l0c2gm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm)。

```cpp
uint16_t ndNum = 2;
uint16_t srcNdStride = 2;
uint16_t dstNdStride = 1;
AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride); // 设置FIX搬运NZ格式到ND格式转换的参数
```
