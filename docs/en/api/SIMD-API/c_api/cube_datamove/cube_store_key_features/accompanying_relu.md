---
okftype: reference.concept
description: "Describes the in-line ReLU capability during cube move-out: the activation mode is described using the four modes of asc_relu_pre_mode, namely NONE, NORMAL, SCALAR, and VECTOR, and the coefficient configuration entry is provided; Atlas A3/A2 uses the integer relu_pre parameter and cannot directly apply the configuration reload dedicated to Ascend 950PR&950DT."
---
# In-line ReLU

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T11:57:34.810Z pushedAt=2026-11-18T08:15:34.363Z nl=2 -->

## Feature Description

The store-out process of cube computation supports the accompanying ReLU capability. The following accompanying ReLU capabilities are currently supported.

Accompanying activation processes values when results are stored out. The C API entry points are [asc_copy_l0c2gm](../cube_compute_store/asc_copy_l0c2gm.md) and [asc_copy_l0c2l1](../cube_compute_store/asc_copy_l0c2l1.md).
<!-- npu="950" id2 -->
The Ascend 950PR&950DT products also include [asc_copy_l0c2ub](../cube_compute_store/asc_copy_l0c2ub.md). The following table uses the [asc_relu_pre_mode](../../defs/enum/asc_relu_pre_mode.md) names of the Ascend 950PR&950DT products.
<!-- end id2 -->

<!-- npu="A3,910b" id3 -->
For the Atlas A3 products and Atlas A2 products, `relu_pre` uses an integer parameter and must be configured according to the supported range of the corresponding interfaces of the Atlas A3 products and Atlas A2 products. The configuration overload dedicated to the Ascend 950PR&950DT products cannot be directly applied.
<!-- end id3 -->

<!-- npu="950" id4 -->
The `asc_relu_pre_mode` enumeration and its configuration interfaces in the following table apply to the Ascend 950PR&950DT products.

| `asc_relu_pre_mode` Enumeration Value | Encoding | Functions and Configuration |
| --- | --- | --- |
| `NONE` | 0 | Activation is not enabled. |
| `NORMAL` | 1 | Normal ReLU, with the negative half-axis coefficient set to 0. |
| `SCALAR` | 2 | The entire cube shares an activation coefficient, configured using [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md). |
| `VECTOR` | 3 | Each N channel has an independent activation coefficient. The coefficient is first moved in, and then `relu_pre_addr` is configured using [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md). |
<!-- end id4 -->

For details on combined use with accompanying quantization, see [Accompanying Quantization and Accompanying ReLU Scenario Combination](accompanying_quantization_and_relu_scenario_combination.md).

Vector activation parameters must be moved into the activation parameter area of the Fixpipe Buffer through [asc_copy_l12fb](../cube_compute_load/asc_copy_l12fb.md), and then `relu_pre_addr` is configured through [asc_set_l0c_copy_config](../cube_store_aux_config/asc_set_l0c_copy_config.md). The activation area and the quantization area are independent. For the unit and capacity limits of the configured address, see the corresponding interface.

<!-- npu="950" id1 -->
The store-out interfaces of the Ascend 950PR&950DT products also provide `enable_clip_relu_pre`, whose use must be combined with Normal ReLU and quantization mode. You cannot set only this switch while omitting the supporting conditions. For the specific supported combinations and parameter requirements, see the parameter and constraint descriptions in [asc_copy_l0c2gm (Ascend 950PR&950DT products)](../cube_compute_store/asc_copy_l0c2gm_arch_3510.md). For the related Scalar configuration interfaces, see [asc_set_l0c_copy_relu_alpha](../cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) and [asc_set_l0c_copy_lrelu_alpha](../cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md).
<!-- end id1 -->

For the complete move-out invocation of Normal ReLU, see [asc_copy_l0c2gm sample](../../../../../../../examples/02_simd_c_api/03_c_api/03_matrix_compute/asc_copy_l0c2gm/README.md) scenario 6.
