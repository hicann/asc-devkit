# Copy（L0C Buffer到Global Memory数据搬运）

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

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将L0C Buffer中的矩阵计算结果搬运到Global Memory。L0C Buffer中的数据通常为`Mmad`的输出，数据格式为`NZ`。搬运到Global Memory时，接口会根据目的张量布局自动选择`NZ`到`ND`、`NZ`到`DN`或`NZ`到`NZ`的随路格式转换。

L0C Buffer到Global Memory搬运支持不量化输出、`float`到`half`或`bfloat16_t`的直接转换输出，以及配合标量或张量量化参数的随路量化输出。随路Relu、通道拆分和量化舍入方式通过`CopyL0C2GMTrait`配置。`Mmad`与`Fixpipe`细粒度并行相关的`unitFlag`通过`FixpipeParams`配置。

接口支持batch模式。batch模式用于一次完成多块矩阵计算结果的搬运。Layout在原矩阵Layout最外层增加Batch维度。源张量为`NZ`格式，分形固定为16×16，可通过`MakeFrameLayout<NZLayoutPtn>(batch, m, n)`构造。目的张量可通过`MakeFrameLayout<NDLayoutPtn>(batch, m, n)`、`MakeFrameLayout<DNLayoutPtn>(batch, m, n)`、`MakeFrameLayout<NDExtLayoutPtn>(batch, m, n)`、`MakeFrameLayout<DNExtLayoutPtn>(batch, m, n)`或`MakeFrameLayout<NZLayoutPtn, DstType>(batch, m, n)`构造。`NZ`格式可通过模板参数`DstType`指定目的数据类型，`C0`表示NZ格式的列分形大小，默认为16。

随路量化、随路Relu、随路格式转换、随路通道拆分以及随路通道合并的有效组合、中间数据类型和数据路径如下图所示。图中的F32到F16、F32到BF16为非量化模式，仅进行cast。其余路径为不量化模式、随路scalar量化模式或随路tensor量化模式。

针对Ascend 950PR/Ascend 950DT：

**图1**  L0C2GM流程图

![L0C2GM流程图](../../../../figures/L0C2GM_Function_Combination_950.png)

## 函数原型

- 执行L0C Buffer到Global Memory的非量化搬运。

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src)
    ```

- 执行L0C Buffer到Global Memory的量化搬运。

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor, typename QuantParam,
        Std::enable_if_t<IsCopyQuantParamV<QuantParam>, int> Enable>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy, const DstTensor& dst, const SrcTensor& src,
        const QuantParam& quant)
    ```

- 使用默认trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- 使用指定trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation, const CopyTraitType& copyTrait)
    ```

## 参数说明

**表1**  `Copy`接口参数说明

|参数名|输入/输出|描述|
|--------|--------|--------|
|atomCopy|输入|搬运原子对象，可由`MakeCopy(CopyL0C2GM{})`或`MakeCopy(CopyL0C2GM{}, CopyL0C2GMTraitDefault{})`构造。|
|dst|输出|目的张量，存储位置为`Location::GM`。支持`ND`、`DN`和`NZ`数据格式。|
|src|输入|源张量，存储位置为`Location::L0C`，数据格式为`NZ`，通常为`Mmad`的计算结果。|
|quant|输入|可选量化参数。传入`uint64_t`时表示scalar量化参数，传入张量时表示tensor量化参数，张量位于L1 Buffer，元素类型为`uint64_t`。|
|fixpipeParams|输入|可选搬运参数，类型为`FixpipeParams`，通过`atomCopy`的`with`接口绑定到搬运原子对象，未绑定时使用默认值。|

**表2**  `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| copyOperation | 输入 | 搬运操作对象。L0C Buffer到Global Memory搬运采用`CopyL0C2GM{}`。 |
| copyTrait | 输入 | 搬运静态特性对象。L0C Buffer到Global Memory搬运默认采用`CopyL0C2GMTraitDefault{}`。 |

### CopyL0C2GMTrait说明

`CopyL0C2GMTrait`用于配置L0C Buffer到Global Memory搬运的静态特性。

```cpp
struct CopyL0C2GMTrait {
    RoundMode roundMode = RoundMode::DEFAULT;
    bool enableRelu = false;
    bool enableChannelSplit = false;
};
```

**表3**  `CopyL0C2GMTrait`成员说明

|成员|默认值|描述|
|--------|--------|--------|
|roundMode|`RoundMode::DEFAULT`|舍入模式。`RoundMode::HYBRID`仅在源类型为`float`、目的类型为`hifloat8_t`的量化输出场景支持。|
|enableRelu|`false`|是否使能随路Relu。|
|enableChannelSplit|`false`|是否使能输出数据通道拆分。只支持NZ格式且源类型和目的类型均为`float`的场景。|

使用自定义trait的示例：

```cpp
constexpr CopyL0C2GMTrait l0c2gmTrait = {
    RoundMode::DEFAULT,
    false,
    true
};

struct CopyL0C2GMTraitCustom {
    using TraitType = CopyL0C2GMTrait;
    static constexpr const TraitType value = l0c2gmTrait;
};
```

### FixpipeParams说明

`FixpipeParams`用于配置运行时搬运参数。

```cpp
struct FixpipeParams {
    uint8_t unitFlag = 0;
    bool subBlockId = false;
};
```

**表4**  `FixpipeParams`成员说明

