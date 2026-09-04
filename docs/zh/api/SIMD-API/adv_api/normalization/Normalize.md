# Normalize

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/Normalize_res.md#id1 -->

## 功能说明

[LayerNorm](LayerNorm.md)中，已知均值和方差，计算shape为\[A，R\]的输入数据的标准差的倒数rstd和y，其计算公式如下：

![](../../../figures/zh-cn_formulaimage_0000002046262966.png)

![](../../../figures/zh-cn_formulaimage_0000002082381949.png)

其中，E和Var分别代表输入在R轴的均值，方差，γ为缩放系数，β为平移系数，ε为防除零的权重系数。

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template < typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
    __aicore__ inline void Normalize(const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon, const NormalizePara& para)
    ```

-   接口框架申请临时空间

    ```
    template < typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
    __aicore__ inline void Normalize(const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean, const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon, const NormalizePara& para)
    ```

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[Normalize Tiling](Normalize-Tiling.md)中提供的GetNormalizeMaxMinTmpSize接口获取所需最大和最小临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| U | beta，gamma操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li171091334311)。 |
| T | output，inputX操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li171091334311)。 |
| isReuseSource | 该参数预留，传入默认值false即可。 |
| config | 配置Normalize接口中输入输出相关信息。NormalizeConfig类型，定义如下方代码所示，其中参数的含义如下。<br><br>reducePattern：当前仅支持ReducePattern::AR模式，表示输入的内轴R轴为reduce计算轴。<br><br>aLength：用于描述输入的A轴大小。支持的取值如下：<br>-1：默认值。取[接口参数](#table2087718184450)para中的aLength作为A轴大小。<!-- npu="950" id7 --><br>1：支持outputRstd数据非对齐搬出，支持inputMean，inputVariance数据非对齐搬入。aLength为其它取值时，不支持上述三个输入输出的非对齐搬入和非对齐搬出。该取值需要与[接口参数](#table2087718184450)para中的aLength数值一致。请注意，仅在Ascend 950PR/Ascend 950DT上支持该取值。<!-- end id7 --><br>其它值：该值需要与[接口参数](#table2087718184450)para中的aLength数值一致。<br><br>isNoBeta：计算时，输入beta是否使用。false：默认值，Normalize计算中使用输入beta。true：Normalize计算中不使用输入beta。此时，公式中与beta相关的计算被省略。<br><br>isNoGamma：可选输入gamma是否使用。false：默认值，Normalize计算中使用可选输入gamma。true：Normalize计算中不使用输入gamma。此时，公式中与gamma相关的计算被省略。<br><br>isOnlyOutput：是否只输出y，不输出标准差的倒数rstd。当前该参数仅支持取值为false，表示y和rstd的结果全部输出。 |

```
struct NormalizeConfig {
    ReducePattern reducePattern = ReducePattern::AR;
    int32_t aLength = -1;
    bool isNoBeta = false;
    bool isNoGamma = false;
    bool isOnlyOutput = false;
};
```

**表2**  接口参数说明

<a name="table2087718184450"></a>

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| output | 输出 | 目的操作数，shape为[A, R]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| outputRstd | 输出 | 标准差的倒数，shape为[A]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputMean | 输入 | 均值，shape为[A]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputVariance | 输入 | 方差，shape为[A]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputX | 输入 | 源操作数，shape为[A, R]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。inputX的数据类型需要与目的操作数保持一致，尾轴长度需要32B对齐。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| gamma | 输入 | 缩放系数，shape为[R]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。gamma的数据类型精度不低于源操作数的数据类型精度。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| beta | 输入 | 平移系数，shape为[R]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。beta的数据类型精度不低于源操作数的数据类型精度。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 共享缓冲区，用于存放API内部计算产生的临时数据。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。共享缓冲区大小的获取方式请参考[Normalize Tiling](Normalize-Tiling.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| epsilon | 输入 | 防除零的权重系数。 |
| para | 输入 | Normalize计算所需的参数信息。NormalizePara类型，定义如下方代码所示，其中参数的含义如下。<br>aLength：指定输入inputX的A轴长度。<br>rLength：指定输入inputX的R轴长度。<br>rLengthWithPadding：指定输入inputX的R轴对齐后的长度，该值是32B对齐的。 |

```
struct NormalizePara {
    uint32_t aLength;
    uint32_t rLength;
    uint32_t rLengthWithPadding;
};
```

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   缩放系数gamma和平移系数beta的数据类型精度必须不低于源操作数inputX的数据类型精度。比如，inputX的数据类型是half，gamma、beta的数据类型可以是half或者float，精度不低于inputX。比如，inputX的数据类型是bfloat16\_t，gamma、beta的数据类型可以是bfloat16\_t或者float，精度不低于inputX。
-   src和dst的Tensor空间不可以复用。
-   输入仅支持ND格式。
-   R轴不支持切分。
-   支持的数据类型<a id="li171091334311"></a>

    <!-- npu="950" id8 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16\_t、float。
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为: half、float。
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为: half、float。
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    Atlas 推理系列产品AI Core，支持的数据类型为: half、float。
    <!-- end id11 -->

<!-- npu="950" id12 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id12 -->

## 调用示例

```
// yLocal: 输出归一化后的结果y，shape为 [A, R]
// rstdLocal: 输出标准差的倒数（1 / sqrt(variance + epsilon)），shape为 [A]
// meanLocal: 输入的均值，shape为 [A]
// varianceLocal: 输入的方差，shape为 [A]
// xLocal: 输入数据X，shape为 [A, R]，数据类型与output一致
// gammaLocal: 缩放参数gamma，shape为 [R]
// betaLocal: 平移参数beta，shape为 [R]
// epsilon: 防除零系数
// para: 包含A、R维度信息的NormalizePara结构体
// config: Normalize配置参数，指定是否跳过gamma/beta、reduce模式等

