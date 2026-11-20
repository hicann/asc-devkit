# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-16T10:24:38.778Z pushedAt=2026-11-18T08:15:34.240Z nl=2 -->

## API List

### Loading for Cube Computation

**Table 1** Loading interfaces for cube computation

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_copy_gm2l1](cube_compute_load/asc_copy_gm2l1.md) | Moves data from GM to L1. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1 (NPU architecture version 2201)](cube_compute_load/asc_copy_gm2l1_arch_2201.md) | Moves cube data from Global Memory to the L1 Buffer and supports different pad modes. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1 (NPU architecture version 3510, high-dimensional split data movement)](cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md) | Moves GM data to the L1 Buffer in a high-dimensional split manner, supporting insertion or removal of Padding by channel. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1 (NPU architecture version 3510, 2D cube movement mode)](cube_compute_load/asc_copy_gm2l1_2d_arch_3510.md) | Splits and moves 2D cube data from GM to the L1 Buffer in units of 512-byte fractals, without performing fractal format conversion during the movement process. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_align](cube_compute_load/asc_copy_gm2l1_align.md) | Pads data in GM and then moves it to L1. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_dn2nz](cube_compute_load/asc_copy_gm2l1_dn2nz.md) | Moves data from GM to L1, performing DN->NZ/NCHW->NC1HWC0/NCHW->C1HWNC0 operations during the process. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_nd2nz](cube_compute_load/asc_copy_gm2l1_nd2nz.md) | Moves GM data to the L1 Buffer and converts the ND format to the Nz fractal format. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_nd2nz (NPU architecture version 2201)](cube_compute_load/asc_copy_gm2l1_nd2nz_arch_2201.md) | Moves GM data to the L1 Buffer and supports conversion from the ND format to the NZ format. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l1_nd2nz (NPU architecture version 3510)](cube_compute_load/asc_copy_gm2l1_nd2nz_arch_3510.md) | Moves ND-format data from GM to the L1 Buffer and converts it to the Nz fractal layout. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l0a](cube_compute_load/asc_copy_gm2l0a.md) | Moves data from GM to L0A. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_gm2l0b](cube_compute_load/asc_copy_gm2l0b.md) | Moves data from GM to L0B. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a](cube_compute_load/asc_copy_l12l0a.md) | Moves a 512B cube stored in the L1 Buffer to the L0A Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a (NPU architecture version 2201)](cube_compute_load/asc_copy_l12l0a_arch_2201.md) | Moves a 512B cube stored in the L1 Buffer to the L0A Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a (NPU architecture version 3510, 2D cube movement)](cube_compute_load/asc_copy_l12l0a_2d_arch_3510.md) | Moves a 2D cube from the L1 Buffer to the L0A Buffer in units of 512-byte fractals, supporting both non-transposed and transposed movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a (NPU architecture version 3510, 3D cube movement)](cube_compute_load/asc_copy_l12l0a_3d_arch_3510.md) | Performs img2col expansion on the Feature Map in the L1 Buffer and then moves it to the L0A Buffer, also supporting 2D cube data movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b](cube_compute_load/asc_copy_l12l0b.md) | Moves a 512B cube stored in the L1 Buffer to the L0B Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b (NPU architecture version 2201)](cube_compute_load/asc_copy_l12l0b_arch_2201.md) | Moves a 512B cube stored in the L1 Buffer to the L0B Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b (NPU architecture version 3510, 2D cube movement)](cube_compute_load/asc_copy_l12l0b_2d_arch_3510.md) | Moves a 2D cube from the L1 Buffer to the L0B Buffer in units of 512-byte fractals, supporting both non-transposed and transposed movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b (NPU architecture version 3510, 3D cube movement)](cube_compute_load/asc_copy_l12l0b_3d_arch_3510.md) | Performs img2col expansion on the Feature Map in the L1 Buffer and then moves it to the L0B Buffer, also supporting 2D cube data movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a_mx](cube_compute_load/asc_copy_l12l0a_mx.md) | Moves the Mx scale cube from the L1 Buffer to the L0A Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_mx](cube_compute_load/asc_copy_l12l0b_mx.md) | Moves the Mx scale cube from the L1 Buffer to the L0B Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0a_trans](cube_compute_load/asc_copy_l12l0a_trans.md) | This interface loads transposed 2D format data from the L1 Buffer to the L0A Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_trans](cube_compute_load/asc_copy_l12l0b_trans.md) | This interface loads transposed 2D format data from the L1 Buffer to the L0B Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_trans (NPU architecture version 2201)](cube_compute_load/asc_copy_l12l0b_trans_arch_2201.md) | This interface loads transposed 2D format data from the L1 Buffer to the L0B Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_trans (NPU architecture version 3510)](cube_compute_load/asc_copy_l12l0b_trans_arch_3510.md) | Moves data from the L1 Buffer to the L0B Buffer, performing 2D fractal cube transposition in units of 512-byte data fractals during the movement process. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0b_sparse](cube_compute_load/asc_copy_l12l0b_sparse.md) | Moves a 512B dense weight cube stored in the L1 Buffer to the L0B Buffer, while reading a 128B index cube used to sparsify the dense cube. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12l0c](cube_compute_load/asc_copy_l12l0c.md) | Moves a cube from the L1 Buffer to the L0C Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12bt](cube_compute_load/asc_copy_l12bt.md) | Moves the Bias data of the MMAD instruction from the L1 Buffer to the BiasTable Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12bt (NPU architecture version 2201)](cube_compute_load/asc_copy_l12bt_arch_2201.md) | Moves data from the L1 Buffer to the BiasTable Buffer, which is used to store the Bias in cube computation. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12bt (NPU architecture version 3510)](cube_compute_load/asc_copy_l12bt_arch_3510.md) | Moves bias data from the L1 Buffer to the BiasTable Buffer, supporting continuous and high-dimensional split movement, and performs conversion or bit-width expansion according to the data type and configuration. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12fb](cube_compute_load/asc_copy_l12fb.md) | Moves data from the L1 Buffer to the Fixpipe Buffer, which is used to store quantization parameters. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12fb (NPU architecture version 2201)](cube_compute_load/asc_copy_l12fb_arch_2201.md) | Moves data from the L1 Buffer to the Fixpipe Buffer, which is used to store quantization parameters. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12fb (NPU architecture version 3510)](cube_compute_load/asc_copy_l12fb_arch_3510.md) | Moves quantization parameters and ReLU parameters from the L1 Buffer to the Fixpipe Buffer, supporting continuous and high-dimensional split movement. | `c_api/cube_datamove/cube_datamove.h` |

