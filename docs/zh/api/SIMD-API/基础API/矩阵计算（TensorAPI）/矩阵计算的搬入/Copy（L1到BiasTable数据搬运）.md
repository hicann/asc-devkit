# Copy（L1 Buffer到BiasTable Buffer数据搬运）

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

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L1 Buffer中的bias数据搬运到BiasTable Buffer，作为Mmad计算中的bias输入。

接口支持batch模式。batch模式用于一次完成多块bias数据的搬运。Layout在原Layout最外层增加Batch维度，可通过`MakeFrameLayout<NDLayoutPtn>(batch, 1, n)`或`MakeFrameLayout<NDExtLayoutPtn>(batch, 1, n)`构造。

## 函数原型

- 执行L1 Buffer到BiasTable Buffer的数据搬运。

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
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## 参数说明

**表1**  Copy接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atomCopy | 输入 | 搬运原子对象，L1 Buffer到BiasTable Buffer通路可通过`MakeCopy(CopyL12BT{})`或`MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{})`构造。 |
| dst | 输出 | 目的张量，存储位置为`Location::BIAS`，数据格式为ND。 |
| src | 输入 | 源张量，存储位置为`Location::L1`，数据格式为ND。 |

**表2**  MakeCopy接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| copyOperation | 输入 | 搬运操作对象，L1 Buffer到BiasTable Buffer通路使用`CopyL12BT{}`。 |
| copyTrait | 输入 | 搬运Trait对象，用于指定搬运特性。L1 Buffer到BiasTable Buffer通路默认Trait使用`CopyL12BTTraitDefault{}`。 |

## 数据类型

支持的目的张量和源张量的数据类型组合如下：

**表3**  支持的数据类型组合

| 目的张量数据类型 | 源张量数据类型 |
| :--- | :--- |
| `float` | `bfloat16_t` |
| `float` | `half` |
| `float` | `float` |
| `int32_t` | `int32_t` |

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 约束说明

- 源张量内存地址和大小需要满足32Byte对齐要求。
- 目的张量内存地址和大小需要满足64Byte对齐要求。
- 由于BiasTable Buffer大小为4KB，因此每次搬运最多4KB数据。
- batch模式要求源张量和目的张量Batch数量一致。
- batch模式下，如果Global Memory上的Shape大小非对齐，由于L1 Buffer和BiasTable Buffer上有数据对齐要求，源张量和目的张量列方向Shape大小必须按32Byte对齐。

## 调用示例

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL1ToBTExample()
{
    using SrcType = half;
    using DstType = float;
    constexpr uint32_t n = 16;

    __cbuf__ SrcType l1Buf[n];
    __biasbuf__ DstType btBuf[n];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto btTensor = MakeTensor(MakeMemPtr(btBuf), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto copyAtom = MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{});
    Copy(copyAtom, btTensor, l1Tensor);
}
```

batch模式调用示例：

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyBatchL1ToBTExample()
{
    using SrcType = half;
    using DstType = float;
    constexpr uint32_t batch = 2;
    constexpr uint32_t n = 15;

    // batch模式下源/目的张量列方向Shape需按C0_SIZE（32Byte）对齐，
    // 分别向上对齐到C0_ELEMENT<srcType> 和C0_ELEMENT<dstType>。
    constexpr uint32_t srcN = Std::ceil_align(n, Std::Int<C0_ELEMENT<SrcType>>{});
    constexpr uint32_t dstN = Std::ceil_align(n, Std::Int<C0_ELEMENT<DstType>>{});

    __cbuf__ SrcType l1Buf[batch * srcN];
    __biasbuf__ DstType btBuf[batch * dstN];

    auto l1Tensor = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NDExtLayoutPtn>(batch, 1, srcN));

    auto btTensor = MakeTensor(MakeMemPtr(btBuf), MakeFrameLayout<NDExtLayoutPtn>(batch, 1, dstN));

    auto copyAtom = MakeCopy(CopyL12BT{}, CopyL12BTTraitDefault{});
    Copy(copyAtom, btTensor, l1Tensor);
}
```
