# MaskReg Move-In (MaskGenWithRegTensor)

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T12:58:03.774Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"`.

**MaskGenWithRegTensor** transfers data from [RegTensor](../register_data_types/RegTensor.md) to [MaskReg](../register_data_types/MaskReg.md).

- When the data type is b16, RegTensor (256B) is divided into 16 data blocks (16B). You can select a data block through a parameter. Each bit of the data block is broadcast into 2 bits to become 32B, which is the output MaskReg.
- When the data type is b32, RegTensor (256B) is divided into 32 data blocks (8B). You can select a data block through a parameter. Each bit of the data block is broadcast into 4 bits to become 32B, which is the output MaskReg.

For the specific data transfer principle, see [Key Features](#key-features).

## Prototype

```cpp
template <typename T = DefaultType, int16_t offset, typename U>
__simd_callee__ inline void MaskGenWithRegTensor(MaskReg& dst, U& srcReg)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the source operand. For supported data types, see [Data Types](#data-type). |
| offset | **offset** determines the data blocks to be transferred from **srcReg**. For details, see [Key Features](#key-features).<br>&bull; When the data type is b16, offset∈[0, 15].<br>&bull; When the data type is b32, offset∈[0, 31]. |
| U | RegTensor type of the source operand, for example, RegTensor\<half>. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dst | Output | Destination operand, of type [MaskReg](../register_data_types/MaskReg.md). |
| srcReg | Input | Source operand, of type [RegTensor](../register_data_types/RegTensor.md). |

## Data Type

The source operand supports the following data types: **b16**, **b32**.

## Return Value

None

## Key Features

According to [MaskReg principle](../register_data_types/MaskReg.md), when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the lowest bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the lowest bit of the 4 bits is valid.

**MaskGenWithRegTensor** supports b16 and b32. The data transfer principle is shown in [Figure 1](#fig-maskgen-b16) and [Figure 2](#fig-maskgen-b32).

- When the operand type is b16, one data block of **RegTensor** is 16B in size. After each bit is broadcast into 2 bits, it becomes 32B, that is, one **MaskReg** (32B). In addition, one **RegTensor** has 256B/16B = 16 data blocks, and the offset value range is [0, 15].
- When the operand type is b32, one data block of **RegTensor** is 8B in size. After each bit is broadcast into 4 bits, it becomes 32B, that is, one **MaskReg** (32B). In addition, one **RegTensor** has 256B/8B = 32 data blocks, and the offset value range is [0, 31].

**Figure 1**  MaskGenWithRegTensor data transfer principle (b16)<a id="fig-maskgen-b16"></a>  
![](../../../../figures/reg_maskgenwithregtensor_b16.png "MaskGenWithRegTensor data transfer principle (b16)")

**Figure 2**  MaskGenWithRegTensor data transfer principle (b32)<a id="fig-maskgen-b32"></a>  
![](../../../../figures/reg_maskgenwithregtensor_b32.png "MaskGenWithRegTensor data transfer principle (b32)")

## Constraints

- When the data type is **b16**, **offset**∈[0, 15]; when the data type is **b32**, **offset**∈[0, 31].

## Example

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

