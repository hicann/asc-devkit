# Cube指令理论性能汇总<a id="ZH-CN_TOPIC_0000002607728945"></a>

<!-- npu="950,A3,910b" id1 -->
本节汇总介绍了主要的Cube指令的理论性能，以下内容针对如下型号生效：

<!-- npu="950" id2 -->
Ascend 950PR/Ascend 950DT
<!-- end id2 -->
<!-- npu="A3" id3 -->
Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id3 -->
<!-- npu="910b" id4 -->
Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id4 -->

## Mmad计算类指令理论性能汇总

以下为Mmad计算类指令理论性能计算公式，在不同shape大小的情况下，unitFlag的开关对性能几乎无影响。

$$
\begin{gathered}
{ceil_m} = \left\lceil \frac{m}{16} \right\rceil \times 16 \\[12pt]
{ceil_n} = \left\lceil \frac{n}{16} \right\rceil \times 16 \\[12pt]
{ceil_k} = \left\lceil \frac{k_0}{16} \right\rceil \times k_0 \\[16pt]
\text{cube利用率} =
\frac{ (m \times n \times k) / ({cube_m} \times {cube_n} \times {cube_k}) }
{ \Delta t + ({ceil_m} \times {ceil_n} \times {ceil_k}) / ({cube_m} \times {cube_n} \times {cube_k}) }
\end{gathered}
$$

关键变量及常量说明：

- $m, n, k$：mmad入参实际计算的大小。
- $ceil_m, ceil_n, ceil_k$：$m, n, k$ 根据分型大小向上对齐后的值。
- $cube_m, cube_n, cube_k$：硬件真实并行度。
- $k_0$：L0 Buffer上最小分型K方向大小。
- $\Delta t$：头开销cycle数。