### Auxiliary Configuration Interfaces for Cube Load

**Table 2** Auxiliary configuration interfaces for cube load

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_fill_l0a](cube_load_aux_config/asc_fill_l0a.md) | Initializes the Local Memory of the L0A Buffer to a specific value. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_fill_l0b](cube_load_aux_config/asc_fill_l0b.md) | Initializes the Local Memory of the L0B Buffer to a specific value. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_fill_l1](cube_load_aux_config/asc_fill_l1.md) | Initializes the Local Memory of the L1 Buffer to a specific value. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_load_image_to_cbuf](cube_load_aux_config/asc_load_image_to_cbuf.md) | Moves image data from Global Memory to the L1 Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_loop1_stride](cube_load_aux_config/asc_set_gm2l1_loop1_stride.md) | Sets the interval between adjacent iteration data blocks in the inner loop when moving data from GM to the L1 Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_loop2_stride](cube_load_aux_config/asc_set_gm2l1_loop2_stride.md) | Sets the interval between adjacent iteration data blocks in the outer loop when moving data from GM to the L1 Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_loop_size](cube_load_aux_config/asc_set_gm2l1_loop_size.md) | Sets the number of loop iterations in the data movement process when moving data from GM to the L1 Buffer. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_nz_para](cube_load_aux_config/asc_set_gm2l1_nz_para.md) | Sets the value of the MTE2_NZ_PARA register. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_pad](cube_load_aux_config/asc_set_gm2l1_pad.md) | Sets the constant padding value for the [asc_copy_gm2l1_align](cube_compute_load/asc_copy_gm2l1_align.md) interface. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_gm2l1_padding](cube_load_aux_config/asc_set_gm2l1_padding.md) | Sets the padding value for [asc_copy_gm2l1 (NPU architecture version 3510, high-dimensional split data movement)](cube_compute_load/asc_copy_gm2l1_highdim_split_arch_3510.md). | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l12l0a_3d_padding](cube_load_aux_config/asc_set_l12l0a_3d_padding.md) | Sets the padding value for 3D left cube movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l12l0b_3d_padding](cube_load_aux_config/asc_set_l12l0b_3d_padding.md) | Sets the padding value for 3D right cube movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_fmatrix](cube_load_aux_config/asc_set_l13d_fmatrix.md) | Configures the Feature Map attribute description parameters of the left cube in 3D img2col movement mode. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_fmatrix_b](cube_load_aux_config/asc_set_l13d_fmatrix_b.md) | Configures the Feature Map attribute description parameters of the right cube in 3D img2col movement mode. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_rpt](cube_load_aux_config/asc_set_l13d_rpt.md) | Configures the parameters required for moving the left cube in `repeat` mode under 3D img2col movement mode. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_rpt_b](cube_load_aux_config/asc_set_l13d_rpt_b.md) | Configures the parameters required for moving the right cube in `repeat` mode under 3D img2col movement mode. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l13d_size](cube_load_aux_config/asc_set_l13d_size.md) | Sets the boundary values in the L1 Buffer for the 3D format movement interfaces [asc_copy_l12l0a](cube_compute_load/asc_copy_l12l0a_arch_2201.md)/[asc_copy_l12l0b](cube_compute_load/asc_copy_l12l0b_arch_2201.md). | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l3d_rpt_b](cube_load_aux_config/asc_set_l3d_rpt_b.md) | Sets the parameters required for moving the right cube in `repeat` mode under 3D img2col movement mode through a 64-bit configuration value. | `c_api/cube_datamove/cube_datamove.h` |

