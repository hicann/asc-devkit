# VF Loop Optimization<a name="ZH-CN_TOPIC_0000002498328772"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T12:58:19.194Z pushedAt=2026-11-03T07:05:13.358Z nl=2 -->

In the architecture of the Ascend 950PR&950DT products, the Vector Function (VF) is the core carrier for implementing high-performance vector computing. A VF function can contain up to four levels of nested loops, and each loop level can contain multiple serial loops. It also supports non-loop vector operations and scalar operations. VF has limited support for control structures: it supports only for loops and conditional statements, and does not support other control structures such as switch, do-while, and while-do. VF loops are optimized into hardware-level vector loops (Hardware Loops) as much as possible to achieve performance optimization.

When a loop in a VF function complies with the [hardware loop coding guidelines](#section11326136133217), the compiler optimizes it into a hardware loop to improve overall execution performance. Otherwise, the loop logic is implemented as a software loop composed of iteration variables and conditional statements, and VF loop optimization cannot be enabled.

On the basis of following the hardware loop coding guidelines to ensure that loops can be optimized into hardware loops, you can further improve performance through member variable access, instruction distribution optimization, and address management optimization.

## Hardware Loop Coding Guidelines<a name="section11326136133217"></a>

To enable the compiler to recognize and generate a Hardware Loop, the corresponding loop code must meet the hardware design requirements. The specific guidelines are as follows:

-   Iteration variable type

    The iteration variable of all loops in the VF must be of the uint16\_t type.

-   Start value and step

    The loop start value must be 0.

    The step of each iteration must be incremented by 1.

-   Jump instructions are not allowed in the loop, such as conditional jumps like if/else and the ternary operator ?:.

    if/else in the VF within a loop hinders the generation of a Hardware Loop. Although the compiler attempts to eliminate if/else as much as possible, completeness is not guaranteed.

    The if control flow can be replaced with if constexpr or for(1). if constexpr is completed at compile time and incurs no runtime overhead, but the parameters passed in must be compile-time constants and cannot depend on runtime variables. for(1) can trigger the compiler's loop optimization. Under the current hardware conditions, the performance of executing a loop on the Vector side is far superior to that of conditional branch jumps.

     This example shows the tail block processing scenario. When the tail block size is not 0, hasTail is 1. Using for(1) instead of the if(hasTail) check improves the loop performance.

    ```
    // [Negative example] Use an if statement
    uint16_t tailK = srcK % floatRepSize;
    uint16_t hasTail = 0;
    // Use !!tailK to determine whether a tail block is generated. If the remainder of srcK % floatRepSize is 0, the bool value of hasTail is 0 (false); otherwise, it is 1 (true).
    hasTail = !!tailK;
    if(hasTail){
      // Process the tail block
    }
    ```

    ```
    // [Example] Use for(1) instead of the if statement
    uint16_t tailK = srcK % floatRepSize;
    uint16_t hasTail = 0;
    hasTail = !!tailK;
    for (uint16_t i = 0; i < hasTail; i++) {
      // Process the tail block
    }
    ```

-   Once executed, the loop count/boundary cannot be changed.
-   To use the count of the outer loop as the loop boundary, move the outer loop counter to another register and then set it as the loop boundary.

The following example shows two scenarios in which the compiler identifies and processes the loop as a Hardware Loop and a Software Loop:

```
// [Example] Optimized by the compiler into a Hardware Loop
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
// [Negative example] Cannot be optimized by the compiler, forming a Software Loop
for (uint16_t i = 0; i < LoopBound; i++) {     // Software Loop, the loop contains an if statement
    if(cond){
        ...
    }
}
 
for (uint16_t i = 2; i < LoopBound*3; i+=2){  // Software Loop, the loop start value is not 0 and the loop step is not 1
...
}
```

## Optimizing Member Variable Access Within a Loop<a name="section168682445382"></a>

Within a VF, directly accessing member variables of a structure or class object is not recommended. Directly accessing member variables of a structure or class object is equivalent to moving content from the stack to a Tensor register and then accessing the content on the Tensor through an address. This operation causes VF fusion failure. It is recommended to pass parameters temporarily through local variables, as shown in the following example:

```
// [Negative example] Directly read member variables of a structure or class object
__aicore__ inline void SoftMaxGenericNDImpVF(__ubuf__ float* dstAddr, __ubuf__ float* sumAddr, __ubuf__ float* maxAddr,
    __ubuf__ float* srcAddr, __ubuf__ float* workAddr, const LastAxisShapeND originalSrcShape, const SoftMaxTiling tiling)
{
    for (uint16_t i = 0; i < (uint16_t)tiling.srcM; i++) {
        AscendC::ReduceMax(maxAddr + i * FLOAT_NUM_PER_BLK, srcAddr + i * tiling.srcK, workAddr, (uint16_t)originalSrcShape.k);
    }
}
```

```
// [Example] Pass member variables of a structure or class object accessed within a VF through local variables
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

Reduce non-index-related statements within a loop. Statements in a for loop that are independent of the index can be moved outside the for loop to reduce the number of instructions.

```
// [Negative example] The Duplicate statement is placed inside the for loop and is executed once per iteration.
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
// [Example] The Duplicate statement is placed outside the for loop and is executed only once, effectively reducing the number of instructions.
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

## Address Management Optimization in Loops<a name="section2717961412"></a>

In a VF loop, when data movement instructions are used, the address offsets for moving data in and out must be calculated, which introduces considerable scalar computation overhead. In the Ascend 950PR&950DT products, address registers are introduced to effectively optimize the calculation of address offsets. When the following address register generation pattern is satisfied, the compiler has the opportunity to generate address registers, thereby eliminating the related scalar computation overhead and improving overall performance.

The address register supports addressing of up to four loop levels, as shown in the following figure.

![](../../../../figures/addressing.png)

The following is a code example that satisfies the address register generation pattern. The source operand address is addressed in up to four dimensions as shown in the preceding figure:

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

In particular, when the four loop levels form a contiguous access scenario, address management can be simplified to a one-dimensional pattern, thereby achieving more efficient movement optimization and further improving the locality of data access and execution efficiency.

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
