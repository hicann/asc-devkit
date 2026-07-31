# Truncate\(ISASI\)<a name="ZH-CN_TOPIC_0000002158484952"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

头文件路径为 `"basic_api/kernel_operator_vec_vconv_intf.h"`。

将源操作数的浮点数元素截断到整数位，同时源操作数的数据类型保持不变。

## 函数原型<a name="section620mcpsimp"></a>

```
template <typename T, RoundMode roundMode>
__aicore__ inline void Truncate(const LocalTensor<T> &dst, const LocalTensor<T> &src, const uint32_t count)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 操作数数据类型。 |
| roundMode | 舍入模式，取值如下。<br>**RoundMode::CAST_RINT**：返回最接近的整数，若存在两个同样接近的整数，则选择偶数。<br>**RoundMode::CAST_ROUND**：四舍五入取整。<br>**RoundMode::CAST_FLOOR**：向下取整。<br>**RoundMode::CAST_CEIL**：向上取整。<br>**RoundMode::CAST_TRUNC**：截断取整（直接舍弃小数部分）。 |


**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>源操作数的数据类型需要与目的操作数保持一致。 |
| count | 输入 | 参与计算的元素个数。 |

## 数据类型
<!-- npu="950" id8 -->
- 针对Ascend 950PR/Ascend 950DT，T支持的数据类型为：half、float、bfloat16_t。
<!-- end id8 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。
<!-- npu="950" id9 -->
-   针对Ascend 950PR/Ascend 950DT，该接口通过VF调用[Reg矢量计算](../../Reg矢量计算/Reg矢量计算.md)API实现兼容，当参数count取值为0时，不保证该接口被视为NOP（空操作）。
<!-- end id9 -->

## 调用示例<a name="section642mcpsimp"></a>

```
AscendC::Truncate<T, RoundMode::CAST_FLOOR>(dstLocal, srcLocal, 512);
```

结果示例如下：

```
输入数据srcLocal：[-37.063736    64.17277    -33.448692 ... ]
输出数据dstLocal：[-38  64 -34 ... ]
```
