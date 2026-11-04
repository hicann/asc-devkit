# Reduce<a name="ZH-CN_TOPIC_0000001938595870"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T16:34:57.110Z pushedAt=2026-10-26T07:41:44.561Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reduction_compute/Reduce_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/reg_compute/kernel_reg_compute_vec_reduce_intf.h"`.

The reduction instruction reduces a data set to a single value or a smaller set according to **ReduceType**.

**ReduceType::SUM**: According to **mask**, calculates the sum of the valid elements in the source operand **srcReg**, and writes the result to the destination operand **dstReg**.

**ReduceType::MAX**: According to **mask**, calculates the maximum value and index among the valid elements in the source operand **srcReg**, and writes the result to the destination operand **dstReg**. When multiple maximum values exist, the index of the first maximum value is saved in **dstReg**.

**ReduceType::MIN**: According to **mask**, calculates the minimum value and index among the valid elements in the source operand **srcReg**, and writes the result to the destination operand **dstReg**. When multiple minimum values exist, the index of the first minimum value is saved in **dstReg**.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <ReduceType type = ReduceType::SUM, typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void Reduce(S& dstReg, V srcReg, MaskReg mask)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| type | ReduceType type, which supports SUM, MAX, and MIN.<br>enum class ReduceType {<br>    SUM = 0,<br>    MAX,<br>    MIN,<br>}; |
| T | Data type of the destination operand `dstReg`. |
| U | Data type of the source operand `srcReg`. |
| mode | [MaskMergeMode](../aux_data_types/MaskMergeMode.md), which selects the MERGING mode or the ZEROING mode.<br>&bull; ZEROING: Elements not selected by the mask are set to zero in `dst`.<br>&bull; MERGING: Currently not supported. |
| S | RegTensor type of the destination operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |
| V | RegTensor type of the source operand, for example, RegTensor&lt;half&gt;, which is automatically deduced by the compiler and does not need to be specified by the user. |

**Table 2** Function parameter description

| Parameter | Description |
| --- | --- |
| dstReg | Destination operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| srcReg | Source operand.<br>The type is [RegTensor](../register_data_types/RegTensor.md). |
| mask | Valid indication of the operation on the source operand elements. For details, see [MaskReg](../register_data_types/MaskReg.md). |

## Data Types

**Table 3** Data type support for ReduceType::SUM

| dstReg | srcReg |
| --- | --- |
| half | half |
| int32_t | int16_t |
| uint32_t | uint16_t |
| int32_t | int32_t |
| uint32_t | uint32_t |
| float | float |
| int64_t | int64_t |
| uint64_t | uint64_t |

**Table 4** Data type support for ReduceType::MAX or ReduceType::MIN

| dstReg | srcReg |
| --- | --- |
| int16_t | int16_t |
| uint16_t | uint16_t |
| half | half |
| int32_t | int32_t |
| uint32_t | uint32_t |
| float | float |
| int64_t | int64_t |
| uint64_t | uint64_t |

## Return Value Description

None

## Constraints<a name="section177921451558"></a>

