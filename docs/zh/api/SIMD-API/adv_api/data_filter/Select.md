# Select

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/data_filter/Select_res.md#id1 -->

## 功能说明

给定两个源操作数src0和src1，根据maskTensor相应位置的值（非bit位）选取元素，得到目的操作数dst。选择的规则为：当Mask的值为0时，从src0中选取，否则从src1选取。

**该接口支持多维Shape，需满足maskTensor和源操作数Tensor的前轴（非尾轴）元素个数相同，且maskTensor尾轴元素个数大于等于源操作数尾轴元素个数，maskTensor多余部分丢弃不参与计算。**

-   **maskTensor尾轴需32字节对齐且元素个数为16的倍数。**
-   **源操作数Tensor尾轴需32字节对齐。**

如下图样例，源操作数src0为Tensor，shape为\(2,16\)，数据类型为half，尾轴长度满足32字节对齐；源操作数src1为scalar，数据类型为half；maskTensor的数据类型为bool，为满足对齐要求shape为\(2,32\)，仅有图中蓝色部分的mask掩码生效，灰色部分不参与计算。输出目的操作数dstTensor如下图所示。

![](../../../figures/nz_reduce_53.png)

## 实现原理

以float类型，ND格式，shape为\[m, k1\]的source输入Tensor，shape为\[m, k2\]的mask Tensor为例，描述Select高阶API内部算法框图，如下图所示。

**图1**  Select算法框图  
![](../../../figures/select_diagram.png "Select算法框图")

计算过程分为如下几步，均在Vector上进行：

1.  GatherMask步骤：如果k1, k2不相等，则根据src的shape\[m, k1\]，对输入mask\[m, k2\]通过GatherMask进行reduce计算，使得mask的k轴多余部分被舍去，shape转换为\[m, k1\]；
2.  Cast步骤：将上一步的mask结果cast成half类型；
3.  Compare步骤：使用Compare接口将上一步的mask结果与0进行比较，得到cmpmask结果；
4.  Select步骤：根据cmpmask的结果，选择srcTensor相应位置的值或者scalar值，输出Output。

**图2**  Select算法框图  
![](../../../figures/select_diagram_1.png "Select算法框图-54")

计算过程在Vector上进行，循环m次，每次对k1个元素进行如下操作：

1.  Compare步骤：使用Compare接口将mask值与0进行比较，得到cmpmask结果；
2.  Select步骤：根据cmpmask的结果，选择srcTensor相应位置的值或者scalar值，输出Output。

## 函数原型

-   src0为srcTensor（tensor类型），src1为srcScalar（scalar类型）

    ```
    template <typename T, typename U, bool isReuseMask = true>
    __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1, const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
    ```

-   src0为srcScalar（scalar类型），src1为srcTensor（tensor类型）

    ```
    template <typename T, typename U, bool isReuseMask = true>
    __aicore__ inline void Select(const LocalTensor<T>& dst, T src0, const LocalTensor<T>& src1, const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
    ```