### Store of Cube Computation

**Table 3** Store interfaces of cube computation

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_copy_l0c2gm](cube_compute_store/asc_copy_l0c2gm.md) | Moves data from L0C to GM. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2gm (NPU architecture version 2201)](cube_compute_store/asc_copy_l0c2gm_arch_2201.md) | Moves the cube computation result in the L0C Buffer to GM, supporting inline quantization. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2gm (NPU architecture version 3510)](cube_compute_store/asc_copy_l0c2gm_arch_3510.md) | Moves the cube computation result from the L0C Buffer to Global Memory, supporting combinations of inline quantization, inline activation, and inline format conversion (Nz2ND/Nz2DN) during the movement process. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2l1](cube_compute_store/asc_copy_l0c2l1.md) | After cube computation completes, quantizes the result and then moves the processed result to L1. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2l1 (NPU architecture version 2201)](cube_compute_store/asc_copy_l0c2l1_arch_2201.md) | Moves the cube computation result in the L0C Buffer to the L1 Buffer, supporting inline quantization. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2l1 (NPU architecture version 3510)](cube_compute_store/asc_copy_l0c2l1_arch_3510.md) | Moves the cube computation result from the L0C Buffer to the L1 Buffer, supporting inline quantization, ReLU, Nz2ND format conversion, and channel splitting. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l0c2ub](cube_compute_store/asc_copy_l0c2ub.md) | Moves data from L0C to UB. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12gm](cube_compute_store/asc_copy_l12gm.md) | Moves data from L1 to GM. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_copy_l12ub](cube_compute_store/asc_copy_l12ub.md) | Moves data from the L1 Buffer to UB. | `c_api/cube_datamove/cube_datamove.h` |

### Auxiliary Configuration Interfaces for Cube Store

**Table 4** Auxiliary configuration interfaces for cube store

| API | Interface Description | Finest-grained Common Header File |
| --- | --- | --- |
| [asc_get_l0c2gm_prequant](cube_store_aux_config/asc_get_l0c2gm_prequant.md) | When inline quantization is performed during data movement, call this interface to obtain the start address of the vector before the quantization operation. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_get_l0c2gm_relu](cube_store_aux_config/asc_get_l0c2gm_relu.md) | When inline quantization is performed during data movement, call this interface to obtain the start address of the vector before the ReLU operation. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_get_l0c2gm_unitflag](cube_store_aux_config/asc_get_l0c2gm_unitflag.md) | When inline quantization is performed during data movement, call this interface to obtain the `unit_flag` setting. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_channel_para](cube_store_aux_config/asc_set_l0c_copy_channel_para.md) | Configures the address offset of adjacent rows in the source Nz cube during Nz2ND format conversion. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_config](cube_store_aux_config/asc_set_l0c_copy_config.md) | Sets the related configuration for inline tensor quantization, inline tensor activation, and UnitFlag functions in the L0C Buffer store scenario. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_lrelu_alpha](cube_store_aux_config/asc_set_l0c_copy_lrelu_alpha.md) | Configures the scale factor required for activation computation when inline Scalar activation is performed during L0C Buffer store. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_nz_para](cube_store_aux_config/asc_set_l0c_copy_nz_para.md) | Sets the related configuration for format conversion when inline format conversion (converting NZ format to ND format) is performed during data movement. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_params](cube_store_aux_config/asc_set_l0c_copy_params.md) | When moving L0C Buffer data to Global Memory or L1 Buffer and performing inline format conversion (converting NZ format to ND format), call this interface to set the related configuration for format conversion. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_prequant](cube_store_aux_config/asc_set_l0c_copy_prequant.md) | When inline quantization is performed during data movement, call this interface to set the scalar quantization parameters in the quantization process. | `c_api/cube_datamove/cube_datamove.h` |
| [asc_set_l0c_copy_relu_alpha](cube_store_aux_config/asc_set_l0c_copy_relu_alpha.md) | Configures the scale factor required for activation computation when inline Scalar activation is performed during L0C Buffer store. | `c_api/cube_datamove/cube_datamove.h` |
