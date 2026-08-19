# Arange

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/index_compute/Arange_res.md#id1 -->

## 功能说明

给定起始值，等差值和长度，返回一个等差数列。

## 实现原理

以float类型，ND格式，firstValue和diffValue输入Scalar为例，描述Arange高阶API内部算法框图，如下图所示。

**图1**  Arange算法框图  
![](../../../figures/arange_algorithm_diagram.png "Arange算法框图")

计算过程分为如下几步，均在Vector上进行：

1.  等差数列长度8以内步骤：按照firstValue和diffValue的值，使用SetValue实现等差数列扩充，扩充长度最大为8，如果等差数列长度小于8，算法结束；
2.  等差数列长度8至64的步骤：对第一步中的等差数列结果使用Adds进行扩充，最大循环7次扩充至64，如果等差数列长度小于64，算法结束；
3.  等差数列长度64以上的步骤：对第二步中的等差数列结果使用Adds进行扩充，不断循环直至达到等差数列长度为止。

## 函数原型

```
template <typename T>
__aicore__ inline void Arange(const LocalTensor<T>& dst, const T firstValue, const T diffValue, const int32_t count)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。不同型号支持的数据类型请参考[支持的数据类型](#li73338460199)。 |

**表2**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst | 输出 | 目的操作数。dst的大小应大于等于count * sizeof(T)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| firstValue | 输入 | 等差数列的首个元素值。 |
| diffValue | 输入 | 等差数列元素之间的差值，应大于等于0。 |
| count | 输入 | 等差数列的长度。count>0。 |

## 返回值说明

无

## 约束说明

-   当前仅支持ND格式的输入，不支持其他格式。
-   支持的数据类型<a id="li73338460199"></a>

    <!-- npu="950" id7 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：int16\_t、half、int32\_t、float、int64\_t。
    <!-- end id7 -->
    <!-- npu="A3" id8 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int16\_t、half、int32\_t、float。
    <!-- end id8 -->
    <!-- npu="910b" id9 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int16\_t、half、int32\_t、float。
    <!-- end id9 -->
    <!-- npu="310p" id10 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：int16\_t、half、int32\_t、float。
    <!-- end id10 -->

## 调用示例

完整样例请参考[Arange样例](../../../../../../examples/01_simd_cpp_api/04_advanced_api/06_index/arange)。

```
// dst：输出Tensor
// firstValue_：等差数列首个元素值
// diffValue_：等差数列的公差
// count_：等差数列的长度
AscendC::Arange<T>(dst, static_cast<T>(firstValue_), static_cast<T>(diffValue_), count_);
```
