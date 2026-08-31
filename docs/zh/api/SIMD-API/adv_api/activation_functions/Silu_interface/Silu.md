# Silu

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Silu_interface/Silu_res.md#id1 -->

## 功能说明

按元素做Silu运算，计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002350049101.png)

![](../../../../figures/zh-cn_formulaimage_0000001729001889.png)

## 函数原型

```
template <typename T, bool isReuseSource = false>
__aicore__ inline void Silu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize)
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
| srcLocal | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| dataSize | 输入 | 实际计算数据元素个数。 |

## 返回值说明

无

## 约束说明

-   操作数地址偏移对齐要求请参见[通用说明和约束](../../../general_description_and_constraints.md)。
-   **不支持源操作数与目的操作数地址重叠。**
-   当前仅支持ND格式的输入，不支持其他格式。

<!-- npu="950" id9 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id9 -->

## 调用示例

```
// dstLocal: 存放Silu计算结果的Tensor
// srcLocal: 存放Silu计算输入的Tensor

// 需要参与计算的元素个数为512
AscendC::Silu(dstLocal, srcLocal, 512);
```

结果示例如下：

```
输入数据(srcLocal):
[-100.    -50.    -20.    -10.     -5.     -4.     -3.     -2.
   -1.     -0.5    -0.4    -0.3    -0.2    -0.1    -0.01    0.
    0.      0.01    0.1     0.2     0.3     0.4     0.5     1.
    2.      3.      4.      5.     10.     20.     50.    100.  ]
输出数据(dstLocal):
[ -0.          -0.          -0.00000004  -0.00045398  -0.03346425  -0.07194484  -0.14227761  -0.23840587
  -0.2689414   -0.18877034  -0.16052495  -0.12766725  -0.0900332   -0.04750208  -0.004975     0.
   0.           0.005025     0.05249792   0.10996681   0.17233276   0.23947507   0.31122968   0.7310586
   1.761594     2.8577225    3.928055     4.9665356    9.999546    20.          50.         100.        ]
```
