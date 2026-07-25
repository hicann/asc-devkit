# SetFixpipePreQuantFlag<a name="ZH-CN_TOPIC_0000002538071284"></a>

## 产品支持情况<a name="zh-cn_topic_0000002516049318_section18204144912492"></a>

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

## 功能说明<a name="zh-cn_topic_0000002516049318_section618mcpsimp"></a>

DataCopy数据搬运（[L0C Buffer->GM](../矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)、[L0C Buffer->L1 Buffer](../矩阵计算的搬出/Fixpipe（L0C到L1数据搬运）.md)）过程中进行随路量化时，通过调用该接口设置scalar量化参数。

## 函数原型<a name="zh-cn_topic_0000002516049318_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipePreQuantFlag(uint64_t config)
```

## 参数说明<a name="zh-cn_topic_0000002516049318_section622mcpsimp"></a>

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| config | 输入 | 量化过程中使用到的scalar量化参数。 |

## 数据类型

参数的数据类型为uint64_t。

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="zh-cn_topic_0000002516049318_section633mcpsimp"></a>

无

## 调用示例<a name="section6461234123118"></a>

完整示例可参考[data_copy_l0c2gm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm)。

```cpp
float tmp = (float)0.5;
// 将float的tmp转换成uint64_t的deqScalar
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp)); 
AscendC::SetFixpipePreQuantFlag(deqScalar);  // 设置量化参数
AscendC::PipeBarrier<PIPE_FIX>();
```
