# Mmad接口边界值汇总<a name="ZH-CN_TOPIC_0000002607849175"></a>

本节汇总介绍了Mmad计算接口在边界值输入下的输出结果，数据类型见基础API下的[mmad](../SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md)，以下内容针对如下型号生效：

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id3 -->

**表1**  Mmad特殊值/边界值输入的计算结果说明

<a name="table7822641411"></a>

| C矩阵的数据类型 | Mmad子阶段 | A矩阵的元素取值 | B矩阵的元素取值 | dst的元素取值<br>（INF/NAN模式） |
| --- | --- | --- | --- | --- |
| float | 乘法阶段 | +inf | norm(> 0.0)/+inf | +inf |
| float | 乘法阶段 | +inf | norm(< 0.0)/-inf | -inf |
| float | 乘法阶段 | -inf | norm(> 0.0)/+inf | -inf |
| float | 乘法阶段 | -inf | norm(< 0.0)/-inf | +inf |
| float | 乘法阶段 | +inf/-inf | 0.0 | nan |
| float | 乘法阶段 | norm(> 0.0) | +inf | +inf |
| float | 乘法阶段 | norm(> 0.0) | -inf | -inf |
| float | 乘法阶段 | norm(< 0.0) | +inf | -inf |
| float | 乘法阶段 | norm(< 0.0) | -inf | +inf |
| float | 乘法阶段 | 0.0 | +inf/-inf | nan |
| float | 乘法阶段 | nan | 任意输入 | nan |
| float | 乘法阶段 | 任意输入 | nan | nan |
| float | 乘法阶段 | 任意输入 | 任意输入 | 结果溢出情况下，正向溢出为inf，负向溢出为-inf |
| float | 加法阶段 | +inf | -inf | nan |
| float | 加法阶段 | +inf | +inf/norm | +inf |
| float | 加法阶段 | -inf | +inf | nan |
| float | 加法阶段 | -inf | -inf/norm | -inf |
| float | 加法阶段 | nan | 任意输入 | nan |
| float | 加法阶段 | 任意输入 | nan | nan |

注：

INF/NAN模式（非饱和模式）下，乘法阶段：inf\*非零值输出inf，inf\*0输出nan，输入包含nan时输出为nan。加法阶段：正负inf相加时输出为nan。
