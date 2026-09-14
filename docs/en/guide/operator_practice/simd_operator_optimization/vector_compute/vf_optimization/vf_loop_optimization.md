# VF Loop Optimization<a name="ZH-CN_TOPIC_0000002498328772"></a>

<!-- md-trans-meta sourceCommit=64a402bfee6184c5fb843086d6ab3eb51538b885 translatedAt=2026-08-26T14:30:42.695Z -->

In the architecture corresponding to Ascend 950PR/Ascend 950DT, the Vector Function (VF) is the core vehicle for implementing high-performance vector computation. A VF function can contain up to four levels of nested loops, and each loop level can contain multiple serial loops. It also supports non-loop vector operations and scalar operations. VF provides limited support for control structures, supporting only for loops and conditional statements, and does not support other control structures such as switch, do-while, and while-do. VF loops are optimized into hardware-level vector loops (Hardware Loops) wherever possible to achieve performance optimization.

When a loop in a VF function meets the [Hardware Loop coding standards](#section11326136133217), it is optimized by the compiler into a Hardware Loop, improving overall execution performance. Otherwise, the loop logic is composed of iteration variables and conditional statements to form a Software Loop, and VF loop optimization cannot be enabled.

On the basis of following the Hardware Loop coding standards to ensure that loops can be optimized into Hardware Loops, performance can be further improved through member variable access, instruction distribution optimization, and address management optimization.

## Hardware Loop Coding Standards<a name="section11326136133217"></a>

To enable the compiler to recognize and generate Hardware Loops, the corresponding loop code must comply with the hardware design requirements. The specific standards are as follows:

-   Iteration variable type

    The iteration variable of all loops in the VF must be of the uint16\_t type.

-   Start value and step

    The loop start value must be 0.

    The step of each iteration must be incremented by 1.

-   Jump instructions are not allowed in the loop, such as conditional jumps like if/else and the ternary operator ?:.

    if/else in the VF within a loop hinders the generation of Hardware Loops. Although the compiler attempts to eliminate if/else as much as possible, completeness is not guaranteed.

    The if control flow can be replaced with if constexpr or for(1). if constexpr is completed at compile time with no runtime overhead, but the passed parameter must be a compile-time constant and cannot depend on runtime variables. for(1) can trigger the compiler's loop optimization. Under the current hardware conditions, executing a loop on the Vector side performs far better than conditional branch jumps.

This example demonstrates the tail block processing scenario. When the tail block size is not 0, hasTail is 1. Using for(1) instead of the if(hasTail) check improves the loop performance.

    ```
    //[Negative example] Use an if statement
    uint16_t tailK = srcK % floatRepSize;
    uint16_t hasTail = 0;
    // Use !!tailK to determine whether a tail block is generated. If the remainder of srcK % floatRepSize is 0, the bool value corresponding to hasTail is 0 (false); otherwise, it is 1 (true).
    hasTail = !!tailK;
    if(hasTail){
      // Tail block processing content
    }
    ```

    ```
    //[Positive example] Use for(1) instead of the if statement
    uint16_t tailK = srcK % floatRepSize;
    uint16_t hasTail = 0;
    hasTail = !!tailK;
    for (uint16_t i = 0; i < hasTail; i++) {
      // Tail block processing content
    }
    ```

-   Once executed, the loop count/boundary must not be changed.
-   To use the count of the outer loop as the loop boundary, move the outer loop counter to another register and then set it as the loop boundary.

The following examples show two scenarios in which the compiler recognizes and processes a loop as a Hardware Loop and a Software Loop:

```
//[Positive example] Optimized by the compiler into a Hardware Loop
// Nested loop
for (uint16_t i = 0; i < LoopBound; i++) {            
    for (uint16_t j = 0; j < LoopBound; j++) {   
        for (uint16_t k = 0; k < LoopBound; k++) {   
            for (uint16_t m = 0; m < LoopBound; m++) {  
              // ...
            }
         }
     }
}
```

```
//[Negative example] cannot be optimized by the compiler and forms a Software Loop
for (uint16_t i = 0; i < LoopBound; i++) {     // Software Loop, the loop contains an if statement
    if(cond){
        ...
    }
}
 
for (uint16_t i = 2; i < LoopBound*3; i+=2){  // Software Loop, the loop start value is not 0 and the step is not 1
...
}
```

## Optimizing Member Variable Access Within a Loop<a name="section168682445382"></a>

Within a VF, directly accessing member variables of a structure or class object is not recommended. Directly accessing member variables of a structure or class object is equivalent to moving content from the stack to a Tensor register and then accessing the content on the Tensor through an address. This operation causes VF fusion to become ineffective. It is recommended to resolve this by passing parameters through local variables, as shown in the following example:

```
//[Negative example] directly reading member variables of a structure or class object
__aicore__ inline void SoftMaxGenericNDImpVF(__ubuf__ float* dstAddr, __ubuf__ float* sumAddr, __ubuf__ float* maxAddr,
    __ubuf__ float* srcAddr, __ubuf__ float* workAddr, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    for (uint16_t i = 0; i < (uint16_t)tiling.srcM; i++) {
        AscendC::ReduceMax(maxAddr + i * FLOAT_NUM_PER_BLK, srcAddr + i * tiling.srcK, workAddr, (uint16_t)originalSrcShape.k);
    }
}
```

```
//[Positive example] passing member variables of a structure or class object accessed within a VF through local variables
__aicore__ inline void SoftMaxGenericNDImpVF(__ubuf__ float* dstAddr, __ubuf__ float* sumAddr, __ubuf__ float* maxAddr,
    __ubuf__ float* srcAddr, __ubuf__ float* workAddr, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t srcM = tiling.srcM;
    uint16_t reduceK = FLOAT_NUM_PER_BLK;
    uint16_t originK = (uint16_t)originalSrcShape.k;
    for (uint16_t i = 0; i < srcM; i++) {
        AscendC::ReduceMax(maxAddr + i * reduceK, srcAddr + i * srcK, workAddr, originK);
    }
}
```

## Instruction Distribution Optimization Within a Loop<a name="section214134513910"></a>

Reduce statements unrelated to the index within a loop. Statements in a for loop that are unrelated to the index can be moved outside the for loop to reduce the number of instructions.

```
//[Negative example] The Duplicate statement is placed inside the for loop and is executed once in each iteration.
template<typename T>
__simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();  
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::Duplicate(dstReg, scalarValue);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

```
//[Positive example] Duplicate is placed outside the for loop and is executed only once, effectively reducing the number of instructions.
template<typename T>
__simd_vf__ inline void DuplicateVF(__ubuf__ T* dstAddr, T scalarValue, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();  
    AscendC::Reg::Duplicate(dstReg, scalarValue);
    for (uint16_t i = 0; i < repeatTimes; i++) {
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```

## Address Management Optimization Within Loops<a name="section2717961412"></a>

In a VF loop, when data movement instructions are used, the address offsets for moving data in and out must be calculated, which introduces considerable scalar computation overhead. In Ascend 950PR/Ascend 950DT, address registers are introduced to effectively optimize the calculation of address offsets. When the following address register generation pattern is satisfied, the compiler has the opportunity to generate address registers, thereby eliminating the related Scalar computation overhead and improving overall performance.

The address register supports up to four levels of loop addressing, as shown in the following figure.

![](../../../../figures/addressing.png)

The following is a code example that satisfies the address register generation pattern, where the source operand addresses are addressed in up to four dimensions as shown in the preceding figure:

```
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        for(uint16_t k = 0;k < extent3; k++){
            for(uint16_t m = 0;m < extent4; m++){
               AscendC::Reg::LoadAlign(srcReg, srcAddr + i * const1 + j * const2 + k * const3 + m * const4);
            }
        }
    }
}
```

The compiler optimizes the preceding pattern to use AddrReg for address management, resulting in the following pattern:

```
AscendC::Reg::AddrReg aReg;
for(uint16_t i = 0;i < extent1; i++){
    for(uint16_t j = 0;j < extent2; j++){
        for(uint16_t k = 0;k < extent3; k++){
            for(uint16_t m = 0;m < extent4; m++){
                aReg = AscendC::Reg::CreateAddrReg<T>(i, const1, j, const2, k, const3, m, const4);
                AscendC::Reg::LoadAlign(srcReg, srcAddr, aReg);
            }
        }
    }
}
```

The compiler performs pattern matching by recognizing instructions such as LoadAlign and StoreAlign. When the code structure satisfies a specific optimization pattern, the compiler has the opportunity to perform efficient optimization and achieve the best performance gains. Conversely, directly using AddrReg to store offsets or using other low-level interfaces may limit the compiler's global optimization capability.

In particular, when the four levels of loops form a contiguous access scenario, address management can be simplified to a one-dimensional pattern, thereby achieving more efficient data movement optimization and further improving data access locality and execution efficiency.

```
// Enable vector address generation instruction optimization
__simd_vf__ inline void ComputeModeVF(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint32_t oneRepeatSize, uint16_t repeatTimes)
{
    AscendC::Reg::RegTensor<T> dstReg;
    AscendC::Reg::MaskReg mask = AscendC::Reg::CreateMask<T>();
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        AscendC::Reg::LoadAlign(dstReg, srcAddr + i * oneRepeatSize);
        AscendC::Reg::StoreAlign(dstAddr + i * oneRepeatSize, dstReg, mask);
    }
}
```
