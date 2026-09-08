# Ndtri

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->

## 功能说明

Ndtri当前属于实验性接口。使用前需要定义`CANN_ASC_USE_EXPERIMENTAL`，并显式包含头文件：

```cpp
#define CANN_ASC_USE_EXPERIMENTAL
#include "adv_api/experimental/ndtri.h"
```

按元素计算标准正态分布累积分布函数的反函数。对于输入概率p，输出x满足：

$$\Phi(x)=p$$

输入数据取值及对应输出如下：

| 输入p | 输出x |
| --- | --- |
| 0 | 负无穷 |
| 1 | 正无穷 |
| (0, 1) | 满足$\Phi(x)=p$的值 |
| 小于0、大于1、NaN或正负无穷 | NaN |

## 函数原型

- 指定参与计算的元素个数

    ```cpp
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Ndtri(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor,
        const uint32_t calCount)
    ```

- 计算整个源Tensor

    ```cpp
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Ndtri(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
    ```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型，支持的数据类型为float。 |
| isReuseSource | 源操作数复用预留参数。当前实现不使用该参数，请保持默认值false。 |

**表2** 接口参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 只读源操作数，元素值表示概率。<br><br>类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。数据类型需要与dstTensor保持一致。输入输出地址分离时，接口不会修改该Tensor中的数据。 |
| calCount | 输入 | 参与计算的元素个数。取值范围为[0, min(srcTensor.GetSize(), dstTensor.GetSize())]。取值为0时接口直接返回，不读取srcTensor、不写入dstTensor。不传入该参数时，计算元素数为srcTensor.GetSize()。 |

## 返回值说明

无

## 约束说明

- srcTensor和dstTensor支持完全同址原地计算，不支持部分地址重叠。
- 操作数地址对齐要求遵循通用地址对齐约束。

## 调用示例

```cpp
// dstLocal：存放计算结果的Tensor
// srcLocal：存放输入概率的Tensor
// 计算1024个float元素
AscendC::experimental::Ndtri<float, false>(dstLocal, srcLocal, 1024);
// 计算srcLocal中的全部元素
AscendC::experimental::Ndtri<float, false>(dstLocal, srcLocal);
```
