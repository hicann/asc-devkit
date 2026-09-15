# RegTensor<a name="ZH-CN_TOPIC_0000001928794188"></a>

<!-- md-trans-meta sourceCommit=6faa1d5619596f551c68a1ae05a92113e7564601 translatedAt=2026-08-27T22:00:15.319Z -->

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

The **RegTensor** register is the basic unit of vector computation. Its bit width is VL (Vector Length), with a length of 256B. For the b64 or complex32 data type, the storage can be extended to 2VL through the **RegTrait** template parameter for complex computation.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, const RegTrait& regTrait = RegTraitNumOne> struct RegTensor;
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :-- | :------------ |
| T | Operand data type. Supported data types (widths) are b8, b16, b32, and b64.<br>&bull; b8: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t (the two b4 types fp4x2_e2m1_t and fp4x2_e1m2_t must be arranged in pairs on the Vector side, appearing as the b8 type; int4b_t is also expressed using the b8 type; the bool data type supports only data transfer).<br>&bull; b16: int16_t, uint16_t, half, bfloat16_t.<br>&bull; b32: int32_t, uint32_t, float, complex32.<br>&bull; b64: int64_t, uint64_t, complex64.| 
| regTrait | Type RegTrait, configurable as RegTraitNumOne or RegTraitNumTwo.<br>&bull; RegTraitNumOne: This RegTensor type contains one vector Reg of the corresponding data type, with a length of VL. Supported data types are b8, b16, b32, and b64.<br>&bull; RegTraitNumTwo: This RegTensor type contains two vector Regs of the corresponding data type, each with a length of VL, for a total length of 2*VL. Supported data types are complex32 and b64.|

## Constraints<a name="section177921451558"></a>

-   The maximum number of RegTensor registers is 32. Register data exceeding this limit is written to the reserved 8K UB memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycle has ended. If both registers and reserved memory have available space, registers are reused first.
-   The lifecycle of a register is limited to within a single VF.

## Key Features

-   Storage structure of the complex32 **RegTensor**

    The figure below shows the storage condition of the **RegTensor** for complex32 in the **RegTraitNumOne** and **RegTraitNumTwo** scenarios:

    **Figure 1** RegTensor transfer of complex32<a id="fig1"></a>

    ![](../../../../figures/reg_tensor_move_complex32.png "RegTensor transfer of complex32")

    complex32 is a composite type that contains two half values (real part and imaginary part). It is usually stored contiguously, with the real part in the lower bits and the imaginary part in the higher bits.

    In the **RegTraitNumOne** scenario, data of **VL** volume is transferred from UB (**src0Addr**) in **DIST_NORM** mode and stored contiguously in the **RegTensor**.

    In the **RegTraitNumTwo** scenario, data of 2*VL volume is read from UB (**src0Addr**) in **DIST_DINTLV_B16** dual-load mode, and the complex32 data is transferred with data interleaving. The elements at even indexes (real part) are stored in reg[0], and the elements at odd indexes (imaginary part) are stored in reg[1], with the data type being uint16_t. The two **RegTensor** instances store 512B of data. reg[0] stores the first 16 bits (real part) of 128 complex32 values, and reg[1] stores the last 16 bits (imaginary part) of 128 complex32 values.

-   Storage structure of the complex64 **RegTensor**

    The figure below shows the storage condition of the **RegTensor** for complex64 in the **RegTraitNumOne** and **RegTraitNumTwo** scenarios:

    **Figure 2** RegTensor transfer of complex64<a id="fig2"></a>

    ![](../../../../figures/reg_tensor_move_complex64.png "RegTensor transfer of complex64")

    complex64 is a composite type that contains two float values (real part and imaginary part). It is usually stored contiguously, with the real part in the lower bits and the imaginary part in the higher bits.

    In the RegTraitNumOne scenario, VL data volume is transferred from UB (src0Addr) in DIST_NORM mode and stored contiguously in RegTensor.

    In the RegTraitNumTwo scenario, 2*VL data volume is read from UB (src0Addr) in DIST_DINTLV_B32 dual-load mode, and the complex64 data is transferred with data interleaving. Elements at even indexes (real part) are stored in reg[0], and elements at odd indexes (imaginary part) are stored in reg[1], with the data type being uint32_t. The two RegTensors store 512B of data volume. reg[0] stores the first 32 bits (real part) of the 64 complex64 values, and reg[1] stores the last 32 bits (imaginary part) of the 64 complex64 values.

-   Storage structure of the b64 type RegTensor

    The figure below shows the storage condition of RegTensor for b64 (uint64_t, int64_t) in the RegTraitNumOne and RegTraitNumTwo scenarios:

    **Figure 3** RegTensor transfer of b64<a id="fig3"></a>

    ![](../../../../figures/reg_tensor_move_b64.png "RegTensor transfer of b64")

    In the RegTraitNumOne scenario, VL data volume is transferred from UB (src0Addr) in DIST_NORM mode.

    In the **RegTraitNumTwo** scenario, read 2*VL data volume from UB (**src0Addr**) in **DIST_DINTLV_B32** dual-load mode, transfer the b64 data by interleaving, store the elements at even indexes (low bits) into reg[0], and store the elements at odd indexes (high bits) into reg[1], with the data type being b32. The two RegTensor objects store 512B of data volume, where reg[0] stores the first 32 bits (low bits) of the 64 b64 values, and reg[1] stores the last 32 bits (high bits) of the 64 b64 values.

## Example<a name="section1398164912391"></a>

-   Example 1

    ```cpp
    AscendC::Reg::RegTensor<uint32_t> reg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<uint32_t>();
    AscendC::Reg::LoadAlign(reg, src, 0);
    AscendC::Reg::Adds(reg, reg, 1);
    AscendC::Reg::StoreAlign(dst, reg, 0, mask);
    ```

-   Example 2

    ```cpp
    // For B64, RegTraitNumTwo can be passed.
    template<typename T, const AscendC::Reg::RegTrait& Trait = AscendC::Reg::RegTraitNumOne>
    __simd_vf__ inline void AddVF(__ubuf__ T* dstAddr, __ubuf__ T* src0Addr, __ubuf__ T* src1Addr, uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
    {
        AscendC::Reg::RegTensor<T,Trait> srcReg0;
        AscendC::Reg::RegTensor<T,Trait> srcReg1;
        AscendC::Reg::RegTensor<T,Trait> dstReg;
        AscendC::Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            mask = AscendC::Reg::UpdateMask<T,Trait>(count);
            AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
            AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
            AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
            AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
        }
    }
    ```
