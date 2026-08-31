# SwiGLU

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SwiGLU_interface/SwiGLU_res.md#id1 -->

## 功能说明

SwiGLU是采用Swish作为激活函数的GLU变体。具体计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002350049237.png)

其中Swish激活函数的计算公式如下（β为常量）：

![](../../../../figures/zh-cn_formulaimage_0000001704667878.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   接口框架申请临时空间
    -   源操作数Tensor全部/部分参与计算

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const uint32_t calCount)
        ```

    -   源操作数Tensor全部参与计算

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, LocalTensor<T>& srcTensor0, LocalTensor<T>& srcTensor1, const float& scalarValue)
        ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor0/srcTensor1 | 输入 | 源操作数。<br><br>源操作数的数据类型需要与目的操作数保持一致。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| scalarValue | 输入 | 激活函数中的β参数。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于SwiGLU内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md)。 |
| calCount | 输入 | 实际计算数据元素个数。 |

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   **不支持源操作数与目的操作数地址重叠。**
-   当前仅支持ND格式的输入，不支持其他格式。
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。

<!-- npu="950" id9 -->
-   针对Ascend 950PR/Ascend 950DT，接口内部计算对Subnormal的处理方式受编译选项`--cce-ftz`控制（默认值为`true`）：
    -   配置为`false`时，计算过程中保留Subnormal，并按照其实际数值参与后续计算。
    -   配置为`true`时，启用FTZ（Flush-To-Zero）模式，计算过程中产生或参与运算的Subnormal将按0处理，可能导致计算结果与保留Subnormal时存在精度差异。
<!-- end id9 -->

## 调用示例

```
// dstLocal: 存放SwiGLU计算结果的Tensor
// src0Local：存放SwiGLU计算的输入Tensor
// src1Local：存放SwiGLU计算的输入Tensor
// scalarValue：激活函数中的β参数
// sharedTmpBuffer: 存放SwiGLU计算过程中临时缓存的Tensor

float scalarValue = 1.0;

// 接口框架申请临时空间，全部参与计算
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue);
// 接口框架申请临时空间，部分参与计算,需要参与计算的元素个数为32
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, 32);

// 通过sharedTmpBuffer入参传入临时空间，全部参与计算
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer);
// 通过sharedTmpBuffer入参传入临时空间，部分参与计算,需要参与计算的元素个数为32
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer, 32);
```

结果示例如下：

```
输入数据(srcTensor0):
[-4.         -3.7419355  -3.483871   -3.2258065  -2.967742   -2.7096775  -2.451613   -2.1935484
 -1.9354838  -1.6774193  -1.4193548  -1.1612903  -0.9032258  -0.6451613  -0.38709676 -0.12903225
  0.12903225  0.38709676  0.6451613   0.9032258   1.1612903   1.4193548   1.6774193   1.9354838
  2.1935484   2.451613    2.7096775   2.967742    3.2258065   3.483871    3.7419355   4.        ]
输入数据(srcTensor1)：
[0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5
 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
输出数据(dstLocal):
[-1.2449187  -1.1646013  -1.084284   -1.0039667  -0.9236493  -0.843332   -0.7630147  -0.68269736
 -0.60238    -0.52206266 -0.4417453  -0.361428   -0.28111064 -0.20079333 -0.12047599 -0.04015867
  0.04015867  0.12047599  0.20079333  0.28111064  0.361428    0.4417453   0.52206266  0.60238
  0.68269736  0.7630147   0.843332    0.9236493   1.0039667   1.084284    1.1646013   1.2449187 ]
```