|成员|默认值|描述|
|--------|--------|--------|
|unitFlag|`0`|控制Mmad指令和Fixpipe指令的细粒度并行。`0`表示不使能，`2`表示使能且执行后不复位单元标记位，`3`表示使能且执行后复位单元标记位。开启时，`MmadParams`和`FixpipeParams`中的`unitFlag`需要配套设置为`2`或`3`。|
|subBlockId|`false`|在启用单目标模式时指示目标UB的编号，L0C Buffer到Global Memory数据搬运时该参数无效。|

## 数据类型

L0C Buffer到Global Memory搬运根据是否传入量化参数自动选择量化模式。

**表5**  数据类型说明

|源张量类型（L0C Buffer）|目的张量类型（Global Memory）|调用形式|说明|
|--------|--------|--------|--------|
|`int32_t`|`int8_t`、`uint8_t`|`Copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`float`|`int8_t`、`uint8_t`|`Copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`float`|`fp8_e4m3fn_t`、`hifloat8_t`|`Copy(atom, dst, src, quant)`|scalar或tensor量化输出。`hifloat8_t`支持`RoundMode::DEFAULT`和`RoundMode::HYBRID`。|
|`int32_t`|`half`、`bfloat16_t`|`Copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`float`|`half`|`Copy(atom, dst, src)`|直接转换输出。|
|`float`|`bfloat16_t`|`Copy(atom, dst, src)`|直接转换输出。|
|`float`|`half`、`bfloat16_t`|`Copy(atom, dst, src, quant)`|scalar或tensor量化输出。|
|`int32_t`|`int32_t`|`Copy(atom, dst, src)`|不量化输出。|
|`float`|`float`|`Copy(atom, dst, src)`|不量化输出。|
|`float`|`float`|`Copy(atom, dst, src, quant)`|scalar或tensor量化输出。|

不传入`quant`时，接口选择不量化或直接转换模式，传入`uint64_t`时选择scalar量化模式，传入张量时选择tensor量化模式。

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 约束说明

- 源矩阵`NZ`格式，地址要求64字节对齐。目的矩阵`DN`或`ND`格式时，地址要求1字节对齐，`NZ`格式时，地址要求32字节对齐。
- 目的矩阵为`NZ`输出时，N方向大小需要为16的倍数；`b8`类型通道合并场景为32的倍数，`float`使能`enableChannelSplit`通道拆分场景为8的倍数。
- tensor量化参数张量应位于L1 Buffer，元素类型为`uint64_t`，地址要求32字节对齐。详情参见[随路量化](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)。
- 使能`unitFlag`时，需要配合`Mmad`同时使能。
- 使用`RoundMode::HYBRID`时，源类型必须为`float`，目的类型必须为`hifloat8_t`。
- `enableChannelSplit`仅在源类型和目的类型均为`float`，且目的格式为`NZ`时生效。详情参见[F32 Channel Split](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/F32-Channel-Split.md)。
- 通道合并特性硬件自动使能，不能通过参数配置。详情参见[Int8 Channel Merge](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/Int8-Channel-Merge.md)。

## 关键特性

L0C Buffer到Global Memory搬运涉及[随路量化](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路量化.md)、[随路Relu](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/随路ReLU.md)、[F32 Channel Split](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/F32-Channel-Split.md)、[Int8 Channel Merge](../../矩阵计算（ISASI）/矩阵搬出关键特性说明/Int8-Channel-Merge.md)和[batch搬运](../矩阵搬出关键特性说明/batch_copy.md)等关键特性。

## 调用示例

### 不量化输出

```cpp

#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToGM()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));

    FixpipeParams params;
    auto atom = MakeCopy(CopyL0C2GM{}).with(params);
    Copy(atom, gm, l0c);
}
```

### scalar量化输出

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;
__aicore__ inline void CopyL0CToGM()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));

    uint64_t quant = 1;
    auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
    Copy(atom, gm, l0c, quant);
}
```

### tensor量化输出

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyL0CToGM()
{
    auto l0c = MakeTensor(MakeMemPtr(l0cAddr), MakeFrameLayout<NZLayoutPtn, 16>(m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(m, n));
    auto quant = MakeTensor(MakeMemPtr(l1QuantAddr), MakeFrameLayout<NDExtLayoutPtn>(1, n));

    auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
    Copy(atom, gm, l0c, quant);
}
```

### batch模式输出

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

__aicore__ inline void CopyBatchL0CToGM(__gm__ half* gmAddr)
{
    constexpr uint32_t srcBatch = 3;
    constexpr uint32_t dstBatch = 9;
    constexpr uint32_t m = 32;
    constexpr uint32_t n = 64;

    __cc__ float l0cBuf[srcBatch * m * n];

    auto l0c = MakeTensor(MakeMemPtr(l0cBuf), MakeFrameLayout<NZLayoutPtn>(srcBatch, m, n));
    auto gm = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(dstBatch, m, n));

    auto atom = MakeCopy(CopyL0C2GM{}).with(FixpipeParams{});
    for (uint32_t i = 0; i < dstBatch / srcBatch; ++i) {
        auto gmSlice = gm.Slice(MakeCoord(i * srcBatch, MakeCoord(0, 0)), MakeShape(srcBatch, MakeShape(m, n)));
        Copy(atom, gmSlice, l0c);
    }
}
```
