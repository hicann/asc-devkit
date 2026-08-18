# 指令双发优化<a name="ZH-CN_TOPIC_0000002498168810"></a>

指令双发指的是处理器在同一个时钟周期内，能够同时发射两条指令到执行单元进行处理。这种能力需要满足以下两个条件：

-   两条指令之间没有数据上的依赖关系（依赖关系指后一条指令需要使用前一条指令产生的结果）
-   硬件中拥有足够的执行资源

这种机制可以在不改变程序逻辑的前提下，提升处理器在单位时间内的指令处理效率，是实现指令级并行的重要基础之一。

如下示例中，VLoop-1循环16次，由于每个循环内的4条指令有数据依赖，所以执行队列的深度是64，循环内指令无法实现双发。指令基于循环展开后，64条指令并发执行顺序如下图所示，LoadAlign\_0和LoadAlign\_1没有依赖关系，可以并发执行。黑框选中的位置仅代表该4条指令有同时执行的资格，真正执行时是乱序选取其中的2条执行。

```cpp
for (uint16_t i = 0; i < 16; ++i) { // VLoop-1
    // 循环16次，每次4条指令
    // 数据依赖性：Adds依赖LoadAlign，Mul依赖Adds ...
    mask = AscendC::Reg::UpdateMask<T>(count);
    int16_t scalar = 2;
    AscendC::Reg::LoadAlign(srcReg, src0Addr + i * oneRepeatSize);
    AscendC::Reg::Adds(dstReg1, srcReg, scalar , mask);
    AscendC::Reg::Mul(dstReg2, dstReg1, srcReg, mask);
    AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg2, mask);
}
```

**图1**  执行队列和指令的执行顺序<a name="fig695403454218"></a>  
![](../../../../figures/exec_order.png "执行队列和指令的执行顺序")

在编写算子时，开发者通常习惯于按“加载数据 → 处理计算 → 存储结果”的顺序来组织代码流程。这种写法在寄存器资源充足时运行良好，但一旦资源紧张，问题就会被放大。当多个计算指令之间会产生依赖关系，这些等待会堆积在执行队列中，导致后续指令无法及时发射。

开发者在编程时应**尽可能保证队列里存在数目充足且无依赖的并发指令**，从而高效的利用硬件双发特性。可以通过合理拆分VF循环以及手动控制循环展开等方案来提高性能。

## 合理拆分VF循环<a name="section136981958175916"></a>

VF并不是写的越长，把所有运算都放在一个for循环内就好，需要适当的搬出中间结果到Unified Buffer（UB），减少数据依赖。

优化前：

```cpp
for (uint16_t i = 0; i < 32; ++i) { // VLoop-1
    // 数据依赖性：每一条Adds的输入都依赖上一条指令的结果
    mask = AscendC::Reg::UpdateMask<T>(count);
    AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
    AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
    AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
}
```

优化后：

```cpp
for (uint16_t i = 0; i < 32; ++i) { // VLoop-1
    mask = AscendC::Reg::UpdateMask<T>(count);
    AscendC::Reg::LoadAlign(srcReg0, src0Addr + i * oneRepeatSize);
    AscendC::Reg::LoadAlign(srcReg1, src1Addr + i * oneRepeatSize);
    AscendC::Reg::Add(dstReg, srcReg0, srcReg1, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
}
for (uint16_t i = 0; i < 32; ++i) { // VLoop-2
    mask = AscendC::Reg::UpdateMask<T>(count);
    AscendC::Reg::LoadAlign(dstReg, dstAddr + i * oneRepeatSize);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::Adds(dstReg, dstReg, 10, mask);
    AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
}
```

## 手动控制循环拆分<a name="section18444625803"></a>

如果循环内存在依赖关系过多的指令，队列没有办法同时加载进for\(i = n\)和for\(i = n+1\)的所有指令，那么即使循环之间没有依赖关系，也无法启用双发特性，指令无法并发执行。可以通过手动展开循环，这样做有两个好处：贴近硬件乱序执行的特性，为下发的指令创造更多执行的机会；减少指令因为寄存器资源未到位而产生的等待。

当前支持以下两种展开方式：

| 展开方式 | 开启方式 | 优点 | 缺点 |
| :-- | :-- | :-- | :-- |
| 手动展开VF循环 | / | &bull;自主可控；<br>&bull;能处理复杂循环或src/dst寄存器复用等场景； | &bull;代码复杂度高；|
| 编译器自动展开 | #pragma unroll number （number表示展开层数）| &bull;代码复杂度低；| &bull;src/dst寄存器复用时无法展开。 |
>[!NOTE]说明
>
> 循环展开通过消除指令依赖，提升指令双发能力来优化VF性能，需根据实际计算过程使用，不保证产生收益。展开时，若寄存器超出数量上限，可能会造成性能劣化或其他非预期行为。

