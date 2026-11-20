---
okftype: reference.concept
description: "Describes the key features of Reg vector computation: HardwareLoop encoding specifications, instruction dual-issue and VF fusion optimization, and data movement optimization suggestions for selecting load and store APIs by scenario."
---
# Key Feature Description

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:02.290357+00:00 pushedAt=2026-11-16T02:10:55.404Z nl=2 -->

## Hardware Loop (HardwareLoop)

In the Reg vector computing architecture, Vector Function (VF) is the core vehicle for achieving high-performance vector computation. A VF function can contain up to four levels of nested loops, and each loop level can also contain multiple serial loops. VF loops have limited support for control structures, supporting only for loops and conditional checks, and do not support other control structures such as switch, do-while, and while-do. When a loop in a VF function meets the Hardware Loop coding specifications, it is optimized by the compiler into a Hardware Loop, improving overall code execution performance. Otherwise, its loop logic consists of iteration variables and conditional check statements, forming a Software Loop, and VF loop optimization cannot be enabled.

### Hardware Loop Coding Specifications

- Hardware Loop supports a maximum of 4 nested levels and supports serial Hardware Loops.
- Iteration variable type<br>The iteration variables of all loops in a VF must be of the uint16_t type.
- Start value and step<br>The loop start value is 0.<br>The iteration variable must be incremented by 1 on each iteration.
- Jump instructions are not allowed in a loop, such as conditional jumps like if/else and the ternary operator ?:.<br>if/else in a VF inside a loop hinders Hardware Loop generation. Although the compiler performs if/else elimination optimization as much as possible, completeness is not guaranteed.
- Once execution starts, the loop count/boundary cannot be changed.
- To use the outer loop count as the loop boundary, assign the outer loop counter to a scalar and then use it as the inner loop boundary.

## Dual Instruction Issue Optimization

For details about single/dual issue of Reg vector compute API instructions, see [Summary of Single/Dual Issue of Reg Vector Compute API Instructions](../../../appendix/reg_vector_compute_interface_instruction_single_dual_issue_summary.md).

For details about performance optimization of dual instruction issue, see [Operator Practice Reference - Dual Instruction Issue Optimization](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_issue_optimization.md).

### Dual Instruction Issue

Dual instruction issue means that the processor can issue two instructions to execution units simultaneously in the same clock cycle.

This mechanism improves the processor's instruction processing efficiency per unit time without changing program logic, and is one of the important foundations for instruction-level parallelism.

### Properly Splitting VF Loops

A VF is not necessarily better when written longer with all operations placed in a single for loop. Intermediate results should be stored to the Unified Buffer (UB) appropriately to reduce data dependencies. When there are too many instructions, an Icache Miss may be triggered, and dual issue cannot be performed even if there are no dependencies between loops. When a loop contains synchronization, splitting the loop can hoist the synchronization out to reduce the number of synchronization operations.

### Manually Controlling Loop Splitting

If a loop contains instructions with excessive dependencies, the instructions cannot be executed concurrently, and the dual issue feature cannot be enabled. Loop unrolling can be used to improve dual instruction issue capability and approximate the hardware's out-of-order execution characteristics, reducing instruction stalls caused by register resources not being ready.

## VF Fusion

VF fusion combines multiple VF functions in the code into one VF function to effectively improve performance. VF fusion is a compiler optimization feature enabled by the compilation option --cce-simd-vf-fusion=true. Automatic VF fusion uses the Loop Fuse algorithm to convert VFs into loop form, fuses control-flow-equivalent VFs, and then restores the VFs. The compiler first performs a legality check before fusion to determine whether two VFs are equivalent, whether the intermediate code on the Main side can be executed within the VF, and whether fusion can produce positive benefits. If the VF fusion conditions are met, the compiler automatically performs VF fusion optimization. To ensure that the execution logic and semantics of the fused VF are consistent with those before fusion, synchronization instructions are conservatively inserted between the original two VFs. The compiler also attempts to hoist and merge instructions in the fused VF to optimize the VF code. The fusion strategy is to fuse as much as possible. Users can increase the opportunities for VF fusion by coding in patterns that comply with the fusion legality check. Users can also manually perform fusion optimization by referring to the fusion principles.

For details about performance optimization of VF fusion, see [Operator Practice Reference - VF Fusion Optimization](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md).

## Data Movement Optimization