该接口需要额外的临时空间来存储计算过程中的中间变量。临时空间需要开发者**申请并通过sharedTmpBuffer入参传入**。临时空间大小BufferSize的获取方式如下：通过[GetSelectMaxMinTmpSize](GetSelectMaxMinTmpSize.md)中提供的接口获取需要预留空间范围的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |
| U | 掩码Tensor mask的数据类型。支持的数据类型为：bool、int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t。 |
| isReuseMask | 是否允许修改maskTensor。默认为true。<br><br>取值为true时，仅在maskTensor尾轴元素个数和srcTensor尾轴元素个数不同的情况下，maskTensor可能会被修改；其余场景，maskTensor不会修改。<br><br>取值为false时，任意场景下，maskTensor均不会修改，但可能会需要更多的临时空间。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src0(srcTensor)<br><br>src1(srcTensor) | 输入 | 源操作数。**源操作数Tensor尾轴需32字节对齐**。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| src1(srcScalar)<br><br>src0(srcScalar) | 输入 | 源操作数。类型为scalar。 |
| mask | 输入 | 掩码Tensor。用于描述如何选择srcTensor和srcScalar之间的值。maskTensor尾轴需32字节对齐且元素个数为16的倍数。<br>**src0为srcTensor（tensor类型），src1为srcScalar（scalar类型）**：<br>若mask的值为0，选择srcTensor相应的值放入dstLocal，否则选择srcScalar的值放入dstLocal。<br>**src0为srcScalar（scalar类型），src1为srcTensor（tensor类型）**：<br>若mask的值为0，选择srcScalar的值放入dstLocal，否则选择srcTensor相应的值放入dstLocal。 |
| sharedTmpBuffer | 输入 | 该API用于计算的临时空间，所需空间大小根据[GetSelectMaxMinTmpSize](GetSelectMaxMinTmpSize.md)获取。 |
| info | 输入 | 描述SrcTensor和maskTensor的shape信息。SelectWithBytesMaskShapeInfo类型，定义如下方代码所示，其中参数的含义为：<br>firstAxis：srcLocal/maskTensor的前轴元素个数。<br>srcLastAxis：srcLocal的尾轴元素个数。<br>maskLastAxis：maskTensor的尾轴元素个数。<br>注意：<br>需要满足srcTensor和maskTensor的前轴元素个数相同，均为firstAxis。<br>需要满足firstAxis * srcLastAxis = srcTensor.GetSize()；firstAxis * maskLastAxis = maskTensor.GetSize()。<br>maskTensor尾轴的元素个数大于等于srcTensor尾轴的元素个数，计算时会丢弃maskTensor多余部分，不参与计算。 |

```cpp
struct SelectWithBytesMaskShapeInfo {
    __aicore__ SelectWithBytesMaskShapeInfo() {};
    uint32_t firstAxis = 0;
    uint32_t srcLastAxis = 0;
    uint32_t maskLastAxis = 0;
};
```

## 返回值说明

无

## 约束说明

-   源操作数与目的操作数可以复用。
-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。
-   maskTensor尾轴元素个数和源操作数尾轴元素个数不同的情况下， maskTensor的数据有可能被接口改写。

## 调用示例

完整示例可以参考[Select样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/07_filter/select)。

```
// dstLocal：结果数据
// srcLocal1：输入张量
// scalar：待比较标量
// maskLocal：选择掩码
// tmpBuffer：临时空间
// info：srcLocal和maskLocal的shape信息
AscendC::SelectWithBytesMaskShapeInfo info;
AscendC::Select(dstLocal, srcLocal1, scalar, maskLocal, tmpBuffer, info);
```

结果示例如下：

```
输入数据srcLocal1:
[-84.6    -24.38    30.97   -30.25    22.28   -92.56    90.44   -58.72  -86.56     5.74     6.754  -86.3    -96.7    -37.38   -81.9     46.9
 -99.4     94.2    -41.78   -60.3    -14.43    78.6      8.93   -65.2    79.94   -46.88     4.516   20.03   -25.56    24.73     0.3223  21.98

 -87.4    -93.9     46.22   -69.9     90.8    -24.17   -96.2    -91.    90.44     9.766   68.25   -57.78   -75.44    -8.86   -91.56    21.6
  76.      82.1    -78.     -23.75    92.     -66.44    75.      94.9   2.62   -90.9     15.945   38.16    50.84    96.94   -59.38    44.22  ]
输入数据scalar:
[35.6]
输入数据maskLocal:
[False  True False False  True  True False  True  True False False  True False  True False  True
 True   False False False  True  True  True  True   True False  True False  True  True  True  True

 False False  True False  True False  True False  True False  True False  True  True  True False
 True False  True False  True False  True  True   True False False False  True False  True  True
]

输出数据dstLocal:
[-84.6    35.6    30.97   -30.25   35.6    35.6    90.44   35.6  35.6    5.74    6.754   35.6   -96.7    35.6   -81.9    35.6
  35.6    94.2    -41.78  -60.3    35.6    35.6    35.6    35.6  35.6   -46.88   35.6    20.03   35.6    35.6    35.6    35.6

 -87.4   -93.9    35.6    -69.9    35.6   -24.17   35.6   -91.   35.6   9.766  35.6   -57.78   35.6     35.6    35.6    21.6
  35.6    82.1    35.6    -23.75   35.6   -66.44   35.6    35.6  35.6   -90.9    15.945  38.16   35.6    96.94   35.6    35.6  ]
```