- For reduction to compute the sum of data, **mask** specifies whether the source operand participates in calculation. Elements that do not participate in calculation are handled as 0.
- For reduction to find the maximum value, **mask** specifies whether the source operand participates in calculation. Elements that do not participate in calculation are handled as the minimum value of the data type.
- For reduction to find the minimum value, **mask** specifies whether the source operand participates in calculation. Elements that do not participate in calculation are handled as the maximum value of the data type.
- For reduction to find the maximum value, when all elements do not participate in calculation, write the minimum value of this data type to **dstReg**. When multiple maximum values exist, save the index of the first maximum value in **dstReg**, max\(-0, +0\) = +0.
- For reduction to find the minimum value, when all elements do not participate in calculation, write the maximum value of this data type to **dstReg**. When multiple minimum values exist, save the index of the first minimum value in **dstReg**, min\(-0, +0\) = -0.
- When reduction is performed to find the minimum value or the maximum value, the data type of the source operand is the same as that of the destination operand.
- For the mask empty scenario and exception value handling, see [Empty Mask Scenario and Exception Value Handling](#empty-mask-scenario-and-exception-value-handling).
- The accumulation order within the instruction uses a binary tree accumulation method. For the rules, see [ReduceType::SUM Accumulation Order](#ReduceType::SUM-accumulation-order).

## Key Features<a name="zh-cn_topic_0000002537812548_section18972943153217"></a>

### Index Value Requires Forced Type Conversion

The maximum/minimum value index of **dstReg** is stored according to the data type of **dstReg**. For example, when **dstReg** is of the **half** type, the index is stored as the **half** type. Therefore, reading the index requires using the reinterpret\_cast method to convert it to an integer type. If the data type is **half**, you need to use reinterpret\_cast\<uint16_t\*\>; if the data type is **float**, you need to use reinterpret\_cast\<uint32\_t\*\>.

For example, if the input data is of the **half** type and the calculation result is \[1.14, 6.8e-06\], 6.8e-06 needs to be converted using the reinterpret\_cast<uint16\_t\*\> method to obtain the index value 114. The conversion pseudo-code example is as follows:

```cpp
__simd_vf__ inline void ReduceVF(__ubuf__ half* dstAddr, __ubuf__ half* srcAddr, uint32_t count)
{
    AscendC::Reg::RegTensor<T> srcReg;
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask;    
    AscendC::Reg::LoadAlign(srcReg, srcAddr);
    mask = AscendC::Reg::UpdateMask<T>(count);
    AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask); 
    AscendC::Reg::StoreAlign(dstAddr, dstReg, mask);
} // End of Vector Function
```

```cpp
// Save the index to Scalar through forced type conversion. This must be handled outside the Vector Function.
AscendC::PipeBarrier<PIPE_V>(); // Insert synchronization and wait for the reduction calculation to complete before handling the index.
half maxIndex = *(dstAddr + 1);
uint16_t realIndex = *reinterpret_cast<uint16_t*>(&maxIndex);
```

![reg_reduce_sum](../../../../figures/reg_reduce_sum.png)

### ReduceType::SUM Accumulation Order<a id="ReduceType::SUM-accumulation-order"></a>

The sum of the valid elements in the source operand **srcReg** is calculated using binary-tree accumulation.

Taking the summation of **half** type data as an example, **srcReg** contains 128 numbers. Through the binary-tree method, they are added pairwise. The calculation process is shown in the following figure:

1. **data1** and **data2** are added to obtain **data01**, **data3** and **data4** are added to obtain **data02**, ..., **data125** and **data126** are added to obtain **data63**, and **data127** and **data128** are added to obtain **data64**;
2. **data01** and **data02** are added to obtain **data001**, **data03** and **data04** are added to obtain **data002**, ..., and **data63** and **data64** are added to obtain **data032**;
3. By analogy, the destination operand is obtained as a single **half** type data **sum**.

**Figure 1** reg_reduce_index<a name="zh-cn_topic_0000002537812548_fig1630121573314"></a>  
![reg_reduce_index](../../../../figures/reg_reduce_index.png "reg_reduce_index")

### Empty Mask Scenario and Exception Value Handling

- For reduction to compute the data sum, when all elements do not participate in calculation, write 0 of the destination operand data type to **dstReg**.
- For reduction to find the maximum value, when all elements do not participate in calculation, write the minimum value of this data type to **dstReg**.
- For reduction to find the minimum value, when all elements do not participate in calculation, write the maximum value of this data type to **dstReg**.
- For reduction to find the maximum/minimum value, if the input data contains nan, write nan of this data type to **dstReg** and save the index of the first nan in **dstReg**.

### Extracting MAX/MIN Results (Value + Index)

MAX/MIN reduction produces two valid results that need to be extracted separately. In the following example, dstAddr is a 32B-aligned address. If you need to write to a non-aligned address, see [continuous non-aligned move-out \(StoreUnAlign\)](../reg_data_store/StoreUnAlign_continuous.md).

```cpp
AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, pregFull); 
```

Method 1: Store the whole result and then access it by offset (the most direct approach, suitable for scenarios where both the value and the index are needed)

```cpp
AscendC::Reg::MaskReg pregVL2 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::VL2>();
// Access directly afterwards.// dstAddr[0] -> maximum value// dstAddr[1] -> maximum value index
AscendC::Reg::StoreAlign(dstAddr, dstReg, pregVL2);  
```

Method 2: Use Mask to separate the result to the destination address

```cpp
AscendC::Reg::MaskReg pregVL1 = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::VL1>();
// Move out only the maximum value to the destination address.
AscendC::Reg::StoreAlign(dstAddr, dstReg, pregVL1); 
```

## Example<a name="section642mcpsimp"></a>

- Reduction sum:

    ```cpp
    template<typename T, typename U>
    __simd_vf__ inline void ReduceVF(__ubuf__ T* dstAddr, __ubuf__ U* srcAddr, uint32_t count, 
     uint32_t srcRepeatSize, uint32_t dstRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<U> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * srcRepeatSize);
            mask = AscendC::Reg::UpdateMask<U>(count);
            AscendC::Reg::Reduce<AscendC::Reg::ReduceType::SUM>(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * dstRepeatSize, dstReg, mask);
        }
    }
    ```

- Reduction to find the maximum or minimum value:

    ```cpp
    template<typename T>
    __aicore__ inline void ReduceVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T> srcReg;
        AscendC::Reg::RegTensor<T> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            AscendC::Reg::LoadAlign(srcReg, srcAddr + i * oneRepeatSize);
            mask = AscendC::Reg::UpdateMask<T>(count);
            // type = ReduceType::MAX
            AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MAX>(dstReg, srcReg, mask);
            // type = ReduceType::MIN
            // AscendC::Reg::Reduce<AscendC::Reg::ReduceType::MIN>(dstReg, srcReg, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
