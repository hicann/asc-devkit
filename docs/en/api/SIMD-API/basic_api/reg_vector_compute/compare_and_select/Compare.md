# Compare<a name="ZH-CN_TOPIC_0000001929827672"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:02:30.400Z pushedAt=2026-10-26T07:41:44.486Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/compare_and_select/Compare_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"`.

Compares two [RegTensor](../register_data_types/RegTensor.md) values element-wise, and writes the comparison result to the corresponding bit of the destination operand [MaskReg](../register_data_types/MaskReg.md) according to the comparison mode specified by the template parameter. If the comparison result is true, the corresponding bit of the output result is 1; otherwise, it is 0.

Here, **cmp** specifies the comparison mode through the template parameter **mode**, with values EQ/GT/LT/GE/NE/LE.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename U>
__simd_callee__ inline void Compare(MaskReg& dst, U& srcReg0, U& srcReg1, MaskReg& mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Operand data type. |
| mode | Comparison mode. The following values are supported:<br>• LT: less than<br>• GT: greater than<br>• GE: greater than or equal to<br>• EQ: equal to<br>• NE: not equal to<br>• LE: less than or equal to |
| U | `RegTensor` type of the source operand, for example, `RegTensor<half>`. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| :----- | :-------- | :--- |
| dst | Output | [MaskReg](../register_data_types/MaskReg.md) type, destination operand. |
| srcReg0, srcReg1 | Input | Source operands.<br>The type is [RegTensor](../register_data_types/RegTensor.md).<br>The data types of the two source operands must be consistent with that of the destination operand. |
| mask | Input | Validity indicator for the element operations of the source operands. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Return Value<a name="section640mcpsimp"></a>

None

## Data Type

**Table 3** Data type combinations

| srcReg0 and srcReg1 data type | dst data type |
| :----------------------- | :---------- |
| int8_t | MaskReg |
| uint8_t | MaskReg |
| int16_t | MaskReg |
| uint16_t | MaskReg |
| half | MaskReg |
| bfloat16_t | MaskReg |
| int32_t | MaskReg |
| uint32_t | MaskReg |
| float | MaskReg |
| int64_t | MaskReg |
| uint64_t | MaskReg |

## Constraints<a name="section633mcpsimp"></a>

- Unselected elements controlled by the **mask** parameter are set to zero in the destination operand.
- Operand overlap constraint: **srcReg0** and **srcReg1** can be the same `RegTensor`.

## Example<a name="section642mcpsimp"></a>

```cpp
// Example: Compare the data of src0Addr and src1Addr element-wise (EQ mode), then select elements by the comparison result mask and write them to dstAddr.
// Input: src0Addr points to source operand 0 (for example, half type [1.0, 4.0, 3.0, 2.0, 5.0, 6.0, 7.0, 8.0, 1.0, 2.0, 9.0, 3.0, 4.0, 6.0, 5.0, 7.0, ...]).
//       src1Addr points to source operand 1 (for example, half type [1.0, 3.0, 3.0, 5.0, 2.0, 6.0, 9.0, 4.0, 2.0, 5.0, 9.0, 1.0, 8.0, 3.0, 5.0, 6.0, ...]).
//       Corresponding position relationship: pos0 equal (1.0==1.0), pos1 greater (4.0>3.0), pos2 equal (3.0==3.0), pos3 less (2.0<5.0)...
// Output: dstMask is the comparison result mask (in EQ mode [1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, ...], where equal positions are 1 and the rest are 0).
//       dstAddr stores the result selected by the mask.
template<typename T>
__simd_vf__ inline void CompareVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg0;   // Source operand 0.
    AscendC::Reg::RegTensor<T> srcReg1;   // Source operand 1.
    AscendC::Reg::RegTensor<T> dstReg;    // Destination operand, storing the result selected by Select.
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg dstMask;        // Comparison result mask, where equal positions are set to 1.
    for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(count);  // Update the mask based on the actual number of elements.
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);  // Load source operand 0.
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);  // Load source operand 1.
        AscendC::Reg::Compare<T, AscendC::CMPMODE::EQ>(dstMask, srcReg0, srcReg1, mask);  // Compare elements for equality and write the result to dstMask.
        AscendC::Reg::Select(dstReg, srcReg0, srcReg1, dstMask);  // Select elements based on the comparison result mask.
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);  // Store the selected result back to dstAddr.
    }
}
```