### 手动展开VF循环
```cpp
for (uint16_t i = 0; i < 32; ++i) { 
// for循环间没有依赖关系：i=0,i=1可以并行执行，但是由于循环内数据依赖指令过多导致i=1的指令无法加载进执行队列中
    AscendC::Reg::LoadAlign(srcReg, srcAddr, count);
    AscendC::Reg::Adds(dstReg0, srcReg, 10, mask);
    AscendC::Reg::Muls(dstReg1, dstReg0, 20, mask);
    ... // 超过64条有数据依赖的指令
    AscendC::Reg::StoreAlign(dstAddr, dstReg1, count, mask);
}
```

展开后

```cpp
for (uint16_t i = 0; i < 8; ++i) { // 32做4展开
    AscendC::Reg::LoadAlign(srcReg0, srcAddr, count);
    AscendC::Reg::LoadAlign(srcReg1, srcAddr, count);
    AscendC::Reg::LoadAlign(srcReg2, srcAddr, count);
    AscendC::Reg::LoadAlign(srcReg3, srcAddr, count);
    AscendC::Reg::Adds(...);
    AscendC::Reg::Adds(...);
    AscendC::Reg::Adds(...);
    AscendC::Reg::Adds(...);
    AscendC::Reg::Muls(...);
    AscendC::Reg::Muls(...);
    AscendC::Reg::Muls(...);
    AscendC::Reg::Muls(...);
    ...
    AscendC::Reg::StoreAlign(...);
    AscendC::Reg::StoreAlign(...);
    AscendC::Reg::StoreAlign(...);
    AscendC::Reg::StoreAlign(...);
}
```

### 编译器自动展开
```cpp
#pragma unroll 4 
for (uint16_t i = 0; i < 32; ++i) {  
    AscendC::Reg::LoadAlign(srcReg, srcAddr + i * offset); 
    AscendC::Reg::Adds(dstReg0, srcReg, 10, mask); 
    AscendC::Reg::Muls(dstReg1, dstReg0, 20, mask); 
    AscendC::Reg::StoreAlign(dstAddr, dstReg1, i * offset, mask); 
}
```

## 避免寄存器数量超限，导致执行队列中依赖指令的数量增加<a name="section1850233910114"></a>

在同一个VF中，硬件可以同时处理的最大RegTensor寄存器个数为32，如果超出，编译器会进行数据的换入换出并加入同步指令，严重拖慢算子的执行效率。
在同一个VF中，MaskReg数量上限为8个，超过8个会产生性能劣化。
在同一个VF中，AddrReg数量上限为8个，超过8个会产生性能劣化。
在同一个VF中，读写寄存器UnalignRegForLoad、UnalignRegForStore的数量上限均为4个，若实际使用超过4个UnalignRegForLoad或UnalignRegForStore会直接导致编译失败。

优化方案：

-   使用布尔代数运算。比如!\(a && b\)可简化为!a || !b，!\(a || b\)可以化简为!a && !b。
-   适当等价调整指令顺序，节省寄存器。

本示例用于判断两个double类型数据是否相等，需要处理两个特殊场景，NaN场景以及+0和-0相等场景。

