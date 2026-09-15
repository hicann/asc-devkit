# Move

<!-- md-trans-meta sourceCommit=b33e7946568512c317e0d6bdc83570cf16200d14 translatedAt=2026-08-27T22:50:11.454Z -->

## Applicable Products<a name="section1550532418810"></a>

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

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_datacopy_intf.h"`.

Copies each valid element in **srcReg** to the corresponding position in **dstReg**, while invalid positions retain the original values of **dstReg**.

## Prototype<a name="section620mcpsimp"></a>

- Supports passing the **mask** parameter:

    ```cpp
    template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::MERGING, typename U>
    __simd_callee__ inline void Move(U& dstReg, U& srcReg, MaskReg mask)
    ```

- Does not support passing the **mask** parameter:

    ```cpp
    template <typename T = DefaultType, typename U>
    __simd_callee__ inline void Move(U& dstReg, U& srcReg)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Operand data type. For supported data types, see [data type](#data-type). |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md) enumeration type, which selects the MERGING mode or the ZEROING mode.<br>&bull; The ZEROING mode is not supported currently.<br>&bull; In the MERGING mode, elements not filtered by the mask retain the original values of **dstReg**. |
| U | **RegTensor** type of the destination operand, for example, **RegTensor\<half>**, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstReg | Output | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Input | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Input | Validity indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Type

The data types of the destination operand and the source operand must be consistent. Supported data types: **bool**, **int8_t**, **uint8_t**, **int16_t**, **uint16_t**, **half**, **bfloat16_t**, **int32_t**, **uint32_t**, **float**, **int64_t**, **uint64_t**.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

The **mode** parameter of this API does not support the **ZEROING** mode.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void MoveVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::Move(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
