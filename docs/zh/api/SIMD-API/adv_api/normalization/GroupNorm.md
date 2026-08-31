# GroupNorm

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
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/GroupNorm_res.md#id1 -->

## 功能说明

对一个特征进行标准化的一般公式如下所示：

![](../../../figures/zh-cn_formulaimage_0000002080754290.png)

其中，i表示特征中的索引，![](../../../figures/zh-cn_formulaimage_0000002116393957.png)和![](../../../figures/zh-cn_formulaimage_0000002080755382.png)表示特征中每个值标准化前后的值，μ和σ表示特征的均值和标准差，计算公式如下所示：

![](../../../figures/zh-cn_formulaimage_0000002080756222.png)

![](../../../figures/zh-cn_formulaimage_0000002116310613.png)

其中，ε是一个很小的常数，S表示参与计算的数据的集合，m表示集合的大小。不同类型的特征标准化方法（BatchNorm、LayerNorm、InstanceNorm、GroupNorm等）的主要区别在于参与计算的数据集合的选取上。不同Norm类算子参与计算的数据集合的选取方式如下：

![](../../../figures/image-20241114155522430.png)

对于一个shape为\[N, C, H, W\]的输入，GroupNorm将每个\[C, H, W\]在C维度上分为groupNum组，然后对每一组进行标准化。最后对标准化后的特征进行缩放和平移。其中缩放参数γ和平移参数β是可训练的。

![](../../../figures/zh-cn_formulaimage_0000002081645994.png)

## 函数原型

-   接口框架申请临时空间

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void GroupNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon, GroupNormTiling& tiling)
    ```

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void GroupNorm(const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, GroupNormTiling& tiling)
    ```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数，默认值为false。如果开发者允许源操作数被改写，可以设置该参数取值为true开启，开启后能够节省部分内存空间。<br><br>设置为true，则本接口内部计算时复用inputX的内存空间，节省内存空间；设置为false，则本接口内部计算时不复用inputX的内存空间。<br><br>对于float数据类型的输入支持开启该参数，half数据类型的输入不支持开启该参数。<br><br>isReuseSource的使用样例请参考[更多样例](../math_compute/more_examples.md#section639165323915)。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| output | 输出 | 目的操作数，对标准化后的输入进行缩放和平移计算的结果。shape为[N, C, H, W]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| outputMean | 输出 | 目的操作数，均值。shape为[N, groupNum]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| outputVariance | 输出 | 目的操作数，方差。shape为[N, groupNum]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputX | 输入 | 源操作数。shape为[N, C, H, W]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| gamma | 输入 | 源操作数，缩放参数。该参数支持的取值范围为[-100, 100]。shape为[C]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| beta | 输入 | 源操作数，平移参数。该参数支持的取值范围为[-100, 100]。shape为[C]。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 接口内部复杂计算时用于存储中间变量，由开发者提供。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GroupNorm Tiling](GroupNorm-Tiling.md)。 |
| epsilon | 输入 | 防除0的权重系数。数据类型需要与inputX/output保持一致。 |
| tiling | 输入 | 输入数据的切分信息，Tiling信息的获取请参考[GroupNorm Tiling](GroupNorm-Tiling.md)。 |

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   当前仅支持ND格式的输入，不支持其他格式。

<!-- npu="950" id7 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id7 -->

## 调用示例

```
// output: 存放GroupNorm计算结果的Tensor
// outputMean: 输出每个group的均值
// outputVariance: 输出每个group的方差
// inputX: 输入数据X，shape为 [N, C, H, W]
// gamma: GroupNorm的缩放参数 γ，shape为 [C]
// beta: GroupNorm的偏置参数 β，shape为 [C]
// epsilon: 防除零系数ε
// tiling: 预计算的Tiling信息，包含分组数、维度等参数

// 使用GroupNorm接口实现Group Normalization
// 若数据类型T为float且允许修改inputX，可设置isReuseSource = true复用inputX内存空间以节省内存
AscendC::GroupNorm<T, isReuseSource>(
    output,         // 输出：归一化并缩放平移后的结果
    outputMean,     // 输出：每组的均值
    outputVariance, // 输出：每组的方差
    inputX,         // 输入：原始特征图
    gamma,          // 输入：缩放参数 γ
    beta,           // 输入：偏置参数 β
    epsilon,        // 输入：防止除零的系数 ε
    tiling          // 输入：Tiling调度信息
);
```

示例结果如下：

```
输入数据(inputXLocal, shape:[2, 8, 4, 2]):
[  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
  32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
  64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
  96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 ]
输入数据(gammaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
输入数据(betaLocal, shape:[8]):
[ 0 1 2 3 4 5 6 7 ]
输出数据(dstLocal):
[ 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  1.1084652 1.3253956 1.542326 1.7592564 1.9761869 2.1931171 2.4100475 2.6269782
  -1.2539563 -0.8200953 -0.38623452 0.047626257 0.48148715 0.91534793 1.3492088 1.7830696
  3.3253956 3.9761868 4.626978 5.277769 5.9285607 6.579352 7.230143 7.8809347
  -2.5079126 -1.6401906 -0.77246904 0.095252514 0.9629743 1.8306959 2.6984177 3.5661392
  5.542326 6.626978 7.71163 8.796282 9.880934 10.965586 12.050238 13.134891
  -3.7618694 -2.4602861 -1.1587038 0.14287853 1.4444613 2.7460437 4.0476265 5.349209
  7.7592564 9.277769 10.796282 12.314795 13.833308 15.351821 16.870335 18.388847
  0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
  1.1084652 1.3253956 1.542326 1.7592564 1.9761869 2.1931171 2.4100475 2.6269782
  -1.2539563 -0.8200953 -0.38623452 0.047626257 0.48148715 0.91534793 1.3492088 1.7830696
  3.3253956 3.9761868 4.626978 5.277769 5.9285607 6.579352 7.230143 7.8809347
  -2.5079126 -1.6401906 -0.77246904 0.095252514 0.9629743 1.8306959 2.6984177 3.5661392
  5.542326 6.626978 7.71163 8.796282 9.880934 10.965586 12.050238 13.134891
  -3.7618694 -2.4602861 -1.1587038 0.14287853 1.4444613 2.7460437 4.0476265 5.349209
  7.7592564 9.277769 10.796282 12.314795 13.833308 15.351821 16.870335 18.388847 ]
输出数据(meanLocal):
[ 7.5 23.5 39.5 55.5 71.5 87.5 103.5 119.5 ]
输出数据(varianceLocal):
[ 21.25 21.25 21.25 21.25 21.25 21.25 21.25 21.25 ]
```
