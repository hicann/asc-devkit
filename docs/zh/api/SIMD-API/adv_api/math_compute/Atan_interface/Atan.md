# Atan

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Atan_interface/Atan_res.md#id1 -->

## 功能说明

按元素做三角函数反正切运算，计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002183047940.png)

![](../../../../figures/zh-cn_formulaimage_0000002227268389.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   接口框架申请临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| config | <!-- npu="950" id9 -->该参数仅支持Ascend 950PR/Ascend 950DT。<br><br><!-- end id9 -->Atan算法的相关配置。此参数可选配，AtanConfig类型，具体定义如下方代码所示，其中参数的含义为：<br>algo：Atan内部实现使用的算法。AtanAlgo类型，支持的取值如下：TAYLOR_EXPANSION：默认值。该算法使用6阶泰勒展开实现Atan，支持的数据类型为half、float。POLYNOMIAL_APPROXIMATION：该算法是17次多项式逼近算法，支持的数据类型为float。 |

```
enum class AtanAlgo {
    TAYLOR_EXPANSION = 0,
    POLYNOMIAL_APPROXIMATION,
};

struct AtanConfig {
    AtanAlgo algo = AtanAlgo::TAYLOR_EXPANSION;
};
```

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>临时空间大小BufferSize的获取方式请参考[GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。

<!-- npu="950" id10 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id10 -->

## 调用示例

```
// dstLocal: 存放Atan计算结果的Tensor
// srcLocal: 存放Atan计算输入的Tensor
// sharedTmpBuffer: 存放Atan计算过程中临时缓存的Tensor

// 接口框架申请临时空间，全部参与计算
AscendC::Atan(dstLocal, srcLocal);
// 接口框架申请临时空间，部分参与计算,需要参与计算的元素个数为512
AscendC::Atan(dstLocal, srcLocal, 512);

// 通过sharedTmpBuffer入参传入临时空间，全部参与计算
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer);
// 通过sharedTmpBuffer入参传入临时空间，部分参与计算,需要参与计算的元素个数为512
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer, 512);

// 指定输入算法为POLYNOMIAL_APPROXIMATION,输入的数据类型为float,实际计算个数为512
static constexpr AscendC::AtanConfig atanConfig = {AscendC::AtanAlgo::POLYNOMIAL_APPROXIMATION};
AscendC::Atan<float, false, atanConfig>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

示例数据如下：

```
输入数据(srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
输出数据(dstLocal):
[-1.19847027, -1.19717361, -1.19560622 ... 0. ... 1.19429046, 1.19560622, 1.19717361]
```
