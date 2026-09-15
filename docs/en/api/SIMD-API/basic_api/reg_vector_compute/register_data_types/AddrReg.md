# AddrReg<a name="ZH-CN_TOPIC_0000002081579973"></a>

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-08-27T21:58:35.725Z -->

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

The header file reference path is **basic_api/reg_compute/kernel_reg_compute_addrreg_intf.h**.

**AddrReg** is a dedicated register used by the Reg vector computation APIs to manage address offsets. After being defined outside a loop, it is assigned and bound to the loop axis index and stride of each loop level by calling the **CreateAddrReg\<T\>** template function inside the loop, which accumulates the address offset level by level in multi-dimensional loops. The **CreateAddrReg** API supports binding 1 to 4 levels of for loops. The returned address offset is determined by index*stride. In a loop, index increments by 1 each time, and the offset of **AddrReg** automatically increases by the corresponding stride. The specific offset satisfies the following pseudo code:

```cpp
// 1-layer loop.
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0);  // Offset = index0 * stride0 
} 
    
// 2-layer loop.
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    for (uint16_t index1 = 0; index1 < loopNum1; ++index1){ 
        aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0, index1, stride1);  // Offset = index0 * stride0 + index1 * stride1 
    } 
} 
 
// 3-layer loop.
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    for (uint16_t index1 = 0; index1 < loopNum1; ++index1){ 
        for (uint16_t index2 = 0; index2 < loopNum2; ++index2){ 
            aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0, index1, stride1, index2, stride2);  // Offset = index0 * stride0 + index1 * stride1 + index2 * stride2 
        } 
    } 
} 
 
// 4-layer loop.
AscendC::Reg::AddrReg aReg; 
for (uint16_t index0 = 0; index0 < loopNum0; ++index0){ 
    for (uint16_t index1 = 0; index1 < loopNum1; ++index1){ 
        for (uint16_t index2 = 0; index2 < loopNum2; ++index2){ 
            for (uint16_t index3 = 0; index3 < loopNum3; ++index3){ 
                aReg = AscendC::Reg::CreateAddrReg<T>(index0, stride0, index1, stride1, index2, stride2, index3, stride3);  // Offset = index0 * stride0 + index1 * stride1 + index2 * stride2 + index3 * stride3 
            } 
        } 
    } 
}
```

**AddrReg** should be initialized through the **CreateAddrReg** API and then used in the loop to store the address offset. **AddrReg** increments automatically in each loop according to the configured stride.

## Function Prototype<a name="section620mcpsimp"></a>

```cpp
// offset = index0 * stride0 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0) 
  
// offset = index0 * stride0 + index1 * stride1 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1) 
  
// offset = index0 * stride0 + index1 * stride1 + index2 * stride2 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1, uint16_t index2, uint32_t stride2)
 
// offset = index0 * stride0 + index1 * stride1 + index2 * stride2 + index3 * stride3 
template <typename T> 
__simd_callee__ inline AddrReg CreateAddrReg(uint16_t index0, uint32_t stride0, uint16_t index1, uint32_t stride1, uint16_t index2, uint32_t stride2, uint16_t index3, uint32_t stride3)
```

## Parameters<a name="section132601254123919"></a>

| Parameter | Description |
| :-- | :------------ | 
| T | Template parameter. Supported data types are **b8**, **b16**, **b32**, and **b64**. |
| index0 | Used as the outermost loop axis when calculating the offset. |
| index1 | Used as the second-level loop axis when calculating the offset. |
| index2 | Used as the third-level loop axis when calculating the offset. |
| index3 | Used as the fourth-level loop axis when calculating the offset. |
| stride0 | Address offset corresponding to the outermost loop axis in each loop. The unit is element. |
| stride1 | Address offset corresponding to the second-level loop axis in each loop. The unit is element. |
| stride2 | Address offset corresponding to the third-level loop axis in each loop. The unit is element. |
| stride3 | Address offset corresponding to the fourth-level loop axis in each loop. The unit is element. |

## Supported Models<a name="section156721693504"></a>

Ascend 950PR/Ascend 950DT

## Constraints<a name="section11585101304320"></a>

- **AddrReg** is an address offset register and is only supported by some copy instructions. Select it based on the prototype of the Reg data copy API. In addition to setting the address offset through **AddrReg**, you can accumulate the address manually or increment the address through the **PostUpdate** mode. For the complete features, see the key features of continuous aligned copy-in. When copying through the **AddrReg** address offset, the address alignment constraints of the corresponding copy instruction must be met.
- The maximum number of **AddrReg** registers is 8.
- Due to the hardware loop (**HardwareLoop**) limitation, **AddrReg** supports at most 4 loop axes.

## Example<a name="section633mcpsimp"></a>

```cpp
__simd_vf__ inline void CreateAddrRegVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    AscendC::Reg::AddrReg aReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        aReg = AscendC::Reg::CreateAddrReg<T>(i, oneRepeatSize);
        AscendC::Reg::LoadAlign(mask, srcAddr, aReg);
        AscendC::Reg::StoreAlign(dstAddr, mask, aReg);
    }
}
```
