# asc\_mark\_stamp

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
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
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

SIMD场景下，用户通过调用接口，用于在算子执行过程中标记特定位置，便于后期通过流水图分析代码执行路径与性能热点。

## 函数原型

```
template<pipe_t pipe = PIPE_S>
__aicore__ inline void asc_mark_stamp(uint16_t idx)

template<pipe_t pipe = PIPE_S, uint16_t idx>
__aicore__ inline void asc_mark_stamp()
```

## 参数说明

| 参数名 | 含义 |
| --- | --- |
| pipe | 指定打点所在的pipeline类型。 |
| idx | 用户设置的打点的唯一标识id。 |

## 返回值说明

无

## 约束说明

-   idx取值范围为\[0,4095\]。为方便从打点图中找到对应的代码，建议不要重复使用相同的idx。
-   AIV侧仅支持输入PIPE_S、PIPE_V、PIPE_MTE2和PIPE_MTE3流水类型；AIC侧仅支持输入PIPE_S、PIPE_MTE1、PIPE_MTE2、PIPE_FIX和PIPE_M流水类型。
-   如果在循环中增加了一个asc_mark_stamp指令，每次执行到指令时都会输出一个打点，且idx是相同的。
-   如果开发者在两个相邻的VF分别打标记，由于编译器可能会对VF A和VF B做融合，asc_mark_stamp1和asc_mark_stamp2则会被优化掉，不会输出打点。

## 调用示例

```
mte2_opt();
//在算子执行开始处打点
asc_mark_stamp<PIPE_V, 0>();
//执行核心计算
vector_opt();
//在算子执行结束处打点
asc_mark_stamp<PIPE_V, 1>();
mte3_opt();
```