The Reg vector compute C API provides various transfer instructions shown in the following table. Select transfer APIs appropriately and use their capabilities for optimization. For details about performance optimization of contiguous unaligned scenarios, see [Operator Practice Reference - Contiguous Unaligned Scenario Optimization](../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/continuous_unaligned_optimization.md).

| Scenario | C API | Description |
| :-- | :-- | :-- |
| Contiguous aligned load | [asc_loadalign](./load/asc_loadalign.md) | Contiguously loads data from a 32-byte-aligned UB start address into a vector data register or mask register.|
| Non-contiguous aligned load | [asc_loadalign](./load/asc_loadalign.md) | Non-contiguously loads multiple DataBlocks from a 32-byte-aligned UB start address.|
| Contiguous unaligned load | [asc_loadunalign](./load/asc_loadunalign.md) | Contiguously loads data from a UB start address that is not aligned to 32 bytes into a vector data register.|
| Mask register load | [asc_loadalign](./load/asc_loadalign.md), [asc_loadalign_mask](./load/asc_loadalign_mask.md) | Contiguously loads data from UB into the mask register.|
| Gather load | [asc_gather](./ub_gather/asc_gather.md) | Gathers elements from UB into a vector data register according to index values.|
| DataBlock gather load | [asc_gather_datablock](./ub_gather/asc_gather_datablock.md) | Gathers elements from UB into a vector data register by DataBlock (32B) according to index values.|
| Contiguous aligned store | [asc_storealign](./store/asc_storealign.md) | Contiguously stores data from a vector data register or mask register to a 32-byte-aligned UB address.|
| Contiguous unaligned store | [asc_storeunalign](./store/asc_storeunalign.md) | Contiguously stores data from a vector data register to a UB address that is not aligned to 32 bytes.|
| Discrete store | [asc_scatter](./scatter/asc_scatter.md) | Discretely stores elements from the vector data register to the UB based on index values.|
| Register-to-register data movement | [asc_copy](./reg_copy/asc_copy.md) | Supports three modes: vector data register to vector data register, mask register to mask register, and vector data register to mask register.|

### Data Movement with Distribution Mode Enabled

- Contiguous aligned load

  As shown in [half to int32_t type conversion process](#fig1), load `half` elements with a data size of VL/2 from the UB address `x_addr` into `x_reg` using [asc_loadalign_unpack](./load/asc_loadalign_unpack.md), call [asc_half2int32_rd](./reg_convert/asc_half2int32.md) to convert `half` to `int32_t` and write the result to `y_reg`, and finally store the result to the UB address `y_addr` using [asc_storealign](./store/asc_storealign.md).

  **Figure 1** half to int32_t type conversion process<a id="fig1"></a>

  ![](../../../figures/capi_half_to_int32_conversion.png "half to int32_t type conversion process")

  ```cpp
  __simd_vf__ inline void half_to_int32_vf(__ubuf__ int32_t* y_addr, __ubuf__ half* x_addr)
  {
      vector_half x_reg;
      vector_int32_t y_reg;
      uint32_t count = 64;
      vector_bool mask = asc_update_mask_b32(count);

      asc_loadalign_unpack(x_reg, x_addr);
      asc_half2int32_rd(y_reg, x_reg, mask, ASC_POSITION_EVEN);
      asc_storealign(y_addr, y_reg, mask);
  }
  ```

- Contiguous aligned store

  As shown in [float to int16_t type conversion process](#fig2), load `float` elements with a data size of VL from the UB address `x_addr` into `x_reg` using [asc_loadalign](./load/asc_loadalign.md), call [asc_float2int16_rn](./reg_convert/asc_float2int16.md) to convert `float` to `int16_t` and write the result to `y_reg`, and finally store the result to the UB address `y_addr` using [asc_storealign_pack](./store/asc_storealign_pack.md).

  **Figure 2** float to int16_t type conversion process<a id="fig2"></a>

  ![](../../../figures/capi_float_to_int16_conversion.png "half to int32_t type conversion process")

  ```cpp
  __simd_vf__ inline void float_to_int16_vf(__ubuf__ int16_t* y_addr, __ubuf__ float* x_addr)
  {
      vector_float x_reg;
      vector_int16_t y_reg;
      uint32_t count = 64;
      vector_bool mask = asc_update_mask_b32(count);

      asc_loadalign(x_reg, x_addr);
      asc_float2int16_rn(y_reg, x_reg, mask, ASC_POSITION_EVEN);
      asc_storealign_pack(y_addr, y_reg, mask);
  }
  ```
