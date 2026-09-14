# Dual-Issue Optimization<a name="ZH-CN_TOPIC_0000002498168810"></a>

<!-- md-trans-meta sourceCommit=d22d62b20baad2e627e0f0946c4cd2b68bc1751e translatedAt=2026-08-26T14:28:39.353Z -->

Dual-issue means that the processor can issue two instructions to the execution units simultaneously within the same clock cycle. This capability requires the following two conditions to be met:

-   There is no data dependency relationship between the two instructions (a dependency relationship means that the latter instruction needs to use the result produced by the former instruction)
-   The hardware has sufficient execution resources

This mechanism can improve the instruction processing efficiency of the processor per unit time without changing the program logic, and is one of the important foundations for implementing instruction-level parallelism.

In the following example, VLoop-1 loops 16 times. Because the four instructions in each loop have data dependencies, the depth of the execution queue is 64, and the instructions within the loop cannot be dual-issued. After the instructions are loop-unrolled, the concurrent execution order of the 64 instructions is shown in the following figure. LoadAlign\_0 and LoadAlign\_1 have no dependency relationship and can be executed concurrently. The positions selected by the black boxes only indicate that these four instructions are eligible for simultaneous execution; during actual execution, two of them are selected out of order for execution.

```cpp
for (uint16_t i = 0; i < 16; ++i) { // VLoop-1
    // Loop 16 times, with four instructions each time
    // Data dependency: Adds depends on LoadAlign, Mul depends on Adds ...
    mask = AscendC::Reg::UpdateMask<T>(count);
    int16_t scalar = 2;
    AscendC::Reg::LoadAlign(srcReg, src0Addr + i * oneRepeatSize);
    AscendC::Reg::Adds(dstReg1, srcReg, scalar , mask);
    AscendC::Reg::Mul(dstReg2, dstReg1, srcReg, mask);
    AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg2, mask);
}
```

**Figure 1**  Execution queue and instruction execution order<a name="fig695403454218"></a>  
![](../../../../figures/queue_and_instruction_execution_order.png "Execution queue and instruction execution order")

When writing operators, developers usually organize the code flow in the order of "load data → process computation → store results". This approach works well when register resources are sufficient, but once resources become tight, the problem is amplified. When dependency relationships arise among multiple computation instructions, these waits accumulate in the execution queue, preventing subsequent instructions from being issued in a timely manner.

When programming, developers should **ensure that the queue contains a sufficient number of concurrent instructions without dependencies** to efficiently leverage the hardware dual-issue capability. Performance can be improved by properly splitting VF loops and manually controlling loop unrolling.

## Properly Splitting the VF Loop<a name="section136981958175916"></a>

A longer VF is not necessarily better, and it is not advisable to place all operations in a single for loop. Instead, intermediate results should be moved out to the UB as appropriate to reduce data dependency.

Before optimization:

