# 离散搬出（Scatter）<a name="ZH-CN_TOPIC_0000001929080422"></a>

## 产品支持情况<a name="section1550532418810"></a>

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

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`。

该指令会根据索引值index将源操作数srcReg中的元素分散到目的操作数UB中。分散过程如图1所示：

**图 1**  Scatter功能说明

![图1 Scatter功能说明](../../../../figures/reg_scatter.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U = DefaultType, typename S, typename V>
__simd_callee__ inline void Scatter(__ubuf__ T* baseAddr, S& srcReg, V& index, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 索引值index的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| S | 源操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |
| V | 索引值的RegTensor类型，例如RegTensor\<uint16_t>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| baseAddr | 输出 | 目的操作数，UB中的基地址，需要32字节对齐。 |
| srcReg | 输入 | 源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |
| index | 输入 | 索引值，srcReg中的每个元素在UB中相对于baseAddr的位置，单位：元素个数。类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 <br>注：index中的值必须唯一。若存在重复的index值，系统仅保留其中一个对应的数据，其余将被忽略。无法确定具体保留哪一个，因此必须确保index值不重复。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 数据类型

**表 3**  Scatter操作数数据类型对应表

| 目的操作数 | 索引值 |
|-----|-----|
| int8_t | uint16_t |
| uint8_t | uint16_t |
| int16_t | uint16_t |
| uint16_t | uint16_t |
| half | uint16_t |
| bfloat16_t | uint16_t |
| int32_t | uint32_t |
| uint32_t | uint32_t |
| float | uint32_t |
| int64_t | uint32_t |
| int64_t | uint64_t |
| uint64_t | uint32_t |
| uint64_t | uint64_t |

## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

- 位于Unified Buffer的首地址必须32B对齐。
- 当T为int8_t或者uint8_t数据类型时，源操作数中仅偶数位元素有效。即srcReg中的偶数位置[0, 2, 4, ..., 252, 254]的数据会被分散存储到目的操作数中。
- index中的值必须唯一。若存在重复的index值，系统仅保留其中一个对应的数据，其余将被忽略。无法确定具体保留哪一个，因此必须确保index值不重复。
- 当目的操作数的数据类型T为B64时，T、U、S、V只支持以下组合：

    | T | U | S（自动推导） | V（自动推导） | 备注 |
    |-----|-----|-----|-----|-----|
    | int64_t | uint32_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint32_t> | index的前32个数有效 |
    | int64_t | uint32_t | RegTensor<int64_t, RegTraitNumTwo> | RegTensor<uint32_t> | - |
    | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumOne> | - |
    | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumTwo> | index的前32个数有效 |
    | int64_t | uint64_t | RegTensor<int64_t, RegTraitNumTwo> | RegTensor<uint64_t, RegTraitNumTwo> | - |
    | uint64_t | uint32_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint32_t> | index的前32个数有效 |
    | uint64_t | uint32_t | RegTensor<uint64_t, RegTraitNumTwo> | RegTensor<uint32_t> | - |
    | uint64_t | uint64_t |  RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumOne> | - |
    | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumOne> | RegTensor<uint64_t, RegTraitNumTwo> | index的前32个数有效 |
    | uint64_t | uint64_t | RegTensor<uint64_t, RegTraitNumTwo> | RegTensor<uint64_t, RegTraitNumTwo> | - |

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template <typename T, typename U>
__simd_vf__ inline void ScatterVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ U* indexAddr, uint32_t count, uint16_t oneRepeatSize)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<U> indexReg;
    AscendC::Reg::MaskReg mask;
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::Scatter(dstAddr, srcReg, indexReg, mask);
    }
}
```