```
template<typename T = Reg::DefaultType, CMPMODE mode = CMPMODE::EQ, typename RegT>
__simd_callee__ inline void CompareDoubleImpl(Reg::MaskReg &dstMask, RegT &srcReg0, RegT &srcReg1, Reg::MaskReg &mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(SupportType<ActualT, double, uint64_t>(), "CompareDoubleImpl only support double and uint64_t type");

    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg0 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg1 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg1;

    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> exponent0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> exponent1;

    Reg::ShiftRights(exponent0, tmpSrcReg0, static_cast<int16_t>(52), mask);
    Reg::ShiftRights(exponent1, tmpSrcReg1, static_cast<int16_t>(52), mask);

    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarExponent;
    Reg::Duplicate(scalarExponent, static_cast<uint64_t>(0x7ff), mask);
    Reg::And(exponent0, exponent0, scalarExponent, mask);
    Reg::And(exponent1, exponent1, scalarExponent, mask);

    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> mantissa0, mantissa1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarMantissa;
    Reg::Duplicate(scalarMantissa, static_cast<uint64_t>(0xfffffffffffff), mask);
    Reg::And(mantissa0, tmpSrcReg0, scalarMantissa, mask);
    Reg::And(mantissa1, tmpSrcReg1, scalarMantissa, mask);

    Reg::MaskReg tmpMask0;
    Reg::Compares(tmpMask0, exponent0, 0x7ff, mask);
    Reg::Compares(dstMask, exponent1, 0x7ff, mask);
    Reg::MaskAnd(dstMask, tmpMask0, dstMask, mask);
    // dstMask表示两个double数的指数部分是否均为0x7ff；接下来判断两个数的尾数部分是否至少一个不为0，结果保存到tmpMask0
    // 当指数均为0x7ff且至少一个尾数不为0时，表示存在NaN场景
    Reg::MaskReg tmpMask1;
    Reg::Compares<uint64_t, CMPMODE::NE>(tmpMask1, mantissa0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::NE>(tmpMask0, mantissa1, 0, mask);
    Reg::MaskOr(tmpMask0, tmpMask1, tmpMask0, mask);
    //【反例】判断指数全为1，尾数至少一个不为0（结果为NAN）的特殊情况，用到的公式为!(a&&b)，需要多申请一个MaskReg即noNaNMask来记录a&&b中间结果
    Reg::MaskReg noNaNMask;
    Reg::MaskAnd(noNaNMask, dstMask, tmpMask0, mask);
    Reg::MaskNot(noNaNMask, noNaNMask, mask);
    // 判断+0和-0的特殊情况
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> unsignedPart0, unsignedPart1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarUnsignedPart;
    Reg::Duplicate(scalarUnsignedPart, static_cast<uint64_t>(0x7fffffffffffff), mask);
    Reg::And(unsignedPart0, tmpSrcReg0, scalarUnsignedPart, mask);
    Reg::And(unsignedPart1, tmpSrcReg1, scalarUnsignedPart, mask);
    //【反例】先分别判断两个无符号数是否为0，再通过MaskAnd合并两个判断结果
    // 该写法相较优化后的写法，需要使用dstMask额外保存unsignedPart1是否为0的结果，并额外执行一次MaskAnd指令
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask0, unsignedPart0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::EQ>(dstMask, unsignedPart1, 0, mask);
    Reg::MaskAnd(tmpMask0, tmpMask0, dstMask, mask);

    Reg::Compare(dstMask, tmpSrcReg0, tmpSrcReg1, mask);
    Reg::MaskAnd(dstMask, dstMask, noNaNMask, mask);
    Reg::MaskOr(dstMask, dstMask, tmpMask0, mask);
}
```

优化后：

```
template <typename T = Reg::DefaultType, CMPMODE mode = CMPMODE::EQ, typename RegT>
__simd_callee__ inline void CompareDoubleImpl(Reg::MaskReg &dstMask, RegT &srcReg0, RegT &srcReg1, Reg::MaskReg &mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(SupportType<ActualT, double, uint64_t>(), "CompareDoubleImpl only support double and uint64_t type");

    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg0 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg1 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> exponent0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> exponent1;

    Reg::ShiftRights(exponent0, tmpSrcReg0, static_cast<int16_t>(52), mask);
    Reg::ShiftRights(exponent1, tmpSrcReg1, static_cast<int16_t>(52), mask);
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarExponent;
    Reg::Duplicate(scalarExponent, static_cast<uint64_t>(0x7ff), mask);
    Reg::And(exponent0, exponent0, scalarExponent, mask);
    Reg::And(exponent1, exponent1, scalarExponent, mask);

    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> mantissa0, mantissa1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarMantissa;
    Reg::Duplicate(scalarMantissa, static_cast<uint64_t>(0xfffffffffffff), mask);
    Reg::And(mantissa0, tmpSrcReg0, scalarMantissa, mask);
    Reg::And(mantissa1, tmpSrcReg1, scalarMantissa, mask);

    Reg::MaskReg tmpMask0, tmpMask1;
    Reg::Compares(tmpMask0, exponent0, 0x7ff, mask);
    Reg::Compares(dstMask, exponent1, 0x7ff, tmpMask0);
    Reg::MaskNot(dstMask, dstMask, mask);
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask1, mantissa0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask0, mantissa1, 0, tmpMask1);
    //【正例】!(a&&b)化简为!a||!b，表示指数不全为1或者tmpMask0均为0时候，可以正常进行判断，不需要多申请寄存器
    Reg::MaskOr(tmpMask0, tmpMask0, dstMask, mask);

    Reg::Compare(dstMask, tmpSrcReg0, tmpSrcReg1, mask);
    Reg::MaskAnd(dstMask, dstMask, tmpMask0, mask);
    // +0 -0
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> unsignedPart0, unsignedPart1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarUnsignedPart;
    Reg::Duplicate(scalarUnsignedPart, static_cast<uint64_t>(0x7fffffffffffff), mask);
    Reg::And(unsignedPart0, tmpSrcReg0, scalarUnsignedPart, mask);
    Reg::And(unsignedPart1, tmpSrcReg1, scalarUnsignedPart, mask);
    //【正例】先判断unsignedPart0是否为0，并将结果保存到tmpMask0；再将tmpMask0作为判断unsignedPart1是否为0的输入mask
    // 使tmpMask1直接表示unsignedPart0和unsignedPart1同时为0的结果，相较于反例省略了一条MaskAnd指令
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask0, unsignedPart0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask1, unsignedPart1, 0, tmpMask0);
    Reg::MaskOr(dstMask, dstMask, tmpMask1, mask);
}
```