constexpr AscendC::NormalizeConfig CONFIG{
    .reducePattern = AscendC::ReducePattern::AR,
    .aLength = -1,
    .isNoBeta = isNoBeta,
    .isNoGamma = isNoGamma,
    .isOnlyOutput = false};

// 使用Normalize接口执行层归一化计算
AscendC::Normalize<DTYPE_Y, DTYPE_X, false, CONFIG>(
    yLocal,        // 输出：归一化结果y，shape [A, R]
    rstdLocal,     // 输出：标准差倒数rstd，shape [A]
    meanLocal,     // 输入：均值mean，shape [A]
    varianceLocal, // 输入：方差variance，shape [A]
    xLocal,        // 输入：原始数据X，shape [A, R]
    gammaLocal,    // 输入：缩放系数γ，shape [R]
    betaLocal,     // 输入：平移系数β，shape [R]
    epsilon,       // 输入：防除零系数ε
    para           // 输入：Tiling参数，包含aLength、rLength、rLengthWithPadding
);
```

示例结果如下：

```
输入数据(srcLocal, shape:[8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
输入数据(meanLocal, shape:[8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
输入数据(varianceLocal, shape:[8]):
[ 0. 1. 2. 3. 4. 5. 6. 7. ]
输入数据(gammaLocal, shape:[8]):
[ 1. 1. 1. 1. 1. 1. 1. 1. ]
输入数据(betaLocal, shape:[8]):
[ 1. 1. 1. 1. 1. 1. 1. 1. ]
输出数据(yLocal):
[ 1.0 32.622772 64.245544 95.868324 127.4911 159.11388 190.73665 222.35942
  7.996503 8.996003 9.995503 10.995004 11.994504 12.994005 13.9935055 14.993006
  10.897021 11.603951 12.310882 13.017812 13.724742 14.431672 15.138602 15.845532
  13.122336 13.699591 14.276845 14.854099 15.431353 16.008606 16.585861 17.163115
  14.998251 15.498188 15.998126 16.498064 16.998001 17.497938 17.997875 18.497814
  16.65091 17.09808 17.545248 17.992416 18.439585 18.886755 19.333923 19.781092
  18.144999 18.553213 18.961428 19.369642 19.777857 20.186071 20.594284 21.002499
  19.518936 19.896873 20.27481 20.652748 21.030685 21.408623 21.78656 22.164497 ]
输出数据(rstdLocal):
[ 31.622774    0.9995004   0.7069301   0.5772541   0.49993753  0.44716886  0.40821427  0.37793747 ]
```
