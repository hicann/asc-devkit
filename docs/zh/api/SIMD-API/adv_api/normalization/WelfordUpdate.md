# WelfordUpdate

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/WelfordUpdate_res.md#id1 -->

## 功能说明

Welford是一种在线计算均值和方差的方法。一方面，它可以在不存储所有样本的情况下，逐步计算所有样本的均值和方差，更适合处理海量数据；另一方面，它只需要对数据进行一次遍历，能减少访存次数，提高计算性能。本接口为Welford算法的前处理。

LayerNorm算法中Reduce轴较大的场景，可以通过切分Reduce轴，联合使用本接口与[WelfordFinalize](WelfordFinalize.md)，实现等效计算LayerNorm。

如下图所示，切分数据的Reduce轴，假设切分后每块数据的形状为\[1, k\]，每块数据标号为1，2，3，…，n。

**图1**  Reduce轴切分示意图  
![](../../../figures/reduce_axis_split.png "Reduce轴切分示意图")

本接口的计算公式如下。进行上述的数据切分后，分n次调用本接口，切分后的每块数据均完成如下公式的计算。

![](../../../figures/zh-cn_image_0000002186230593.png)

![](../../../figures/zh-cn_image_0000002150833386.png)

上式中，x<sub>i</sub>、Meant<sub>i</sub>、M<sub>i</sub>的形状均为\[1, k\]，x<sub>i</sub>表示切分后的第i块数据，Meant<sub>i</sub>表示第i次调用本接口得到的前i块数据的均值，M<sub>i</sub>表示第i次调用本接口得到的前i块数据的方差中间结果（即为求方差而保存的中间计算结果，本节后续内容中写作方差中间结果）。其中，第一次调用本接口，即i=1时，公式中的Meant<sub>0</sub>和M<sub>0</sub>由用户定义为形状\[1, k\]、取值全0的数据。

Meant<sub>n</sub>的计算过程示意如下图，调用n次本接口后，得到形状为\[1, k\]的Meant<sub>n</sub>和M<sub>n</sub>，Meant<sub>n</sub>和M<sub>n</sub>用于后续[WelfordFinalize](WelfordFinalize.md)接口的计算。

**图2**  均值Meant<sub>n</sub>计算过程示意图  
![](../../../figures/meantn_compute_process.png "均值Meantn计算过程示意图")

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, typename U,bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void WelfordUpdate(const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean, const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer, const WelfordUpdateParam& para)
    ```

-   接口框架申请临时空间

    ```
    template <typename T, typename U,bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
    __aicore__ inline void WelfordUpdate(const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean, const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const WelfordUpdateParam& para)
    ```

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[WelfordUpdate Tiling](WelfordUpdate-Tiling.md)中提供的GetWelfordUpdateMaxMinTmpSize接口获取所需最大和最小临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | inputX操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li1773114612461)。 |
| U | outputMean、outputVariance、inputMean、inputVariance操作数的数据类型。支持的数据类型为：float。 |
| isReuseSource | 是否允许修改源操作数，默认值为false。如果开发者允许源操作数被改写，可以设置该参数取值为true开启，开启后能够节省部分内存空间。<br><br>设置为true，则本接口内部计算时复用inputX的内存空间，节省内存空间；设置为false，则本接口内部计算时不复用inputX的内存空间。<!-- npu="310p" id7 --><br><br>在Atlas 推理系列产品AI Core中，该参数预留，传入默认值false即可。<!-- end id7 --><br><br>isReuseSource的使用样例请参考[更多样例](../math_compute/more_examples.md#section639165323915)。 |
| config | 配置非指定计算范围内的目的操作数与源操作数的复用关系。WelfordUpdateConfig类型，定义如下方代码所示，其中参数的含义如下。<br>isInplace：接口参数para中的abComputeLength参数指定了输入数据内层轴的计算长度，在该指定计算长度之外的输出数据具体为何值，通过本参数设置。本参数表示，在指定计算长度之外的目的操作数是否复用源操作数；若复用，对于指定计算长度之外的输出，直接使用对应位置的源操作数代替输出目的操作数；若不复用，则本接口不会输出计算范围外的目的操作数。<br>false：默认值。表示目的操作数不复用源操作数。<br>true：表示目的操作数复用源操作数。outputMean复用inputMean，outputVariance复用inputVariance。<br><br>此参数一般用于配合kernel侧tiling计算的接口使用。 |

```
struct WelfordUpdateConfig {
    bool isInplace = false; // 目的操作数是否复用源操作数。
};
```

配置示例如下：

```
constexpr WelfordUpdateConfig WFUPDATE_DEFAULT_CFG = {false};
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| outputMean | 输出 | 均值目的操作数，对应接口公式中的Meanti。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>shape和源操作数inputMean需要保持一致。 |
| outputVariance | 输出 | 方差中间结果目的操作数，对应接口公式中的Mi。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>shape和源操作数inputVariance需要保持一致。 |
| inputMean | 输入 | 均值源操作数，对应接口公式中的Meanti-1。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputVariance | 输入 | 方差中间结果源操作数，对应接口公式中的Mi-1。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputX | 输入 | 源操作数，对应接口公式中的xi。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 临时空间。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>接口内部复杂计算时用于存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[WelfordUpdate Tiling](WelfordUpdate-Tiling.md)。 |
| para | 输入 | 计算所需的参数信息。WelfordUpdateParam类型，定义如下方代码所示，其中参数的含义如下。<br>rnLength：预留参数，固定设置为1。<br>abLength：Reduce轴拆分的大小。<br>abComputeLength：从输入的起始地址开始的Reduce轴实际计算长度。<br>nRec：取值为1/i，i为当前调用本接口的累积次数。i的取值范围为[1, n]，n为对输入数据inputX的Reduce轴切分的块数。<br><br>各目的操作数和源操作数的shape均为[rnLength, abLength]。 |

