# MaskReg搬入（MaskGenWithRegTensor）

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

头文件路径为：`"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`。

MaskGenWithRegTensor能够实现数据从[RegTensor](../寄存器数据类型/RegTensor.md)搬运至[MaskReg](../寄存器数据类型/MaskReg.md)。

- 数据类型为b16时，RegTensor（256B）分成16个数据块（16B），用户通过参数可选择某一个数据块，数据块的每个bit broadcast成2bit后变为32B，即输出的MaskReg。
- 数据类型为b32时，RegTensor（256B）分成32个数据块（8B），用户通过参数可选择某一个数据块，数据块的每个bit broadcast成4bit后变为32B，即输出的MaskReg。

具体搬运原理请参考[关键特性说明](#关键特性说明)。

## 函数原型

```cpp
template <typename T = DefaultType, int16_t offset, typename U>
__simd_callee__ inline void MaskGenWithRegTensor(MaskReg& dst, U& srcReg)
```

## 参数说明

**表 1**  模板参数说明

| 参数名 | 描述 |
|-----|-----|
| T | 源操作数的数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |
| offset | offset决定了srcReg中需要搬运的数据块，具体描述请参考[关键特性说明](#关键特性说明)。<br>&bull; 当数据类型为b16，offset∈[0, 15]。<br>&bull; 当数据类型为b32，offset∈[0, 31]。 |
| U | 源操作数的RegTensor类型。例如RegTensor\<half>，由编译器自动推导，用户不需要手动填写。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
|-----|-----|-----|
| dst | 输出 | 目的操作数，类型为[MaskReg](../寄存器数据类型/MaskReg.md)。 |
| srcReg | 输入 | 源操作数，类型为[RegTensor](../寄存器数据类型/RegTensor.md)。 |

## 数据类型

源操作数支持的数据类型为：b16、b32。

## 返回值说明

无

## 关键特性说明

由[MaskReg原理](../寄存器数据类型/MaskReg.md)可知，当操作数类型为b16时，每一个element对应2bit的MaskReg，且仅2bit中的最低位是有效的；当操作数类型为b32时，每一个element对应4bit的MaskReg，且仅4bit中的最低位是有效的。

MaskGenWithRegTensor支持b16和b32，搬运原理如[图1](#fig-maskgen-b16)、[图2](#fig-maskgen-b32)所示。

- 当操作数类型为b16时，RegTensor的一个数据块大小为16B，每个bit经过broadcast成2bit后变为32B，即一个MaskReg（32B）。此外，一个RegTensor有256B/16B = 16个数据块，偏移量offset取值范围为[0, 15]。
- 当操作数类型为b32时，RegTensor的一个数据块大小为8B，每个bit经过broadcast成4bit后变为32B，即一个MaskReg（32B）。此外，一个RegTensor有256B/8B = 32个数据块，偏移量offset取值范围为[0, 31]。

**图 1**  MaskGenWithRegTensor搬运原理（b16）<a id="fig-maskgen-b16"></a>  
![](../../../../figures/reg_maskgenwithregtensor_b16.png "MaskGenWithRegTensor搬运原理（b16）")

**图 2**  MaskGenWithRegTensor搬运原理（b32）<a id="fig-maskgen-b32"></a>  
![](../../../../figures/reg_maskgenwithregtensor_b32.png "MaskGenWithRegTensor搬运原理（b32）")

## 约束说明

- 当数据类型为b16，offset∈[0, 15]；当数据类型为b32，offset∈[0, 31]。

## 调用示例

```cpp
template <typename T, int16_t offset>
__simd_vf__ inline void MaskGenWithRegTensorVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::LoadAlign(srcReg, srcAddr);
    AscendC::Reg::MaskGenWithRegTensor<T, offset>(mask, srcReg);
    AscendC::Reg::StoreAlign(dstAddr, mask);
}
```

