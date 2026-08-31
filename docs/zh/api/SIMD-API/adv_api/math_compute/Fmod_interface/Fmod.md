# Fmod

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Fmod_interface/Fmod_res.md#id1 -->

## 功能说明

按元素计算两个浮点数a, b相除后的余数。计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002348879736.png)

![](../../../../figures/zh-cn_formulaimage_0000002188778220.png)

其中，Trunc为向零取整操作。举例如下：

Fmod\(2.0, 1.5\) = 0.5

Fmod\(-3.0, 1.1\) = -0.8

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   接口框架申请临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false, const FmodConfig& config = DEFAULT_FMOD_CONFIG>
        __aicore__ inline void Fmod(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
        ```

由于该接口的内部实现中涉及精度转换。需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[GetFmodMaxMinTmpSize](GetFmodMaxMinTmpSize.md)中提供的接口获取需要预留空间的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| config | <!-- npu="950" id9 -->该参数仅支持Ascend 950PR/Ascend 950DT。<br><br><!-- end id9 -->Fmod计算的相关配置。此参数可选配，FmodConfig类型，具体定义如下方代码所示，其中参数的含义为：<br>algo：指定Fmod的算法。该参数支持的取值如下：NORMAL：algo的默认值，使用模拟的普通模式，支持的数据类型为：half、float。ITERATION_COMPENSATION：迭代补偿的高精度模式，支持的数据类型为：float。<br><br>iterationNum：迭代补偿的高精度模式下的迭代补偿轮次，该参数仅在algo为ITERATION_COMPENSATION模式下生效，轮次范围1至11，默认值为11次。迭代轮次越多，结果精度越高，但性能会相应降低。使用时，可根据两个浮点数的指数位差异来选择迭代轮次，float类型共有8位指数位，src0Tensor和src1Tensor之间的指数位差异不应超过24*iterationNum。 |

```
constexpr uint32_t FMOD_ITERATION_NUM_MAX = 11;
enum class FmodAlgo {
    NORMAL = 0,
    ITERATION_COMPENSATION = 1,
};
struct FmodConfig {
    FmodAlgo algo = FmodAlgo::NORMAL;
    uint32_t iterationNum = FMOD_ITERATION_NUM_MAX;
};
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src0Tensor、src1Tensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时空间。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于Fmod内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetFmodMaxMinTmpSize](GetFmodMaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

<!-- npu="310p" id8 -->
-   针对Atlas 推理系列产品AI Core，输入数据限制在\[-2147483647.0, 2147483647.0\]范围内。
<!-- end id8 -->
-   源操作数src0Tensor与src1Tensor的数据长度必须保持一致。
-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
<!-- npu="950" id10 -->
-   对于Ascend 950PR/Ascend 950DT，模板参数config中的algo为ITERATION\_COMPENSATION迭代补偿模式下，操作数的数据类型仅支持float。
<!-- end id10 -->
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。

<!-- npu="950" id11 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id11 -->

## 调用示例

完整的调用样例请参考[fmod算子样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/fmod)。

```
// dstLocal: 存放Fmod计算结果的输出Tensor
// src0Local: 存放Fmod计算除数的输入Tensor
// src1Local: 存放Fmod计算被除数的输入Tensor
// sharedTmpBuffer: 存放Fmod计算过程中临时缓存的Tensor

// 接口框架申请临时空间，全部参与计算
AscendC::Fmod(dstLocal, src0Local, src1Local);
// 接口框架申请临时空间，部分参与计算,需要参与计算的元素个数为512
AscendC::Fmod(dstLocal, src0Local, src1Local, 512);

// 通过sharedTmpBuffer入参传入临时空间，全部参与计算
AscendC::Fmod(dstLocal, src0Local, src1Local, sharedTmpBuffer);
// 通过sharedTmpBuffer入参传入临时空间，部分参与计算,需要参与计算的元素个数为512
AscendC::Fmod(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

```
__aicore__ constexpr AscendC::FmodConfig GetConfig()
{
    return {.algo = AscendC::FmodAlgo::ITERATION_COMPENSATION, .iterationNum = 11};
}
static constexpr AscendC::FmodConfig config = GetConfig();
AscendC::Fmod<float, false, config>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

结果示例如下：

```
输入数据(src0Local): [-2.56 -2.55 -2.54 ... -0.01 0. 0.01 ... 2.53  2.54  2.55]
输入数据(src1Local): [2.    2.    2.    ... 2.    2. 2.   ... 2.    2.    2.]
输出数据(dstLocal):  [-0.56 -0.55 -0.54 ... -0.01 0. 0.01 ... 0.53  0.54  0.55]
```
