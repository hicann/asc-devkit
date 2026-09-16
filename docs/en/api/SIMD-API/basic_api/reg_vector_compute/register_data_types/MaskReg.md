# MaskReg<a name="ZH-CN_TOPIC_0000001955873477"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T07:18:49.925Z -->

## Product Support<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: not supported
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

## Function Description<a name="section618mcpsimp"></a>

The header file reference path is "basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h".

The MaskReg register is used to indicate which elements participate in the computation process. Its width is one-eighth of <a href="./RegTensor.md">RegTensor</a> (VL/8). As shown in the following figure, when the operand type is b8, each element corresponds to 1 bit of MaskReg; when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the lowest bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the lowest bit of the 4 bits is valid.

**Figure 1** MaskReg computation process<a id="fig1"></a>

![](../../../../figures/mask_reg_calculation.png "MaskReg computation process")

## Function Prototype<a name="section620mcpsimp"></a>
- CreateMask API
  ```cpp
  template <typename T, MaskPattern mode = MaskPattern::ALL, const RegTrait& regTrait = RegTraitNumOne>
  __simd_callee__ inline MaskReg CreateMask()
  ```
- UpdateMask API
  ```cpp
  template <typename T, const RegTrait& regTrait = RegTraitNumOne>
  __simd_callee__ inline MaskReg UpdateMask(uint32_t& scalarValue)
  ```

## Parameter Description<a name="section622mcpsimp"></a>

<a id="create-mask-params"></a>
**Table 1**  CreateMask parameter description

