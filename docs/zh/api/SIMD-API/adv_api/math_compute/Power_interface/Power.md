# Power

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Power_interface/Power_res.md#id1 -->

## 功能说明

实现按元素做幂运算功能，提供3类接口，处理逻辑如下：

![](../../../../figures/zh-cn_formulaimage_0000002218328733.png)

![](../../../../figures/zh-cn_formulaimage_0000002192030526.png)

## 函数原型

-   Power\(dstTensor, src0Tensor, src1Tensor\)
    -   通过sharedTmpBuffer入参传入临时空间
        -   源操作数Tensor全部/部分参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   源操作数Tensor全部参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   接口框架申请临时空间
        -   源操作数Tensor全部/部分参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor, uint32_t calCount)
            ```

        -   源操作数Tensor全部参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor)
            ```

-   Power\(dstTensor, src0Tensor, src1Scalar\)
    -   通过sharedTmpBuffer入参传入临时空间
        -   源操作数Tensor全部/部分参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   源操作数Tensor全部参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   接口框架申请临时空间
        -   源操作数Tensor全部/部分参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar, uint32_t calCount)
            ```

        -   源操作数Tensor全部参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const T& src1Scalar)
            ```

-   Power\(dstTensor, src0Scalar, src1Tensor\)
    -   通过sharedTmpBuffer入参传入临时空间
        -   源操作数Tensor全部/部分参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
            ```

        -   源操作数Tensor全部参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
            ```

    -   接口框架申请临时空间
        -   源操作数Tensor全部/部分参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor, uint32_t calCount)
            ```

        -   源操作数Tensor全部参与计算

            ```
            template <typename T, bool isReuseSource = false, const PowerConfig& config = defaultPowerConfig>
            __aicore__ inline void Power(const LocalTensor<T>& dstTensor, const T& src0Scalar, const LocalTensor<T>& src1Tensor)
            ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为sharedTmpBuffer申请空间。临时空间大小BufferSize的获取方式如下：通过[GetPowerMaxMinTmpSize](GetPowerMaxMinTmpSize.md)中提供的GetPowerMaxMinTmpSize接口获取需要预留空间的范围大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li559613463410)。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| config | <!-- npu="950" id7 -->该参数仅支持Ascend 950PR/Ascend 950DT。<br><br><!-- end id7 -->Power计算的相关配置。此参数可选配，PowerConfig类型，具体定义如下方代码所示，其中参数的含义为：<br>algo：不同的数据类型支持的不同Power算法。该参数支持的取值如下：INTRINSIC：默认值。如果数据类型是整型，INTRINSIC算法使用快速幂算法实现Power计算，支持的数据类型为uint8_t、int8_t、uint16_t、int16_t、uint32_t、int32_t。如果数据类型是浮点数类型，INTRINSIC算法按照公式Power(x, y) = exp(y * ln(x))进行Power计算，支持的数据类型为half、float。DOUBLE_FLOAT_TECH：DOUBLE_FLOAT_TECH算法是高精度浮点数算法，将源操作数的精度提升后，按照公式Power(x, y) = exp(y * ln(x))进行Power计算，减少计算过程中的精度损失，支持的数据类型为bfloat16_t、half、float。 |

```
enum class PowerAlgo {
    INTRINSIC = 0,
    DOUBLE_FLOAT_TECH,
};

struct PowerConfig {
    PowerAlgo algo = PowerAlgo::INTRINSIC;
};
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src0Tensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| src1Tensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| src0Scalar/src1Scalar | 输入 | 源操作数，类型为Scalar。源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时内存空间。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>针对3个power接口，不同输入数据类型情况下，临时空间大小BufferSize的获取方式请参考[GetPowerMaxMinTmpSize](GetPowerMaxMinTmpSize.md)。 |
| calCount | 输入 | 参与计算的元素个数。 |

## 返回值说明

无

## 约束说明

- **不支持源操作数与目的操作数地址重叠。**
<!-- npu="310p" id8 -->
-   对于Atlas 推理系列产品AI Core，幂运算的指数必须小于2<sup>31</sup>-1。
<!-- end id8 -->
-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   支持的数据类型<a id="li559613463410"></a>

    <!-- npu="950" id9 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：uint8\_t、int8\_t、uint16\_t、int16\_t、uint32\_t、int32\_t、half、bfloat16\_t、float。
    <!-- end id9 -->
    <!-- npu="A3" id10 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float、int32\_t。
    <!-- end id10 -->
    <!-- npu="910b" id11 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float、int32\_t。
    <!-- end id11 -->
    <!-- npu="310p" id12 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：half、float、int32\_t。
    <!-- end id12 -->

<!-- npu="950" id13 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id13 -->

## 调用示例

完整的调用样例请参考[Power样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/power)。

```
// dstLocal: 存放计算结果的Tensor
// srcLocalExp: Power计算使用的指数Tensor
// srcLocalBase: Power计算使用的底数Tensor

// 使用srcLocalBase做底数对srcLocalExp中的全部元素做幂运算
AscendC::Power<T, false>(dstLocal, srcLocalBase, srcLocalExp);

// scalarValueBase: Power计算使用的底数
T scalarValueBase = srcLocalBase.GetValue(0);
// 使用同一个底数scalarValueBase对srcLocalExp中的全部元素做幂运算
AscendC::Power<T, false>(dstLocal, scalarValueBase, srcLocalExp);

// scalarValueExp: Power计算使用的指数
T scalarValueExp = srcLocalExp.GetValue(0);
// 使用同一个指数scalarValueExp对srcLocalBase中的全部元素做幂运算
AscendC::Power<T, false>(dstLocal, srcLocalBase, scalarValueExp);

// static constexpr AscendC::PowerConfig config = { AscendC::PowerAlgo::DOUBLE_FLOAT_TECH };
// AscendC::Power<srcType, false, config>(dstLocal, scalarValue, srcLocal2);
```

AscendC::Power<T, false\>\(dstLocal, srcLocalBase, srcLocalExp\)示例数据如下：

```
输入数据(srcLocalBase): [2 3 4 5 6 7 8 9]
输入数据(srcLocalExp): [4 3 2 1 4 3 2 1]
输出数据(dstLocal): [16 27 16 5 1296 343 64 9]
```

AscendC::Power<T, false\>\(dstLocal, scalarValueBase, srcLocalExp\)示例数据如下：

```
输入数据(scalarValueBase): 2
输入数据(srcLocalExp): [4 3 2 1 4 3 2 1]
输出数据(dstLocal): [16 8 4 2 16 8 4 2]
```

AscendC::Power<T, false\>\(dstLocal, srcLocalBase, scalarValueExp\)示例数据如下：

```
输入数据(srcLocalBase): [2 3 4 5 6 7 8 9]
输入数据(scalarValueExp): 4
输出数据(dstLocal): [16 81 256 625 1296 2401 4096 6561]
```
