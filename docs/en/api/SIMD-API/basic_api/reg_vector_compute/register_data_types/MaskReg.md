# MaskReg<a name="ZH-CN_TOPIC_0000001955873477"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:03:34.148Z pushedAt=2026-10-26T07:41:44.599Z nl=2 -->

## Product Support<a name="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file reference path is **basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h**.

The MaskReg register indicates which elements participate in the calculation process. Its width is one-eighth of <a href="./RegTensor.md">RegTensor</a> (VL/8). As shown in the following figure, when the operand type is b8, each element corresponds to 1 bit of MaskReg; when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the lowest bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the lowest bit of the 4 bits is valid.

**Figure 1** MaskReg calculation process<a id="fig1"></a>

![](../../../../figures/maskreg_process.png "MaskReg calculation process")

## Prototype<a name="section620mcpsimp"></a>
- **CreateMask** API
  ```cpp
  template <typename T, MaskPattern mode = MaskPattern::ALL, const RegTrait& regTrait = RegTraitNumOne>
  __simd_callee__ inline MaskReg CreateMask()
  ```
- **UpdateMask** API
  ```cpp
  template <typename T, const RegTrait& regTrait = RegTraitNumOne>
  __simd_callee__ inline MaskReg UpdateMask(uint32_t& scalarValue)
  ```

## Parameters<a name="section622mcpsimp"></a>

<a id="create-mask-params"></a>
**Table 1** CreateMask parameter description

