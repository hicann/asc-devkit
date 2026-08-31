# BatchNorm

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
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/BatchNorm_res.md#id1 -->

## 功能说明

BatchNorm是对于每一层的输入做规范化处理，使得每一层的分布尽可能的相同，从而加速训练过程和提高模型的泛化能力（有效减少梯度消失和梯度爆炸问题）。基本思想是对于每个batch中的样本，对其输入的每个特征在batch的维度上进行归一化。具体来说，对于输入特征x，BatchNorm的计算过程可以表示为：

1.  对输入特征x，在batch维度上计算均值μ和方差σ：

    ![](../../../figures/zh-cn_formulaimage_0000001819896105.png)

2.  对于每个特征i，对输入特征x进行归一化：

    ![](../../../figures/zh-cn_formulaimage_0000001819898557.png)

3.  对归一化后的特征进行缩放和平移：

    ![](../../../figures/zh-cn_formulaimage_0000001773144218.png)

## 函数原型

-   接口框架申请临时空间

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void BatchNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const T epsilon, BatchNormTiling& tiling)
    ```

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void BatchNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| isBasicBlock | inputX、output的shape信息和Tiling切分策略满足基本块要求的情况下，可以设置该参数取值为true，开启该参数，用于提升性能，默认不开启。基本块要求如下：<br>originB是8的倍数；<br>S*H是64的倍数，但小于2048。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| output | 输出 | 目的操作数，shape为[B，S，H]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| outputMean | 输出 | 均值，目的操作数，shape为[S，H]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| outputVariance | 输出 | 方差，目的操作数，shape为[S，H]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputX | 输入 | 源操作数，shape为[B，S，H]。inputX的数据类型需要与目的操作数保持一致，S*H需要32B对齐。支持inputX与output地址重叠。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| gamm | 输入 | 源操作数，shape为[B]。gamm的数据类型需要与目的操作数保持一致，长度需要32B对齐。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| beta | 输入 | 源操作数，shape为[B]。beta的数据类型需要与目的操作数保持一致，长度需要32B对齐。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 接口内部复杂计算时用于存储中间变量，由开发者提供。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[BatchNorm Tiling](BatchNorm-Tiling.md)。 |
| epsilon | 输入 | 防除0的权重系数。数据类型需要与inputX/output保持一致。 |
| tiling | 输入 | 输入数据的切分信息，Tiling信息的获取请参考[BatchNorm Tiling](BatchNorm-Tiling.md)。 |

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   当前仅支持ND格式的输入，不支持其他格式。
-   输入数据的S\*H必须满足32B对齐的要求。

<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id7 -->

## 调用示例

```
// outputLocal：存放BatchNorm计算结果的Tensor
// meanLocal：存放计算结果均值的Tensor
// varianceLocal：存放计算结果方差的Tensor
// inputXLocal：参与计算的输入Tensor
// gammaLocal：输入张量，归一化后数据的缩放系数γ
// betaLocal：输入张量，归一化后数据的平移系数β
// epsilon：防除0的权重系数ε
// batchNormTiling：Tiling数据，从Host侧获取

AscendC::BatchNorm<dataType, isReuseSource, isBasicBlock>(
    outputLocal, meanLocal, varianceLocal, inputXLocal, gammaLocal, betaLocal, (dataType)epsilon, batchNormTiling);
```

示例结果如下：

```
输入数据(inputXLocal, shape:[8, 4, 2]):
[  0  1  2  3  4  5  6  7
   8  9 10 11 12 13 14 15
  16 17 18 19 20 21 22 23
  24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39
  40 41 42 43 44 45 46 47
  48 49 50 51 52 53 54 55
  56 57 58 59 60 61 62 63 ]
输入数据(gammaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
输入数据(betaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
输出数据(dstLocal):
[ 0. 0. 0. 0. 0. 0. 0. 0.
  -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155 -0.091073155
  0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122 0.6907122
  2.345356 2.345356 2.345356 2.345356 2.345356 2.345356 2.345356 2.345356
  4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585 4.8728585
  8.27322 8.27322 8.27322 8.27322 8.27322 8.27322 8.27322 8.27322
  12.546439 12.546439 12.546439 12.546439 12.546439 12.546439 12.546439 12.546439
  17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 17.692516 ]
输出数据(meanLocal):
[ 28 29 30 31 32 33 34 35 ]
输出数据(varianceLocal):
[ 336 336 336 336 336 336 336 336 ]
```
