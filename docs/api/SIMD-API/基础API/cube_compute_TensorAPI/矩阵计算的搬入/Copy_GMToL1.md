# Copy（Global Memory到L1 Buffer数据搬运）

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

Tensor API通过`Copy`接口统一执行不同通路数据搬运。该接口用于将Global Memory中的数据搬运到L1 Buffer。`Copy`接口根据源张量和目的张量的存储位置、数据类型和Layout选择具体搬运实现。搬运块数、搬运长度、源/目的侧步长以及格式转换相关信息由Tensor Layout推导，用户不需要在`Copy`调用中额外传入搬运参数。

该通路用于矩阵计算输入搬入，支持以下场景：

- 连续数据搬入：源数据从Global Memory搬入L1 Buffer后数据格式不变。
- 高维切分数据搬入：源/目的侧存在步长时，根据Layout推导多段搬运。
- 随路格式转换搬入：从Global Memory搬入L1 Buffer的同时完成ND/DN等到NZ/ZN等的矩阵格式转换。
- Batch多矩阵搬入：源/目的Layout带Batch轴时，一次`Copy`完成多个矩阵的搬运，支持`ND2ND`、`ND2NZ`、`DN2NZ`、`ND2ZN`、`DN2ZN`、`ScaleAND2ZZ`、`ScaleADN2ZZ`、`ZZ2ZZ`、`ScaleBND2NN`、`ScaleBDN2NN`、`NN2NN`随路格式转换。
- Scale数据搬入：用于MX矩阵计算中ScaleA/ScaleB数据从Global Memory搬入L1 Buffer。

## 函数原型

- 执行Global Memory到L1 Buffer搬运。

    ```cpp
    template <typename AtomType, typename DstTensor, typename SrcTensor>
    __aicore__ inline void Copy(const CopyAtom<AtomType>& atomCopy,
        const DstTensor& dst, const SrcTensor& src)
    ```

- 使用默认trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperationType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation)
    ```

- 使用指定trait构造搬运原子对象。

    ```cpp
    template <typename CopyOperationType, typename CopyTraitType>
    __aicore__ inline constexpr auto MakeCopy(const CopyOperationType& copyOperation,
        const CopyTraitType& copyTrait)
    ```

Global Memory到L1 Buffer通路使用`CopyGM2L1{}`作为`copyOperation`，使用`CopyGM2L1TraitDefault{}`作为默认trait对象。

## 参数说明

**模板参数说明**

| 参数名 | 描述 |
| :--- | :--- |
| AtomType | `CopyAtom`的模板参数，由`MakeCopy(CopyGM2L1{})`或`MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{})`推导得到。 |
| DstTensor | 目的张量类型。Global Memory到L1 Buffer搬运时需为Tensor API Tensor类型，存储位置为`Location::L1`。 |
| SrcTensor | 源张量类型。Global Memory到L1 Buffer搬运时需为Tensor API Tensor类型，存储位置为`Location::GM`。 |
| CopyOperationType | 搬运操作对象类型。Global Memory到L1 Buffer搬运取`CopyGM2L1`。 |
| CopyTraitType | 搬运trait对象类型。Global Memory到L1 Buffer默认取`CopyGM2L1TraitDefault`。 |

**函数参数说明**

**表1**  `Copy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| atomCopy | 输入 | 搬运原子对象。Global Memory到L1 Buffer搬运可通过`MakeCopy(CopyGM2L1{})`或`MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{})`构造。 |
| dst | 输出 | 目的张量，存储位置为`Location::L1`。 |
| src | 输入 | 源张量，存储位置为`Location::GM`。 |

**表2**  `MakeCopy`接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| copyOperation | 输入 | 搬运操作对象。Global Memory到L1 Buffer搬运取`CopyGM2L1{}`。 |
| copyTrait | 输入 | 搬运trait对象。Global Memory到L1 Buffer默认取`CopyGM2L1TraitDefault{}`。 |

## 数据类型

源张量和目的张量的数据类型需要保持一致。

| 源操作数数据格式 | 目的操作数数据格式 | 源操作数/目的操作数数据类型 |
| -- | -- | -- |
| ND | ND | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ND | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| ND | ZN | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | NZ | int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| DN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float |
| NZ | NZ | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ZN | ZN | fp4x2_e2m1_t/fp4x2_e1m2_t/int8_t/uint8_t/hifloat8_t/fp8_e5m2_t/fp8_e4m3fn_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/int64_t/uint64_t |
| ScaleAND | ZZ | fp8_e8m0_t |
| ScaleADN | ZZ | fp8_e8m0_t |
| ZZ | ZZ | fp8_e8m0_t |
| ScaleBND | NN | fp8_e8m0_t |
| ScaleBDN | NN | fp8_e8m0_t |
| NN | NN | fp8_e8m0_t |

## 返回值说明

`Copy`无返回值。`MakeCopy`返回`CopyAtom`对象。

## 约束说明

- 位于Global Memory的源地址必须1B对齐，位于L1 Buffer的目的地址必须32B对齐。
- 数据连续搬运场景中，搬运字节数需要32B对齐。若未对齐，搬运量会向下取整到32B对齐。
- 高维切分搬运场景中，搬运长度、源步长和目的步长均按32B对齐。
- 当输入数据是b4类型时，按b8类型搬运粒度处理，Layout推导参数需要满足对应粒度约束。
- Batch多矩阵搬入仅支持`ND2ND`、`ND2NZ`、`DN2NZ`、`ND2ZN`、`DN2ZN`、`ScaleAND2ZZ`、`ScaleADN2ZZ`、`ZZ2ZZ`、`ScaleBND2NN`、`ScaleBDN2NN`、`NN2NN`格式转换；Batch数受搬运指令字段范围限制，需不大于4095。

