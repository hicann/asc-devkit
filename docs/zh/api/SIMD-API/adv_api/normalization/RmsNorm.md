# RmsNorm

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/RmsNorm_res.md#id1 -->

## 功能说明

实现对shape大小为\[B，S，H\]的输入数据的RmsNorm归一化，其计算公式如下：

![](../../../figures/zh-cn_formulaimage_0000001692451498.png)

其中，γ为缩放系数，ε为防除零的权重系数。

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool isBasicBlock = false>
    __aicore__ inline void RmsNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, const RmsNormTiling& tiling)
    ```

-   接口框架申请临时空间

    ```
    template <typename T, bool isBasicBlock = false>
    __aicore__ inline void RmsNorm(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal, const T epsilon, const RmsNormTiling& tiling)
    ```

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[RmsNorm Tiling](RmsNorm-Tiling.md)中提供的GetRmsNormMaxMinTmpSize接口获取所需最大和最小临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isBasicBlock | srcTensor和dstTensor的shape信息和Tiling切分策略满足基本块要求的情况下，可以设置该参数取值为true，开启该参数，用于提升性能，默认不开启。基本块要求srcTensor和dstTensor的shape需要满足如下条件：<br>last轴即H的长度为64的倍数，但小于2048；非last轴长度（B*S）为8的倍数。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstLocal | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>dstLocal的shape和源操作数srcLocal需要保持一致。 |
| srcLocal | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>shape为[B, S, H]，尾轴H长度需要满足32字节对齐。 |
| gammaLocal | 输入 | 缩放系数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>shape需要与srcLocal和dstLocal的尾轴H长度相等，即shape为[H]。 |
| sharedTmpBuffer | 输入 | 临时空间。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>接口内部复杂计算时用于存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[RmsNorm Tiling](RmsNorm-Tiling.md)。 |
| epsilon | 输入 | 防除零的权重系数，数据类型需要与srcLocal/dstLocal保持一致。 |
| tiling | 输入 | RmsNorm计算所需Tiling信息，Tiling信息的获取请参考[RmsNorm Tiling](RmsNorm-Tiling.md)。 |

## 返回值说明

无

## 约束说明

-   dstLocal和gammaLocal的Tensor空间不允许复用。
-   当前仅支持ND格式的输入，不支持其他格式。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   当srcLocal的原始shape中H轴非32字节对齐时，开发者需要对原始输入在H轴方向补齐数据到32字节对齐，API的计算结果会覆盖dstLocal中对应srcLocal补齐位置的数据。

<!-- npu="950" id9 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id9 -->

## 调用示例

```
// dstLocal：存放RmsNorm计算结果的Tensor
// srcLocal：参与计算的输入Tensor
// gammaLocal：输入张量，归一化后数据的缩放系数γ
// epsilon：防除0的权重系数ε
// tiling：Tiling数据，从Host侧获取

// // 若尾轴的长度（H）不超过2040且为64的倍数，同时非尾轴长度（B*S）为8的倍数，可设置isBasicBlock = true提升性能
AscendC::RmsNorm<dataType, isBasicBlock>(dstLocal, srcLocal, gammaLocal, epsilon, tiling);
```

示例结果如下：

```
输入数据(srcLocal, shape:[1, 1, 16]):
[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ]
输入数据(gammaLocal, shape:[16]):
[ 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 ]
输出数据(dstLocal):
[ 0.          0.11359233  0.4543693   1.022331    1.8174772   2.8398082   4.089324    5.566024
  7.269909    9.200979   11.359233   13.744672   16.357296   19.197104   22.264095   25.558275 ]
```
