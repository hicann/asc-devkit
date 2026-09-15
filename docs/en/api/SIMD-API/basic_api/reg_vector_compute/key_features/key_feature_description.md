# Key Feature Description

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-09-01T07:17:44.768Z -->

## Hardware Loop

In the Reg vector computation architecture, the Vector Function (VF) is the core carrier for achieving high-performance vector computation. A VF function can contain up to four levels of nested loops, and each loop level can also contain multiple serial loops. VF loops have limited support for control structures, supporting only for loops and conditional statements, and do not support other control structures such as switch, do-while, and while-do. When a loop in a VF function satisfies the Hardware Loop coding specification, it is optimized by the compiler into a Hardware Loop, thereby improving overall coding performance. Otherwise, its loop logic is composed of iteration variables and conditional statements to form a Software Loop, and VF loop optimization cannot be enabled.

### Hardware Loop Coding Specifications
- Hardware Loop supports a maximum nesting depth of 4 levels; serial Hardware Loop loops are supported.
- Iteration variable type<br>The iteration variables of all loops within a VF must be of type uint16_t.
- Start value and step<br>The loop start value begins from 0.<br>The step of each iteration must increment by 1.
- Jump instructions are not allowed within the loop, such as conditional jumps like if/else and the ternary operator ?:.<br>if/else within a loop in a VF will hinder the generation of a Hardware Loop. Although the compiler will perform if/else elimination optimization as much as possible, completeness is not guaranteed.
- Once execution begins, the loop count/boundary must not be modified.
- To use the count of an outer loop as the loop boundary, assign the outer loop counter to a scalar and then use it as the inner loop boundary.

## Dual-Issue Optimization
For detailed performance optimization content of dual-issue optimization, see [Operator Practice Reference - Dual-Issue Optimization](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_instruction_issue_optimization.md).

### Dual-Issue

Dual-issue refers to the processor's ability to issue two instructions simultaneously to the execution units for processing within the same clock cycle.

This mechanism improves the processor's instruction processing efficiency per unit time without altering the program logic, and serves as one of the important foundations for achieving instruction-level parallelism.

### Reasonably Splitting VF Loops

A VF loop is not necessarily better when written longer with all operations placed in a single for loop. Intermediate results should be moved out to UB as appropriate to reduce data dependencies. When there are too many instructions, an Icache Miss may be triggered, and dual-issue cannot be performed even if there are no dependencies between loops. When a loop contains synchronization, the loop can be split to hoist the synchronization out and reduce the number of synchronization operations.

### Manually Controlled Loop Splitting

If a loop contains instructions with excessive dependencies, the instructions cannot execute concurrently, meaning the dual-issue feature cannot be enabled. Loop unrolling can be used to improve the dual-issue capability, approaching the out-of-order execution characteristics of the hardware, and to reduce the waiting caused by register resources not being ready.

## VF Fusion
VF fusion combines multiple VF functions in the code into a single VF function, effectively improving performance. The VF fusion feature is a compiler optimization feature, enabled through the compilation option --cce-simd-vf-fusion=true. VF automatic fusion uses the Loop Fuse algorithm to convert VFs into Loop form, then fuses control-flow-equivalent VFs, and finally restores the VFs. The compiler first performs a legality check before fusion to determine whether two VFs are equivalent, whether the intermediate code on the Main side can be executed within the VF, and whether positive benefits can be generated after fusion. If the VF fusion conditions are met, the compiler automatically performs VF fusion optimization. To ensure that the execution logic and semantics of the fused VF are consistent with those before fusion, synchronization instructions are conservatively inserted between the original two VFs. The compiler also attempts to hoist and merge instructions in the fused VF to optimize the VF code. The fusion strategy is to fuse as much as possible. Users can increase the opportunities for VF fusion by coding according to patterns that comply with the fusion legality check. Users can also refer to the fusion principles to perform fusion optimization manually.

