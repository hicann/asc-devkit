# LayerNormGradBeta

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/normalization/LayerNormGradBeta_res.md#id1 -->

## 功能说明

LayerNormGradBeta接口用于获取反向beta/gamma的数值，和LayerNormGrad共同输出pdx, gamma和beta：

算法公式为:

![](../../../figures/zh-cn_formulaimage_0000001718651312.png)

![](../../../figures/zh-cn_formulaimage_0000001766331937.png)

## 函数原型

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间大小BufferSize的获取方法：通过[LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md)中提供的GetLayerNormGradBetaMaxMinTmpSize接口获取所需最大和最小临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式，因此LayerNormGradBeta接口的函数原型有两种：

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGradBeta(const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormGradBetaTiling& tiling)
    ```

    该方式下开发者需自行申请并管理临时内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

-   接口框架申请临时空间

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void LayerNormGradBeta(const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy, LayerNormGradBetaTiling& tiling)
    ```

    该方式下开发者无需申请，但是需要预留临时空间的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数，默认值为false。如果开发者允许源操作数被改写，可以设置该参数取值为true开启，开启后能够节省部分内存空间。<br><br>设置为true，则本接口内部计算时复用inputDy的内存空间，节省内存空间；设置为false，则本接口内部计算时不复用inputDy的内存空间。<br><br>对于float数据类型输入支持开启该参数，half数据类型输入不支持开启该参数。<br><br>isReuseSource的使用样例请参考[更多样例](../math_compute/more_examples.md#section639165323915)。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| outputPdGamma | 输出 | 目的操作数，shape为[H]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。尾轴长度需要32B对齐<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| outputPdBeta | 输出 | 目的操作数，shape为[H]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。尾轴长度需要32B对齐<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| resForGamma | 输入 | 源操作数，shape为[B, S, H]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。resForGamma的数据类型需要与目的操作数保持一致，尾轴长度需要32B对齐。需提前调用[LayerNormGrad](LayerNormGrad.md)接口获取resForGamma参数值。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| inputDy | 输入 | 源操作数，shape为[B, S, H]，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。inputDy的数据类型需要与目的操作数保持一致，尾轴长度需要32B对齐。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 共享缓冲区，用于存放API内部计算产生的临时数据。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。共享缓冲区大小的获取方式请参考[LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| tiling | 输入 | LayerNormGradBeta计算所需Tiling信息，Tiling信息的获取请参考[LayerNormGradBeta Tiling](LayerNormGradBeta-Tiling.md)。 |

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   源操作数和目的操作数的Tensor空间可以复用。
-   仅支持输入shape为ND格式。
-   输入数据不满足对齐要求时，开发者需要进行补齐，补齐的数据应设置为0，防止出现异常值从而影响网络计算。
-   不支持对尾轴H轴的切分。

## 调用示例

```
// outputPdGamma: 输出对gamma参数的梯度，shape为 [H]
// outputPdBeta: 输出对beta参数的梯度，shape为 [H]
// resForGamma: 前一步LayerNormGrad输出的中间结果，即normalizedX * inputDy，shape为 [B, S, H]
// inputDy: 上游传入的梯度，shape为 [B, S, H]
// tiling: Tiling调度信息，包含并行划分、块大小等参数

// 使用LayerNormGradBeta接口计算gamma和beta的梯度
AscendC::LayerNormGradBeta<T, isReuseSource>(
    outputPdGamma, // 输出：gamma的梯度，shape [H]
    outputPdBeta,  // 输出：beta的梯度，shape [H]
    resForGamma,   // 输入：中间结果normalizedX * inputDy，来自LayerNormGrad
    inputDy,       // 输入：上游梯度dy，shape [B, S, H]
    tiling         // 输入：Tiling信息
);
```

示例结果如下：

```
输入数据(inputDy, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
输入数据(resForGamma, shape:[1, 8, 8]):
[  0.  1.  2.  3.  4.  5.  6.  7.
   8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23.
  24. 25. 26. 27. 28. 29. 30. 31.
  32. 33. 34. 35. 36. 37. 38. 39.
  40. 41. 42. 43. 44. 45. 46. 47.
  48. 49. 50. 51. 52. 53. 54. 55.
  56. 57. 58. 59. 60. 61. 62. 63. ]
输出数据(outputPdGamma):
[ 8960.  9416.  9888. 10376. 10880. 11400. 11936. 12488.]
输出数据(outputPdBeta):
[ 224. 232. 240. 248. 256. 264. 272. 280. ]
```
