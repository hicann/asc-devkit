# Copy（L1 Buffer到L0B Buffer数据搬运）

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的右矩阵数据搬运到L0B Buffer。

该通路以512Byte数据分形为基本搬运单位。不同数据类型对应的分形矩阵形态如下：

| 数据位宽 | 分形矩阵形态 |
| :--- | :--- |
| b4 | 64 * 16 |
| b8 | 32 * 16 |
| b16 | 16 * 16 |
| b32 | 8 * 16 |

接口支持非转置搬运和转置搬运。源张量和目的张量的Layout需要匹配当前通路支持的格式组合。

接口支持Batch模式。Batch模式下，源张量和目的张量的Layout需要在原有分形Layout最前面增加Batch维，Shape形态为`(B, 单矩阵Shape)`，其中`B`表示Batch数量。用户可使用`MakeFrameLayout<ZNLayoutPtn, DataType>(B, k, n)`或`MakeFrameLayout<NZLayoutPtn, DataType>(B, k, n)`构造带Batch维的Layout。

## 函数原型

- 执行L1 Buffer到L0B Buffer的数据搬运。

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- 构造默认搬运原子对象。

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- 构造指定Trait的搬运原子对象。

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(
        const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## 参数说明

**表1**  `Copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atomCopy | 输入 | 搬运原子对象。L1 Buffer到L0B Buffer通路可通过`MakeCopy(CopyL12L0B{})`或`MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{})`构造。 |
| dst | 输出 | 目的张量，存储位置必须为`Location::L0B`。 |
| src | 输入 | 源张量，存储位置必须为`Location::L1`。 |

**表2**  `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| copyOperation | 输入 | 搬运操作对象。L1 Buffer到L0B Buffer通路使用`CopyL12L0B{}`。 |
| copyTrait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到L0B Buffer默认Trait使用`CopyL12L0BTraitDefault{}`。 |

## 数据类型

支持的数据类型包括：

`fp4x2_e2m1_t`、`fp4x2_e1m2_t`、`int8_t`、`uint8_t`、`hifloat8_t`、`fp8_e5m2_t`、`fp8_e4m3fn_t`、`half`、`bfloat16_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`float`。

源张量和目的张量的数据类型需要保持一致。

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 约束说明

- 目的地址位于L0B Buffer时，起始地址需要满足512Byte对齐要求。
- 源地址位于L1 Buffer时，起始地址需要满足32Byte对齐要求。
- 特殊数据类型约束：L1 Buffer到L0B Buffer通路使能转置时，即`NZ2ZN`，支持的数据类型约束如下：
  - b32数据类型要求源矩阵Shape在K轴方向16对齐。
  - b8数据类型要求源矩阵Shape在M轴方向32对齐。
  - b4数据类型要求源矩阵Shape在M轴方向64对齐。
- Batch模式要求源张量和目的张量的Batch数量一致，且在L1 Buffer和L0B Buffer上张量数据连续排布。

## 关键特性说明

### 非转置搬运

当源张量和目的张量数据格式均为`ZN`时，接口按非转置方式从L1 Buffer搬运到L0B Buffer。

**图1**  非转置搬运示意图

![非转置搬运示意图](../../../../figures/notranspose.png)

### 转置搬运

当源张量数据格式为`NZ`、目的张量数据格式为`ZN`时，接口按转置方式搬运。转置搬运会对源张量每个分形矩阵执行转置。

不同数据位宽的转置搬运示意如下。

#### b4转置搬运

**图2**  b4转置搬运示意图

![b4转置搬运示意图](../../../../figures/transpose_b4.png)

#### b8转置搬运

**图3**  b8转置搬运示意图

![b8转置搬运示意图](../../../../figures/transpose_b8.png)

#### b16转置搬运

**图4**  b16转置搬运示意图

![b16转置搬运示意图](../../../../figures/transpose_b16.png)

#### b32转置搬运

**图5**  b32转置搬运示意图

![b32转置搬运示意图](../../../../figures/transpose_b32.png)

### Batch搬运

当源张量和目的张量均使用带Batch维的Layout时，`Copy`接口会按照Batch维描述的数据范围完成L1 Buffer到L0B Buffer的数据搬运。非转置搬运时源张量、目的张量均为`ZN`格式，转置搬运时源张量为`NZ`格式，目的张量为`ZN`格式。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0BExample()
{
    using BType = half;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ BType l1Buf[k * n];
    __cb__ BType l0bBuf[k * n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<ZNLayoutPtn, BType>(k, n));

    auto l0bTensor = MakeTensor(MakeMemPtr(l0bBuf), MakeFrameLayout<ZNLayoutPtn, BType>(k, n));

    auto copyAtom = MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{});
    Copy(copyAtom, l0bTensor, l1Tensor);
}
```

Batch模式示例如下。

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToL0BBatchExample()
{
    using BType = half;
    constexpr uint32_t batch = 2;
    constexpr uint32_t k = 16;
    constexpr uint32_t n = 16;

    __cbuf__ BType l1Buf[batch * k * n];
    __cb__ BType l0bBuf[batch * k * n];

    auto layout = MakeFrameLayout<ZNLayoutPtn, BType>(batch, k, n);
    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), layout);
    auto l0bTensor = MakeTensor(MakeMemPtr(l0bBuf), layout);

    auto copyAtom = MakeCopy(CopyL12L0B{}, CopyL12L0BTraitDefault{});
    Copy(copyAtom, l0bTensor, l1Tensor);
}
```
