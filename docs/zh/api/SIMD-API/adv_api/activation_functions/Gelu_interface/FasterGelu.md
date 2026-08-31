# FasterGelu

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Gelu_interface/FasterGelu_res.md#id1 -->

## 功能说明

在神经网络中，GELU是一个重要的激活函数，其灵感来源于relu和dropout，在激活中引入了随机正则的思想。为了降低GELU的算力需求，业界提出了FastGelu等版本。本接口FasterGelu是针对FastGelu的化简版本，公式化简可以大幅度提升计算性能。计算公式如下:

![](../../../../figures/zh-cn_formulaimage_0000002353089833.png)

![](../../../../figures/zh-cn_formulaimage_0000001697647873.png)，化简后可得![](../../../../figures/zh-cn_formulaimage_0000001649291696.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t dataSize)
    ```

-   接口框架申请临时空间

    ```
    template <typename T, bool highPrecision = false, bool highPerformance = false>
    __aicore__ inline void FasterGelu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dataSize)
    ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetGeluMaxMinTmpSize](GetGeluMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

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
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   当前仅支持ND格式的输入，不支持其他格式。
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。

<!-- npu="950" id10 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id10 -->

## 调用示例

```
// dstLocal：输出Tensor
// srcLocal：输入Tensor
// dataSize：实际参与计算的元素个数

// 默认不开启高精度和高性能模式
AscendC::FasterGelu(dstLocal, srcLocal, dataSize);
// 开启高精度模式，高精度模式只在srcType为half时生效
AscendC::FasterGelu<srcType, true, false>(dstLocal, srcLocal, dataSize);
// 开启高性能模式
AscendC::FasterGelu<srcType, false, true>(dstLocal, srcLocal, dataSize);
```

结果示例如下：

```
输入数据(srcLocal): [-1.83887 -3.60742 3.12891 -0.620605 2.0625 -2.77344 -0.04422 -3.54297 -3.16211 2.67383 1.3291 -1.57617 -0.0123901 3.77539 -1.61621 -0.616699]
输出数据(dstLocal): [-0.0769653 -0.00775528 3.11328 -0.160034 2.00195 -0.0244446 -0.021286 -0.00849152 -0.0144653 2.64453 1.20312 -0.100769 -0.00613022 3.76758 -0.0969238 -0.159912]
```
