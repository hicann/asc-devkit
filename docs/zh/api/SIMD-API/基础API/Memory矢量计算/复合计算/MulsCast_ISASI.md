# MulsCast\(ISASI\)<a name="ZH-CN_TOPIC_0000002158325172"></a>

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

头文件路径为：`"basic_api/kernel_operator_vec_binary_scalar_intf.h"`。

将矢量源操作数前count个数据与标量相乘再按照CAST\_ROUND模式转换成half类型，并将计算结果写入dst，此接口支持标量在前和标量在后两种场景。

$$dst_i = Cast(src0_i \times scalar)$$

$$dst_i = Cast(scalar \times src1_i)$$

其中标量输入也支持配置LocalTensor单点元素，计算公式如下，idx表示LocalTensor单点元素的位置系数。

$$dst_i = Cast(src0_i \times src1_{idx})$$

$$dst_i = Cast(src0_{idx} \times src1_i)$$

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, const BinaryConfig &config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void MulsCast(const T2 &dst, const T3 &src0, const T4 &src1, const uint32_t count)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :----- | :--- |
| T0 | 目的操作数数据类型。<br>预留参数，暂未启用，为后续的功能扩展做保留，需要指定时，传入默认值BinaryDefaultType即可。 |
| T1 | 源操作数数据类型。<br>预留参数，暂未启用，为后续的功能扩展做保留，需要指定时，传入默认值BinaryDefaultType即可。 |
| config | 类型为BinaryConfig，当标量为LocalTensor单点元素类型时生效，用于指定单点元素操作数位置。默认值DEFAULT_BINARY_CONFIG，表示右操作数为标量。<br>struct BinaryConfig {<br>    int8_t scalarTensorIndex = 1; // 用于指定标量为LocalTensor单点元素时标量的位置，0表示左操作数，1表示右操作数<br>};<br>constexpr BinaryConfig DEFAULT_BINARY_CONFIG = {1}; |
| T2 | LocalTensor类型，根据输入参数dst自动推导相应的数据类型，开发者无需配置该参数，保证dst满足数据类型的约束即可。 |
| T3 | LocalTensor类型或标量类型，根据输入参数src0自动推导相应的数据类型，开发者无需配置该参数，保证src0满足数据类型的约束即可。 |
| T4 | LocalTensor类型或标量类型，根据输入参数src1自动推导相应的数据类型，开发者无需配置该参数，保证src1满足数据类型的约束即可。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| :----- | :-------- | :--- |
| dst | 输出 | 目的操作数。<br>类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>Ascend 950PR/Ascend 950DT，支持的数据类型为：half<br>不同数据类型对应的精度转换规则见[精度转换规则](概述.md#精度转换)。 |
| src0/src1 | 输入 | 源操作数。<br>&bull; 类型为LocalTensor时，支持当作矢量操作数或标量单点元素，支持的TPosition为VECIN/VECCALC/VECOUT。<br>LocalTensor的起始地址需要32字节对齐。<br>Ascend 950PR/Ascend 950DT，支持的数据类型为：float<br>&bull; 类型为标量时：<br>Ascend 950PR/Ascend 950DT，支持的数据类型为：float |
| count | 输入 | 参与计算的元素个数。 |

**表3**  精度转换规则

| src类型 | dst类型 | 类型转换模式介绍 |
| :----- | :------ | :------------- |
| float | half | 将源操作数按照CAST_ROUND模式取到half所能表示的数，以half格式（溢出默认按照饱和处理）存入dst中。 |

## 数据类型
<!-- npu="950" id8 -->
Ascend 950PR/Ascend 950DT，src支持的数据类型为：float。dst支持的数据类型为：half。
<!-- end id8 -->

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

-   左操作数及右操作数中，必须有一个为矢量；当前不支持左右操作数同时为标量。
-   本接口传入LocalTensor单点数据作为标量时，idx参数需要传入编译期已知的常量，传入变量时需要声明为constexpr。
<!-- npu="950" id9 -->
- 该接口通过VF调用[Reg矢量计算API](../../Reg矢量计算/Reg矢量计算.md)实现兼容，当参数count取值为0时，不保证该接口将被视为NOP（空操作）。
  <!-- end id9 -->

## 调用示例<a name="section642mcpsimp"></a>

```cpp
// 标量在后示例
AscendC::MulsCast(dstLocal, src0Local, src1Local[0], 512);

// 标量在前示例
static constexpr AscendC::BinaryConfig config = { 0 };
AscendC::MulsCast<BinaryDefaultType, BinaryDefaultType, config>(dstLocal, src0Local[0], src1Local, 512);
```

结果示例如下：

```plain
输入数据src0Local：[6 5 11 ... ]
输入数据src1Local：2
输出数据dstLocal：[12 10 22 ... ]
```
