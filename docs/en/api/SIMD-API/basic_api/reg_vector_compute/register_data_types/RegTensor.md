# RegTensor<a name="ZH-CN_TOPIC_0000001928794188"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T17:05:18.139Z pushedAt=2026-10-26T07:41:44.601Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/RegTensor_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The **RegTensor** register is the basic unit of vector computation. The bit width of **RegTensor** is VL (Vector Length), and its length is 256B. For the b64 or complex32 type, storage can be extended to 2VL through the **RegTrait** template parameter for complex computation.

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T, const RegTrait& regTrait = RegTraitNumOne> struct RegTensor;
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Template parameter description

| Parameter | Description |
| :-- | :------------ |
| T | Operand data type. Supported data types (width) are b8, b16, b32, and b64.<br>&bull; b8: bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, and fp8_e8m0_t. (The two b4 types fp4x2_e2m1_t and fp4x2_e1m2_t must be arranged in pairs in memory on the Vector side, represented as the b8 type. int4b_t is also represented as the b8 type. For the bool data type, only data transfer is supported.)<br>&bull; b16: int16_t, uint16_t, half, and bfloat16_t.<br>&bull; b32: int32_t, uint32_t, float, and complex32.<br>&bull; b64: int64_t, uint64_t, and complex64.| 
| regTrait | Type is **RegTrait**, which can be configured as **RegTraitNumOne** or **RegTraitNumTwo**.<br>&bull; **RegTraitNumOne**: This **RegTensor** type contains one vector Reg of the corresponding data type, with a length of VL. Supported data types are b8, b16, b32, and b64.<br>&bull; **RegTraitNumTwo**: This **RegTensor** type contains two vector Regs of the corresponding data type, each with a length of VL, and the total length is 2*VL. Supported data types are complex32 and b64.|

## Constraints<a name="section177921451558"></a>

-   The maximum number of **RegTensor** registers is 32. Register data exceeding this limit is written to the reserved 8K **UB** memory, which may cause performance degradation. The compiler automatically reuses registers and reserved memory whose lifecycles have ended. If both registers and reserved memory have available space, registers are reused first.
-   The lifecycle of a register is limited to within a single **VF**.

## Key Features

-   Storage structure of the **complex32** RegTensor

    The following figure shows the storage status of the **complex32** RegTensor in the **RegTraitNumOne** and **RegTraitNumTwo** scenarios:

    **Figure 1** RegTensor transfer of complex32<a id="fig1"></a>

    ![](../../../../figures/regtensor_move_complex32.png "RegTensor transfer of complex32")

    **complex32** is a composite type that contains two **half** types (real part and imaginary part). It is usually stored contiguously, with the real part in the low bits and the imaginary part in the high bits.

    In the **RegTraitNumOne** scenario, a data volume of **VL** is transferred from the Unified Buffer (**UB**, **src0Addr**) in **DIST_NORM** mode and stored contiguously in the **RegTensor**.

    In the **RegTraitNumTwo** scenario, a data volume of 2*VL is read from the UB (**src0Addr**) in **DIST_DINTLV_B16** dual-load mode, and the **complex32** data is interleaved. The elements at even indexes (real parts) are stored in **reg[0]**, and the elements at odd indexes (imaginary parts) are stored in **reg[1]**, with the data type being **uint16_t**. The two **RegTensors** store a data volume of 512B. **reg[0]** stores the first 16 bits (real part) of the 128 **complex32** values, and **reg[1]** stores the last 16 bits (imaginary part) of the 128 **complex32** values.

-   Storage structure of the **complex64** RegTensor

    The following figure shows the storage status of the **complex64** RegTensor in the **RegTraitNumOne** and **RegTraitNumTwo** scenarios:

    **Figure 2** RegTensor transfers complex64<a id="fig2"></a>

    ![](../../../../figures/regtensor_move_complex64.png "RegTensor transfers complex64")

    complex64 is a composite type that contains two float values (real part and imaginary part). It is usually stored contiguously, with the real part in the low bits and the imaginary part in the high bits.

    In the RegTraitNumOne scenario, VL data volume is transferred from UB (src0Addr) in DIST_NORM mode and stored contiguously in RegTensor.

In the RegTraitNumTwo scenario, a data volume of 2*VL is read from UB (src0Addr) in DIST_DINTLV_B32 dual-load mode, and the complex64 data is transferred in an interleaved manner. Elements at even indexes (real part) are stored in reg[0], and elements at odd indexes (imaginary part) are stored in reg[1], with the data type being uint32_t. The two RegTensors store 512B of data. reg[0] stores the first 32 bits (real part) of the 64 complex64 values, and reg[1] stores the last 32 bits (imaginary part) of the 64 complex64 values.

-   b64 type RegTensor storage structure

    The following figure shows the RegTensor storage status of b64 (uint64_t, int64_t) in the RegTraitNumOne and RegTraitNumTwo scenarios:

    **Figure 3** RegTensor transfers b64<a id="fig3"></a>

    ![](../../../../figures/regtensor_move_b64.png "RegTensor transfers b64")

    In the RegTraitNumOne scenario, VL data volume is transferred from UB (src0Addr) in DIST_NORM mode.

    In the **RegTraitNumTwo** scenario, read 2*VL data volume from **UB** (**src0Addr**) in **DIST_DINTLV_B32** dual-load mode, transfer the b64 data in an interleaved manner, store the elements at even indexes (low bits) into **reg[0]**, and store the elements at odd indexes (high bits) into **reg[1]**, with the data type being b32. The two **RegTensor** objects store 512B of data volume, where **reg[0]** stores the first 32 bits (low bits) of the 64 b64 values, and **reg[1]** stores the last 32 bits (high bits) of the 64 b64 values.

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
    // For B64, RegTraitNumTwo can be passed in.
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