| Parameter Name | Input/Output | Description |
| :-- | :------------ | :------------ |
| T | Input | Template parameter. The supported data types are b8/b16/b32/b64. |
| mode | Input | The mode for creating MaskReg, of enum class type.<br>enum class MaskPattern {<br>    ALL,        // Set all elements to valid data.<br>    VL1,        // Set the lowest 1 element to valid data.<br>    VL2,        // Set the lowest 2 elements to valid data.<br>    VL3,        // Set the lowest 3 elements to valid data.<br>    VL4,        // Set the lowest 4 elements to valid data.<br>    VL8,        // Set the lowest 8 elements to valid data.<br>    VL16,       // Set the lowest 16 elements to valid data.<br>    VL32,       // Set the lowest 32 elements to valid data.<br>    VL64,       // Set the lowest 64 elements to valid data.<br>    VL128,      // Set the lowest 128 elements to valid data.<br>    M3,         // Set multiples of 3 to valid data.<br>    M4,         // Set multiples of 4 to valid data.<br>    H,          // Set the lowest half of elements to valid data.<br>    Q,          // Set the lowest quarter of elements to valid data.<br>    ALLF = 15   // Set all elements to invalid data.<br>};<br>|
| regTrait | Input | The default value of this parameter is RegTraitNumOne. |

<a id="update-mask-params"></a>
**Table 2**  UpdateMask parameter description

| Parameter Name | Input/Output | Description |
| :-- | :------------ | :------------ |
| T | Input | Template parameter. The supported data types are b8, b16, b32, and b64. |
| regTrait | Input | The default value of this parameter is RegTraitNumOne. |
| scalarValue | Input/Output | The specific number of elements that vector computation needs to operate on, used to generate the corresponding MaskReg. The valid element range is from 0 to VL_T (a vector register with a bit width of VL can hold VL_T elements of data type T).<br>After this function is executed, scalarValue is decremented by VL_T.<br>`scalarValue = (scalarValue < VL_T) ? 0 : (scalarValue - VL_T)` |


## Return Value Description<a name="section1575141714439"></a>

MaskReg

## Constraints<a name="section177921451558"></a>

- The maximum number of MaskReg registers is 8. Register data exceeding this limit is written to the reserved 8K UB memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycle has ended. If both registers and reserved memory have available space, registers are reused first.

## Key Features

### MaskReg in Cast Precision Conversion

The mask bit width corresponding to an element differs across data types. During type conversion by Cast, MaskReg filters valid elements based on the input source operand.

Figure [b16 to b32 type conversion process](#fig4) and Figure [b32 to b16 type conversion process](#fig5) show the type conversion process between b16 and b32 when MaskReg and RegLayout act simultaneously.

**Figure 4** b16 to b32 type conversion process<a id="fig4"></a>

![](../../../../figures/b16_to_b32_conversion.png "b16 to b32 type conversion process")

**Figure 5** b32 to b16 type conversion process<a id="fig5"></a>

![](../../../../figures/b32_to_b16_conversion.png "b32 to b16 type conversion process")

In particular, for conversions between int4x2_t/fp4x2_e2m1_t/fp4x2_e1m2_t and b16, the instruction reads and writes every 2 elements as a pair. When converting from a larger type to a smaller type, the valid mask bit is determined by the even-numbered bit.

Figure [fp4x2_e2m1_t to bfloat16_t type conversion process](#fig6) and Figure [bfloat16_t to fp4x2_e2m1_t type conversion process](#fig7) show the conversion between fp4x2_e2m1_t and bfloat16_t when MaskReg and RegLayout act simultaneously.

**Figure 6** fp4x2_e2m1_t to bfloat16_t type conversion process<a id="fig6"></a>

![](../../../../figures/fp4_to_bf16_conversion.png "fp4x2_e2m1_t to bfloat16_t type conversion process")

**Figure 7** bfloat16_t to fp4x2_e2m1_t type conversion process<a id="fig7"></a>

![](../../../../figures/bf16_to_fp4_conversion.png "bfloat16_t to fp4x2_e2m1_t type conversion process")

### Mask Setting Method

In mask setting, Reg vector computation supports multiple flexible configuration methods. You can select strategies such as fixed Mask mode, moving from UB, or moving from RegTensor according to the actual computation scenario to meet the requirements of different computation scenarios. The specific setting methods are shown in the following table:

**Table 3** Mask setting method

| Number | Setting Method | Involved API | Description |
| :-- | :------------ | :------------ | :------------ |
| 1 | Set by calling an API | CreateMask | Call the CreateMask API to set the Mask with a fixed Pattern, and this Mask is used in each iteration of loop computation. See [Method 1](#method1). |
| 2 | Set by calling an API | UpdateMask | Call UpdateMask to set the Mask. See Method 2.<br>&bull; Call UpdateMask outside the loop to set a fixed Mask. The Mask indicates that the first count elements participate in the computation in one iteration of loop computation, and this Mask is used in each iteration of loop computation.<br>&bull; Call UpdateMask inside the loop to set the Mask, indicating that the first count elements participate in the computation, and the Mask is automatically updated in each loop. See [Method 2](#method2). |
| 3 | Move from UB | LoadAlign | Move the Mask from UB to MaskReg. See [Method 3](#method3). |
| 4 | Move from RegTensor | MaskGenWithRegTensor | Move the Mask from RegTensor to MaskReg. See [Method 4](#method4). |
| 5 | Read from the mask register | MoveMask | Read the Mask value from the mask register {MASK1,MASK0} set by SetVectorMask. See [Method 5](#method5). |

In Reg vector computation, the mask configuration method is determined by the MaskReg register, and only setting the Mask within the API is supported. The Mask value can be directly passed as an input parameter to the vector computation API through a MaskReg type parameter.

- CreateMask: Set the Mask to the MaskReg register with a specific pattern. For the patterns, see [CreateMask parameter description](#create-mask-params).

  **Figure 2** Element selection in different MaskReg modes of the CreateMask API under the b8 data type<a id="fig2"></a>

![](../../../../figures/b8_create_mask_reg_select.png "Element selection in different MaskReg modes of the CreateMask API under the b8 data type")

- UpdateMask: Generate a valid-bit mask of the corresponding length based on the current value of scalarValue, and automatically subtract the current vector length from scalarValue to update the number of remaining elements to be processed.

  **Figure 3** Mask generation based on scalarValue of the UpdateMask API under the b16 data type<a id="fig3"></a>

![](../../../../figures/b16_update_mask_scalar.png "Mask generation of the UpdateMask API based on scalarValue for the b16 data type")

The basic arithmetic API Add is used as an example to introduce the different setting methods of Mask.

```cpp
template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Add(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
```

- Method 1<a id="method1"></a>
  Adopt the CreateMask API to set Mask with a fixed Pattern. For details about Pattern, see [CreateMask parameter description](#create-mask-params). Call CreateMask outside the loop to set a fixed Mask, and use this Mask for each iteration of loop computation.

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

  Call the UpdateMask API to set Mask.

  - Call UpdateMask outside the loop to set a fixed Mask. Mask indicates that the first count elements participate in the computation in one iteration, and this Mask is used for each iteration of loop computation.

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
  >When the data type is int16_t (the operand is 16-bit), the number of elements that can be processed in each loop is Mask∈[1, 128]. When count>128, it is still executed as one repeat (128 elements), and Mask control exceeding 128 elements is not supported.

  - Call UpdateMask inside the loop to set Mask, indicating that the first count elements participate in the computation, and Mask is automatically updated each loop.

    ```cpp
    template <typename T>
    __simd_vf__ inline void SetMaskUpdateAutoVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count)
    {
      AscendC::Reg::RegTensor<T, AscendC::Reg::RegTraitNumOne> srcReg0, srcReg1, dstReg;
      AscendC::Reg::MaskReg mask;
      // Number of data elements computed in one repeat
      constexpr uint16_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
      // Total number of loops
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

  Adopt [LoadAlign](../reg_data_load/LoadAlign_MaskReg.md) to move the Mask from UB to MaskReg. Set the Mask in UB.

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

  Adopt [MaskGenWithRegTensor](../reg_data_load/MaskGenWithRegTensor_MaskReg.md) to move the Mask from RegTensor to MaskReg. Set the Mask in RegTensor.

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

  Adopt [MoveMask](../MaskReg_compute/MoveMask.md) to read the Mask value from the mask register {MASK1,MASK0} set by SetVectorMask, convert it according to the data format corresponding to the template parameter T, and then write it to the return value MaskReg. For the b16 type, the complete 128-bit {MASK1,MASK0} is read, and each bit is replicated as 2 bits; for the b32 type, the 64-bit MASK0 is read, and each bit is replicated as 4 bits.

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

### Mask Mechanism Comparison Between NPU Architecture Version 2201 and NPU Architecture Version 3510

This section compares the Mask mechanism in the per-bit computation of [NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md) in Normal mode with the Mask mechanism based on Reg vector computation in [NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md).

- For Atlas A2 training products/Atlas A2 inference products and Atlas A3 training products/Atlas A3 inference products

  - The Mask register length is 256 bits.

  - Each element corresponds to 1 bit of Mask. The data type differs, and the maximum number of elements that can be processed in each loop differs. When the operand type is b16, at most 128 elements participate in the computation in one iteration; when the operand type is b32, at most 64 elements participate in the computation in one iteration; when the operand type is b64, at most 32 elements participate in the computation in one iteration. For details, see [mask-related parameters](../../memory_vector_compute/SIMD_compute/mask.md#sheet2).

- For Ascend 950PR/Ascend 950DT

  - The MaskReg register length is VL/8, that is, 256 bits.

  - When the operand type is b8, each element corresponds to 1 bit of MaskReg; when the operand type is b16, each element corresponds to 2 bits of MaskReg, and only the lowest bit of the 2 bits is valid; when the operand type is b32, each element corresponds to 4 bits of MaskReg, and only the lowest bit of the 4 bits is valid.

**Figure 8** Mask mechanism comparison between Memory vector computation of NPU architecture version 2201 and Reg vector computation of NPU architecture version 3510<a id="fig8"></a>

![](../../../../figures/mem_reg_vector_mask_compare.png "Mask mechanism comparison between Memory vector computation of NPU architecture version 2201 and Reg vector computation of NPU architecture version 3510")

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
