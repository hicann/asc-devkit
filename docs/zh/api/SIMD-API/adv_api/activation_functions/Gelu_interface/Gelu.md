# Gelu

## 产品支持情况

<!-- npu="950" id4 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id4 -->
<!-- npu="A3" id5 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id5 -->
<!-- npu="910b" id6 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id6 -->
<!-- npu="310b" id7 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas 训练系列产品：不支持
<!-- end id9 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Gelu_interface/Gelu_res.md#id1 -->

## 功能说明

在神经网络中，GELU是一个重要的激活函数，其灵感来源于Relu和Dropout，在激活中引入了随机正则的思想。计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002316009816.png)

![](../../../../figures/zh-cn_formulaimage_0000001750683329.png)，化简后可得![](../../../../figures/zh-cn_formulaimage_0000001702905034.png)

## 函数原型

-   接口框架申请临时空间

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void Gelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void Gelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| highPrecision | 是否开启高精度模式，以提升运算准确度。默认值为false，表示不开启高精度模式。<br><br>注意：高精度模式只在half数据类型下开启后生效，该参数的取值不影响float数据类型下的接口精度和性能。 |
| highPerformance | 是否开启高性能模式，以提升运算效率。默认值为false，表示不开启高性能模式。<br><br>注意：开启高性能模式相比于默认不开启高精度和高性能模式会有精度下降，同时开启高精度和高性能模式相比于仅开启高性能模式可能会有性能下降。<!-- npu="950" id3 -->针对Ascend 950PR/Ascend 950DT，该参数保留但不生效，取值为true或者false，接口的精度和性能没有区别。<!-- end id3 --> |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstLocal | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcLocal | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于接口内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md)。 |
| dataSize | 输入 | 实际计算数据元素个数。 |

## 返回值说明

无

## 约束说明

-   源操作数和目的操作数的Tensor空间可以复用。
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   仅支持输入shape为ND格式。

<!-- npu="950" id10 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id10 -->

## 调用示例

```
// dstLocal: 存放Gelu计算结果的Tensor
// srcLocal: 存放Gelu的输入Tensor
// sharedTmpBuffer: 存放Gelu计算过程中临时缓存的Tensor

// 接口框架申请临时空间，部分参与计算,需要参与计算的元素个数为32
AscendC::Gelu<srcType, false>(dstLocal, srcLocal, 32);

// 通过sharedTmpBuffer入参传入临时空间，部分参与计算,需要参与计算的元素个数为32
AscendC::Gelu<srcType, false>(dstLocal, srcLocal, sharedTmpBuffer, 32);
```

结果示例如下：

```
输入数据(srcLocal):
[-2.56   -2.395  -2.23   -2.066  -1.9    -1.735  -1.571  -1.406
 -1.241  -1.076  -0.9116 -0.7466 -0.582  -0.417  -0.2522 -0.0874
  0.0774  0.2423  0.407   0.572   0.737   0.902   1.066   1.231
  1.396   1.561   1.726   1.891   2.055   2.22    2.385   2.55  ]
输出数据(dstLocal):
[-0.01295471 -0.01953125 -0.02836609 -0.03991699 -0.05453491 -0.07196045 -0.09130859 -0.11254883
 -0.13342285 -0.15185547 -0.16516113 -0.17004395 -0.16320801 -0.14111328 -0.10101318 -0.04067993
  0.04107666  0.14428711  0.26782227  0.40942383  0.56689453  0.7363281   0.9135742   1.0966797
  1.2822266   1.4677734   1.6533203   1.8349609   2.0136719   2.1914062   2.3632812   2.5390625 ]
```
