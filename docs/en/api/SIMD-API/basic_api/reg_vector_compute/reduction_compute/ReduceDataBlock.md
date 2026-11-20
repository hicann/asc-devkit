# ReduceDataBlock<a name="ZH-CN_TOPIC_0000001938755198"></a>

<!-- md-trans-meta sourceCommit=0f9a59471afc8678a19cd607420c13ec5e8e0290 translatedAt=2026-11-05T03:15:37.151Z pushedAt=2026-11-18T01:25:30.914Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/ReduceDataBlock_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The reduction instruction simplifies a data set into a single value within each DataBlock (32B) based on ReduceType.

When ReduceType is SUM, the elements that participate in calculation in each DataBlock (32B) are added, and the final calculation result is stored in the lowest bits of dstReg in sequence.

When ReduceType is MAX, the maximum value in each DataBlock (32B) is stored in the lowest bits of dstReg in sequence.

When ReduceType is MIN, the minimum value in each DataBlock (32B) is stored in the lowest bits of dstReg in sequence.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <ReduceType type = ReduceType::SUM, typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceDataBlock(U& dstReg, U srcReg, MaskReg mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| type | ReduceType type, which supports SUM, MAX, and MIN.<br>enum class ReduceType {<br>    SUM = 0,<br>    MAX,<br>    MIN,<br>}; |
| T | Data type of the destination operand and source operand. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md), which selects the MERGING mode or the ZEROING mode. Currently, only the ZEROING mode is supported.<br>&bull; ZEROING: Elements not selected by the mask are set to zero in `dst`.<br>&bull; MERGING: Currently not supported. |
| U | RegTensor type of the destination operand and source operand, for example, RegTensor&lt;int32_t&gt;. It is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Description |
| --- | --- |
| dstReg | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Valid indicator for the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Types

<!-- npu="950" id8 -->
**Table 3** Data type support for ReduceType::SUM on Ascend 950PR&950DT products

| dstReg | srcReg |
| --- | --- |
| half | half |
| int32_t | int32_t |
| int32_t | int16_t |
| uint32_t | uint32_t |
| uint32_t | uint16_t |
| float | float |

**Table 4** Data type support for ReduceType::MAX or ReduceType::MIN on Ascend 950PR&950DT products

| dstReg | srcReg |
| --- | --- |
| half | half |
| int16_t | int16_t |
| uint16_t | uint16_t |
| int32_t | int32_t |
| uint32_t | uint32_t |
| float | float |
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/ReduceDataBlock_res.md#id2 -->

## Return Value

None

## Constraints<a name="section177921451558"></a>

- For reduction to find the maximum, max\(-0, +0\) = +0.
- For reduction to find the minimum, min\(-0, +0\) = -0.
- For reduction to find the sum of data, mask specifies whether the source operands participate in calculation. Elements that do not participate in calculation are handled as 0.
- For reduction to find the maximum, mask specifies whether the source operands participate in calculation. Elements that do not participate in calculation are handled as the minimum value of the data type.
- For reduction to find the minimum, mask specifies whether the source operands participate in calculation. Elements that do not participate in calculation are handled as the maximum value of the data type.
- For the empty mask scenario and abnormal value handling, see [Empty Mask Scenario and Abnormal Value Handling](#empty-mask-scenario-and-abnormal-value-handling).
- The accumulation order within the instruction uses a binary tree accumulation method. For the rules, see [ReduceType::SUM accumulation order](#reducetypesum-accumulation-order).

## Key Features<a name="zh-cn_topic_0000002568852247_section18972943153217"></a>

### ReduceType::SUM accumulation order<a id="ReduceType::SUM accumulation order"></a>

The sum within each DataBlock is calculated by binary-tree accumulation.

Taking the summation of `half` data as an example, each DataBlock contains 16 numbers. They are added pairwise in a binary-tree manner, and the calculation process is shown in the following figure:

1. data01 is obtained by adding data1 and data2, data02 is obtained by adding data3 and data4, ..., data07 is obtained by adding data13 and data14, and data08 is obtained by adding data15 and data16;
2. data001 is obtained by adding data01 and data02, data002 is obtained by adding data03 and data04, ..., data004 is obtained by adding data07 and data08;
3. By analogy, the destination operand is obtained as one half data value sum.

**Figure 1** ReduceDataBlock schematic diagram<a name="zh-cn_topic_0000002568852247_fig10524085414"></a>  
![ReduceDataBlock schematic diagram](../../../../figures/reg_reduce_datablock_index.png "ReduceDataBlock schematic diagram")

### Empty Mask Scenario and Abnormal Value Handling

- For reduction to find the sum of data, when no element in the DataBlock participates in calculation, 0 of the destination operand data type is written to dstReg.
- For reduction to find the maximum, when no element in the DataBlock participates in calculation, the minimum value of the data type is written to dstReg.
- For reduction to find the minimum, when no element in the DataBlock participates in calculation, the maximum value of the data type is written to dstReg.
- For reduction to find the maximum/minimum, if nan exists in the DataBlock, nan of the data type is written to dstReg.

## Example<a name="section642mcpsimp"></a>

```cpp
template<typename T>
__simd_vf__ inline void ReduceDataBlockVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, 
 uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
        mask = AscendC::Reg::UpdateMask<T>(count);
        // type = ReduceType::SUM
        AscendC::Reg::ReduceDataBlock<AscendC::Reg::ReduceType::SUM>(dstReg, srcReg, mask);
        // type = ReduceType::MAX
        // AscendC::Reg::ReduceDataBlock<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask);
        // type = ReduceType::MIN
        // AscendC::Reg::ReduceDataBlock<AscendC::Reg::ReduceType::MIN>(dstReg, srcReg, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask); 
    }
}
```
