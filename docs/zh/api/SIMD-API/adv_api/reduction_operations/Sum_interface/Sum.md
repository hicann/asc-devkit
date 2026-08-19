# Sum

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/reduction_operations/Sum_interface/Sum_res.md#id1 -->

## 功能说明

获取最后一个维度的元素总和。

如果输入是向量，则在向量中对各元素相加；如果输入是矩阵，则沿最后一个维度对每行中元素求和。**本接口最多支持输入为二维数据，不支持更高维度的输入。**

如下图所示，对shape为\(2, 3\)的二维矩阵进行运算，输出结果为\[6, 15\]。

![](../../../../figures/reduce_sum_adv_api.png)

为计算如上过程，引入一些必备概念：行数称之为**外轴长度（outter）**，每行实际的元素个数称之为**内轴的实际元素个数（n）**，存储n个元素所需的字节长度向上补齐到32整数倍后转换的元素个数称之为**补齐后的内轴元素个数\(inner\)**。本接口要求输入的内轴长度为32字节的整数倍，所以当n占据的字节长度不是32的整数倍时，需要开发者将其向上补齐到32的整数倍。比如，如下的样例中，元素类型为half，每行的实际元素个数n为3，占据字节长度为6字节，不是32字节的整数倍，向上补齐后得到32字节，转换为元素个数为16。故outter = 2，n =3，inner=16。图中的padding代表补齐操作。n和inner的关系如下：**inner = \(n \*sizeof\(T\) + 32 - 1\) / 32 \* 32 / sizeof\(T\)**。

![](../../../../figures/sum.png)

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void Sum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SumParams& sumParams)
    ```

-   接口框架申请临时空间

    ```
    template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
    __aicore__ inline void Sum(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const SumParams& sumParams)
    ```

由于该接口的内部实现中涉及复杂的数学计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间支持开发者**通过sharedTmpBuffer入参传入**和**接口框架申请**两种方式。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。
-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间；接口框架申请的方式，开发者需要预留临时空间。临时空间大小BufferSize的获取方式如下：通过[GetSumMaxMinTmpSize](GetSumMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| reduceDim | 用于指定按数据的哪一维度进行求和。本接口按最后一个维度实现，不支持reduceDim参数，传入默认值-1即可。 |
| isReuseSource | 是否允许修改源操作数。该参数预留，传入默认值false即可。 |
| isBasicBlock | 预留参数，暂不支持。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>输出值需要outter * sizeof(T)大小的空间进行保存。开发者要根据该大小和框架的对齐要求来为dstTensor分配实际内存空间。<br>**注意**：遵循框架对内存开辟的要求（开辟内存的大小满足32Byte对齐），即outter * sizeof(T)不是32Byte对齐时，需要向上进行32Byte对齐。为了对齐而多开辟的内存空间不填值，为一些随机值。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>源操作数的数据类型需要与目的操作数保持一致。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于Sum内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetSumMaxMinTmpSize](GetSumMaxMinTmpSize.md)。 |
| sumParams | 输入 | srcTensor的shape信息。SumParams类型，具体定义如下方代码所示，其中参数的含义为：<br>outter：输入数据的外轴长度。<br>inner：输入数据内轴实际元素个数32字节补齐后的元素个数，inner\*sizeof(T)必须是32字节的整数倍。<br>n：输入数据内轴的实际元素个数。<br>请注意：sumParams.inner是sumParams.n向上32字节对齐后的值，inner = (n *sizeof(T) + 32 - 1) / 32 * 32 / sizeof(T)，因此sumParams.n的大小应该满足：1 <= sumParams.n <= sumParams.inner。 |

```
struct SumParams {
    uint32_t outter = 1;
    uint32_t inner;
    uint32_t n;
};
```

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   **不支持源操作数与目的操作数地址重叠。**
-   不支持sharedTmpBuffer与源操作数和目的操作数地址重叠。
-   当前仅支持ND格式的输入，不支持其他格式。
-   一维输入的outter值填为1；二维输入按实际情况填写outter和n，inner计算请按如上公式计算，否则功能不正确。
-   srcTensor需要能够容纳内轴对齐后的数据占用空间大小，dstTensor需要能够容纳outter个结果对齐后的数据占用空间大小。
-   对于Sum，其内部使用的底层相加方式和[ReduceSum](../../../basic_api/memory_vector_compute/reduction_compute/ReduceSum.md)以及[ReduceRepeat](../../../basic_api/memory_vector_compute/reduction_compute/ReduceRepeat.md)的内部的相加方式一致，采用二叉树方式，两两相加：

    假设源操作数为128个half类型的数据\[data0,data1,data2...data127\]，一个repeat可以计算完，计算过程如下。

    1.  data0和data1相加得到data00，data2和data3相加得到data01...data124和data125相加得到data62，data126和data127相加得到data63；
    2.  data00和data01相加得到data000，data02和data03相加得到data001...data62和data63相加得到data031；
    3.  以此类推，得到目的操作数为1个half类型的数据\[data\]。

## 调用示例

```
// 定义srcTensor的shape信息，输入元素类型为half，大小为2*3的二维数据
AscendC::SumParams params;
// m为2, outter等于m的值
params.outter = outter;
// n为3
params.n = n;
// inner = (n * sizeof(half) + 32 - 1)/32 * 32 / sizeof(half) = 16
params.inner = inner;
// 填充时的值
T scalar(0);
// yLocal填充为0，out_inner是2向上16取整到的值，为16，计算方法参考inner
AscendC::Duplicate<T>(yLocal, scalar, out_inner);
// 对shape为(2, 3)的二维矩阵进行运算，输出结果为[6, 15]
// xLocal和yLocal中的0是计算时padding的值
AscendC::Sum(yLocal, xLocal, sharedTmpBuffer, params);
```

结果示例如下。

```
输入数据srcLocal: [[1 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0],
                     [4 5 6 0 0 0 0 0 0 0 0 0 0 0 0 0]]
输出数据dstLocal: [6 15 0 0 0 0 0 0 0 0 0 0 0 0 0 0]
```
