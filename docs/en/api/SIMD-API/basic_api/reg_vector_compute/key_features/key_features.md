# Key Features

<!-- md-trans-meta sourceCommit=5c07153f668df7dd8585d7a94d76b4114ca4b1ac translatedAt=2026-11-05T03:00:28.138Z pushedAt=2026-11-18T01:25:30.885Z nl=2 -->

## Hardware Loop

In the Reg vector computation architecture, the Vector Function (VF) is the core vehicle for high-performance vector computation. A VF function can contain up to four levels of nested loops, and each loop level can contain multiple serial loops. VF loops have limited support for control structures: only for loops and conditional statements are supported, while other control structures such as switch, do-while, and while-do are not supported. When a loop in a VF function meets the Hardware Loop coding specification, the compiler optimizes it into a Hardware Loop, improving overall coding performance. Otherwise, its loop logic is composed of iteration variables and conditional statements to form a Software Loop, and VF loop optimization cannot be enabled.

### Hardware Loop Coding Specifications

- Hardware Loop supports up to four nested levels and supports serial Hardware Loop loops.
- Iteration variable type<br>The iteration variables of all loops in a VF must be of the uint16_t type.
- Start value and step<br>The loop start value must be 0.<br>The step of each iteration must increment by 1.
- Jump instructions are not allowed inside a loop, such as conditional jumps like if/else and the ternary operator ?:.<br>if/else inside a loop in a VF prevents Hardware Loop generation. Although the compiler attempts to eliminate if/else as much as possible, complete elimination is not guaranteed.
- Once execution starts, the loop count/boundary must not be modified.
- To use the count of an outer loop as the loop boundary, assign the outer loop counter to a scalar and then use it as the inner loop boundary.

## Dual-Issue Optimization

For the single-issue and dual-issue status of Reg vector computation API instructions, see [Reg Vector Computation API Instruction Single/Dual-Issue Summary](../../../../appendix/reg_vector_compute_interface_instruction_single_dual_issue_summary.md).

For details on dual-issue optimization, see [Operator Practice Reference - Dual-Issue Optimization](../../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_issue_optimization.md).

### Dual-Issue

Dual-issue means that the processor can issue two instructions to the execution units simultaneously within the same clock cycle.

Without changing the program logic, this mechanism improves the processor's instruction processing efficiency per unit of time and serves as an important foundation for instruction-level parallelism.

### Reasonably Splitting VF Loops

A VF is not necessarily better when written longer with all operations placed in a single for loop. You should move intermediate results out to the Unified Buffer (UB) as appropriate to reduce data dependencies. When there are too many instructions, an Icache Miss may be triggered, and dual-issue cannot be performed even if there are no dependencies between loops. When a loop contains synchronization, you can split the loop to hoist the synchronization out and reduce the number of synchronization operations.

### Manually Controlled Loop Splitting

If a loop contains too many instructions with heavy dependencies, the instructions cannot execute concurrently, which means the dual-issue feature cannot be enabled. You can unroll the loop to improve dual-issue capability and better align with the hardware's out-of-order execution, reducing instruction stalls caused by register resources not being ready.

## VF Fusion

VF fusion combines multiple VF functions in the code into a single VF function to effectively improve performance. VF fusion is a compiler optimization feature enabled by the compilation option --cce-simd-vf-fusion=true. Automatic VF fusion uses the Loop Fuse algorithm to convert VFs into loop form, fuses control-flow-equivalent VFs, and then restores the VFs. The compiler first performs a legality check before fusion to determine whether the two VFs are equivalent, whether the intermediate code on the Main side can be executed within the VF, and whether fusion produces a positive benefit. If the VF fusion conditions are met, the compiler automatically performs VF fusion optimization. To ensure that the execution logic and semantics of the fused VF are consistent with those before fusion, the compiler conservatively inserts synchronization instructions between the original two VFs. The compiler also attempts to hoist and merge instructions in the fused VF to optimize the VF code. The fusion strategy is to fuse whenever possible. Coding in a pattern that satisfies the fusion legality check increases the opportunities for VF fusion. You can also manually perform fusion optimization by referring to the fusion principles.

For details on VF fusion performance optimization, see [Operator Practice Reference - VF Fusion Optimization](../../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md).

## Data Movement Optimization

The Reg vector computation API provides various load instructions as shown in the following table. Select the appropriate load API and leverage its capabilities for optimization. For details about performance optimization in non-contiguous scenarios, see [Operator Practice Reference - Non-contiguous Scenario Optimization](../../../../../guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/continuous_unaligned_optimization.md).

| Scenario | Description |
| :-- | :-- | 
| Contiguous aligned load (LoadAlign) | Loads data contiguously from a UB whose start address is 32B-aligned into RegTensor. |
| Non-contiguous aligned load (LoadAlign) | Loads data non-contiguously from a UB whose start address is 32B-aligned into RegTensor.|
| Contiguous non-aligned load (LoadUnAlign) | Loads data contiguously from a UB whose start address is not 32B-aligned into RegTensor.|
| MaskReg load (LoadAlign) | Loads data contiguously from a UB whose start address is 32B-aligned into MaskReg.|
| MaskReg load (MaskGenWithRegTensor) | Loads data from RegTensor into MaskReg.|
| Scatter load (Gather) | Collects source operands element-wise into the destination RegTensor based on the index value.|
| Scatter load (GatherB) | Collects source operands into the destination RegTensor by DataBlock (32B) based on the index value.|

### Enabling Distribution Pattern for Data Movement

- Contiguous aligned load (LoadAlign)

  As shown in [half-to-int32_t type conversion](#fig1), the half-type data elements with a data volume of VL/2 at UB address xAddr are loaded into xReg through LoadAlign (with the distribution pattern DIST_UNPACK_B16). Cast is then called to convert half to int32_t and write the result to yReg. Finally, the data is stored back to UB address yAddr through StoreAlign (with the distribution pattern DIST_NORM).

  **Figure 1** half-to-int32_t type conversion<a id="fig1"></a>

  ![](../../../../figures/half_to_int32_conversion.png "half-to-int32_t type conversion")

  ```cpp
  // Small-to-large bit width, using half->int32_t as an example. 
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

- Contiguous aligned store (StoreAlign)

  As shown in [float-to-int16_t type conversion](#fig2), the float elements with a data volume of VL at UB address xAddr are loaded into xReg through LoadAlign (with the distribution pattern DIST_NORM). Cast is then called to convert float to int16_t and write the result to yReg. Finally, the data is stored back to UB address yAddr through StoreAlign (with the distribution pattern DIST_PACK_B32).

  **Figure 2** float-to-int16_t type conversion<a id="fig2"></a>

  ![](../../../../figures/float_to_int16_conversion.png "float-to-int16_t type conversion")

  ```cpp
  // When the bit width is converted from large to small, take float->int16_t as an example. 
  AscendC::Reg::LoadAlign<float, AscendC::Reg::PostLiteral::POST_MODE_UPDATE>(xReg, xAddr, oneRepeatSize);  
  AscendC::Reg::Cast<int16_t, float, castTrait>(yReg, xReg, mask); 
  AscendC::Reg::StoreAlign<int16_t, AscendC::Reg::PostLiteral::POST_MODE_UPDATE, AscendC::Reg::StoreDist::DIST_PACK_B32>(yAddr, yReg, oneRepeatSize, mask); 
  ```
