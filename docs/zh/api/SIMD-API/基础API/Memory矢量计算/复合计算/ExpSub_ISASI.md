# ExpSub\(ISASI\)<a name="ZH-CN_TOPIC_0000002158484948"></a>

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
- Atlas 推理系列产品 AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品 Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_binary_intf.h"`。

src0与src1相减，将差值作为指数计算自然常数e的幂次，并将计算结果写入dst。

源操作数数据类型为float类型时，计算公式如下：

$dst_i = e^{(src0_i - src1_i)}$

源操作数数据类型为half类型时，计算公式如下：

$dst_i = e^{(cast\_f16\_to\_f32(src0_i) - cast\_f16\_to\_f32(src1_i))}$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, typename U>
__aicore__ inline void ExpSub(const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<U>& src1, const uint32_t count)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T | 目的操作数数据类型。|
| U | 源操作数数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| src0、src1 | 输入 | 源操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。 |
| count | 输入 | 参与计算的元素个数。 |

## 数据类型
<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，T和U支持的数据类型为：half、float。
<!-- end id8 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../通用说明和约束.md#section796754519912)。

- 操作数地址重叠约束请参考[通用地址重叠约束](../../../通用说明和约束.md#section668772811100)。

<!-- npu="950" id9 -->
- 该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id9 -->

## 调用示例<a name="section642mcpsimp"></a>

```cpp
AscendC::ExpSub(dstLocal, src0Local, src1Local, 512);
```

结果示例如下：

```plain
输入数据src0Local：[1 2 4 ... 510]
输入数据src1Local：[1 1 2 ... 510]
输出数据dstLocal：[1 2.71828 7.38905 ... 1]
```