```cpp
for (uint16_t i = 0; i < 32; ++i) { // VLoop-1
    // Data dependency: the input of each Adds instruction depends on the result of the previous instruction
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

After optimization:

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

## Manually Controlling Loop Splitting<a name="section18444625803"></a>

If a loop contains too many instructions with dependency relationships, the queue cannot load all instructions of for\(i = n\) and for\(i = n+1\) at the same time. In this case, even if there is no dependency relationship between loops, the dual-issue feature cannot be enabled, and the instructions cannot be executed concurrently. You can manually unroll the loop, which offers two benefits: it aligns with the out-of-order execution characteristics of the hardware, creating more execution opportunities for the issued instructions; and it reduces the waiting caused by register resources not being ready.

The following two unrolling methods are currently supported:

| Unrolling Method | How to Enable | Advantages | Disadvantages |
| :-- | :-- | :-- | :-- |
| Manual unrolling of VF loop | / | &bull;Fully controllable;<br>&bull;Can handle complex loops or scenarios such as src/dst register reuse; | &bull;High code complexity;|
| Compiler automatic unrolling | #pragma unroll number (number indicates the unrolling level) | &bull;Low code complexity;| &bull;Cannot unroll when src/dst registers are reused. |
>[!NOTE] Note
>
> Loop unrolling optimizes VF performance by eliminating instruction dependencies and improving the dual-issue capability. It must be used based on the actual computation process and does not guarantee performance gains. During unrolling, if the number of registers exceeds the upper limit, performance degradation or other unexpected behavior may occur.

### Manual Unrolling of the VF Loop
```cpp
for (uint16_t i = 0; i < 32; ++i) { 
// There is no dependency relationship between for loops: i=0 and i=1 can be executed in parallel, but because there are too many data-dependent instructions within the loop, the instructions for i=1 cannot be loaded into the execution queue
    AscendC::Reg::LoadAlign(srcReg, srcAddr, count);
    AscendC::Reg::Adds(dstReg0, srcReg, 10, mask);
    AscendC::Reg::Muls(dstReg1, dstReg0, 20, mask);
    ... // More than 64 data-dependent instructions
    AscendC::Reg::StoreAlign(dstAddr, dstReg1, count, mask);
}
```

After unrolling

```cpp
for (uint16_t i = 0; i < 8; ++i) { // Unroll 32 iterations by 4
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

### Compiler Automatic Unrolling
```cpp
#pragma unroll 4 
for (uint16_t i = 0; i < 32; ++i) {  
    AscendC::Reg::LoadAlign(srcReg, srcAddr + i * offset); 
    AscendC::Reg::Adds(dstReg0, srcReg, 10, mask); 
    AscendC::Reg::Muls(dstReg1, dstReg0, 20, mask); 
    AscendC::Reg::StoreAlign(dstAddr, dstReg1, i * offset, mask); 
}
```

## Avoiding Register Overflow That Increases Dependent Instructions in the Execution Queue<a name="section1850233910114"></a>

Within the same VF, the hardware can process a maximum of 32 RegTensor registers simultaneously. If this limit is exceeded, the compiler swaps data in and out and inserts synchronization instructions, severely degrading operator execution efficiency.
Within the same VF, the maximum number of MaskReg registers is 8. Exceeding 8 causes performance degradation.
Within the same VF, the maximum number of AddrReg registers is 8. Exceeding 8 causes performance degradation.
Within the same VF, the maximum number of read/write registers UnalignRegForLoad and UnalignRegForStore is 4 each. If more than 4 UnalignRegForLoad or UnalignRegForStore registers are actually used, compilation fails directly.

Optimization solutions:

-   Use Boolean algebra operations. For example, !\(a && b\) can be simplified to !a || !b, and !\(a || b\) can be simplified to !a && !b.
-   Adjust the instruction order appropriately and equivalently to save registers.

This example determines whether two double-type data values are equal. It needs to handle two special scenarios: the NaN scenario and the scenario where +0 and -0 are equal.

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
    // dstMask indicates whether the exponent parts of the two double values are both 0x7ff. Next, determine whether at least one of the mantissa parts of the two values is non-zero, and save the result to tmpMask0.
    // When the exponents are both 0x7ff and at least one mantissa is non-zero, a NaN scenario exists.
    Reg::MaskReg tmpMask1;
    Reg::Compares<uint64_t, CMPMODE::NE>(tmpMask1, mantissa0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::NE>(tmpMask0, mantissa1, 0, mask);
    Reg::MaskOr(tmpMask0, tmpMask1, tmpMask0, mask);
    //[Counterexample] To determine the special case where the exponents are all 1s and at least one mantissa is non-zero (the result is NAN), the formula used is !(a&&b), which requires an additional MaskReg, namely noNaNMask, to record the intermediate result of a&&b.
    Reg::MaskReg noNaNMask;
    Reg::MaskAnd(noNaNMask, dstMask, tmpMask0, mask);
    Reg::MaskNot(noNaNMask, noNaNMask, mask);
    // Determine the special case of +0 and -0.
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> unsignedPart0, unsignedPart1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarUnsignedPart;
    Reg::Duplicate(scalarUnsignedPart, static_cast<uint64_t>(0x7fffffffffffff), mask);
    Reg::And(unsignedPart0, tmpSrcReg0, scalarUnsignedPart, mask);
    Reg::And(unsignedPart1, tmpSrcReg1, scalarUnsignedPart, mask);
    //[Counterexample] First determine whether the two unsigned numbers are 0 separately, and then combine the two determination results through MaskAnd.
    // Compared with the optimized implementation, this implementation requires dstMask to additionally store the result of whether unsignedPart1 is 0, and additionally executes one MaskAnd instruction.
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask0, unsignedPart0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::EQ>(dstMask, unsignedPart1, 0, mask);
    Reg::MaskAnd(tmpMask0, tmpMask0, dstMask, mask);

    Reg::Compare(dstMask, tmpSrcReg0, tmpSrcReg1, mask);
    Reg::MaskAnd(dstMask, dstMask, noNaNMask, mask);
    Reg::MaskOr(dstMask, dstMask, tmpMask0, mask);
}
```

After optimization:

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
    //[Positive example] !(a&&b) is simplified to !a||!b, indicating that when the exponents are not all 1 or tmpMask0 is all 0, the determination can be performed normally without applying for additional registers.
    Reg::MaskOr(tmpMask0, tmpMask0, dstMask, mask);

    Reg::Compare(dstMask, tmpSrcReg0, tmpSrcReg1, mask);
    Reg::MaskAnd(dstMask, dstMask, tmpMask0, mask);
    // +0 -0
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> unsignedPart0, unsignedPart1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarUnsignedPart;
    Reg::Duplicate(scalarUnsignedPart, static_cast<uint64_t>(0x7fffffffffffff), mask);
    Reg::And(unsignedPart0, tmpSrcReg0, scalarUnsignedPart, mask);
    Reg::And(unsignedPart1, tmpSrcReg1, scalarUnsignedPart, mask);
    //[Positive example] First determine whether unsignedPart0 is 0 and save the result to tmpMask0; then use tmpMask0 as the input mask for determining whether unsignedPart1 is 0.
    // This makes tmpMask1 directly represent the result that unsignedPart0 and unsignedPart1 are both 0, omitting one MaskAnd instruction compared with the counterexample.
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask0, unsignedPart0, 0, mask);
    Reg::Compares<uint64_t, CMPMODE::EQ>(tmpMask1, unsignedPart1, 0, tmpMask0);
    Reg::MaskOr(dstMask, dstMask, tmpMask1, mask);
}
```