```
struct WelfordUpdateParam {
    uint32_t rnLength;
    uint32_t abLength;
    uint32_t abComputeLength;
    float nRec;
};
```

## 返回值说明

无

## 约束说明

-   接口参数para.rnLength当前只支持取值为1；
-   接口参数para.abLength的取值必须为32/sizeof\(T\)的整数倍；
-   接口参数para.abComputeLength的取值必须大于0。
-   不支持源操作数与目的操作数地址重叠。
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   支持的数据类型<a id="li1773114612461"></a>

    <!-- npu="950" id8 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16\_t、float。
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float。
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float。
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：half、float。
    <!-- end id11 -->

## 调用示例

```
// outputMean: 输出更新后的均值Meant，shape为 [1, abLength]
// outputVariance: 输出更新后的方差中间结果Mi，shape为 [1, abLength]
// inputMean: 上一时刻的均值Meant-1，作为输入
// inputVariance: 上一时刻的方差中间结果Mi-1，作为输入
// inputX: 当前时间步的输入数据xi，shape为 [1, abLength]
// sharedTmpBuffer: 开发者管理的临时空间，用于内部复杂计算
// para: 包含Reduce轴分块信息和归一化系数的参数结构

// 使用WelfordUpdate接口执行Welford在线算法更新
struct AscendC::WelfordUpdateParam para = {nLength, rLength, abComputeLength, 0.3};
AscendC::WelfordUpdate<T, U, false, WELFORD_UPDATE_ENABLE_INPLACE_CFG>(
    outputMean,      // 输出：更新后的均值
    outputVariance,  // 输出：更新后的方差中间结果
    inputMean,       // 输入：上一时刻均值
    inputVariance,   // 输入：上一时刻方差中间结果
    inputX,          // 输入：当前输入xi
    sharedTmpBuffer, // 输入：临时缓冲区（由开发者提供）
    para             // 输入：Welford更新参数
);
```

示例结果如下：

```
输入数据(inputX, shape:[1, 64]):
[1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1.
 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1.
 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. 1. ]
输入数据(gammaLocal, shape:[64]):
[ 0.  1.  2.  3.  4.  5.  6.  7.  8.  9. 10. 11. 12. 13. 14. 15. 16. 17.
 18. 19. 20. 21. 22. 23. 24. 25. 26. 27. 28. 29. 30. 31. 32. 33. 34. 35.
 36. 37. 38. 39. 40. 41. 42. 43. 44. 45. 46. 47. 48. 49. 50. 51. 52. 53.
 54. 55. 56. 57. 58. 59. 60. 61. 62. 63. ]
输入数据(betaLocal, shape:[64]):
[ 0.  1.  2.  3.  4.  5.  6.  7.  8.  9. 10. 11. 12. 13. 14. 15. 16. 17.
 18. 19. 20. 21. 22. 23. 24. 25. 26. 27. 28. 29. 30. 31. 32. 33. 34. 35.
 36. 37. 38. 39. 40. 41. 42. 43. 44. 45. 46. 47. 48. 49. 50. 51. 52. 53.
 54. 55. 56. 57. 58. 59. 60. 61. 62. 63. ]
输出数据(meanLocal):
[ 0.125  1.     1.875  2.75   3.625  4.5    5.375  6.25   7.125  8.
  8.875  9.75  10.625 11.5   12.375 13.25  14.125 15.    15.875 16.75
 17.625 18.5   19.375 20.25  21.125 22.    22.875 23.75  24.625 25.5
 26.375 27.25  28.125 29.    29.875 35.    36.    37.    38.    39.
 40.    41.    42.    43.    44.    45.    46.    47.    48.    49.
 50.    51.    52.    53.    54.    55.    56.    57.    58.    59.
 60.    61.    62.    63. ]
输出数据(varianceLocal):
[8.75000e-01 1.00000e+00 2.87500e+00 6.50000e+00 1.18750e+01 1.90000e+01
 2.78750e+01 3.85000e+01 5.08750e+01 6.50000e+01 8.08750e+01 9.85000e+01
 1.17875e+02 1.39000e+02 1.61875e+02 1.86500e+02 2.12875e+02 2.41000e+02
 2.70875e+02 3.02500e+02 3.35875e+02 3.71000e+02 4.07875e+02 4.46500e+02
 4.86875e+02 5.29000e+02 5.72875e+02 6.18500e+02 6.65875e+02 7.15000e+02
 7.65875e+02 8.18500e+02 8.72875e+02 9.29000e+02 9.86875e+02 3.50000e+01
 3.60000e+01 3.70000e+01 3.80000e+01 3.90000e+01 4.00000e+01 4.10000e+01
 4.20000e+01 4.30000e+01 4.40000e+01 4.50000e+01 4.60000e+01 4.70000e+01
 4.80000e+01 4.90000e+01 5.00000e+01 5.10000e+01 5.20000e+01 5.30000e+01
 5.40000e+01 5.50000e+01 5.60000e+01 5.70000e+01 5.80000e+01 5.90000e+01
 6.00000e+01 6.10000e+01 6.20000e+01 6.30000e+01 ]
```
