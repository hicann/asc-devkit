# 3510 New Feature Usage Guide

<!-- md-trans-meta sourceCommit=5e55c38a4e567465079c4ca7387d07ab9e6347c5 translatedAt=2026-10-23T11:00:19.090Z pushedAt=2026-11-03T07:05:13.258Z nl=2 -->

This document describes the new features of [NPU Architecture Version 3510](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), providing a reference for developers to migrate operator programs from other architectures to the 3510 architecture. For the overall changes of the 3510 architecture, see [2201 to 3510 Architecture Changes](../3510_arch_migration/2201_to_3510_arch_changes.md).

## Feature Overview

**Table 1**  3510 new feature overview<a name="table_3510_new_feature_overview"></a>

| Category | Feature | Related Basic APIs | Related C APIs |
|------|------|------------|-----------|
| Data Movement | [New UB-to-L1 Buffer data movement path](#section_ub2l1) | [DataCopy (UB to L1 Buffer)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md), [DataCopyPad (UB to L1 Buffer)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md) | [asc_copy_ub2l1](../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md) |
| Data Movement | [New L1 Buffer-to-UB data movement path](#section_l12ub) | [DataCopyL1ToUB](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md) | [asc_copy_l12ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md) |
| Data Movement | [New L0C Buffer-to-UB data movement path](#section_fixpipe_l0c2ub) | [Fixpipe (L0C Buffer to UB)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md) | [asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md) |
| Data Movement | [New ND-DMA multi-dimensional data movement](#section_nddma) | [DataCopy (GM-to-UB multi-dimensional data movement)](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md), [NdDmaDci](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci.md) | [asc_ndim_copy_gm2ub](../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md) |
| Data Movement | [New loop mode for aligned/unaligned data movement](#section_loop_mode) | [SetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara.md), [ResetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/ResetLoopModePara.md) | [asc_set_gm2l1_loop_size, etc.](../../../api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_loop_size.md), [asc_set_ub2gm_loop1_stride, etc.](../../../api/SIMD-API/c_api/vector_datamove/asc_set_ub2gm_loop1_stride.md) |
| Cube Computation | [New MX cube movement and computation](#section_mx) | [LoadData (2D MX movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md), [MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md) | [asc_copy_l12l0a_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_mx.md), [asc_copy_l12l0b_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_mx.md), [asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md) |
| Cube Computation | [2D cube movement (on-the-fly transpose) capability enhancement](#section_transpose) | [LoadDataWithTranspose](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md) | [asc_copy_l12l0b_trans (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md) |
| Cube Computation | [New NZ2DN on-the-fly conversion in Fixpipe](#section_nz2dn) | [Fixpipe (L0C Buffer to GM, L1 Buffer, UB)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md) | [asc_copy_l0c2gm (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1 (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md), [asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md) |
| Cube Computation | [2D cube movement capability enhancement](#section_loaddata_2d_v2) | [LoadData (2D cube movement V2)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md), [LoadData (GM-to-L1 Buffer 2D V2)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md) | [asc_copy_l12l0a (2D, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md), [asc_copy_l12l0b (2D, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md), [asc_copy_gm2l1 (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md) |
| Cube Computation | [New Stride convolution cube movement](#section_loaddata_with_stride) | [LoadDataWithStride](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md) | [asc_copy_l12l0a (3D, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md), [asc_copy_l12l0b (3D, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md), [asc_set_l13d_rpt](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l13d_rpt.md), [asc_set_l3d_rpt_b](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l3d_rpt_b.md) |
| Cube Computation | [Cube computation supports more data types](#section_mmad_data_types) | [Mmad](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md), [MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md) | [asc_mmad](../../../api/SIMD-API/c_api/cube_compute/asc_mmad.md), [asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md) |
| Cube Computation | [L0A Buffer fractal layout changed to NZ](#section_l0a_nz) | - | - |
| Vector Computation | [Vector Computation basic API extension](#section_vec_api_ext) | [Related Basic APIs](#section_vec_api_ext) | - |
| Vector Computation | [New Regbase vector computation method](#section_regbase) | [Reg vector computation basic APIs](../../../api/SIMD-API/basic_api/reg_vector_compute/overview.md) | [Reg vector computation C APIs](../../../api/SIMD-API/c_api/reg_compute/reg_compute.md) |
| Atomic Operations | [Atomic Operation API extension](#section_atomic_api) | [AtomicAdd, etc.](../../../api/SIMD-API/basic_api/atomic_operations/AtomicAdd.md) | [asc_atomic_add](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md), [asc_atomic_max](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_max.md), [asc_atomic_min](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_min.md), [asc_atomic_exch](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_exch.md), [asc_atomic_cas](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_cas.md) |
| Cache | [New DCI (data cache invalidation) capability](#section_dci) | – | [asc_dci](../../../api/SIMD-API/c_api/cache_ctrl/asc_dci.md) |
| Storage | [New SSBuffer on-chip storage unit](#section_ssbuf) | [GetSsbufBaseAddr](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md) | – |
| Synchronization | [New Mutex intra-core synchronization capability](#section_mutex) | [Mutex::Lock/Unlock](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md), [AllocMutexID](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI.md) | [asc_lock](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md), [asc_unlock](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md) |
| Synchronization | [New inter-core synchronization control mode 4](#section_crosscore) | [CrossCoreSetFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md), [CrossCoreWaitFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) | [asc_sync_intra_arrive](../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_arrive.md), [asc_sync_intra_wait](../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_wait.md) |

> [!NOTE]
> In addition to the features listed in the table above, the 3510 architecture also introduces the SIMT programming model and SIMD-SIMT hybrid programming. For details, see:
> - SIMT programming: [AI Core SIMT Programming](../../programming_guide/programming_model/ai_core_simt_programming/ai_core_simt_programming.md), [SIMT API](../../../api/SIMT-API/overview.md).
> - SIMD-SIMT hybrid programming: [SIMD and SIMT Hybrid Programming](../../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/overview.md).

## Feature Description

### New UB to L1 Buffer Data Movement Path<a name="section_ub2l1"></a>

#### Feature Application Scenario Description

In CV fused operators, data moved from UB to L1 Buffer no longer needs to be transferred through GM.

#### Function Description

- For C APIs, use the [asc_copy_ub2l1](../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2l1.md) API to move data from UB to L1 Buffer. No compilation option needs to be configured.
- For basic APIs, use [DataCopy (UB to L1 Buffer)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous.md) and [DataCopyPad (UB to L1 Buffer)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopyPad_UBToL1.md) to move data. To enable this feature, configure the compilation option [`ENABLE_CV_COMM_VIA_SSBUF`](../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches):
  - When this compilation option is enabled, data is moved directly through the hardware channel between UB and L1 Buffer. This method does not require transfer through GM and delivers higher performance.
  - When this compilation option is not enabled, data must be moved to L1 Buffer through GM. In this scenario, the UB-to-L1 Buffer movement API is implemented through software emulation.

#### Typical Example Description<a name="section_ub2l1_sample"></a>

- [data_copy_ub2l1 sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1): In the Mmad cube multiplication scenario, this sample implements UB-to-L1 Buffer data movement based on DataCopy. The compilation option `SCENARIO_NUM` switches between two scenarios: continuous movement and on-the-fly ND2NZ movement. A value of **1** indicates continuous movement, and a value of **2** indicates on-the-fly ND2NZ movement. The complete data flow of the sample is GM → UB → L1 Buffer → L0A Buffer, L0B Buffer → Mmad → L0C Buffer → GM. The sample compilation project has configured `-DENABLE_CV_COMM_VIA_SSBUF=true` through `ascendc_compile_definitions`.

### New L1 Buffer to UB Data Movement Path<a name="section_l12ub"></a>

#### Feature Application Scenario Description

In the CV fused operator scenario, data is moved from L1 Buffer to UB. Architecture version 3510 adds the L1 Buffer-to-UB data movement path.

#### Function Description

Refer to the following resources:
- Basic API: [DataCopyL1ToUB](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB.md).
- C API: [asc_copy_l12ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12ub.md).

### New L0C Buffer to UB Data Movement Path<a name="section_fixpipe_l0c2ub"></a>

#### Feature Application Scenario Description

In CV fused operator scenarios, if the result after cube computation needs to undergo vector computation such as normalization and activation on the Vector core, the data in the L0C Buffer must first be moved to the UB. The 3510 architecture adds a unidirectional data movement path from the L0C Buffer to the UB.

#### Function Description

Refer to the following resources:
- Basic API: [Fixpipe (L0C Buffer to UB data movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToUB.md). Use the Fixpipe API with the `FixpipeParamsArch3510` parameter to move data out of the L0C Buffer to the UB.
- C API: [asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md).


#### Typical Example Description

[fixpipe_l0c2ub example](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2ub): Implements L0C Buffer to UB data movement based on Fixpipe. The complete data flow is GM to L1 Buffer to L0A Buffer, L0B Buffer to Mmad to L0C Buffer to UB.

### New ND-DMA Multi-dimensional Data Movement<a name="section_nddma"></a>

#### Feature Application Scenario Description

In vector operator scenarios, when moving data from GM to UB, if complex data transformations such as multi-dimensional cubes, irregular strides, and Padding, Transpose, Broadcast, or Slice are involved, the basic movement APIs need to be called multiple times. The 3510 architecture adds the ND-DMA hardware instruction. Compared with the basic data movement APIs, this instruction supports flexible configuration of the dimension information and stride of the moved data, and can complete the data movement of the above complex scenarios with a single instruction.

#### Function Description

Refer to the following resources:
- Basic APIs: [DataCopy (GMToUB multi-dimensional data movement NDDMA)](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMToUB_NDDMA.md), [NdDmaDci](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/NdDmaDci.md).
- C APIs: [asc_ndim_copy_gm2ub](../../../api/SIMD-API/c_api/vector_datamove/asc_ndim_copy_gm2ub.md).

#### Typical Example Description

[data_copy_gm2ub_nddma example](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_nddma): Implements GM-to-UB multi-dimensional data movement based on ND-DMA instructions, simulating data movement of a moving window under multi-dimensional shapes, including the configuration of Slice, input Stride, and non-contiguous source data movement.

### Aligned/Unaligned Data Movement Adds Loop Mode<a name="section_loop_mode"></a>

#### Feature Application Scenario Description

In vector operator scenarios, when multiple data segments in GM need to be alternately moved to different addresses in UB, or moved in opposite directions, multiple DataCopy instructions usually need to be executed in a loop, which increases the instruction count and pipeline overhead. The 3510 architecture adds loop-mode data movement capability, allowing the movement to be completed with a single API call, thereby reducing instruction dispatch overhead and improving movement efficiency.

#### Function Description

Refer to the following resources:
- Basic APIs: [SetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/SetLoopModePara.md), [ResetLoopModePara](../../../api/SIMD-API/basic_api/memory_vector_compute/data_move_aux_config/ResetLoopModePara.md).
- C APIs: [asc_set_gm2l1_loop_size](../../../api/SIMD-API/c_api/cube_datamove/asc_set_gm2l1_loop_size.md), [asc_set_ub2gm_loop1_stride](../../../api/SIMD-API/c_api/vector_datamove/asc_set_ub2gm_loop1_stride.md), etc.


### New MX Cube Movement and Computation<a name="section_mx"></a>

#### Feature Application Scenario Description

MicroScaling (MX) is a low-bit quantization data format, such as fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e5m2_t, and fp8_e4m3fn_t. Cube data is stored together with the quantization coefficient cube corresponding to each data block, so the cube data and the quantization coefficient cube must be moved synchronously during cube computation. The 3510 architecture adds data movement capabilities for MicroScaling scenarios, which can move cube data and the quantization coefficient cube synchronously, providing support for cube computation in the MX low-bit format.

#### Function Description

Refer to the following resources:
- Basic APIs: [LoadData (2D MX movement)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_MX.md), [MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md).
- C APIs: [asc_copy_l12l0a_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a_mx.md), [asc_copy_l12l0b_mx](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_mx.md), [asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md).

#### Typical Example Description

- [load_data_2dmx_l12l0 example](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dmx_l12l0): Demonstrates the synchronous movement of MX cube data and its quantization coefficient cube.
- [mmad_mx example](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/mmad_mx): Demonstrates the MmadMx cube multiply-accumulate computation.

### 2D Cube Movement (On-the-Fly Transpose) Capability Enhancement<a name="section_transpose"></a>

#### Feature Application Scenario Description

In cube multiplication, the right cube B in L1 Buffer needs to be moved to L0B Buffer in a transposed fractal layout. For scenarios where the fractals in the source cube are non-contiguously arranged, the movement parameters of the 2201 architecture use the concatenated block cube as the configuration unit, making it difficult to directly describe the fractal-level gaps. The 3510 architecture enhances the on-the-fly transpose movement capability, allowing the movement start position, the source stride of adjacent iterations, and the source fractal gap within an iteration to be configured in units of 512-byte fractals. This applies to scenarios such as fractal concatenation and non-contiguous movement.

#### Function Description

Refer to the following resources:
- Basic API: [LoadDataWithTranspose](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose.md). The 3510 architecture uses `LoadData2dTransposeParamsV2` as the parameter type, supports configuring the source stride in units of 512-byte fractals, and allows configuring the source fractal gap within each iteration through `srcFracGap`.
- C API: [asc_copy_l12l0b_trans (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b_trans/asc_copy_l12l0b_trans_arch_3510.md).

### Fixpipe Adds NZ2DN On-the-Fly Conversion<a name="section_nz2dn"></a>

#### Feature Application Scenario Description

The Mmad computation result in the L0C Buffer is in NZ fractal format, while the output of some operators requires the column-major DN format. The Fixpipe interface of the 3510 architecture adds the NZ2DN on-the-fly conversion capability, which can complete format conversion during the move-out process from the L0C Buffer to GM, L1 Buffer, and UB.

#### Function Description

Refer to the following resources:
- Basic API: [Fixpipe](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM.md).
- C APIs: [asc_copy_l0c2gm (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2gm/asc_copy_l0c2gm_arch_3510.md), [asc_copy_l0c2l1 (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2l1/asc_copy_l0c2l1_arch_3510.md), [asc_copy_l0c2ub](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l0c2ub.md).

### 2D Cube Movement Capability Enhancement<a name="section_loaddata_2d_v2"></a>

#### Feature Application Scenario Description

In the 3510 architecture, the 2D cube movement capability is enhanced, making the movement method more flexible and supporting the movement of all data types.

#### Function Description

Refer to the following resources:
- Basic API: [LoadData (2D Cube Movement V2)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_2D_V2.md), [LoadData (2D Cube Movement V2 from GM to L1 Buffer)](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_GMToL1_2DV2.md).
- C API: [asc_copy_l12l0a (2D Cube Movement, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_2d_arch_3510.md), [asc_copy_l12l0b (2D Cube Movement, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_2d_arch_3510.md), [asc_copy_gm2l1 (arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_gm2l1/asc_copy_gm2l1_2d_arch_3510.md).

#### Typical Example Description

[load_data_2dv2_l12l0 example](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_2dv2_l12l0): demonstrates using the 2D V2 instruction to move cube data from the L1 Buffer to the L0A Buffer.

### New Stride Convolution Cube Movement<a name="section_loaddata_with_stride"></a>

#### Feature Application Scenario Description

Architecture 3510 adds the Stride Convolution Cube Movement capability, which is used for convolution data movement scenarios that require configuring the offset of the output cube in the K-axis direction.

#### Function Description

Refer to the following resources:
- Basic API: [LoadDataWithStride](../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithStride.md).
- C APIs: [asc_copy_l12l0a (3D cube movement, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0a/asc_copy_l12l0a_3d_arch_3510.md), [asc_copy_l12l0b (3D cube movement, arch3510)](../../../api/SIMD-API/c_api/cube_datamove/asc_copy_l12l0b/asc_copy_l12l0b_3d_arch_3510.md), [asc_set_l13d_rpt](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l13d_rpt.md), and [asc_set_l3d_rpt_b](../../../api/SIMD-API/c_api/cube_datamove/asc_set_l3d_rpt_b.md). Among them, `asc_set_l13d_rpt` and `asc_set_l3d_rpt_b` are used to configure the output cube stride of the left register group and the right register group, respectively.

#### Typical Example Description

[load_data_with_stride example](../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_with_stride): demonstrates Stride convolution cube movement.

### Cube Computation Supports More Data Types<a name="section_mmad_data_types"></a>

#### Feature Application Scenario Description

The 3510 architecture extends the low-bit floating-point input types supported by cube computation, enabling more cube computation scenarios.

#### Function Description

Compared with the 2201 architecture, the 3510 architecture adds support for the following cube input types:

- Cube computation adds support for hifloat8_t, fp8_e5m2_t, and fp8_e4m3fn_t.
- MX cube computation supports fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e5m2_t, and fp8_e4m3fn_t. MX computation uses a Scale cube of the fp8_e8m0_t type.

Refer to the following resources:
- Basic APIs: [Mmad](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md), [MmadMx](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/MmadMx.md).
- C APIs: [asc_mmad](../../../api/SIMD-API/c_api/cube_compute/asc_mmad.md), [asc_mmad_mx](../../../api/SIMD-API/c_api/cube_compute/asc_mmad_mx.md).

### L0A Buffer Fractal Layout Changes to NZ<a name="section_l0a_nz"></a>

#### Feature Application Scenario Description

The L0A Buffer of the 3510 architecture adopts the NZ layout instead of the ZZ layout used on the 2201 architecture.

#### Function Description

Due to the change in fractal layout, under the 3510 architecture, in the scenario where cube A is transposed and not aligned, the K direction of the L0A Buffer is aligned to 8, with no [K-direction alignment constraint](../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_key_features/k_direction_alignment_constraint.md#ZH-CN_TOPIC_0000002569070973).

### Vector Computation Basic API Extension<a name="section_vec_api_ext"></a>

#### Feature Application Scenario Description

The 3510 architecture Vector Computation basic APIs add multiple interfaces and parameter variants.

#### Function Description

**Table 3**  Vector Computation basic API extension description<a name="table_vec_api_ext"></a>

| API | Description |
|-----|---------|
| [Adds](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Adds_flexible_scalar.md) | Adds each element in the vector to a scalar. The scalar input supports configuring a single-point element of a LocalTensor, and supports both scalar-first and scalar-last scenarios. |
| [Muls](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Muls_flexible_scalar.md) | Multiplies each element in the vector by a scalar. The scalar input supports configuring a single-point element of a LocalTensor. |
| [Subs](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Subs_flexible_scalar.md) | Subtracts a scalar from each element in the vector. The scalar input supports configuring a single-point element of a LocalTensor. |
| [Divs](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Divs_flexible_scalar.md) | Divides each element in the vector by a scalar. The scalar input supports configuring a single-point element of a LocalTensor. |
| [Maxs](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Maxs_flexible_scalar.md) | Computes the maximum of each element in the vector and a scalar. The scalar input supports configuring a single-point element of a LocalTensor. |
| [Mins](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mins_flexible_scalar.md) | Computes the minimum of each element in the vector and a scalar. The scalar input supports configuring a single-point element of a LocalTensor. |
| [Compares](../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Compares_flexible_scalar.md) | Compares each element in the vector with a scalar. The scalar input supports configuring a single-point element of a LocalTensor, and supports both scalar-first and scalar-last scenarios. |
| [Select](../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/Select_flexible_scalar.md) | Selects elements from two source operands based on the bits of `selMask` and writes them to the destination operand. |
| [Neg](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Neg_ISASI.md) | Negates the source operand. |
| [Prelu](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Prelu_ISASI.md) | Performs ReLU computation with a parameter. |
| [Mull](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Mull_ISASI.md) | Multiplies 32-bit elements to obtain a 64-bit product, and writes the low 32 bits and high 32 bits to two destination operands respectively. |
| [AbsSub](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/AbsSub_ISASI.md) | Computes the absolute value of the difference between two source operands. |
| [ExpSub](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/ExpSub_ISASI.md) | Computes `exp(src0)-exp(src1)`. |
| [MulsCast](../../../api/SIMD-API/basic_api/memory_vector_compute/composite_compute/MulsCast_ISASI.md) | Multiplies a vector by a scalar and then performs data type conversion. |
| [Truncate](../../../api/SIMD-API/basic_api/memory_vector_compute/type_conversion/Truncate_ISASI.md) | Performs a floor operation on the source operand. |
| [Interleave](../../../api/SIMD-API/basic_api/memory_vector_compute/data_rearrange_ISASI/Interleave.md) | Interleaves the elements of two source operands into the result operand. |
| [DeInterleave](../../../api/SIMD-API/basic_api/memory_vector_compute/data_rearrange_ISASI/DeInterleave.md) | Splits the elements of the source operand into two result operands in an interleaved manner. |
| [ShiftLeft](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftLeft_shift_amount_tensor.md) | Shifts each element of the source operand left by a shift amount specified by a tensor input. |
| [ShiftRight](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/ShiftRight_shift_amount_tensor.md) | Shifts each element of the source operand right by a shift amount specified by a tensor input. |
| [Ands](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Ands.md) | Performs a bitwise AND operation on each element in the vector and a scalar. The scalar input supports configuring a single-point element of a LocalTensor. |
| [Ors](../../../api/SIMD-API/basic_api/memory_vector_compute/logical_compute/Ors.md) | Performs a bitwise OR operation on each element in the vector and a scalar. The scalar input supports configuring a single-point element of a LocalTensor. |


### New Regbase Vector Computation Method<a name="section_regbase"></a>

#### Feature Application Scenario Description

The AIV core of the 3510 architecture adopts the Regbase architecture, where intermediate results can be temporarily stored in registers, thereby reducing UB reads and writes and improving execution efficiency. For the programming model of the Regbase architecture, see [Reg Vector Computation Programming](../../programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/reg_vector_computation.md).

#### Function Description

For the complete list of Reg Vector Computation related APIs, refer to the [Reg Vector Computation](../../../api/SIMD-API/basic_api/reg_vector_compute/overview.md) document.

For the corresponding C APIs, see [Reg Vector Computation C APIs](../../../api/SIMD-API/c_api/reg_compute/reg_compute.md).

#### Typical Example Description

- Data movement: [reg_load_store_align sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_store_align), [reg_load_store_mask sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_store_mask), [reg_load_store_unalign sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_store_unalign), [reg_load_gather sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_load_gather), [reg_store_scatter sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_store_scatter), [reg_move sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_move), [reg_auxscalar sample](../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/reg_auxscalar).
- Reg computation: [mul sample](../../../../../examples/01_simd_cpp_api/03_basic_api/02_reg_vector_compute/mul), [reduce sample](../../../../../examples/01_simd_cpp_api/03_basic_api/02_reg_vector_compute/reduce), [reduce_pair sample](../../../../../examples/01_simd_cpp_api/03_basic_api/02_reg_vector_compute/reduce_pair), etc.
- High-performance operators: [gelu_high_performance sample](../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance), [gelu_eltwise_high_performance sample](../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_eltwise_high_performance), [softmax_high_performance sample](../../../../../examples/01_simd_cpp_api/05_best_practices/02_reg_compute/softmax_high_performance).

### Atomic Operation API Extension<a name="section_atomic_api"></a>

#### Feature Application Scenario Description

In scenarios where multiple threads or multiple cores concurrently access the same data in GM, atomic read-modify-write operations need to be performed on the data at the GM address to avoid data races caused by concurrent writes. The 3510 architecture extends the basic atomic operation APIs.

#### Function Description

**Table 5**  New atomic operation APIs<a name="table_atomic_api"></a>

| Description | Related Basic APIs | Related C APIs |
|---------|------------|-----------|
| Sums the old value at the GM address with the input scalar, writes the result back, and returns the value before modification. | [AtomicAdd](../../../api/SIMD-API/basic_api/atomic_operations/AtomicAdd.md) | [asc_atomic_add](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_add.md) |
| Takes the maximum of the old value at the GM address and the input scalar, writes the result back, and returns the value before modification. | [AtomicMax](../../../api/SIMD-API/basic_api/atomic_operations/AtomicMax.md) | [asc_atomic_max](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_max.md) |
| Takes the minimum of the old value at the GM address and the input scalar, writes the result back, and returns the value before modification. | [AtomicMin](../../../api/SIMD-API/basic_api/atomic_operations/AtomicMin.md) | [asc_atomic_min](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_min.md) |
| Exchanges the old value at the GM address with the input scalar and returns the value before modification. | [AtomicExch](../../../api/SIMD-API/basic_api/atomic_operations/AtomicExch.md) | [asc_atomic_exch](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_exch.md) |
| Compares the old value at the GM address with the expected value; if they are equal, writes the new value, and returns the value before modification. | [AtomicCas](../../../api/SIMD-API/basic_api/atomic_operations/AtomicCas.md) | [asc_atomic_cas](../../../api/SIMD-API/c_api/atomic/scalar_atomic/asc_atomic_cas.md) |

#### Typical Example Description

[scalar_atomic_operations example](../../../../../examples/01_simd_cpp_api/03_basic_api/06_atomic/scalar_atomic_operations): Based on the AtomicAdd and AtomicCas APIs, this example demonstrates the implementation process of scalar atomic addition and atomic compare-and-swap on GM addresses, supporting multi-core parallel updates to a single memory address.

### New DCI (Data Cache Invalidation) Capability<a name="section_dci"></a>

#### Feature Application Scenario Description

When an operator finishes execution, the DCache must be invalidated to prevent subsequent operators from reading invalid data in it. By default, the compilation framework inserts a DCCI (DataCacheCleanAndInvalid) instruction at the end of the operator, which invalidates the DCache while writing dirty data back to GM, thus incurring additional Clean overhead. The 3510 architecture adds the DCI (DataCacheInvalid) data cache invalidation capability. The DCI instruction can directly invalidate the entire data cache without writing back dirty cache lines. Using the DCI instruction instead of the DCCI instruction at the end of the operator skips the Clean process of synchronizing DCache dirty data to GM, thereby reducing the operator tail overhead.

#### Function Description

The DCI instruction can be inserted in either of the following two ways:
1. Compilation option: Configure [`--cce-no-dcache-flush`](../../operator_practice/simd_operator_optimization/overhead_optimization/dci_compile_option.md) at compile time. The compilation framework will insert the DCI instruction at the end of the operator, replacing the default DCCI instruction. This method is recommended for enabling the DCI capability. For details, see [Setting the DCI Compilation Option to Reduce Operator Tail Overhead](../../operator_practice/simd_operator_optimization/overhead_optimization/dci_compile_option.md).
2. API: Use the [asc_dci](../../../api/SIMD-API/c_api/cache_ctrl/asc_dci.md) API to invalidate the data cache. Before calling it, insert a DSB_ALL instruction through [asc_sync_data_barrier](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_data_barrier.md) to wait for all memory access instructions to complete.

### New SSBuffer On-chip Storage Unit<a name="section_ssbuf"></a>

#### Feature Application Scenario Description

In Mix operators, control information and scalar data need to be transferred between AIC cores and AIV cores. On [NPU Architecture Version 2201](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), AIC and AIV communicate through GM, which incurs high communication latency and consumes GM bandwidth. The 3510 architecture adds the SSBuffer on-chip storage unit, allowing the AIC core and AIV core within an AI Core to directly access the SSBuffer through Scalar for inter-core communication. This approach does not require GM access, reducing communication latency and GM bandwidth consumption. SSBuffer is suitable for small-volume data communication between AIC cores and AIV cores within the same task, such as transferring control information, addresses, or a small amount of scalar data. Large blocks of data should be moved through the corresponding data movement paths, and SSBuffer should not be used as a general-purpose data cache.

#### Function Description

- Storage specification: [SSBuffer](../../programming_guide/advanced_programming/hardware_implementation/architecture_spec/npu_arch_3510.md) is a newly added inter-core communication storage unit within the AI Core of the 3510 architecture, with a capacity of 3 KB. SSBuffer uses an address space independent of L1 Buffer and UB, and the corresponding address space qualifier is `__ssbuf__`. For details, see [SIMD BuiltIn Keywords](../../programming_guide/language_extension/simd_builtin_keywords.md).
- Usage scope:

  - In a hybrid operator where the AIC core count to AIV core count ratio is 1:2 in separate mode, the AIC core and the corresponding AIV cores can exchange data through SSBuffer. The kernel function type and the AIC core count to AIV core count ratio must satisfy the constraints of the inter-core synchronization API.

- Access method:

  - A Mix operator is an operator type that includes both cube computation and vector computation, and MIX mode is a kernel mode in which AIC cores and AIV cores run in a mixed manner.
  - In non-MIX mode, AIC, AIV0, and AIV1 each independently occupy 1 KB of space; in MIX mode, the AIC core and each AIV core share the entire 3 KB space.
  - Obtain the SSBuffer base address through the [GetSsbufBaseAddr](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSsbufBaseAddr.md) API. This API returns a `__ssbuf__` pointer, based on which read/write access can be performed using base address offsets.
  - SSBuffer supports access only through 32-byte-aligned read/write instructions. Both the start address and the access granularity of the communication data block must satisfy the 32-byte alignment requirement. Member read/write of less than 32 bytes must not be performed on SSBuffer; an exception is triggered when the access address exceeds the SSBuffer range.

- Constraints:

  - The initial content of SSBuffer is undefined, and the read result is not guaranteed to be all zeros. For each communication, the producer must write the data completely, and the consumer must not rely on the historical content in SSBuffer.
  - The data layout must be consistent between the communicating parties (AIC and AIV), and the data must not exceed the end address of SSBuffer.
  - AIC and AIV cannot access SSBuffer when starting different tasks; the current task cannot access SSBuffer when its SSBuffer capability is not enabled.



#### Typical Example Description

The following code example demonstrates the complete usage of SSBuffer inter-core communication, where the ratio of AIC cores to AIV cores is 1:2, and AIV0 passes a 32-byte message to AIC in one direction. Before using SSBuffer, enable the [`ENABLE_CV_COMM_VIA_SSBUF`](../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches) compilation option.

- Obtain the base address and define the message:

  ```cpp
  // Obtain the SSBuffer base address, which returns an __ssbuf__ pointer. In the NPU domain, this special base address may appear as 0 or nil and must not be dereferenced as an ordinary null pointer.
  __ssbuf__ void* ssbuf = AscendC::GetSsbufBaseAddr();

  // Define a 32-byte-aligned communication message.
  struct alignas(32) SsbufferMessage {
      uint64_t value[4];
  };

  // Locate the message area based on the base address offset. The start offset must be 32-byte aligned and must not exceed the end of SSBuffer.
  __ssbuf__ uint8_t* ssbuffer = reinterpret_cast<__ssbuf__ uint8_t*>(ssbuf);
  __ssbuf__ SsbufferMessage* message =
      reinterpret_cast<__ssbuf__ SsbufferMessage*>(ssbuffer + 256);
  ```

- The AIV core (producer) writes the message and publishes the ready flag:

  ```cpp
  if ASCEND_IS_AIV {
      // In the 1:2 scenario, only AIV0 produces the message in this example, but AIV1 must still participate in mode 2 synchronization.
      if (AscendC::GetSubBlockIdx() == 0) {
          // The producer writes the complete 32-byte message to SSBuffer.
          message->value[0] = 0x1122334455667788ULL;
          message->value[1] = 0x8877665544332211ULL;
          message->value[2] = 0x0123456789abcdefULL;
          message->value[3] = 0xfedcba9876543210ULL;
      }
      // SSBuffer access is on the Scalar pipeline. First establish the Scalar-to-Vector on-chip ordering, then publish the AIV-to-AIC ready flag in mode 2.
      AscendC::SetFlag<AscendC::HardEvent::S_V>(0);
      AscendC::WaitFlag<AscendC::HardEvent::S_V>(0);
      AscendC::CrossCoreSetFlag<2, PIPE_V>(AIV_TO_AIC_FLAG);
  }
  ```

- The AIC core (consumer) waits for the flag to complete and then reads the message:

  ```cpp
  if ASCEND_IS_AIC {
      // Mode 2 waits for both AIVs on the same logical AI Core to finish publishing.
      AscendC::CrossCoreWaitFlag<2, PIPE_M>(AIV_TO_AIC_FLAG);
      // First establish the order from Cube pipeline to Scalar read, then read the message in SSBuffer.
      AscendC::SetFlag<AscendC::HardEvent::M_S>(0);
      AscendC::WaitFlag<AscendC::HardEvent::M_S>(0);
      // The consumer uses the message data in message according to the layout agreed by both communication parties.
      output[0] = message->value[0];
      output[1] = message->value[1];
      output[2] = message->value[2];
      output[3] = message->value[3];
  }
  ```

### New Mutex Intra-core Synchronization Capability<a name="section_mutex"></a>

#### Feature Application Scenario Description

In Vector operators, asynchronous pipeline instructions are typically synchronized through SetFlag and WaitFlag events, for example, GM-to-UB movement (PIPE_MTE2), vector computation (PIPE_V), and UB-to-GM movement (PIPE_MTE3). This approach requires explicit event pairing, which makes coding complex and is limited by the number of event resources. The 3510 architecture adds the Mutex capability, which establishes synchronization dependencies between pipelines by locking and releasing specified pipelines. Compared with SetFlag and WaitFlag, Mutex is decoupled from other pipelines, which simplifies reverse synchronization and provides more available synchronization semaphores.

#### Function Description

Refer to the following resources:
- Basic APIs: [Mutex::Lock](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md), [Mutex::Unlock](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md), [AllocMutexID](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI.md).
- C APIs: [asc_lock](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md), [asc_unlock](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md).

#### Typical Example Description

[mutex example](../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex): Uses Mutex to implement synchronization among the MTE2, V pipeline, and MTE3 pipeline, correctly completing data movement and computation without using SetFlag or WaitFlag.

### New Inter-core Synchronization Control Mode 4<a name="section_crosscore"></a>

#### Feature Application Scenario Description

In a Mix operator, there is a data dependency between the AIV core (Vector) and the AIC core (Cube). The inter-core synchronization APIs must be used to ensure that data is ready before subsequent computation is initiated. On [NPU Architecture Version 2201](../../programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), when the AIC core waits for synchronization, AIV0 and AIV1 must report simultaneously. That is, mode 2 is used to synchronize the AIC core with all AIV cores, resulting in coarse synchronization granularity. Architecture 3510 adds inter-core synchronization control mode 4, which is used to synchronize the AIC core with a single AIV core. AIV0 and AIV1 can trigger the AIC core wait separately, thereby refining the synchronization granularity, reducing redundant waits, and improving execution efficiency.

#### Function Description

Refer to the following resources:
- Basic APIs: [CrossCoreSetFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md), [CrossCoreWaitFlag](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md).
- C APIs: [asc_sync_intra_arrive](../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_arrive.md), [asc_sync_intra_wait](../../../api/SIMD-API/c_api/sync/inter_core_sync/asc_sync_intra_wait.md).

#### Typical Example Description

[cross_core_set_wait_flag example](../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/cross_core_set_wait_flag): In the Mix operator scenario, mode 4 inter-core synchronization is used, where AIV0 and AIV1 each independently trigger the AIC core to wait, completing the pipeline collaboration between UB-to-L1 Buffer data movement and Mmad computation.

### Other Enhancements

In addition to the features described above, the 3510 architecture provides the following enhanced capabilities.

#### Architecture Specifications

- **Increased AI Core count**: The 3510 architecture has more AI Cores than the 2201 architecture, supporting a larger on-chip parallel scale.
- **Increased UB capacity**: The total UB capacity of the 3510 architecture is increased to 256 KB, consisting of 16 banks organized into 8 bank groups. Each bank group contains 2 banks, and each bank is 16 KB in size. If a UB conflict occurs, see [Avoiding UB Bank Conflicts](../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/overview.md) for a solution. The [bank_conflict_3510 sample](../../../../../examples/01_simd_cpp_api/05_best_practices/04_memory_access/bank_conflict_3510) provides a method for verifying and analyzing UB bank conflict scenarios under the 3510 architecture.

#### Data Type

- **Data type extension**: The 3510 architecture extends the supported data types, such as fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, and fp8_e4m3fn_t. For details, see [Data Type Introduction](../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md). On the host side, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int4x2_t, and bfloat16_t can only be defined and declared in the form of C++ templates. For details, see [Constraints](../../programming_guide/compilation_and_execution/operator_compilation/constraints.md).

#### Cache Management

- **New cache type**: The 2201 architecture supports only L2 Cache, DCache, and ICache, while the 3510 architecture additionally supports NDDMA Cache. ND-DMA is the name of the multi-dimensional data movement capability, and NDDMA Cache is the name of the cache type corresponding to that capability. For the data consistency description across multiple cores, see [Cache Coherence](../../programming_guide/advanced_programming/memory_model/cache_coherence.md). For the complete cache types and function descriptions, see [System Cache Overview](../../../api/SIMD-API/basic_api/cache_control/system_cache_overview.md).
- **Cache control API enhancement**: Under the 3510 architecture, the [DataCacheCleanAndInvalid](../../../api/SIMD-API/basic_api/cache_control/DataCacheCleanAndInvalid.md) API newly supports the `CACHELINE_ATOMIC` parameter, which is used to ensure consistency between DCache and GM during atomic operations.

#### Compilation Options and Space Optimization

- **UB space usage and compilation options**: The total UB capacity is 256 KB, part of which may be reserved by the system or divided into Data Cache. Under the 3510 architecture, the Ascend C APIs internally use **2 KB** of reserved UB space for instruction compatibility or storing intermediate data; under the 2201 architecture, the reserved UB space is 8 KB. The following compilation options take effect and can adjust the available UB space size:
  - `--cce-disable-asc-reserved-ubuf`: Disables the use of reserved UB space by Ascend C APIs. After this option is enabled, APIs that depend on the reserved UB space become unavailable. For the range of APIs that use the reserved UB space under the 3510 architecture, see [List of APIs Using Reserved UB Space](../../programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api).
  - `--cce-disable-vf-stack-reserved-ubuf`: Disables the UB space reserved for the SIMD VF stack. This option takes effect only under the 3510 architecture. After it is enabled, 6 KB of reserved space is released for use as normal UB, but the compiler can no longer use this space to cache register spill data, so you must ensure that registers do not spill.
- **Performance optimization compilation macro**: `NO_OVERLAP_IN_MULTI_REPEAT` can remove unnecessary memory synchronization instructions when there is no address overlap, improving the performance of high-dimensional split computation scenarios of basic APIs. For details, see [Built-in Compilation Macro Switch](../../programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches).
- **DCI compilation option**: For usage instructions of the `--cce-no-dcache-flush` compilation option, see [New DCI (Data Cache Invalidation) Capability](#section_dci).
