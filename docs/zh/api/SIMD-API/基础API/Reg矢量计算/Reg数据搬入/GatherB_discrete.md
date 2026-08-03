# 离散搬入（GatherB）<a name="ZH-CN_TOPIC_0000001955999929"></a>

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

该指令会根据索引值index将源操作数按DataBlock（32B）收集到目的操作数dstReg中。收集过程如图1所示：

**图 1**  GatherB功能说明

![图1 GatherB功能说明](../../../../figures/reg_gatherb.png)

## 函数原型<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void GatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
```

## 参数说明<a name="section622mcpsimp"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 目的操作数和源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| U | 目的操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |
| S | 索引值的RegTensor类型，例如RegTensor\<uint32_t>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dstReg | 输出 | 目的操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。|
| baseAddr | 输入 | 源操作数，UB中的基地址，需要32字节对齐。 |
| index | 输入 | 索引值，dstReg中的每个DataBlock在UB中相对于baseAddr的位置，单位：字节。类型为[RegTensor](../寄存器数据类型/RegTensor.md)。索引值必须32B对齐，即一个索引值对应1个DataBlock。index中的值可以重复。例如：<br>baseAddr: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock7, ... , DataBlock32, ...]。<br>index: [0\*32, 1\*32, 2\*32, 3\*32, 4\*32, 5\*32, 6\*32, 32\*32]<br>dstReg: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock32]。 |
| mask | 输入 | 源操作数元素操作的有效指示，详细说明请参考[MaskReg](../寄存器数据类型/MaskReg.md)。 |

## 数据类型

目的操作数与源操作数的数据类型需要保持一致。支持的数据类型为：b8、b16、b32、b64。

索引值支持的数据类型为：uint32_t。

## 返回值说明

无

## 约束说明<a name="section177921451558"></a>

- 位于UB的地址必须32字节对齐。
- 源操作数和目的操作数数据类型必须相同。
- index索引值必须32字节对齐，即一个索引值对应1个DataBlock。
- index索引值对应的数据必须在UB有效地址范围内。
- RegTensor模板参数regTrait只支持RegTraitNumOne。

## 调用示例<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void GatherBVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ uint32_t* indexAddr, uint32_t count, uint16_t oneRepeatSize)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::RegTensor<uint32_t> indexReg;
    AscendC::Reg::MaskReg mask;
    uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::GatherB(dstReg, srcAddr, indexReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