## 关键特性说明

### 随路格式转换搬入

随路格式转换搬入表示从Global Memory搬入L1 Buffer时同步完成矩阵格式转换。接口根据源Layout和目的Layout自动选择转换方向，支持以下格式转换：
`ND2ND`、`ND2NZ`、`ND2ZN`、`DN2NZ`、`DN2ZN`、`NZ2NZ`、`ZN2ZN`。
ND到NZ的格式转换等价于DN到ZN的格式转换，DN到NZ的格式转换等价于ND到ZN的格式转换。

#### ND2NZ连续搬运

ND2NZ连续搬运表示源矩阵按ND格式连续排布，目的矩阵按NZ格式连续排布。该场景由Layout自动推导单段搬运参数，搬运过程中完成ND到NZ的格式转换，目的端按32B对齐补0。

#### ND2NZ非连续搬运

ND2NZ非连续搬运表示源侧或目的侧存在步长、切分维度等高维切分信息。该场景归属于高维切分数据搬入，`Copy`会根据Layout自动推导多段搬运参数，搬运过程中完成ND到NZ的格式转换，目的端按32B对齐补0。

#### DN2NZ连续搬运

DN2NZ连续搬运表示源矩阵按DN格式连续排布，目的矩阵按NZ格式连续排布。该场景由Layout自动推导单段搬运参数，搬运过程中完成DN到NZ的排布转换，目的端按32B对齐补0。

#### DN2NZ非连续搬运

DN2NZ非连续搬运表示源侧或目的侧存在步长、切分维度等高维切分信息。该场景归属于高维切分数据搬入，`Copy`会根据Layout自动推导多段搬运参数，搬运过程中完成DN到NZ的格式转换，目的端按32B对齐补0。

### Scale数据搬入

Scale数据搬入用于矩阵计算中scale相关数据从Global Memory搬入L1 Buffer。A矩阵相关scale支持`ScaleAND`到`ZZ`、`ScaleADN`到`ZZ`、`ZZ`到`ZZ`的格式转换。B矩阵相关scale支持`ScaleBND`到`NN`、`ScaleBDN`到`NN`、`NN`到`NN`的格式转换。

### Batch多矩阵搬入

Batch多矩阵搬入表示一次`Copy`完成多个矩阵的搬运。当源张量和目的张量的Layout带有Batch轴时，`Copy`一条指令完成多个矩阵的搬运，接口调用方式与单矩阵搬运完全一致，无需额外传入Batch相关参数。

带Batch轴的Layout在原矩阵Layout外层增加一维Batch，Batch内存连续场景可通过`MakeFrameLayout`构造，Batch内存非连续场景可通过`MakeLayout`显式指定Shape和Stride构造。

例如Batch内存连续场景通过`MakeFrameLayout<LayoutPattern>(B, M, K)`构造，其中`B`为Batch数，`M`、`K`为单个矩阵的行、列。Layout语义为：

- Shape为`(B, (M, K))`。
- Stride为`(M * K, (K, 1))`，即各矩阵在内存上按`M * K`连续排布，单矩阵内存按照ND格式连续排布。

Batch内存非连续场景可通过`MakeLayout(shape, stride)`构造，其中`shape`描述Batch数和单个矩阵的Shape，`stride`描述Batch轴和矩阵内部各轴的地址间隔。以按K方向跨Batch连续排布的ND矩阵为例，Layout语义为：

- Shape为`(B, (M, K))`。
- Stride为`(K, (B * K, 1))`，即各矩阵在内存上为非连续，内存跨Batch在K方向上连续排布。

搬运行为：

- 源（Global Memory）侧：各Batch矩阵的源地址间距由Layout的Batch轴Stride决定，借助Batch轴Stride参数，Batch数据支持Batch间连续和非连续两种排布形式。
- 目的（L1 Buffer）侧：各Batch矩阵在L1 Buffer中连续存放，Batch间的地址间隔等于对齐后单个矩阵的占用空间。

## 调用示例

以下示例将Global Memory中的ND矩阵搬入L1 Buffer，并转换为NZ格式。

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t M = 32;
constexpr uint32_t K = 64;

__aicore__ inline void CopyGmToL1(__gm__ half* gmAddr)
{
    __cbuf__ half l1Buf[M * K];

    // 源张量：Global Memory上的ND格式矩阵；目的张量：L1 Buffer上的NZ格式矩阵
    auto gmA = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(M, K));
    auto l1A = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, half>(M, K));

    // 构造GM到L1搬运原子对象，Copy根据源/目的Layout自动完成ND到NZ的随路格式转换
    auto copyGm2L1 = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
    Copy(copyGm2L1, l1A, gmA);
}
```

以下示例将Global Memory中的B个ND矩阵一次性搬入L1 Buffer，并转换为NZ格式。源张量和目的张量均使用带Batch轴的Layout，接口调用方式与单矩阵搬运一致。

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

constexpr uint32_t B = 4;
constexpr uint32_t M = 32;
constexpr uint32_t K = 64;

__aicore__ inline void CopyBatchGmToL1(__gm__ half* gmAddr)
{
    __cbuf__ half l1Buf[B * M * K];

    // 构造带Batch轴的源/目的Layout（B个矩阵），MakeFrameLayout在原Layout外层增加一维Batch
    auto gmA = MakeTensor(MakeMemPtr(gmAddr), MakeFrameLayout<NDExtLayoutPtn>(B, M, K));
    auto l1A = MakeTensor(MakeMemPtr(l1Buf), MakeFrameLayout<NZLayoutPtn, half>(B, M, K));

    // 调用方式与单矩阵一致，Copy一条指令完成B个矩阵的ND到NZ搬运
    auto copyGm2L1 = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
    Copy(copyGm2L1, l1A, gmA);
}
```
