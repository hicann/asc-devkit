# asc_enable_hf32

## 产品支持情况

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

用于设置Mmad计算开启HF32模式，开启该模式后，Mmad计算FP32数据的性能将得到提升，但会带来一定的精度损失。

是否开启HF32对Mmad计算理论性能的影响见下表：

<!-- npu="A3,910b" id8 -->
**表1** HF32对Mmad计算理论性能的影响（[NPU架构版本2201](../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123815311"></a>  

| 接口 | 左矩阵A | 右矩阵B | $cube_m$ | $cube_n$ | $cube_k$ | $k_0$ |
| --- | --- | --- | --- | --- | --- | --- |
| asc_mmad（不开启HF32） | float | float | 16 | 16 | 4 | 8 |
| asc_mmad（开启HF32） | float | float | 16 | 16 | 8 | 8 |
<!-- end id8 -->

<!-- npu="950" id9 -->
**表2** HF32对Mmad计算理论性能的影响（[NPU架构版本3510](../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123815915"></a>  

| 接口 | 左矩阵A | 右矩阵B | $cube_m$ | $cube_n$ | $cube_k$ | $k_0$ |
| --- | --- | --- | --- | --- | --- | --- |
| asc_mmad（不开启HF32） | float | float | 16 | 16 | 1 | 8 |
| asc_mmad（开启HF32） | float | float | 16 | 16 | 8 | 8 |
<!-- end id9 -->

性能计算公式如下：

$$
\begin{gathered}
{ceil_m} = \left\lceil \frac{m}{16} \right\rceil \times 16 \\[12pt]
{ceil_n} = \left\lceil \frac{n}{16} \right\rceil \times 16 \\[12pt]
{ceil_k} = \left\lceil \frac{k}{16} \right\rceil \times k_0 \\[16pt]
\text{cube利用率} =
\frac{ (m \times n \times k) / ({cube_m} \times {cube_n} \times {cube_k}) }
{ \Delta t + ({ceil_m} \times {ceil_n} \times {ceil_k}) / ({cube_m} \times {cube_n} \times {cube_k}) }
\end{gathered}
$$

关键变量及常量说明：

- $m, n, k$：mmad入参实际计算的大小。
- $ceil_m, ceil_n, ceil_k$：$m, n, k$ 根据分型大小向上对齐后的值。
- $cube_m, cube_n, cube_k$：硬件真实并行度（单位：elements/cycle）。
- $k_0$：L0 Buffer上最小分型K方向大小。
- $\Delta t$：头开销cycle数。

开启HF32模式后，L0A Buffer/L0B Buffer中的FP32数据将在参与Mmad计算之前被舍入为HF32格式，舍入模式由[asc_enable_hf32_trans](asc_enable_hf32_trans.md)接口配置，中间计算使用HF32格式，最终的运算结果仍以FP32格式输出，以保证后续处理的兼容性。

FP32与HF32格式的精度对比如下图所示：

<!-- npu="A3,910b" id10 -->
**图1** FP32与HF32格式精度示意图（[NPU架构版本2201](../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh_cn_topic_hf32_figure1"></a>

![FP32与HF32格式精度示意图（NPU架构版本2201）](../../../figures/mmad_hf32.png "FP32与HF32格式精度示意图（NPU架构版本2201）")
<!-- end id10 -->

<!-- npu="950" id11 -->
**图2** FP32与HF32格式精度示意图（[NPU架构版本3510](../../../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="zh_cn_topic_hf32_figure2"></a>

![FP32与HF32格式精度示意图（NPU架构版本3510）](../../../figures/mmad_hf32_950.png "FP32与HF32格式精度示意图（NPU架构版本3510）")
<!-- end id11 -->

## 函数原型

 ```cpp
__aicore__ inline void asc_enable_hf32()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
asc_enable_hf32();
```
