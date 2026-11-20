---
okftype: reference.concept
description: "Overall description of cube computation move-out, introducing the move-out process from L0C Buffer to Global Memory, L1 Buffer, and UB, real-time quantization, real-time ReLU, real-time format conversion, and channel split and merge capabilities, and providing the Fixpipe Buffer quantization mode layout diagram."
---
# Cube Computation Move-Out Overview

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:19:38.801Z pushedAt=2026-11-18T08:15:34.308Z nl=2 -->

The move-out of cube computation is a core set of interfaces for data transfer in the Ascend C programming framework, mainly implementing data transfer from the L0C Buffer to Global Memory and the L1 Buffer. This interface series provides multiple data transfer modes to meet the data transfer requirements in cube computation, and is typically used in conjunction with the asc_mmad interface.

The move-out interfaces of cube computation support multiple real-time transfer scenarios, including **real-time quantization, real-time ReLU, real-time format conversion (Nz2ND, Nz2DN), real-time channel split, and real-time channel merge**. By configuring interface parameters and auxiliary registers, developers can precisely control key information such as the data transfer path, start position, transfer length, and real-time transfer capabilities, fully leveraging the hardware data transfer capability to maximize the overall execution performance of the operator.

<!-- npu="950,A3,910b" id1 -->
The overall flow of cube computation move-out is shown in the following figure:
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
**Figure 1** Overall flow of cube computation move-out ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_execution_flow_a2a3.png)
<!-- end id2 -->

<!-- npu="950" id3 -->
**Figure 2** Overall flow of cube computation move-out ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

![](../../../../figures/fixpipe_execution_flow_a5.png)
<!-- end id3 -->

Among them, the memory layout corresponding to different quantization modes of the Fixpipe Buffer is as follows:

**Figure 3** Fixpipe Buffer memory layout

![](../../../../figures/fixpipe_buffer.png)

For the store interfaces and supported architecture versions, see [L0C to GM](asc_copy_l0c2gm.md) and [L0C to L1](asc_copy_l0c2l1.md). For output layouts, see [NZ2NZ](../cube_store_key_features/NZ2NZ.md) and [NZ2ND](../cube_store_key_features/NZ2ND.md); for numerical processing, see [real-time quantization](../cube_store_key_features/accompanying_quantization.md), [real-time ReLU](../cube_store_key_features/accompanying_relu.md), and [scenario combination](../cube_store_key_features/accompanying_quantization_and_relu_scenario_combination.md). For loading quantization/activation parameters, see [asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb.md); for the corresponding register configuration, see [cube store auxiliary configuration interfaces](../cube_store_aux_config/cube_store_aux_config.md).
<!-- npu="950" id4 -->
The Ascend 950PR&950DT products also support [L0C to UB](asc_copy_l0c2ub.md) store; for the L0C to GM output layout, [NZ2DN](../cube_store_key_features/NZ2DN.md) is also available.
<!-- end id4 -->
