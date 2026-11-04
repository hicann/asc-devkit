# GatherB<a name="ZH-CN_TOPIC_0000001955999929"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:36:31.513Z pushedAt=2026-10-26T07:41:44.566Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 Inference Product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/GatherB_discrete_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

This instruction collects the source operand into the destination operand **dstReg** by **DataBlock** (32B) according to the index value **index**. The collection process is shown in Figure 1:

**Figure 1** GatherB description

![Figure 1 GatherB description](../../../../figures/reg_gatherb.png)

Among them, only the first 8 elements in **index** are valid, and each element corresponds to one **DataBlock**. For example, the first element is 96 (3 * 32), which means selecting **DataBlock** 3 and writing it to the corresponding position in **dstReg**.

The following Python code is used to equivalently describe the data selection process of GatherB. Each element in the **baseAddr** list represents one **DataBlock**, and the elements in **index** represent the byte offset relative to **baseAddr**.

```python
def gather_b(base_addr, index):
    data_block_size = 32
    dst_reg = []
    for offset in index[:8]:
        data_block_id = offset // data_block_size
        dst_reg.append(base_addr[data_block_id])
    return dst_reg
```

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void GatherB(U& dstReg, __ubuf__ T* baseAddr, S& index, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
|-----|-----|
| T | Data type of the destination operand and source operand. For supported data types, see [Data Type](#data-type). |
| U | RegTensor type of the destination operand. For example, RegTensor\<half>, which is automatically deduced by the compiler and does not need to be filled in by the user. |
| S | RegTensor type of the index value. For example, RegTensor\<uint32_t>, which is automatically deduced by the compiler and does not need to be filled in by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
|-----|-----|-----|
| dstReg | Output | Destination operand. Type: [RegTensor](../register_data_types/RegTensor.md).|
| baseAddr | Input | Source operand, the base address in the UB, which requires 32-byte alignment. |
| index | Input | Index value, the position of each DataBlock in **dstReg** relative to **baseAddr** in the Unified Buffer (UB). Only the first 8 elements are valid. Unit: byte. Type: [RegTensor](../register_data_types/RegTensor.md). The index value must be 32B-aligned, that is, one index value corresponds to one DataBlock. Values in **index** can be repeated. For example:<br>baseAddr: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock7, ... , DataBlock32, ...].<br>index: [0\*32, 1\*32, 2\*32, 3\*32, 4\*32, 5\*32, 6\*32, 32\*32]<br>dstReg: [DataBlock0, DataBlock1, DataBlock2, DataBlock3, DataBlock4, DataBlock5, DataBlock6, DataBlock32]. |
| mask | Input | Valid indicator for DataBlock transfer, interpreted in b32 format. One DataBlock corresponds to 4 bits, and only the lowest bit of each 4 bits is valid. Since only the first 8 elements of **index** are valid, **mask** uses only bits 0, 4, 8, 12, 16, 20, 24, and 28 corresponding to the first 8 b32 elements to control whether DataBlock0 to DataBlock7 in **dstReg** are updated, and the remaining bits are invalid. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. The supported data types are b8, b16, b32, and b64.

The supported data type of the index value is uint32_t.

## Return Value

None

## Constraints<a name="section177921451558"></a>

- The address in **UB** must be 32-byte aligned.
- The source operand and destination operand must have the same data type.
- The **index** values must be 32-byte aligned, that is, one index value corresponds to one **DataBlock**.
- The data corresponding to the **index** values must be within the valid address range of **UB**.
- The **regTrait** template parameter of **RegTensor** supports only **RegTraitNumOne**.
- Only the first 8 elements of **index** are valid.

## Example<a name="section642mcpsimp"></a>

```cpp
template <typename T>
__simd_vf__ inline void GatherBVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, __ubuf__ uint32_t* indexAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::RegTensor<uint32_t> indexReg;
    AscendC::Reg::MaskReg calMask = AscendC::Reg::CreateMask<uint32_t, AscendC::Reg::MaskPattern::VL8>();
    AscendC::Reg::MaskReg storeMask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign(indexReg, indexAddr + i * oneRepeatSize);
        AscendC::Reg::GatherB(dstReg, srcAddr, indexReg, calMask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, storeMask);
    }
}
```
