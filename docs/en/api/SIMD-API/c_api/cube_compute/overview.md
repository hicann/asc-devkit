# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T09:57:43.268Z pushedAt=2026-11-18T08:15:34.207Z nl=2 -->

## Cube Computation Overview

The cube computation C API provides cube multiply-accumulate and its auxiliary configuration APIs.

### Cube Compute Unit

The Cube compute unit is dedicated to executing cube operations. The dedicated caches it directly accesses are as follows: the L0A Buffer is used to store the left cube, the L0B Buffer is used to store the right cube, and the L0C Buffer is used to store the initialization accumulation values and the cube computation results.

<!-- npu="910b,A3,950" id1 -->
The highlighted part in the following figure shows the Cube compute unit and the dedicated caches it directly accesses.
<!-- end id1 -->

<!-- npu="910b,A3" id2 -->
**Figure 1** [NPU architecture version 2201](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) cube compute unit architecture diagram

![](../../../figures/architecture_of_cube_compute_unit_a2a3.png "Cube compute unit architecture diagram - A2A3")
<!-- end id2 -->

<!-- npu="950" id3 -->
**Figure 2** [NPU architecture version 3510](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md) cube compute unit architecture diagram

![](../../../figures/npu_3510_hw_arch_cube.png "Cube compute unit architecture diagram - 950")
<!-- end id3 -->

The storage units associated with cube computation are as follows:

**Table 1** Storage units associated with cube computation

| Buffer Type | Description |
| ----------- | ------ |
| L0A Buffer | An internal physical storage unit of the AI Core, typically used to store the left cube of cube computation. |
| L0B Buffer | An internal physical storage unit of the AI Core, typically used to store the right cube of cube computation. |
| L0C Buffer | An internal physical storage unit of the AI Core, typically used to store the results of cube computation and the cube accumulation initialization values. |
| L1 Buffer | An internal physical storage unit of the AI Core with relatively large space, typically used to cache the input data of cube computation. The input of cube computation generally needs to be moved from GM to the L1 Buffer, and then moved to the L0A Buffer and L0B Buffer respectively. |
| Fixpipe Buffer | An internal physical storage unit of the AI Core, typically used to store data such as quantization parameters required during the Fixpipe movement process. |
| BiasTable Buffer | Bias storage, an internal physical storage unit of the AI Core, typically used to store the Bias data required for cube computation. |

