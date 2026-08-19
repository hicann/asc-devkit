# CumSum

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/CumSum_interface/CumSum_res.md#id1 -->

## 功能说明

用于对输入张量按行或列进行累加和操作，输出结果中每个元素都是输入张量中对应位置及之前所有行或列的元素累加和。

计算公式如下：

![](../../../../figures/zh-cn_formulaimage_0000002340790900.png)

-   逐行累加算法
    -   First轴处理，按行累加和操作，即第一行不变，后面的行依次累加，输出结果的第i行第j列计算公式如下：

        ![](../../../../figures/zh-cn_formulaimage_0000002223791509.png)

        以tensor\(\[\[0, 1, 2\], \[3, 4, 5\]\]\)为例，输出结果是tensor\(\[\[0, 1, 2\], \[3, 5, 7\]\]\)

    -   Last轴处理，按列累加和操作，即第一列不变，后面的列依次累加，输出结果的第i行第j列计算公式如下：

        ![](../../../../figures/zh-cn_formulaimage_0000002223797889.png)

        以tensor\(\[\[0, 1, 2\], \[3, 4, 5\]\]\)为例，输出结果是tensor\(\[\[0, 1, 3\], \[3, 7, 12\]\]\)

-   Sklansky二分累加算法

    <!-- npu="950" id7 -->仅支持Ascend 950PR/Ascend 950DT。<!-- end id7 -->

    Sklansky二分累加算法是基于Sklansky Adder的并行前缀和逻辑实现的。[图1](#fig45153311379)为一维二进制的并行前缀和算法示意图。将该算法扩展至二维张量的累加和算法，以按行累加为例，[图2](#fig7422174293712)为该算法的执行步骤示意图，通过并行计算多行的加和，实现Sklansky二分累加算法下的按行累加和。

    **图1**  Sklansky Adder算法示意图<a name="fig45153311379"></a>  
    ![](../../../../figures/sklansky_adder_diagram.png "Sklansky-Adder算法示意图")

    **图2**  基于Sklansky的二分累加示意图<a name="fig7422174293712"></a>  
    ![](../../../../figures/sklansky_binary_accumulation.png "基于Sklansky的二分累加示意图")

## 函数原型

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T, const CumSumConfig& config = defaultCumSumConfig>
    __aicore__ inline void CumSum(LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const CumSumInfo& cumSumInfo)
    ```

-   接口框架申请临时空间

    ```
    template <typename T, const CumSumConfig& config = defaultCumSumConfig>
    __aicore__ inline void CumSum(LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor, const CumSumInfo& cumSumInfo)
    ```

由于该接口的内部实现中涉及精度转换。需要额外的临时空间来存储计算过程中的中间变量。临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式。

-   接口框架申请临时空间，开发者无需申请，但是需要预留临时空间的大小。

-   通过sharedTmpBuffer入参传入，使用该tensor作为临时空间进行处理，接口框架不再申请。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

接口框架申请的方式，开发者需要预留临时空间；通过sharedTmpBuffer传入的情况，开发者需要为tensor申请空间。临时空间大小BufferSize的获取方式如下：通过[GetCumSumMaxMinTmpSize](GetCumSumMaxMinTmpSize.md)中提供的接口获取需要预留空间的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| config | 定义CumSum接口编译时config参数。CumSumConfig类型，具体定义如下方代码所示，其中参数的含义为：<br><br>isLastAxis：取值为true表示计算按last轴处理，取值为false表示计算按first轴处理；<br><br>isReuseSource：是否可以复用srcTensor的内存空间；该参数预留，传入默认值false即可。<br><br>outputLastRow：是否输出最后一行数据。<br>algorithm：CumSum内部实现使用的累加和算法，<!-- npu="950" id8 -->该参数仅支持Ascend 950PR/Ascend 950DT，<!-- end id8 -->支持的取值如下：CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE：逐行累加算法。CumSumAlgorithm::CUMSUM_ALGORITHM_SKLANSKY：Sklansky二分累加算法。 |

```
struct CumSumConfig {
    bool isLastAxis{true};
    bool isReuseSource{false};
    bool outputLastRow{false};
    CumSumAlgorithm algorithm{CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
};
enum class CumSumAlgorithm { CUMSUM_ALGORITHM_LINEBYLINE = 0, CUMSUM_ALGORITHM_SKLANSKY = 1 };
```

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。按first轴或last轴处理，输入元素的累加和。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| lastRowTensor | 输出 | 目的操作数。模板参数config中的outputLastRow参数取值为true时，输出的最后一行数据。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| sharedTmpBuffer | 输入 | 临时缓存。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br><br>用于CumSum内部复杂计算时存储中间变量，由开发者提供。<br><br>临时空间大小BufferSize的获取方式请参考[GetCumSumMaxMinTmpSize](GetCumSumMaxMinTmpSize.md)。 |
| cumSumInfo | 输入 | srcTensor的shape信息。CumSumInfo类型，具体定义如下方代码所示，其中参数的含义为：<br><br>outter：表示输入数据的外轴长度。<br><br>inner：表示输入数据的内轴长度。<br><br>请注意：<br><br>cumSumInfo.outter和cumSumInfo.inner都应大于0。<br><br>cumSumInfo.outter * cumSumInfo.inner不能大于dstTensor或srcTensor的大小。<br><br>cumSumInfo.inner * sizeof(T)必须是32字节的整数倍。<br><br>当模板参数config中的outputLastRow取值为true时，cumSumInfo.inner不能大于lastRowTensor输出的最后一行数据的大小。 |

```
struct CumSumInfo {
    uint32_t outter{0};
    uint32_t inner{0};
};
```

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
-   输入input只支持二维结构。
-   cumSumInfo.inner \* sizeof\(T\)必须是32字节的整数倍。

## 调用示例

完整的调用样例请参考[CumSum样例](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/cumsum)。

```
// dstLocal: 存放计算结果的Tensor
// lastRowLocal: 存放计算结果最后一行数据的Tensor
// srcLocal: 参与计算的输入Tensor

// 按last轴处理（按列累加），输出最后一列数据，采用逐行累加算法
constexpr AscendC::CumSumConfig cumSumConfig{true, false, true, AscendC::CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
// outer: 外轴长度
// inner: 内轴长度
const AscendC::CumSumInfo cumSumInfo{outer, inner};
AscendC::CumSum<T, cumSumConfig>(dstLocal, lastRowLocal, srcLocal, cumSumInfo);

// 按first轴处理（按行累加），输出最后一行数据，采用逐行累加算法
constexpr AscendC::CumSumConfig cumSumConfig{false, false, true, AscendC::CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
AscendC::CumSum<T, cumSumConfig>(dstLocal, lastRowLocal, srcLocal, cumSumInfo);
```

cumSumConfig取值\{true, false, true, AscendC::CumSumAlgorithm::CUMSUM\_ALGORITHM\_LINEBYLINE\}时示例数据如下：

```
输入数据(srcLocal): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
输出数据(dstLocal): [1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8]
输出数据(lastRowLocal): [8 8 8 8]
```

cumSumConfig取值\{false, false, true, AscendC::CumSumAlgorithm::CUMSUM\_ALGORITHM\_LINEBYLINE\}时示例数据如下：

```
输入数据(srcLocal): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
输出数据(dstLocal): [1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4]
输出数据(lastRowLocal): [4 4 4 4 4 4 4 4]
```
