# Mean

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/Mean_interface/Mean_res.md#id1 -->

## 功能说明

根据最后一轴的方向对各元素求平均值。

如果输入是向量，则在向量中对各元素相加求平均；如果输入是矩阵，则沿最后一个维度对元素求平均。**本接口最多支持输入为二维数据，不支持更高维度的输入。**

如下图所示，对shape为\(4, 5\)的二维矩阵进行求平均操作，输出结果为\[3， 8， 13， 18\]。

![](../../../../figures/nz_reduce.png)

在了解接口具体功能之前，需要了解一些必备概念：数据的行数称之为**外轴长度（outter）**，每行实际的元素个数称之为**内轴的实际元素个数（n）**，内轴实际元素个数n向上32字节对齐后的元素个数称之为**补齐后的内轴元素个数\(inner\)**。本接口要求输入的内轴长度满足32字节对齐，所以当n占据的字节长度不是32字节的整数倍时，需要开发者将其向上补齐到32字节的整数倍。如下样例中，元素类型为float，每行的实际元素个数n为5，占据字节长度为20字节，不是32字节的整数倍，向上补齐后得到32字节，对应的元素个数为8。图中的padding代表补齐操作。n和inner的关系如下：**inner = \(n \*sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\)**。

![](../../../../figures/nz_reduce_47.png)

## 函数原型

- 通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
    __aicore__ inline void Mean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const MeanParams& meanParams)
    ```

- 接口框架申请临时空间

    ```
    template <typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
    __aicore__ inline void Mean(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const MeanParams& meanParams)
    ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

- 通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
- 接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetMeanMaxMinTmpSize](GetMeanMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| accType | 实际参与计算的数据类型，设置的accType精度高于输入T的情况下，在计算之前会将输入转换为accType，使用accType类型计算，计算完成后再转换为原来的数据类型。设置accType值升精度可以防止数据类型溢出。T为half时，您可以将accType设置为float，表示为输入half类型升精度至float进行计算。不支持accType精度低于输入T的情况。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| isBasicBlock | 预留参数，暂不支持。 |
| reduceDim | 用于指定按数据的哪一维度进行求和。本接口按最后一个维度实现，不支持reduceDim参数，传入默认值-1即可。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>输出值需要outter * sizeof(T)大小的空间进行保存。开发者要根据该大小和框架的对齐要求来为dstTensor分配实际内存空间。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。<br><br>输入数据shape为outter * inner。开发者需要为其开辟大小为outter * inner * sizeof(T)的空间。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于Mean内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetMeanMaxMinTmpSize](GetMeanMaxMinTmpSize.md)。 |
| MeanParams | 输入 | srcTensor的shape信息。MeanParams类型，具体定义如下方代码所示，其中参数的含义为：<br>outter：输入数据的外轴长度。<br>inner：输入数据内轴实际元素个数32字节补齐后的元素个数，inner\*sizeof(T)必须是32字节的整数倍。<br>n：输入数据内轴的实际元素个数。<br>请注意：MeanParams.inner是MeanParams.n向上32字节对齐后的值，inner = (n *sizeof(T) + 32 - 1) / 32 * 32 / sizeof(T)，因此MeanParams.n的大小应该满足：1 <= MeanParams.n <= MeanParams.inner。 |

```
struct MeanParams {
    uint32_t outter = 1;
    uint32_t inner;
    uint32_t n;
};
```

## 返回值说明

无

## 约束说明

- 操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
- **不支持源操作数与目的操作数地址重叠。**
- 不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
- 当前仅支持ND格式的输入，不支持其他格式。
- 对于mean，采用的方式为先求和再做除法，其求和时内部使用的底层相加方式与[Sum](../Sum_interface/Sum.md)、[ReduceSum](../../../basic_api/memory_vector_compute/reduction_compute/ReduceSum.md)以及[ReduceRepeat](../../../basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md)的内部的相加方式一致，采用二叉树方式，两两相加，可参考[Sum](../Sum_interface/Sum.md)。

## 调用示例

```
#include "kernel_operator.h"
// 定义srcTensor的shape信息，输入元素类型为half，大小为2*3的二维数据
AscendC::MeanParams meanParams;
// m为2, outter等于m的值
meanParams.outter = outter;
// n为3
meanParams.n = n;
// inner = (n * sizeof(half) + 32 - 1)/32 * 32 / sizeof(half) = 16
meanParams.inner = inner;
// T为half，accTypes为实际计算的类型，这里是half
// dstLocal输出数据，srcLocal输入数据，tmplocalTensor用户传入的临时缓存
// AscendC::Mean<T, accType>(dstLocal, srcLocal,tmplocalTensor, meanParams)
// 也可不使用tmplocalTensor。
// 调用如下 AscendC::Mean<T, accType>(dstLocal, srcLocal, meanParams);
```

结果示例如下：

```
输入数据(srcLocal): [[1 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0],
                     [4 5 6 0 0 0 0 0 0 0 0 0 0 0 0 0]]
输出数据(dstLocal): [2 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
```
