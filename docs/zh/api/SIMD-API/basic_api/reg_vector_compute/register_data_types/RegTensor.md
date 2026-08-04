# RegTensor<a name="ZH-CN_TOPIC_0000001928794188"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

RegTensor寄存器是矢量计算基本单元，RegTensor位宽为VL（Vector Length），长度为256B。b64或complex32类型下，支持通过RegTrait模板参数扩展存储2VL用于复杂计算。

## 定义原型<a name="section620mcpsimp"></a>

```cpp
template <typename T, const RegTrait& regTrait = RegTraitNumOne> struct RegTensor;
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| :-- | :------------ |
| T | 操作数数据类型，支持的数据类型(宽度)为b8、b16、b32、b64。<br>&bull; b8：bool、int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、fp8_e8m0_t（fp4x2_e2m1_t、fp4x2_e1m2_t这两个b4类型在Vector侧的内存排布需要为两个一组，表现为b8类型；int4b_t也使用b8类型表达；bool数据类型只支持数据搬运）。<br>&bull; b16：int16_t、uint16_t、half、bfloat16_t。<br>&bull; b32：int32_t、uint32_t、float、complex32。<br>&bull; b64：int64_t、uint64_t、complex64。| 
| regTrait | 类型为RegTrait，支持配置为RegTraitNumOne或RegTraitNumTwo。<br>&bull; RegTraitNumOne：该RegTensor类型中包含1个相应数据类型的矢量Reg，长度为VL。支持的数据类型为：b8、b16、b32、b64。<br>&bull; RegTraitNumTwo：该RegTensor类型中包含2个相应数据类型的矢量Reg，每个矢量Reg长度为VL，总长度为2*VL。支持的数据类型为：complex32、b64。|

## 约束说明<a name="section177921451558"></a>

-   RegTensor寄存器数量上限为32。超出限制上限的寄存器数据会写入预留的8KUB内存中，可能会引起性能劣化。编译器会自动复用生命周期结束的寄存器和预留内存，若寄存器与预留内存均存在可用空间，将优先复用寄存器。
-   寄存器的生命周期限定于单个VF内部。

## 关键特性

-   complex32类型RegTensor存储结构

    下图为complex32在RegTraitNumOne和RegTraitNumTwo场景下RegTensor存储情况：

    **图1** RegTensor搬运complex32<a id="fig1"></a>

    ![](../../../../figures/RegTensor搬运complex32.png "RegTensor搬运complex32")

    complex32是一个包含两个half（实部real、虚部imag）类型的复合类型，通常是连续存储，低位为实部高位为虚部。
    
    在RegTraitNumOne场景下，从UB（src0Addr）中以DIST_NORM模式搬运VL数据量，在RegTensor中连续存储。
    
    在RegTraitNumTwo场景下，从UB（src0Addr）中以DIST_DINTLV_B16双搬入模式读取2*VL数据量，将complex32数据交错搬运，偶数索引（实部）的元素存入reg[0]，将奇数索引（虚部）的元素存入reg[1]，数据类型为uint16_t。两个RegTensor存储512B的数据量，reg[0]存的是128个complex32的前16位（实部），reg[1]存的是128个complex32的后16位（虚部）。

-   complex64类型RegTensor存储结构

    下图为complex64在RegTraitNumOne和RegTraitNumTwo场景下RegTensor存储情况：

    **图2** RegTensor搬运complex64<a id="fig2"></a>

    ![](../../../../figures/RegTensor搬运complex64.png "RegTensor搬运complex64")

    complex64是一个包含两个float（实部real、虚部imag）类型的复合类型，通常是连续存储，低位为实部高位为虚部。
    
    在RegTraitNumOne场景下，从UB（src0Addr）中以DIST_NORM模式搬运VL数据量，在RegTensor中连续存储。
    
    在RegTraitNumTwo场景下，从UB（src0Addr）中以DIST_DINTLV_B32双搬入模式读取2*VL数据量，将complex64数据交错搬运，偶数索引（实部）的元素存入reg[0]，将奇数索引（虚部）的元素存入reg[1]，数据类型为uint32_t。两个RegTensor存储512B的数据量，reg[0]存的是64个complex64的前32位（实部），reg[1]存的是64个complex64的后32位（虚部）。

-   b64类型RegTensor存储结构

    下图为b64（uint64_t、int64_t）在RegTraitNumOne和RegTraitNumTwo场景下RegTensor存储情况：

    **图3** RegTensor搬运b64<a id="fig3"></a>

    ![](../../../../figures/RegTensor搬运b64.png "RegTensor搬运b64")

    在RegTraitNumOne场景下，从UB（src0Addr）中以DIST_NORM模式搬运VL数据量。
    
    在RegTraitNumTwo场景下，从UB（src0Addr）中以DIST_DINTLV_B32双搬入模式读取2*VL数据量，将b64数据交错搬运，偶数索引（低位）的元素存入reg[0]，将奇数索引（高位）的元素存入reg[1]，数据类型为b32。两个RegTensor存储512B的数据量，reg[0]存的是64个b64的前32位（低位），reg[1]存的是64个b64的后32位（高位）。

## 调用示例<a name="section1398164912391"></a>

-   示例一

    ```cpp
    AscendC::Reg::RegTensor<uint32_t> reg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<uint32_t>();
    AscendC::Reg::LoadAlign(reg, src, 0);
    AscendC::Reg::Adds(reg, reg, 1);
    AscendC::Reg::StoreAlign(dst, reg, 0, mask);
    ```

-   示例二

    ```cpp
    // 针对B64,可以传入RegTraitNumTwo
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