| Parameter | Input/Output | Description |
| :-- | :------------ | :------------ |
| T | Input | Template parameter. Supported data types are b8/b16/b32/b64. |
| mode | Input | Mode for creating a MaskReg, of the enum class type.<br>enum class MaskPattern {<br>    ALL,        // Set all elements to valid data.<br>    VL1,        // Set the lowest 1 element to valid data.<br>    VL2,        // Set the lowest 2 elements to valid data.<br>    VL3,        // Set the lowest 3 elements to valid data.<br>    VL4,        // Set the lowest 4 elements to valid data.<br>    VL8,        // Set the lowest 8 elements to valid data.<br>    VL16,       // Set the lowest 16 elements to valid data.<br>    VL32,       // Set the lowest 32 elements to valid data.<br>    VL64,       // Set the lowest 64 elements to valid data.<br>    VL128,      // Set the lowest 128 elements to valid data.<br>    M3,         // Set multiples of 3 to valid data.<br>    M4,         // Set multiples of 4 to valid data.<br>    H,          // Set the lowest half of the elements to valid data.<br>    Q,          // Set the lowest quarter of the elements to valid data.<br>    ALLF = 15   // Set all elements to invalid data.<br>}; |
| regTrait | Input | The default value of this parameter is RegTraitNumOne. |

<a id="update-mask-params"></a>
**Table 2** UpdateMask parameter description

| Parameter | Input/Output | Description |
| :-- | :------------ | :------------ |
| T | Input | Template parameter. Supported data types are b8, b16, b32, and b64. |
| regTrait | Input | The default value of this parameter is RegTraitNumOne. |
| scalarValue | Input/Output | Specific number of elements to be operated on in the vector calculation, used to generate the corresponding MaskReg. The valid element range is from 0 to VL_T (a vector register with a bit width of VL can hold VL_T elements of data type T).<br>After this function is executed, scalarValue is decremented by VL_T.<br>`scalarValue = (scalarValue < VL_T) ? 0 : (scalarValue - VL_T)` |


## Return Value<a name="section1575141714439"></a>

MaskReg

## Constraints<a name="section177921451558"></a>

- The maximum number of **MaskReg** registers is 8. Register data exceeding this limit is written to the reserved 8 KB UB memory, which may cause performance degradation. The compiler automatically reuses registers whose lifecycle has ended and the reserved memory. If both registers and reserved memory have available space, registers are reused first.

## Key Features

### MaskReg in Cast Precision Conversion

The mask bit width corresponding to an element varies with the data type. During type conversion in Cast, MaskReg filters valid elements based on the input source operand.

Figure [b16-to-b32 type conversion process](#fig4) and Figure [b32-to-b16 type conversion process](#fig5) show the type conversion between b16 and b32 when MaskReg and RegLayout act simultaneously.

**Figure 4** b16-to-b32 type conversion process<a id="fig4"></a>

![](../../../../figures/b16_to_b32_conversion.png "b16-to-b32 type conversion process")

**Figure 5** b32-to-b16 type conversion process<a id="fig5"></a>

![](../../../../figures/b32_to_b16_conversion.png "b32-to-b16 type conversion process")

In particular, for conversions between int4x2_t/fp4x2_e2m1_t/fp4x2_e1m2_t and b16, the instruction reads and writes every two elements as a pair. When converting from a larger to a smaller type, the valid mask bits are determined by even-numbered bits.

Figure [fp4x2_e2m1_t-to-bfloat16_t type conversion process](#fig6) and Figure [bfloat16_t-to-fp4x2_e2m1_t type conversion process](#fig7) show the conversion between fp4x2_e2m1_t and bfloat16_t when MaskReg and RegLayout act simultaneously.

**Figure 6** fp4x2_e2m1_t-to-bfloat16_t type conversion process<a id="fig6"></a>

![](../../../../figures/fp4x2_e2m1_to_bfloat16.png "fp4x2_e2m1_t to bfloat16_t type conversion process")

**Figure 7** bfloat16_t to fp4x2_e2m1_t type conversion process<a id="fig7"></a>

![](../../../../figures/bfloat16_to_fp4x2_e2m1.png "bfloat16_t to fp4x2_e2m1_t type conversion process")

### Mask Setting Method

In Mask setting, Reg vector calculation supports multiple flexible configuration methods. You can select strategies such as the fixed Mask mode, moving from the Unified Buffer (UB), or moving from RegTensor based on the actual calculation scenario to meet the requirements of different calculation scenarios. The specific setting methods are shown in the following table:

**Table 3** Mask setting methods

| Number | Setting Method | Involved APIs | Description |
| :-- | :------------ | :------------ | :------------ |
| 1 | Set by calling an API | CreateMask | Call the CreateMask API to set the mask with a fixed pattern. Each loop calculation uses this mask. See [Method 1](#method1). |
| 2 | Set by calling an API | UpdateMask | Call UpdateMask to set the mask. See Method 2.<br>&bull; Call UpdateMask outside the loop to set a fixed mask. The mask indicates that the first count elements participate in the calculation in one iteration, and each loop calculation uses this mask.<br>&bull; Call UpdateMask inside the loop to set the mask, indicating that the first count elements participate in the calculation. The mask is automatically updated in each iteration. See [Method 2](#method2). |
| 3 | Move from UB | LoadAlign | Move the mask from UB to MaskReg. See [Method 3](#method3). |
| 4 | Move from RegTensor | MaskGenWithRegTensor | Move the mask from RegTensor to MaskReg. See [Method 4](#method4). |
| 5 | Read from the mask register | MoveMask | Read the mask value from the mask register {MASK1,MASK0} set by SetVectorMask. See [Method 5](#method5). |

In Reg vector calculation, the mask configuration method is determined by the MaskReg register, and only in-API mask setting is supported. The mask value can be passed directly as an input parameter to the vector calculation API through a MaskReg type parameter.

- CreateMask: Sets a mask with a specific pattern to the MaskReg register. For the patterns, see [CreateMask parameter description](#create-mask-params).

  **Figure 2** Element selection under different MaskReg patterns of the CreateMask API for the b8 data type<a id="fig2"></a>

![](../../../../figures/b8_createmask_maskreg.png "Element selection under different MaskReg patterns of the CreateMask API for the b8 data type")

- UpdateMask: Generates a valid-bit mask of the corresponding length based on the current value of scalarValue, and automatically subtracts the current vector length from scalarValue to update the number of remaining elements to be processed.

  **Figure 3** Mask generation based on scalarValue by the UpdateMask API for the b16 data type<a id="fig3"></a>

![](../../../../figures/b16_updatemask_scalarvalue.png "Mask generation based on scalarValue by the UpdateMask API for the b16 data type")

The following uses the basic arithmetic API Add as an example to describe the different Mask setting methods.

```cpp
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Add(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

- Method 1<a id="method1"></a>
Use the CreateMask API to set the Mask with a fixed Pattern. For details about Pattern, see [CreateMask parameter description](#create-mask-params). Call CreateMask outside the loop to set a fixed Mask, and each iteration of the loop uses this Mask.

  ```cpp
  template <typename T>
  __simd_vf__ inline void SetMaskFixedPatternVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
    AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T, AscendC::Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTimes; i++) {
      AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
      AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
      AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
      AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
  }
  ```

- Method 2<a id="method2"></a>

  Call the UpdateMask API to set the Mask.

  - Call UpdateMask outside the loop to set a fixed Mask. The Mask indicates that the first count elements participate in the calculation in one iteration, and each loop calculation uses this Mask.

    ```cpp
    template <typename T>
    __simd_vf__ inline void SetMaskUpdateFixedVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      mask = AscendC::Reg::UpdateMask<T, AscendC::Reg::RegTraitNumOne>(count);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
    }
    ```
  >[!NOTE]Note
  >
  >When the data type is int16_t (the operand is 16-bit), the number of elements that can be processed within each iteration is Mask∈[1, 128]. When count>128, it is still executed as one repeat (128 elements), and Mask control exceeding 128 elements is not supported.

  - Call UpdateMask inside the loop to set the Mask, indicating that the first count elements participate in the calculation, and the Mask is automatically updated in each iteration.

    ```cpp
    template <typename T>
    __simd_vf__ inline void SetMaskUpdateAutoVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count)
    {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      // Number of data elements in one repeat calculation.
      constexpr uint16_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
      // Total number of iterations.
      uint16_t repeatTimes = AscendC::CeilDivision(count, oneRepeatSize);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T, AscendC::Reg::RegTraitNumOne>(count);
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
    }
    ```

- Method 3<a id="method3"></a>

  Use [LoadAlign](../reg_data_load/LoadAlign_MaskReg.md) to move the Mask from UB to MaskReg. Set the Mask in UB.

  ```cpp
  template <typename T>
  __simd_vf__ inline void SetMaskFromUBVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, __ubuf__ T* maskAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      AscendC::Reg::LoadAlign(mask, maskAddr);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- Method 4<a id="method4"></a>

  Use [MaskGenWithRegTensor](../reg_data_load/MaskGenWithRegTensor_MaskReg.md) to move the Mask from RegTensor to MaskReg. Set the Mask in RegTensor.

  ```cpp
  template <typename T>
  __simd_vf__ inline void SetMaskFromRegTensorVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg, maskReg;
      AscendC::Reg::MaskReg mask;
      // Set the mask in RegTensor.
      T scalarValue = static_cast<T>(0xFFFF);
      AscendC::Reg::Duplicate(maskReg, scalarValue);
      AscendC::Reg::MaskGenWithRegTensor<T, 0>(mask, maskReg);
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

- Method 5<a id="method5"></a>

  Use [MoveMask](../MaskReg_compute/MoveMask.md) to read the Mask value from the mask registers {MASK1,MASK0} set by **SetVectorMask**, convert it according to the data format corresponding to template parameter T, and write it to the returned **MaskReg**. For the b16 type, the complete 128-bit {MASK1,MASK0} is read, and each bit is duplicated into 2 bits; for the b32 type, the 64-bit MASK0 is read, and each bit is duplicated into 4 bits.

  ```cpp
  template <typename T>
  __simd_vf__ inline void MoveMaskVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint16_t oneRepeatSize, uint16_t repeatTimes)
  {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask = AscendC::Reg::MoveMask<T>();
      for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
        AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
      }
  }
  ```

<!-- npu="910b,A3,950" id8 -->
### Mask Mechanism Comparison Between NPU Architecture Version 2201 and NPU Architecture Version 3510

This section compares the Mask mechanism in the per-bit calculation of [NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) in Normal mode with the Mask mechanism in the Reg vector calculation of [NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md).

<!-- npu="910b" id9 -->
- For Atlas A2 products

  - The Mask register length is 256 bits.

  - Each element corresponds to 1 bit of Mask. The maximum number of elements that can be processed in one iteration varies with the data type. When the operand type is b16, at most 128 elements participate in the calculation in one iteration; when the operand type is b32, at most 64 elements participate in the calculation in one iteration; when the operand type is b64, at most 32 elements participate in the calculation in one iteration. For details, see [mask-related parameters](../../memory_vector_compute/SIMD_compute/mask.md#sheet2).
<!-- end id9 -->

<!-- npu="A3" id10 -->
- For Atlas A3 products

  - The Mask register length is 256 bits.

  - Each element corresponds to 1 bit of Mask. The maximum number of elements that can be processed in one iteration varies with the data type. When the operand type is b16, at most 128 elements participate in the calculation in one iteration; when the operand type is b32, at most 64 elements participate in the calculation in one iteration; when the operand type is b64, at most 32 elements participate in the calculation in one iteration. For details, see [mask-related parameters](../../memory_vector_compute/SIMD_compute/mask.md#sheet2).
<!-- end id10 -->

<!-- npu="950" id11 -->
- For Ascend 950PR&950DT products

  - The MaskReg register length is VL/8, that is, 256 bits.

  - When the operand type is b8, each element corresponds to 1 bit of MaskReg; when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the lowest bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the lowest bit of the 4 bits is valid.
<!-- end id11 -->

**Figure 8** Mask mechanism comparison between Memory vector calculation of NPU architecture version 2201 and Reg vector calculation of NPU architecture version 3510<a id="fig8"></a>

![](../../../../figures/memory_vs_reg_mask_comparison.png "Mask mechanism comparison between Memory vector calculation of NPU architecture version 2201 and Reg vector calculation of NPU architecture version 3510")
<!-- end id8 -->

## Call Example<a name="section642mcpsimp"></a>

```cpp
AscendC::Reg::RegTensor<uint32_t> srcReg;
AscendC::Reg::MaskReg mask0 = AscendC::Reg::CreateMask<uint32_t, AscendC::Reg::MaskPattern::ALL>();
AscendC::Reg::MaskReg mask1;
uint32_t scalarValue = 127;
for (uint16_t i = 0; i < 2; i++) {
    mask1 = AscendC::Reg::UpdateMask<uint32_t>(scalarValue);
    AscendC::Reg::LoadAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(srcReg, srcAddr, 0);
    AscendC::Reg::Adds(srcReg, srcReg, 1, mask0);
    AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dst0Addr, srcReg, 0, mask0);
    AscendC::Reg::StoreAlign<T, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(dst1Addr, srcReg, 0, mask1);
}
```
