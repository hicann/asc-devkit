# Neg\(ISASI\)<a name="ZH-CN_TOPIC_0000002193851733"></a>

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_binary_scalar_intf.h"`。

Neg属于单目矢量类计算接口，负责将输入的tensor按元素取相反数。计算公式如下：

$dst_i = -src_i$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void Neg(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## 参数说明<a name="section176711403104"></a>

**表1** 模板参数说明

| 参数名 | 描述 |
| ---- | ---- |
| T | 操作数数据类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| ---- | ---- | ---- |
| dst | 输出 | 目的操作数。<br>类型为LocalTensor，支持的TPosition为VECIN、VECCALC、VECOUT。<br>地址对齐约束参考[地址对齐约束](../../../通用说明和约束.md)。 |
| src | 输入 | 源操作数。<br>类型为LocalTensor，支持的TPosition为VECIN、VECCALC、VECOUT。<br>地址对齐约束参考[地址对齐约束](../../../通用说明和约束.md)。 |
| count | 输入 | 参与计算的元素个数。关于该参数的具体说明请参考[连续计算](../SIMD计算说明/连续计算.md)。 |

## 数据类型

<!-- npu="950" id8 -->
针对Ascend 950PR/Ascend 950DT，支持的数据为int8_t、int16_t、half、int32_t、float、int64_t。
<!-- end id8 -->

## 返回值说明<a name="section14483414194"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md)。
- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md)。

## 调用示例<a name="section176061616102911"></a>

更多样例可参考[LINK](更多样例-9.md)。

```cpp
AscendC::Neg(dstLocal, srcLocal, 512);
```

结果示例如下（srcLocal和dstLocal均为int64\_t类型）：

```bash
输入数据(srcLocal): [1 2 3 4 ...]
输出数据(dstLocal): [-1 -2 -3 -4 ...]
```
