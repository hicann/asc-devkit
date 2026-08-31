# Swish

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
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Swish_interface/Swish_res.md#id1 -->

## 功能说明

在神经网络中，Swish是一个重要的激活函数。计算公式如下，其中β为常数：

![](../../../../figures/zh-cn_formulaimage_0000002316174174.png)

![](../../../../figures/zh-cn_formulaimage_0000001680848998.png)

## 函数原型

```
template <typename T, bool isReuseSource = false>
__aicore__ inline void Swish(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize, const T scalarValue)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstLocal | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcLocal | 输入 | 源操作数。<br><br>源操作数的数据类型需要与目的操作数保持一致。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| dataSize | 输入 | 实际计算数据元素个数。 |
| scalarValue | 输入 | 激活函数中的β参数。支持的数据类型为：half、float。<br><br>β参数的数据类型需要与源操作数和目的操作数保持一致。 |

## 返回值说明

无

## 约束说明

-   操作数地址偏移对齐要求请参见[通用说明和约束](../../../general_description_and_constraints.md)。
-   **不支持源操作数与目的操作数地址重叠。**
-   当前仅支持ND格式的输入，不支持其他格式。

<!-- npu="950" id8 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id8 -->

## 调用示例

```
// dstLocal: 存放Swish计算结果的Tensor
// srcLocal：存放Swish计算的输入Tensor
// scalarValue: 激活函数中的β参数

// 参与计算的元素个数为32，β参数为-1.702
AscendC::Swish(dstLocal, srcLocal, 32, -1.702);
```

结果示例如下：

```
输入数据(srcLocal):
[-4.         -3.7419355  -3.483871   -3.2258065  -2.967742   -2.7096775  -2.451613   -2.1935484
 -1.9354838  -1.6774193  -1.4193548  -1.1612903  -0.9032258  -0.6451613  -0.38709676 -0.12903225
  0.12903225  0.38709676  0.6451613   0.9032258   1.1612903   1.4193548   1.6774193   1.9354838
  2.1935484   2.451613    2.7096775   2.967742    3.2258065   3.483871    3.7419355   4.        ]
输出数据(dstLocal):
[-0.00441472 -0.00640367 -0.00924141 -0.01325663 -0.01887952 -0.02665107 -0.03720944 -0.05122496
 -0.06923404 -0.09129371 -0.11635891 -0.14131825 -0.15980731 -0.16135658 -0.13200021 -0.05746017
  0.07157208  0.25509655  0.48380467  0.74341846  1.019972    1.3029958   1.5861256   1.8662498
  2.1423235   2.4144034   2.6830263   2.9488626   3.2125497   3.4746296   3.735532    3.995585  ]
```