For the coordination of each storage unit and pipeline during computation, see [Cube Computation Flow](#cube-computation-flow); for the load-in and store-out paths and their supporting APIs, see [Overall Description of Cube Load-In](../cube_datamove/cube_compute_load/cube_compute_load_overall_description.md) and [Overall Description of Cube Store-Out](../cube_datamove/cube_compute_store/cube_compute_store_overall_description.md).

### Cube Computation Flow

#### Regular Cube Computation Flow

The C API provides data load-in, cube computation, and result store-out APIs for the cube computation programming model, which respectively carry the data movement and computation capabilities of each path in the Cube core, as shown in the following figure:

**Figure 3** Regular cube basic computation flowchart

![](../../../figures/matrix_computation_workflow_c_api.png "Regular cube basic computation flowchart")

1. Load in the arranged A and B cubes through [asc_copy_gm2l1](../cube_datamove/cube_compute_load/asc_copy_gm2l1.md). For ND inputs, use [asc_copy_gm2l1_nd2nz](../cube_datamove/cube_compute_load/asc_copy_gm2l1_nd2nz.md) to complete the fractal conversion. Bias and the accompanying quantization coefficients are also loaded into the L1 Buffer first.

2. Load A and B into the L0A Buffer and L0B Buffer respectively through [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a.md) and [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b.md). Select the transpose variant based on the input layout. Bias and quantization coefficients are loaded into the corresponding buffers through [asc_copy_l12bt](../cube_datamove/cube_compute_load/asc_copy_l12bt.md) and [asc_copy_l12fb](../cube_datamove/cube_compute_load/asc_copy_l12fb.md) respectively.

3. Perform computation through [asc_mmad](mmad_compute/asc_mmad.md) and output to the L0C Buffer. The parameter order is `m, k, n`. For the first computation from zero, set `c_matrix_init_val=true`. For subsequent K-block accumulation, set `c_matrix_source=false` and `c_matrix_init_val=false`. For the parameter description of Bias initialization, see [asc_mmad](mmad_compute/asc_mmad.md).

4. Store out the result through [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm.md), and configure the accompanying quantization, activation, and output format. The result can also be moved to the L1 Buffer. For details, see [Overall Description of Store-out](../cube_datamove/cube_compute_store/cube_compute_store_overall_description.md).

<!-- npu="950" id4 -->
#### Mx Cube Computation Flow

Mx cube computation is supported only on the Ascend 950PR&950DT products.

MX cube multiplication (Microscaling cube multiply-accumulate) is cube multiplication with quantization coefficients, that is, both the left cube and the right cube have corresponding quantization coefficient cubes: the left quantization coefficient cube scaleA and the right quantization coefficient cube scaleB. In the MX cube multiplication scenario, the left quantization coefficient cube is multiplied with the left cube, the right quantization coefficient cube is multiplied with the right cube, and cube multiplication is performed on the results of the two products. The C API supports MX cube computation through asc_mmad_mx and independent data/coefficient move APIs, as shown in the following figure:

**Figure 4** Mx cube basic computation flowchart

![](../../../figures/mx_matrix_computation_workflow_c_api.png "Mx cube basic computation flowchart")

1. Move A, B, and the [scaleA/scaleB coefficient cubes](cube_compute_fractal_intro/aux_cube_fractal_format_details.md) from GM to the L1 Buffer. The A/B data fractal differs from the coefficient fractal, so configure their respective layouts, sizes, and strides separately.

2. Use [asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) and [asc_copy_l12l0b](../cube_datamove/cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) to load in A/B; additionally call [asc_copy_l12l0a_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0a_mx.md) and [asc_copy_l12l0b_mx](../cube_datamove/cube_compute_load/asc_copy_l12l0b_mx.md) to load in scaleA/scaleB.

3. After the data, coefficients, and optional Bias are ready, use [asc_mmad_mx](mmad_compute/asc_mmad_mx.md) to compute and output the result to the L0C Buffer.

4. Use [asc_copy_l0c2gm](../cube_datamove/cube_compute_store/asc_copy_l0c2gm_arch_3510.md) to process and move out the result.
<!-- end id4 -->

<!-- npu="910b,A3" id5 -->
#### 2-out-of-4 Sparse Cube Computation Flow

A sparse cube is a special type of cube that contains a relatively large number of zero elements. 2-out-of-4 structured sparse cube computation requires that in a group of four consecutive weights or activation values (usually a row or column in a tensor), at most two values are non-zero, and the remaining two are forced to zero. For the Atlas A2 products and Atlas A3 products, Ascend C provides corresponding sparse APIs for the 2-out-of-4 sparse cube computation programming model, as shown in the following figure:

**Figure 5** 2-out-of-4 sparse cube basic computation flowchart

![](../../../figures/sparse_matrix_computation_workflow_c_api.png "2-out-of-4 sparse cube basic computation flowchart")

1. First, divide the original cube B into groups of four consecutive elements at a fixed granularity, and generate the corresponding compressed cube B and [index cube](cube_compute_fractal_intro/aux_cube_fractal_format_details.md#2-out-of-4-sparse-index-cube) offline as inputs according to the 2-out-of-4 structured sparse compression rule.

2. Use the asc_copy_gm2l1 API to move the GM data of the original cube A, compressed cube B, and index cube to the L1 Buffer (if quantization along the way exists, it is also moved to the L1 Buffer through asc_copy_gm2l1).

3. Use the asc_copy_l12l0a API to load the original cube A into the L0A Buffer, and use the [asc_copy_l12l0b_sparse API](../cube_datamove/cube_compute_load/asc_copy_l12l0b_sparse.md) to load the compressed cube B and the index cube into the L0B Buffer and the built-in IDX Buffer respectively for computation (if quantization along the way exists, use asc_copy_l12fb to move the quantization coefficient data in the L1 Buffer to the Fixpipe Buffer).

4. Use the asc_mmad_sparse API to perform 2-out-of-4 sparse cube computation on the data in the L0A Buffer, L0B Buffer, and built-in IDX Buffer, and output the result to the L0C Buffer. For details, see [asc_mmad_sparse computation](mmad_compute/asc_mmad_sparse.md).

5. Use the asc_copy_l0c2gm API to process the data in the L0C Buffer and move it out to GM. The asc_copy_l0c2gm API can use the Fixpipe Buffer data to perform operations such as in-line quantization and Relu. For details, see [asc_copy_l0c2gm (L0C Buffer to GM data movement)](../cube_datamove/cube_compute_store/asc_copy_l0c2gm.md).
<!-- end id5 -->

#### Conv2D Forward Propagation Flow

The essence of Conv2D forward propagation is to convert the convolution operation into cube multiplication: first, use img2col (Image to Column) to expand the FeatureMap into the left cube A according to the convolution kernel sliding window, use the Weight as the right cube B, and then perform cube multiply-accumulate to obtain the output. The difference from the [Regular Cube Computation Flow](#regular-cube-computation-flow) is that after cube A is loaded in directly from GM, the [3D reload of asc_copy_l12l0a](../cube_datamove/cube_compute_load/asc_copy_l12l0a.md) completes the img2col spatial expansion online during the transfer from L1 Buffer to L0A Buffer, based on the convolution kernel size, stride, dilation, and padding, as shown in the following figure:

**Figure 6** Conv2D forward propagation flowchart

![](../../../figures/conv2d_forward_c_api.png "Conv2D forward propagation flowchart")

1. Load the FeatureMap and Weight from GM into the L1 Buffer. The FeatureMap is prepared in the [NC1HWC0 format](cube_compute_fractal_intro/key_fractal_format_details.md#nc1hwc0-format); the Weight is prepared according to the layout required for the subsequent L0B Buffer transfer.

2. Use the 3D reload of `asc_copy_l12l0a` to perform img2col, and load the Weight through `asc_copy_l12l0b`. Before the call, configure the FeatureMap attributes, repeat stride, boundary, and padding. For specific APIs, see [Overall Description of Loading In](../cube_datamove/cube_compute_load/cube_compute_load_overall_description.md).

3. Use [asc_mmad](mmad_compute/asc_mmad.md) to complete the cube multiply-accumulate, and store the result in the L0C Buffer.

4. Use the [Nz2ND](../cube_datamove/cube_store_key_features/NZ2ND.md) capability of `asc_copy_l0c2gm` to output the convolution result as NHWC. For dimension mapping, see [Convolution Format Conversion](../cube_datamove/cube_store_key_features/convolution_format_conversion.md).<!-- npu="950" id6 -->In particular, for the Ascend 950PR&950DT products, the convolution result can also be output in NCHW format through [Nz2DN](../cube_datamove/cube_store_key_features/NZ2DN.md).<!-- end id6 -->

## API List

### Mmad Computation

**Table 2** Mmad computation APIs

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_mmad](mmad_compute/asc_mmad.md) | Performs the cube multiply-accumulate operation. | `c_api/cube_compute/cube_compute.h` |
| [asc_mmad_mx](mmad_compute/asc_mmad_mx.md) | Performs cube multiply-accumulate computation with quantization coefficients for the left and right cubes. | `c_api/cube_compute/cube_compute.h` |
| [asc_mmad_sparse](mmad_compute/asc_mmad_sparse.md) | Performs the cube multiply-accumulate operation, where the input left cube A is a sparse cube and the right cube B is a dense cube. | `c_api/cube_compute/cube_compute.h` |

### Mmad Computation Auxiliary Configuration APIs

**Table 3** Mmad computation auxiliary configuration APIs

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_disable_hf32](mmad_compute_aux_config/asc_disable_hf32.md) | Used to disable HF32 mode for Mmad computation. After this mode is set, the FP32 data in L0A Buffer/L0B Buffer is not rounded before participating in Mmad computation. | `c_api/cube_compute/cube_compute.h` |
| [asc_disable_hif8](mmad_compute_aux_config/asc_disable_hif8.md) | Used to disable HiF8 mode for Mmad computation. When both left cube A and right cube B are of the `fp8_e4m3fn_t` type, the data is not converted to the `hifloat8_t` type and directly participates in cube multiplication as the `fp8_e4m3fn_t` type. | `c_api/cube_compute/cube_compute.h` |
| [asc_enable_hf32](mmad_compute_aux_config/asc_enable_hf32.md) | Used to enable HF32 mode for Mmad computation. After this mode is enabled, the FP32 data in L0A Buffer/L0B Buffer is rounded to HF32 before participating in Mmad computation. | `c_api/cube_compute/cube_compute.h` |
| [asc_enable_hif8](mmad_compute_aux_config/asc_enable_hif8.md) | Enables HiF8 mode. When both left cube A and right cube B are of the `fp8_e4m3fn_t` type, the data is converted to the `hifloat8_t` type before participating in cube multiplication. | `c_api/cube_compute/cube_compute.h` |
| [asc_set_hf32_round_mode](mmad_compute_aux_config/asc_set_hf32_round_mode.md) | Sets the rounding method for HF32 mode. This configuration takes effect only while HF32 mode is enabled. | `c_api/cube_compute/cube_compute.h` |
| [asc_set_mmad_direction_m](mmad_compute_aux_config/asc_set_mmad_direction_m.md) | Sets mmad computation to preferentially generate results through the M direction of M/N, and then generate results through the N direction. M is the row of the cube, and N is the column of the cube. | `c_api/cube_compute/cube_compute.h` |
| [asc_set_mmad_direction_n](mmad_compute_aux_config/asc_set_mmad_direction_n.md) | Sets mmad computation to preferentially generate results through the N direction of M/N, and then generate results through the M direction. M is the row of the cube, and N is the column of the cube. | `c_api/cube_compute/cube_compute.h` |
