# sqrt

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
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/cpp_stdlib/math_functions/sqrt_res.md#id1 -->

## 功能说明

计算输入数据的平方根。

## 函数原型

```
template <typename T>
__aicore__ inline T sqrt(const T src)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| T | 输入数据src的数据类型。当前支持的数据类型为float、int64_t。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 含义 |
| --- | --- | --- |
| src | 输入 | 源操作数。 |

## 约束说明

输入必须为非负数。若输入为负数，则接口返回值无意义。

## 返回值说明

输入数据的平方根。

对于数据类型为int64\_t的数据，其计算结果将被截断为整数。

## 调用示例

```
int64_t src = 4;

int64_t result = AscendC::Std::sqrt(src);
// result: 2
```