For detailed information on VF fusion performance optimization, see [Operator Practice Reference - VF Fusion Optimization](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md).

## Data Loading Optimization
The Reg vector computation API provides multiple loading instructions as shown in the following table. Select loading APIs appropriately and leverage their capabilities for optimization. For details on performance optimization for continuous non-alignment scenarios, see [Operator Practice Reference - Continuous Non-alignment Scenario Optimization](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/continuous_unaligned_scenario_optimization.md).
| Scenario | Description |
| :-- | :-- | 
| Continuous aligned loading (LoadAlign) | Loads data continuously from UB with a 32B-aligned start address into RegTensor. |
| Non-continuous aligned loading (LoadAlign) | Loads data non-continuously from UB with a 32B-aligned start address into RegTensor.|
| Continuous non-aligned loading (LoadUnAlign) | Loads data continuously from UB with a non-32B-aligned start address into RegTensor.|
| MaskReg loading (LoadAlign) | Loads data continuously from UB with a 32B-aligned start address into MaskReg.|
| MaskReg loading (MaskGenWithRegTensor) | Loads data from RegTensor into MaskReg.|
| Discrete loading (Gather) | Collects source operand elements into the destination operand RegTensor based on index values.|
| Discrete loading (GatherB) | Collects source operand DataBlocks (32B) into the destination operand RegTensor based on index values.|

### Enabling Distribution Mode for Data Movement

- Continuous aligned loading (LoadAlign)

  As shown in [half to int32_t type conversion process](#fig1), the half-type elements with a data volume of VL/2 at UB address xAddr are loaded into xReg through LoadAlign (distribution mode DIST_UNPACK_B16), then Cast is called to convert half to int32_t and write the result to yReg, and finally the data is moved out to UB address yAddr through StoreAlign (distribution mode DIST_NORM).

  **Figure 1** half to int32_t type conversion process<a id="fig1"></a>

  ![](../../../../figures/half_to_int32_conversion.png "half to int32_t type conversion process")

  ```cpp
  // Convert from a smaller bit width to a larger one, using half->int32_t as an example.
  static constexpr AscendC::Reg::CastTrait castTrait = {
      AscendC::Reg::RegLayout::ZERO,
      AscendC::Reg::SatMode::UNKNOWN,
      AscendC::Reg::MaskMergeMode::ZEROING,
      AscendC::RoundMode::CAST_FLOOR
  };
  AscendC::Reg::LoadAlign<half, AscendC::Reg::PostLiteral::POST_MODE_UPDATE, AscendC::Reg::LoadDist::DIST_UNPACK_B16>(xReg, xAddr, oneRepeatSize); 
  AscendC::Reg::Cast<int32_t, half, castTrait>(yReg, xReg, mask);
  AscendC::Reg::StoreAlign<int32_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(yAddr, yReg, oneRepeatSize, mask);  
  ```

- Continuous aligned storing (StoreAlign)

  As shown in [float to int16_t type conversion process](#fig2), the float-type elements with a data volume of VL at UB address xAddr are loaded into xReg through LoadAlign (distribution mode DIST_NORM), then Cast is called to convert float to int16_t and write the result to yReg, and finally the data is moved out to UB address yAddr through StoreAlign (distribution mode DIST_PACK_B32).

  **Figure 2** float to int16_t type conversion process<a id="fig2"></a>

  ![](../../../../figures/float_to_int16_conversion.png "float to int16_t type conversion process")

  ```cpp
  // Convert from a larger bit width to a smaller one, using float->int16_t as an example.
  AscendC::Reg::LoadAlign<float, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);  
  AscendC::Reg::Cast<int16_t, float, castTrait>(yReg, xReg, mask); 
  AscendC::Reg::StoreAlign<int16_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE, AscendC::Reg::StoreDist::DIST_PACK_B32>(yAddr, yReg, oneRepeatSize, mask); 
  ```