<!-- npu="A3,910b" id5 -->
**表1** Mmad计算类指令理论性能计算公式中并行度和k0的取值（[NPU架构版本2201](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123815211"></a>  

| 接口 | 左矩阵A | 右矩阵B | cube<sub>m</sub> | cube<sub>n</sub> | cube<sub>k</sub> | k<sub>0</sub> |
| --- | --- | --- | --- | --- | --- | --- |
| [Mmad](../SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) | int4b_t | int4b_t | 16 | 16 | 64 | 64 |
| Mmad | int8_t | int8_t | 16 | 16 | 32 | 32 |
| Mmad | half | half | 16 | 16 | 16 | 16 |
| Mmad | bfloat16_t | bfloat16_t | 16 | 16 | 16 | 16 |
| Mmad | float | float | 16 | 16 | 4 | 8 |
| Mmad（开启[HF32](../SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算关键特性说明/HF32.md)） | float | float | 16 | 16 | 8 | 8 |
| [MmadWithSparse](../SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/MmadWithSparse.md) | int8_t | int8_t | 16 | 16 | 32 | 32 |
<!-- end id5 -->

<!-- npu="950" id6 -->
**表2** Mmad计算类指令理论性能计算公式中并行度和k0的取值（[NPU架构版本3510](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123815985"></a>  

| 接口 | 左矩阵A | 右矩阵B | cube<sub>m</sub> | cube<sub>n</sub> | cube<sub>k</sub> | k<sub>0</sub> |
| --- | --- | --- | --- | --- | --- | --- |
| [Mmad](../SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) | int8_t | int8_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e4m3fn_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e4m3fn_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e5m2_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| Mmad | fp8_e5m2_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
| Mmad | hifloat8_t | hifloat8_t | 16 | 16 | 32 | 32 |
| Mmad | half | half | 16 | 16 | 16 | 16 |
| Mmad | bfloat16_t | bfloat16_t | 16 | 16 | 16 | 16 |
| Mmad | float | float | 16 | 16 | 1 | 8 |
| Mmad（开启[HF32](../SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算关键特性说明/HF32.md)） | float | float | 16 | 16 | 8 | 8 |
| [MmadMx](../SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/MmadMx.md) | fp4x2_e1m2_t | fp4x2_e1m2_t | 16 | 16 | 64 | 64 |
| MmadMx | fp4x2_e2m1_t | fp4x2_e1m2_t | 16 | 16 | 64 | 64 |
| MmadMx | fp4x2_e1m2_t | fp4x2_e2m1_t | 16 | 16 | 64 | 64 |
| MmadMx | fp4x2_e2m1_t | fp4x2_e2m1_t | 16 | 16 | 64 | 64 |
| MmadMx | fp8_e4m3fn_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| MmadMx | fp8_e4m3fn_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
| MmadMx | fp8_e5m2_t | fp8_e4m3fn_t | 16 | 16 | 32 | 32 |
| MmadMx | fp8_e5m2_t | fp8_e5m2_t | 16 | 16 | 32 | 32 |
<!-- end id6 -->

## 矩阵计算搬入类指令理论性能汇总

以下内容为矩阵计算搬入类指令在L1 Buffer->L0A Buffer、L1 Buffer->L0B Buffer、L1 Buffer->L0A_MX Buffer、L1 Buffer->L0B_MX Buffer、L1 Buffer->BiasTable Buffer、L1 Buffer->Fixpipe Buffer多条数据搬运通路下的理论性能汇总说明。

数据搬运指令占用cycle数的理论计算公式：

$$
{cycles} = {data\_size} / {bandwidth} + \Delta T
$$

关键变量及常量说明：

- $data\_size$：搬运的数据总量，单位为Byte。
- $bandwidth$：搬运的带宽，每个cycle能够搬运的数据量大小，单位为Byte/cycle。
- $\Delta T$：搬运过程中的带宽延迟等额外开销，不同搬运指令的参考值不相同。

注意，下列表格中展示的理论性能为搬运带宽$bandwidth$，未考虑额外开销$\Delta T$。

<!-- npu="A3,910b" id7 -->
**表3** 矩阵计算搬入类指令理论性能说明（[NPU架构版本2201](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123827121"></a>  

| 接口 | 搬运带宽（单位：Byte/cycle） |
| --- | --- |
| [LoadData（2D矩阵搬运）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)（L1 Buffer->L0A Buffer） | 256 |
| LoadData（2D矩阵搬运）（L1 Buffer->L0B Buffer） | 128 |
| [LoadDataWithTranspose](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithTranspose.md)（L1 Buffer->L0A Buffer） | 256 |
| LoadDataWithTranspose（L1 Buffer->L0B Buffer） | 128 |
| [LoadDataWithSparse](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithSparse.md)（L1 Buffer->L0B Buffer） | 128 |
| [DataCopy（L1 Buffer->BiasTable Buffer）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToBiasTable-Buffer数据搬运）.md) | 32 |
| [DataCopy（L1 Buffer->Fixpipe Buffer）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToFixpipe-Buffer数据搬运）.md) | 32 |

[LoadData（卷积数据搬运）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md)指令的理论性能与参数相关，相关参数说明见[LoadData3DParamsV2结构体内参数说明](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)，理论性能说明见下表：

**表4** LoadData（卷积数据搬运）指令理论性能说明（[NPU架构版本2201](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123821113"></a>  

| strideW的取值 | 卷积核在H方向滑动的行数n | L1 Buffer->L0A Buffer带宽（单位：Byte/cycle） | L1 Buffer->L0B Buffer带宽（单位：Byte/cycle） |
| --- | --- | --- | --- |
| 1 | 1 | 256 | 128 |
| 1 | 大于1 | $\left[\frac{512}{n+2}\right]$（最小不会低于32） | $\left[\frac{512}{n+4}\right]$（最小不会低于$\left[\frac{512}{16+2}\right]$） |
| 2、4、8 | 1 | $\left[\frac{512}{(strideW \times 2)}\right]$（最小不会低于32） | $\left[\frac{512}{(strideW \times 2 + 2)}\right]$（最小不会低于$\left[\frac{512}{16+2}\right]$） |
| 2、4、8 | 大于1 | $\left[\frac{512}{(strideW \times 2 + n)}\right]$（最小不会低于32） | $\left[\frac{512}{(strideW \times 2 + 2 + n)}\right]$（最小不会低于$\left[\frac{512}{16+2}\right]$） |
| 大于1，且不等于2、4、8 | 任意值 | 32 | $\left[\frac{512}{16+2}\right]$ |
<!-- end id7 -->

<!-- npu="950" id8 -->
**表5** 矩阵计算搬入类指令理论性能说明（[NPU架构版本3510](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123827112"></a>  

| 接口 | 搬运带宽（单位：Byte/cycle） |
| --- | --- |
| [LoadData（2D矩阵搬运V2）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_V2.md)（L1 Buffer->L0A Buffer） | 256 |
| LoadData（2D矩阵搬运V2）（L1 Buffer->L0B Buffer） | 256 |
| [LoadData（MX矩阵搬运）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D_MX.md)（L1 Buffer->L0A Buffer） | 256 |
| LoadData（MX矩阵搬运）（L1 Buffer->L0B Buffer） | 256 |
| LoadData（MX矩阵搬运）（L1 Buffer->L0A_MX Buffer） | 32 |
| LoadData（MX矩阵搬运）（L1 Buffer->L0B_MX Buffer） | 32 |
| [LoadDataWithTranspose](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithTranspose.md)（L1 Buffer->L0A Buffer） | 256 |
| LoadDataWithTranspose（L1 Buffer->L0B Buffer） | 128 |
| [DataCopy（L1 Buffer->BiasTable Buffer）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToBiasTable-Buffer数据搬运）.md) | 32 |
| [DataCopy（L1 Buffer->Fixpipe Buffer）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/DataCopy（L1ToFixpipe-Buffer数据搬运）.md) | 32 |

针对Ascend 950PR/Ascend 950DT，LoadData（2D矩阵搬运）接口仅为兼容实现，内部使用了LoadData（2D矩阵搬运V2）接口实现，相关理论性能可参考LoadData（2D矩阵搬运V2）接口，但需要注意，该兼容实现会造成性能损失。

由于LoadData（MX矩阵搬运）接口内部实现涉及两条数据通路的搬运，其占用cycle数的理论计算公式有所不同，具体公式如下：

$$
{cycles} = {data\_size} / {bandwidth} + {scale\_data\_size} / {bandwidth\_mx} + \Delta T
$$

关键变量及常量说明：

- $data\_size$：搬运左/右矩阵的数据总量，单位为Byte。
- $scale\_data\_size$：搬运scale矩阵的数据总量，单位为Byte。
- $bandwidth$：搬运左/右矩阵的带宽，单位为Byte/cycle。
- $bandwidth\_mx$：搬运scale矩阵的带宽，单位为Byte/cycle。
- $\Delta T$：搬运过程中的带宽延迟等额外开销。

针对Ascend 950PR/Ascend 950DT，[LoadData（卷积数据搬运）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md)和[LoadDataWithStride](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadDataWithStride.md)指令的理论性能与参数相关，相关参数说明见[LoadData3DParamsV2结构体内参数说明](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)。此外，LoadDataWithStride接口相比于LoadData（卷积数据搬运）接口，内部不包含针对其他芯片版本的兼容性实现，减少了兼容造成的额外开销，性能表现有所优化。两个接口的理论性能说明见下表：

**表6** LoadData（卷积数据搬运）和LoadDataWithStride理论性能说明（[NPU架构版本3510](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877123821313"></a>  

| strideW的取值 | 卷积核在H方向滑动的行数n | L1 Buffer->L0A Buffer带宽（单位：Byte/cycle） | L1 Buffer->L0B Buffer带宽（单位：Byte/cycle） |
| --- | --- | --- | --- |
| 1 | 1 | 256 | 256 |
| 1 | 大于1 | $\left[\frac{512}{n+2}\right]$（最小不会低于32） | $\left[\frac{512}{n+2}\right]$（最小不会低于32） |
| 2、4、8 | 1 | $\left[\frac{512}{(strideW \times 2)}\right]$（最小不会低于32） | $\left[\frac{512}{(strideW \times 2)}\right]$（最小不会低于32） |
| 2、4、8 | 大于1 | $\left[\frac{512}{(strideW \times 2 + n)}\right]$（最小不会低于32） | $\left[\frac{512}{(strideW \times 2 + n)}\right]$（最小不会低于32） |
| 大于1，且不等于2、4、8 | 任意值 | 32 | 32 |
<!-- end id8 -->

## 矩阵计算搬出类指令理论性能汇总

以下内容为矩阵计算搬出类指令在L0C Buffer->L1 Buffer和L0C Buffer->Unified Buffer两条数据搬运通路下的理论性能汇总说明。

数据搬运指令占用cycle数的理论计算公式：

$$
{cycles} = {data\_size} / {bandwidth} + \Delta T
$$

关键变量及常量说明：

- $data\_size$：搬运的数据总量，单位为Byte。
- $bandwidth$：搬运的带宽，每个cycle能够搬运的数据量大小，单位为Byte/cycle。
- $\Delta T$：搬运过程中的带宽延迟等额外开销，不同搬运指令的参考值不相同。

注意，下列表格中展示的理论性能为搬运带宽$bandwidth$，未考虑额外开销$\Delta T$。

矩阵计算搬出类指令及其随路功能的详细介绍可参考[DataCopy（L0C到GM数据搬运）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/DataCopy（L0C到GM数据搬运）.md)、[Fixpipe（L0C到GM数据搬运）](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)以及相同目录下的其他通路对应资料。以下为理论性能说明列表：

<!-- npu="A3,910b" id9 -->
**表7** 矩阵计算搬出类指令理论性能说明（[NPU架构版本2201](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877112315211"></a>  

| 数据通路 | L0C Buffer上的数据类型 | 量化模式 | 格式转换模式 | 目的内存上的数据类型 | 并行度（单位：elements/cycle） | 搬运带宽（单位：Byte/cycle） |
| --- | --- | --- | --- | --- | --- | --- |
| L0C Buffer->L1 Buffer | float | F322F16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16 | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
<!-- end id9 -->

<!-- npu="950" id10 -->
**表8** 矩阵计算搬出类指令理论性能说明（[NPU架构版本3510](../../guide/编程指南/语言扩展层/SIMD-BuiltIn关键字.md)）<a id="table1877112315211"></a>  

| 数据通路 | L0C Buffer上的数据类型 | 量化模式 | 格式转换模式 | 目的内存上的数据类型 | 并行度（单位：elements/cycle） | 搬运带宽（单位：Byte/cycle） |
| --- | --- | --- | --- | --- | --- | --- |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2NZ | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2NZ + Channel Split | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2ND | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride = 1 | float | 32 | 128 |
| L0C Buffer->L1 Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride > 1 | float | 8 | 32 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2ND | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2NZ + Channel Merge | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2ND | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride = 1 | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride > 1 | fp8_e4m3fn_t | 16 | 16 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2NZ + Channel Merge | hifloat8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2ND | hifloat8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride = 1 | hifloat8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride > 1 | hifloat8_t | 16 | 16 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2ND | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->L1 Buffer | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->L1 Buffer | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |
| L0C Buffer->Unified Buffer（开启[双目标模式](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵搬出关键特性说明/L0C-Buffer到UB双目标模式.md)） | float | NoQuant | NZ2NZ | float | 32 + 32 | 128 + 128 |
| L0C Buffer->Unified Buffer（开启双目标模式） | float | NoQuant | NZ2ND | float | 32 + 32 | 128 + 128 |
| L0C Buffer->Unified Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2NZ | float | 32 | 128 |
| L0C Buffer->Unified Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2NZ + Channel Split | float | 32 | 128 |
| L0C Buffer->Unified Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2ND | float | 32 | 128 |
| L0C Buffer->Unified Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride = 1 | float | 32 | 128 |
| L0C Buffer->Unified Buffer | float | NoQuant/(V)QF322F32_PRE | NZ2DN + srcNzC0Stride > 1 | float | 8 | 32 |
| L0C Buffer->Unified Buffer | float | F322F16/(V)QF322F16_PRE | NZ2NZ | half | 64 | 128 |
| L0C Buffer->Unified Buffer | float | F322F16/(V)QF322F16_PRE | NZ2ND | half | 64 | 128 |
| L0C Buffer->Unified Buffer | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->Unified Buffer | float | F322F16/(V)QF322F16_PRE | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->Unified Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->Unified Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->Unified Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->Unified Buffer | float | F322BF16/(V)QF322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->Unified Buffer | float | (V)QF322FP8_PRE | NZ2NZ + Channel Merge | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322FP8_PRE | NZ2ND | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride = 1 | fp8_e4m3fn_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322FP8_PRE | NZ2DN + srcNzC0Stride > 1 | fp8_e4m3fn_t | 16 | 16 |
| L0C Buffer->Unified Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2NZ + Channel Merge | hifloat8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2ND | hifloat8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride = 1 | hifloat8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322HIF8_PRE/(V)QF322HIF8_PRE_HYBRID | NZ2DN + srcNzC0Stride > 1 | hifloat8_t | 16 | 16 |
| L0C Buffer->Unified Buffer | float | (V)QF322B8_PRE | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322B8_PRE | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | float | (V)QF322B8_PRE | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |
| L0C Buffer->Unified Buffer | int32_t | (V)DEQF16 | NZ2NZ | half | 64 | 128 |
| L0C Buffer->Unified Buffer | int32_t | (V)DEQF16 | NZ2ND | half | 64 | 128 |
| L0C Buffer->Unified Buffer | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride = 1 | half | 64 | 128 |
| L0C Buffer->Unified Buffer | int32_t | (V)DEQF16 | NZ2DN + srcNzC0Stride > 1 | half | 16 | 32 |
| L0C Buffer->Unified Buffer | int32_t | (V)QS322BF16_PRE | NZ2NZ | bfloat16_t | 64 | 128 |
| L0C Buffer->Unified Buffer | int32_t | (V)QS322BF16_PRE | NZ2ND | bfloat16_t | 64 | 128 |
| L0C Buffer->Unified Buffer | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride = 1 | bfloat16_t | 64 | 128 |
| L0C Buffer->Unified Buffer | int32_t | (V)QS322BF16_PRE | NZ2DN + srcNzC0Stride > 1 | bfloat16_t | 16 | 32 |
| L0C Buffer->Unified Buffer | int32_t | (V)REQ8 | NZ2NZ + Channel Merge | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | int32_t | (V)REQ8 | NZ2ND | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride = 1 | int8_t/uint8_t | 64 | 64 |
| L0C Buffer->Unified Buffer | int32_t | (V)REQ8 | NZ2DN + srcNzC0Stride > 1 | int8_t/uint8_t | 16 | 16 |

注：srcNzC0Stride为启用NZ2DN功能需配置的参数之一，参数详细介绍请参考[NZ2DN关键特性说明](../SIMD-API/基础API/矩阵计算（ISASI）/矩阵搬出关键特性说明/NZ2DN.md)。
<!-- end id10 -->
<!-- end id1 -->
