# Cos

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Cos_interface/Cos_res.md#id1 -->

## 功能说明

按元素做三角函数余弦运算，计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002218333665.png)

Cos\(x\)的泰勒展开式为：

![](../../../../figures/zh-cn_formulaimage_0000002376868752.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   接口框架申请临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetCosMaxMinTmpSize](GetCosMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数，默认值为false。该参数仅在输入的数据类型为float时生效。<br>true：开发者允许源操作数被改写，可以设置该参数取值为true开启，开启后本接口内部计算时复用srcTensor的内存空间，节省部分内存空间；false：本接口内部计算时不复用srcTensor的内存空间。<br><br>isReuseSource的使用样例请参考[更多样例](../more_examples.md#section639165323915)。 |
| config | <!-- npu="950" id13 -->该参数仅支持Ascend 950PR/Ascend 950DT。<br><br><!-- end id13 -->Cos算法的相关配置。此参数可选配，CosConfig类型，具体定义如下方代码所示，其中参数的含义为：<br>algo：Cos内部实现使用的算法。CosAlgo类型，支持的取值如下：POLYNOMIAL_APPROXIMATION：默认值。该算法通过简单的多项式逼近实现Cos接口，支持的输入值域范围为[-65504.0, 65504.0]，支持的数据类型为half、float。RADIAN_REDUCTION：该算法通过完整的周期约减实现Cos接口，支持输入全值域，支持的数据类型为half、float。 |

```
struct CosConfig {
    CosAlgo algo = CosAlgo::POLYNOMIAL_APPROXIMATION;
};
enum class CosAlgo {
    POLYNOMIAL_APPROXIMATION = 0,
    RADIAN_REDUCTION,
};
```

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GetCosMaxMinTmpSize](GetCosMaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

<!-- npu="950" id9 -->
-   对于Ascend 950PR/Ascend 950DT，模板参数config中使用多项式拟合算法POLYNOMIAL\_APPROXIMATION时，必须保证输入源数据的值域在\[-65504.0, 65504.0\]范围内。
<!-- end id9 -->
-   对于以下产品，输入源数据必须保持值域在\[-65504.0, 65504.0\]范围内。
        <!-- npu="A3" id10 -->
    -   Atlas A3 训练系列产品/Atlas A3 推理系列产品
        <!-- end id10 -->
        <!-- npu="910b" id11 -->
    -   Atlas A2 训练系列产品/Atlas A2 推理系列产品
        <!-- end id11 -->
        <!-- npu="310p" id12 -->
    -   Atlas 推理系列产品AI Core
        <!-- end id12 -->

-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。

## 调用示例

```
// dstLocal: 存放计算结果的Tensor
// srcLocal: 参与计算的输入Tensor
// sharedTmpBuffer: 临时缓存,内部复杂计算时存储中间变量
// 输入tensor长度为1024,算子输入的数据类型为half,实际计算个数为512
AscendC::Cos(dstLocal, srcLocal, sharedTmpBuffer, 512);
constexpr AscendC::CosAlgo algo = AscendC::CosAlgo::RADIAN_REDUCTION;
constexpr AscendC::CosConfig config = {algo};
AscendC::Cos<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

结果示例如下：

```
输入数据(srcLocal): [0.00            0.01            0.02             ...  5.10            5.11]
输出数据(dstLocal): [1.00000000e+00  9.99949992e-01  9.99800026e-01   ...  3.77977639e-01  3.87216508e-01]
```
