# Tensor API List

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:07:50.425Z pushedAt=2026-11-19T05:54:06.620Z nl=2 -->

## Basic Data Structures

**Table 1** Basic data structure API list

| Type or API Name | Function Description |
| --- | --- |
| [layout](layout/layout.md) | Uses Shape and Stride to describe the logical shape and memory layout of data. |
| [Shape](layout/shape.md) | Describes the logical shape of each dimension of a Tensor, supporting plain integers and nested tuples. |
| [Stride](layout/stride.md) | Describes the address stride between adjacent elements of each dimension of a Tensor in memory, supporting plain integers and nested tuples. |
| [Coord](layout/coord.md) | Describes the logical coordinates of a Tensor in each dimension. |
| [global_tensor and local_tensor](tensor/tensor.md) | Describe Tensor objects in Global Memory and on-chip Local Memory, respectively. |
| [Pointer](tensor/pointer.md) | Describes the access address of a Tensor and its corresponding storage location. |
| [view_engine](tensor/view_engine.md) | Holds the memory pointer of a tensor view and provides capabilities for managing the view start position and cache mode. |
| [make_layout](layout/make_layout.md) | Constructs a plain Layout based on Shape and Stride. |
| [make_pattern_layout](layout/make_pattern_layout.md) | Constructs a Layout with layout type information based on Layout Pattern, Shape, and Stride. |
| [make_frame_layout](layout/make_frame_layout.md) | Constructs a Layout based on the Layout Pattern and shape parameters of a convolution feature map or cube fractal. |
| [make_shape](layout/make_shape.md) | Constructs a Shape object. |
| [make_stride](layout/make_stride.md) | Constructs a Stride object. |
| [make_coord](layout/make_coord.md) | Constructs a Coord object. |
| [make_mem_ptr](tensor/make_mem_ptr.md) | Constructs a Tensor API memory pointer object based on a pointer and storage location. |
| [make_tensor](tensor/make_tensor.md) | Binds a memory pointer and a Layout to construct a global_tensor or local_tensor. |
| [slice](tensor/slice.md) | Creates a sub-Tensor of the original Tensor based on the start coordinates and a Shape or Layout description. |
| [mutex_id](sync/mutex.md) | Type alias for the pipeline mutex hardware ID. |
| [mutex](sync/mutex.md) | Object-based pipeline mutex that controls the handover of asynchronous pipeline resources within the core via compile-time pipeline parameters and a runtime lock ID. |
| [lock_guard](sync/lock_guard.md) | Scoped lock guard object that automatically releases the referenced mutex upon destruction. |

## Layout Operations

**Table 2** Layout operation API list

| API Name | Function Description |
| --- | --- |
| [capacity](layout/capacity.md) | Gets the actual memory capacity corresponding to the Layout or a specified sub-dimension. |
| [coshape](layout/coshape.md) | Gets the Shape structure of a specified sub-dimension of the Layout at compile time. |
| [cosize](layout/cosize.md) | Gets the capacity or number of elements of a specified sub-dimension of the Layout at compile time. |
| [crd2idx](layout/crd2idx.md) | Converts multi-dimensional coordinates into a linear memory index according to the Layout. |
| [get](layout/get.md) | Gets a specified element in a tuple, supporting single or multiple compile-time indexes. |
| [get_shape](layout/get_shape.md) | Gets the Shape of the Layout or of a specified sub-dimension. |
| [get_stride](layout/get_stride.md) | Gets the Stride of the Layout or of a specified sub-dimension. |
| [rank](layout/rank.md) | Gets the hierarchy or dimension information of the Layout. |
| [select](layout/select.md) | Selects a substructure from the Shape and Stride of the Layout by a specified index. |
| [size](layout/size.md) | Gets the number of logical elements of the Layout. |
| [squeeze](layout/squeeze.md) | Removes the specified dimension of size 1 from the top-level structure of the Shape. |

## Algorithm

**Table 3** Algorithm API and type list

| Type or API Name | Function Description |
| --- | --- |
| [Operation, Trait, and Atom](algorithm.md) | Describes how Operation, Trait, Params, and Atom are combined in movement and cube computation, as well as the unified call entries of `copy` and `mmad`. |

## Movement

**Table 4** Movement path and API list

| Movement Path or API Name | Function Description |
| --- | --- |
| [Global Memory to Unified Buffer](reg_vector_compute/vector_compute_load/copy_gm_to_ub.md) | Uses `copy` to move data from Global Memory to the Unified Buffer (UB), supporting coordinate region movement. |
| [Unified Buffer to Global Memory](reg_vector_compute/vector_compute_store/copy_ub_to_gm.md) | Uses `copy` to move data from the UB to Global Memory, supporting coordinate region movement. |
| [Global Memory to L1 Buffer](matrix_compute/cube_compute_load/copy_gm_to_l1.md) | Uses `copy` to move data from Global Memory to the L1 Buffer, supporting multiple cube format conversions and batch movement. |
| [Unified Buffer to L1 Buffer](reg_vector_compute/vector_compute_load/copy_ub_to_l1.md) | Uses `copy` to move data from the UB to the L1 Buffer. |
| [L1 Buffer to Unified Buffer](matrix_compute/cube_compute_load/copy_l1_to_ub.md) | Uses `copy` to move data from the L1 Buffer to the UB. |
| [L1 Buffer to L0A Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0a.md) | Uses `copy` to move the left cube data from the L1 Buffer to the L0A Buffer, supporting cube format conversion and convolution feature map movement. |
| [L1 Buffer to L0B Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0b.md) | Uses `copy` to move the right cube data from the L1 Buffer to the L0B Buffer, supporting cube format conversion and batch movement. |
| [L1 Buffer to L0ScaleA Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0scalea.md) | Uses `copy` to move the ScaleA data from the L1 Buffer to the L0ScaleA Buffer. |
| [L1 Buffer to L0ScaleB Buffer](matrix_compute/cube_compute_load/copy_l1_to_l0scaleb.md) | Uses `copy` to move the ScaleB data from the L1 Buffer to the L0ScaleB Buffer. |
| [L1 Buffer to BiasTable Buffer](matrix_compute/cube_compute_load/copy_l1_to_biastable.md) | Uses `copy` to move the Bias data from the L1 Buffer to the BiasTable Buffer. |
| [L1 Buffer to Fixpipe Buffer](matrix_compute/cube_compute_load/copy_l1_to_fixbuf.md) | Uses `copy` to move the quantization parameters from the L1 Buffer to the Fixpipe Buffer. |
| [L0C Buffer to Global Memory](matrix_compute/cube_compute_store/copy_l0c_to_gm.md) | Uses `copy` to move the cube result from the L0C Buffer to Global Memory, supporting format conversion and quantized output. |
| [L0C Buffer to Unified Buffer](matrix_compute/cube_compute_store/copy_l0c_to_ub.md) | Uses `copy` to move the cube result from the L0C Buffer to the UB, supporting format conversion and quantized output. |
| [L0C Buffer to L1 Buffer](matrix_compute/cube_compute_store/copy_l0c_to_l1.md) | Uses `copy` to move the cube result from the L0C Buffer to the L1 Buffer. |
| [Unified Buffer Internal Movement](reg_vector_compute/vector_compute_load/copy_ub_to_ub.md) | Uses `copy` to move data between tensors within the UB. |
| [fill](matrix_compute/cube_data_fill/fill.md) | Writes the scalar fill value into the L1 Buffer in units of 32-byte physical blocks. |

## Cube Computation

**Table 5** Cube computation API and feature list

| API or Topic | Function Description |
| --- | --- |
| [Cube Computation Overview](matrix_compute/overview.md) | Describes the basic process and API composition of Tensor API cube computation. |
| [Cube Computation Fractal Introduction](matrix_compute/cube_compute_fractal_intro.md) | Describes the commonly used data fractal formats in cube computation and their Layout expressions. |
| [mmad](matrix_compute/mmad_compute/mmad.md) | Uses L0A, L0B, and optional Bias data to perform cube multiply-accumulate computation and writes the result to L0C. |
| [Key Features of mmad Computation](matrix_compute/mmad_compute_key_features/mmad_compute_key_features.md) | Describes key cube multiply-accumulate features such as GEMV, HF32, and unit_flag. |
| [GEMV](matrix_compute/mmad_compute_key_features/gemv.md) | Describes the GEMV computation mode in the scenario where M is 1. |
| [HF32](matrix_compute/mmad_compute_key_features/hf32.md) | Describes the HF32 precision mode in cube computation. |
| [unit_flag](matrix_compute/mmad_compute_key_features/unit_flag.md) | Describes the fine-grained parallel control between mmad and the subsequent cube data move-out. |
| [Auxiliary Configuration for mmad Computation](matrix_compute/mmad_compute_aux_config.md) | Lists the C API auxiliary configuration APIs required for mmad computation. |
| [Overall Description of Cube Computation Move-In](matrix_compute/cube_compute_load/overall_description.md) | Describes the paths for moving cube data from Global Memory or the L1 Buffer into the cube computation buffers at each level. |
| [Overall Description of Cube Computation Move-Out](matrix_compute/cube_compute_store/overall_description.md) | Describes the paths for moving the cube result from the L0C Buffer to Global Memory, the UB, or the L1 Buffer. |
| [Key Features of Cube Move-Out](matrix_compute/cube_store_key_features/cube_store_key_features.md) | Describes real-time quantization, real-time ReLU, channel split, channel merge, batch movement, and dual-destination mode. |
| [Real-Time Quantization](matrix_compute/cube_store_key_features/quant_pre.md) | Describes the real-time quantization capability during cube result move-out. |
| [Real-Time ReLU](matrix_compute/cube_store_key_features/relu_pre.md) | Describes the real-time ReLU capability during cube result move-out. |
| [F32 Channel Split](matrix_compute/cube_store_key_features/f32_channel_split.md) | Describes the channel split capability during F32 result move-out. |
| [Int8 Channel Merge](matrix_compute/cube_store_key_features/int8_channel_merge.md) | Describes the channel merge capability during Int8 result move-out. |
| [Batch Copy](matrix_compute/cube_store_key_features/batch_copy.md) | Describes the multi-batch move-out capability of the cube result. |
| [L0C to Unified Buffer Dual-Destination Mode](matrix_compute/cube_store_key_features/l0c_to_ub_dual_dst.md) | Introduces the dual-destination move-out mode from L0C to the UB. |

## Vector Computation API

**Table 6**  Vector computation API list

| Type or API Name | Function Description |
| --- | --- |
| [cast_layout](reg_vector_compute/aux_data_types/cast_layout.md) | Specifies the data layout position in register type conversion. |
| [cast_round_mode](reg_vector_compute/aux_data_types/cast_round_mode.md) | Specifies the rounding method for register type conversion. |
| [cast_sat_mode](reg_vector_compute/aux_data_types/cast_sat_mode.md) | Specifies whether to use saturation mode in register type conversion. |
| [merge_mode](reg_vector_compute/aux_data_types/merge_mode.md) | Specifies how unselected elements of the destination register are processed in a masked register write operation. |
| [reg_tensor](reg_vector_compute/reg_tensor/reg_tensor.md) | Encapsulates a vector data register and its mask register for register data computation. |
| [reg_pair](reg_vector_compute/reg_tensor/reg_pair.md) | Stores two `reg_tensor` objects whose element types can be the same or different, used for register APIs with dual results. |
| [all_mask](reg_vector_compute/mask_reg_compute/all_mask.md) | Creates an all-valid Mask register matching the bit width of the specified element type. |
| [make_mask](reg_vector_compute/mask_reg_compute/make_mask.md) | Creates a Mask register based on the Mask mode and data element width. |
| [none_mask](reg_vector_compute/mask_reg_compute/none_mask.md) | Creates an all-invalid Mask register matching the bit width of the specified element type. |
| [update_mask](reg_vector_compute/mask_reg_compute/update_mask.md) | Generates a valid-bit Mask based on the number of remaining elements to be processed, and updates the remaining element count. |
| [interleave (mask register interleave)](reg_vector_compute/mask_reg_compute/interleave.md) | Interleaves two Mask registers by the specified element width and returns the low-half and high-half results. |
| [deinterleave (mask register deinterleave)](reg_vector_compute/mask_reg_compute/deinterleave.md) | Deinterleaves two Mask registers by the specified element width and returns the even-group and odd-group results. |
| [move](reg_vector_compute/reg_data_copy/move.md) | Copies the valid elements selected by the Mask in the source register to the corresponding positions in the destination register. |
| [move_mask](reg_vector_compute/reg_data_copy/move_mask.md) | Extracts and expands bits from the specified data block of the source vector data register to generate a Mask register. |
| [load](reg_vector_compute/reg_data_load/load.md) | Moves data in the UB into `reg_tensor`, supporting multiple data arrangement modes. |
| [load_broadcast](reg_vector_compute/reg_data_load/load_broadcast.md) | Moves data in from the UB and broadcasts it to `reg_tensor` by the specified mode. |
| [load_gather](reg_vector_compute/reg_data_load/load_gather.md) | Collects data from discrete positions of the UB Tensor into `reg_tensor` based on index. |
| [unalign_load](reg_vector_compute/reg_data_load/unalign_load.md) | Moves UB data that is not 32-byte aligned into `reg_tensor`, supporting continuous move-in. |
| [store](reg_vector_compute/reg_data_store/store.md) | Moves data in `reg_tensor` out to the UB, supporting multiple data arrangement modes. |
| [store_scatter](reg_vector_compute/reg_data_store/store_scatter.md) | Scatters the valid elements in `reg_tensor` into the UB Tensor based on index. |
| [unalign_store](reg_vector_compute/reg_data_store/unalign_store.md) | Continuously moves the specified number of elements in `reg_tensor` out to a UB address that is not 32-byte aligned, and provides a wrap-up operation. |
| [Rounding Mode and Saturation Mode](reg_vector_compute/type_conversion/rounding_mode.md) | Introduces the rounding rules, saturated and non-saturated behavior, and configuration methods for register type conversion. |
| [cast](reg_vector_compute/type_conversion/cast.md) | Converts the register data type, supporting configuration of data layout, rounding, and saturation modes. |
| [trunc](reg_vector_compute/type_conversion/trunc.md) | Rounds the floating-point elements selected by the Mask toward zero while preserving the original data type. |
| [abs](reg_vector_compute/basic_arithmetic/abs.md) | Computes the absolute value element-by-element on the source operand. |
| [add_carry](reg_vector_compute/basic_arithmetic/add_carry.md) | Performs addition with carry element-by-element on the source operand. |
| [exp](reg_vector_compute/basic_arithmetic/exp.md) | Computes the exponential element-by-element on the effective elements in the source operand. |
| [leaky_relu](reg_vector_compute/basic_arithmetic/leaky_relu.md) | Performs Leaky ReLU computation element-by-element on the source operand. |
| [log](reg_vector_compute/basic_arithmetic/log.md) | Computes the natural logarithm element-by-element on the effective elements in the source operand. |
| [ln](reg_vector_compute/basic_arithmetic/ln.md) | Computes the natural logarithm element-by-element on the effective elements in the source operand. |
| [log10](reg_vector_compute/basic_arithmetic/log10.md) | Computes the base-10 logarithm element-by-element on the effective elements in the source operand. |
| [log2](reg_vector_compute/basic_arithmetic/log2.md) | Computes the base-2 logarithm element-by-element on the effective elements in the source operand. |
| [max](reg_vector_compute/basic_arithmetic/max.md) | Computes the maximum value element-by-element on two source operands. |
| [min](reg_vector_compute/basic_arithmetic/min.md) | Computes the minimum value element-by-element on two source operands. |
| [mull](reg_vector_compute/basic_arithmetic/mull.md) | Performs high-low bit multiplication element-by-element on two source operands. |
| [neg](reg_vector_compute/basic_arithmetic/neg.md) | Negates the source operand element-by-element. |
| [+](reg_vector_compute/basic_arithmetic/operator_add.md) | Performs addition element-by-element on two source operands. |
| [-](reg_vector_compute/basic_arithmetic/operator_sub.md) | Performs subtraction element-by-element on two source operands. |
| [*](reg_vector_compute/basic_arithmetic/operator_mul.md) | Performs multiplication element-by-element on two source operands. |
| [/](reg_vector_compute/basic_arithmetic/div.md) | Performs division element-by-element on two source operands. |
| [div](reg_vector_compute/basic_arithmetic/div_software.md) | Performs element-wise division on two source operands, supporting division precision and algorithm configuration via template parameters. |
| [prelu](reg_vector_compute/basic_arithmetic/prelu.md) | Performs PReLU computation element-by-element on the source operand. |
| [relu](reg_vector_compute/basic_arithmetic/relu.md) | Performs ReLU computation element-by-element on the source operand. |
| [sqrt](reg_vector_compute/basic_arithmetic/sqrt.md) | Computes the square root element-by-element on the source operand. |
| [sub_carry](reg_vector_compute/basic_arithmetic/sub_carry.md) | Performs subtraction with borrow element-by-element on the source operand. |
| [axpy](reg_vector_compute/composite_compute/axpy.md) | Multiplies the source register by a scalar element-by-element, then adds the result to the original value of the destination register. |
| [abs_diff](reg_vector_compute/composite_compute/abs_diff.md) | Computes the absolute value of the difference element-by-element on two source registers. |
| [exp_diff](reg_vector_compute/composite_compute/exp_diff.md) | Computes the difference element-by-element on two source registers, and calculates the exponential function with the difference as the exponent. |
| [madd](reg_vector_compute/composite_compute/madd.md) | Multiplies the destination register by the first source register element-by-element, then adds the result to the second source register. |
| [mula](reg_vector_compute/composite_compute/mula.md) | Multiplies two source registers element-by-element, then adds the result to the original value of the destination register. |
| [fma](reg_vector_compute/composite_compute/fma.md) | Performs fused multiply-add computation element-by-element on three source registers. |
| [muls_cast](reg_vector_compute/composite_compute/muls_cast.md) | Multiplies the source register by a scalar element-by-element, and converts the result to the destination data type. |
| [not](reg_vector_compute/logical_compute/not.md) | Performs logical NOT computation element-by-element on the source operand. |
| [&](reg_vector_compute/logical_compute/and.md) | Performs bitwise AND computation element-by-element on two source operands. |
| [\|](reg_vector_compute/logical_compute/or.md) | Performs bitwise OR computation element-by-element on two source operands. |
| [^](reg_vector_compute/logical_compute/xor.md) | Performs bitwise XOR computation element-by-element on two source operands. |
| [&lt;&lt;](reg_vector_compute/logical_compute/shift_left.md) | Performs left shift computation element-by-element on the source operand. |
| [&gt;&gt;](reg_vector_compute/logical_compute/shift_right.md) | Performs right shift computation element-by-element on the source operand. |
| [Select and compare](reg_vector_compute/compare_and_select/compare_and_select.md) | Selects elements from source operands src0 and src1 based on the bit values of the mask to obtain the destination operand. |
| [operator==](reg_vector_compute/compare_and_select/operator_compare.md#operator-eq) | Performs equality comparison element-by-element on two source operands. |
| [operator!=](reg_vector_compute/compare_and_select/operator_compare.md#operator-ne) | Performs inequality comparison element-by-element on two source operands. |
| [operator<](reg_vector_compute/compare_and_select/operator_compare.md#operator-lt) | Performs less-than comparison element-by-element on two source operands. |
| [operator<=](reg_vector_compute/compare_and_select/operator_compare.md#operator-le) | Performs less-than-or-equal comparison element-by-element on two source operands. |
| [operator>](reg_vector_compute/compare_and_select/operator_compare.md#operator-gt) | Performs greater-than comparison element-by-element on two source operands. |
| [operator>=](reg_vector_compute/compare_and_select/operator_compare.md#operator-ge) | Performs greater-than-or-equal comparison element-by-element on two source operands. |
| [select](reg_vector_compute/compare_and_select/select.md) | Selects data element-by-element from two source registers based on the condition Mask. |
| [Data padding](reg_vector_compute/data_padding/data_padding.md) | Fills the lowest-bit element of source operand src or a scalar operand into the destination operand based on the mask. |
| [fill](reg_vector_compute/data_padding/fill.md) | Broadcasts and fills a scalar or the lowest-bit element of the source register into the destination register. |
| [Data reorder](reg_vector_compute/data_reorder/data_reorder.md) | Performs interleave, deinterleave, index-based gather, bit-width compression and expansion, mask-based filtering, and prefix counting on register data. |
| [interleave (data register interleave)](reg_vector_compute/data_reorder/interleave.md) | Interleaves the elements in two source registers into two result registers. |
| [deinterleave (data register deinterleave)](reg_vector_compute/data_reorder/deinterleave.md) | Deinterleaves the elements in two source registers into two result registers. |
| [gather](reg_vector_compute/data_reorder/gather.md) | Collects data in the source register element-by-element based on the index register. |
| [pack](reg_vector_compute/data_reorder/pack.md) | Extracts the low-half bit-width data of the source elements, compresses them, and writes them into the low half or high half of the result register. |
| [unpack](reg_vector_compute/data_reorder/unpack.md) | Selects the low half or high half of the source register and expands the element width to twice the original. |
| [compress](reg_vector_compute/data_reorder/compress.md) | Arranges the selected source elements contiguously in the low bits of the result register in their original order based on the selection mask. |
| [expand](reg_vector_compute/data_reorder/expand.md) | Generates a prefix count index that excludes the current position based on the mask. |
| [Index generation](reg_vector_compute/reg_index/reg_index.md) | Generates a register index sequence with a scalar as the base value. |
| [iota](reg_vector_compute/reg_index/iota.md) | Generates an increasing or decreasing sequence with a step of 1 using a scalar as the base value. |
| [histograms](reg_vector_compute/histogram_compute/histograms.md) | Counts the frequency distribution or cumulative distribution of input values, and accumulates the statistical result onto the original value of the destination register. |
| [reduce_sum](reg_vector_compute/reduction_compute/reduce_sum.md) | Sums the effective elements in the source register by the specified reduction range. |
| [reduce_max](reg_vector_compute/reduction_compute/reduce_max.md) | Computes the maximum value of the effective elements in the source register by the specified reduction range. |
| [reduce_min](reg_vector_compute/reduction_compute/reduce_min.md) | Computes the minimum value of the effective elements in the source register by the specified reduction range. |
| [mem_bar](reg_vector_compute/reg_sync_control/mem_bar.md) | Synchronizes UB access between different pipelines within the Reg vector computation function. |

## Utility APIs

**Table 7**  Utility API list

| Type or API Name | Function Description |
| --- | --- |
| [Compile-time Integer Aliases](utils/compile_time_integer_aliases.md) | Provides type aliases for expressing compile-time integer values. |
| [Dimension Keep Mark](utils/dimension_keep_mark.md) | Provides types and constants for marking dimension-keeping behavior. |
| [get_layout_pattern](utils/get_layout_pattern.md) | Extracts Layout Pattern type information from a Layout type. |
| [Type Traits](utils/type_traits.md) | Provides type determination utilities for Layout, Tensor, Coord, Shape, quantization parameters, and others. |
| [print API](utils/print.md) | Provides formatting, Layout table, and Tensor element printing capabilities for Tensor API debugging scenarios. |
| [init_soc_state](utils/init_soc_state.md) | Restores the global control state of the current AI Core to its initial state. |
| [get_sub_block_id](utils/get_sub_block_id.md) | Gets the logic index of the AIC or AIV on the current logic AI Core. |
| [get_sub_block_num](utils/get_sub_block_num.md) | Gets the quantity of AICs or AIVs on the current AI Core. |
| [get_vf_len](utils/get_vf_len.md) | Gets the vector register length in bytes. |
| [get_phy_buf_addr](utils/get_phy_buf_addr.md) | Gets the actual on-chip physical address based on the offset. |
| [get_arch_ver](utils/get_arch_ver.md) | Gets the architecture version number of the current AI processor. |
| [get_core_id](utils/get_core_id.md) | Gets the number of the current AI Core. |
| [get_phy_stack_base](utils/get_phy_stack_base.md) | Gets the physical stack base address. |
| [get_program_counter](utils/get_program_counter.md) | Gets the program counter pointer. |
| [get_smmu_tag_version](utils/get_smmu_tag_version.md) | Gets the SMMU version information. |
| [get_sys_virtual_base](utils/get_sys_virtual_base.md) | Reads the system virtual base address. |
| [get_system_cycle](utils/get_system_cycle.md) | Reads the current system cycle counter. |
| [nop](utils/nop.md) | Generates a no-operation instruction. |

## Special Register

**Table 8**  Special Register API List

| API | Description |
| --- | --- |
| [get_saturation_flag](special_register/get_saturation_flag.md) | Reads whether saturation is currently enabled by saturation mode. |
| [get_saturation_strategy](special_register/get_saturation_strategy.md) | Reads the current saturation control strategy. |
| [get_squeeze_status](special_register/get_squeeze_status.md) | Reads the valid data length after the squeeze operation. |
| [set_saturation_flag](special_register/set_saturation_flag.md) | Sets the saturation switch by saturation mode. |
| [set_saturation_strategy](special_register/set_saturation_strategy.md) | Sets the saturation control strategy. |

## Scalar Computation

**Table 9** Scalar Computation API List

| API | Description |
| --- | --- |
| [clear_nthbit](scalar_compute/clear_nthbit.md) | Clears the specified bit of a 64-bit unsigned integer. |
| [set_nthbit](scalar_compute/set_nthbit.md) | Sets the specified bit of a 64-bit unsigned integer. |
| [clz](scalar_compute/clz.md) | Counts the number of leading zeros in a 64-bit unsigned integer. |
| [ffs](scalar_compute/ffs.md) | Finds the first bit with a value of 1. |
| [ffz](scalar_compute/ffz.md) | Finds the first bit with a value of 0. |
| [popc](scalar_compute/popc.md) | Counts the number of bits with a value of 1. |
| [sflbits](scalar_compute/sflbits.md) | Counts the number of consecutive bits that are the same as the sign bit. |
| [zero_bits_cnt](scalar_compute/zero_bits_cnt.md) | Counts the number of bits with a value of 0. |
| [load_dev](scalar_compute/load_dev.md) | Reads integer data from Global Memory without going through DCache. |
| [store_dev](scalar_compute/store_dev.md) | Writes integer data to Global Memory without going through DCache. |

## mmad Computation Configuration

**Table 10** mmad computation configuration API list

| API | Description |
| --- | --- |
| [enable_hf32](matrix_compute/mmad_compute_config/enable_hf32.md) | Enables HF32 Mode. |
| [disable_hf32](matrix_compute/mmad_compute_config/disable_hf32.md) | Closes HF32 Mode. |
| [set_hf32_round_mode](matrix_compute/mmad_compute_config/set_hf32_round_mode.md) | Sets the rounding mode of HF32 Mode. |
| [set_mmad_direction](matrix_compute/mmad_compute_config/set_mmad_direction.md) | Sets the computation direction of matrix multiplication. |

## Movement Configuration

**Table 11** Movement configuration API List

| API | Description |
| --- | --- |
| [set_copy_pad_val](data_move/set_copy_pad_val.md) | Sets the constant fill value used for aligned movement from GM to UB. |

## Synchronization Control

**Table 12** Synchronization control API list

| API | Description |
| --- | --- |
| [block, intra, inter, and subblock synchronization](sync/block_sync.md) | Provides four types of arrival/wait synchronization APIs: block, intra, inter, and subblock. |
| [sync_data_barrier](sync/data_barrier.md) | Waits for the completion of preceding memory access instructions. |
| [sync_pipe](sync/sync_pipe.md) | Waits for the completion of preceding instructions in the specified pipeline or all pipelines. |

## Atomic Operations

**Table 13** Atomic operation API list

| API | Description |
| --- | --- |
| [set_atomic_add](atomic/set_atomic_add.md) | Enables atomic accumulation for subsequent supported movements whose destination address is GM. |
| [set_atomic_max](atomic/set_atomic_max.md) | Enables atomic maximum for subsequent supported movements whose destination address is GM. |
| [set_atomic_min](atomic/set_atomic_min.md) | Enables atomic minimum for subsequent supported movements whose destination address is GM. |
| [disable_dma_atomic](atomic/disable_dma_atomic.md) | Closes the DMA atomic mode. |
| [atomic_add](atomic/atomic_add.md) | Executes scalar atomic addition on a single element in Global Memory. |
| [atomic_sub](atomic/atomic_sub.md) | Executes scalar atomic subtraction on a single element in Global Memory. |
| [atomic_min](atomic/atomic_min.md) | Executes scalar atomic minimum on a single element in Global Memory. |
| [atomic_max](atomic/atomic_max.md) | Executes scalar atomic maximum on a single element in Global Memory. |
| [atomic_inc](atomic/atomic_inc.md) | Executes scalar atomic increment on a single element in Global Memory. |
| [atomic_dec](atomic/atomic_dec.md) | Executes scalar atomic decrement on a single element in Global Memory. |
| [atomic_and](atomic/atomic_and.md) | Executes scalar atomic bitwise AND on a single element in Global Memory. |
| [atomic_or](atomic/atomic_or.md) | Executes scalar atomic bitwise OR on a single element in Global Memory. |
| [atomic_xor](atomic/atomic_xor.md) | Executes scalar atomic bitwise XOR on a single element in Global Memory. |
| [atomic_exch](atomic/atomic_exch.md) | Executes scalar atomic exchange on a single element in Global Memory. |
| [atomic_cas](atomic/atomic_cas.md) | Executes scalar atomic compare-and-swap on a single element in Global Memory. |

## Cache Control

**Table 14** Cache Control API List

| API | Description |
| --- | --- |
| [datacache_clean_and_invalid](cache_control/datacache_clean_and_invalid.md) | Cleans and invalidates cache lines in the Data Cache by target domain. |
| [datacache_clean](cache_control/datacache_clean.md) | Marks the entire Data Cache as invalid without writing back dirty data. |
| [datacache_preload](cache_control/datacache_preload.md) | Prefetches one cache line of data from Global Memory to the DCache. |
| [datacache_clean_and_invalid_line](cache_control/datacache_clean_and_invalid_line.md) | Cleans and invalidates the Data Cache line where the Global Memory or Unified Buffer address resides. |
| [get_icache_preload_status](cache_control/get_icache_preload_status.md) | Gets the ICache prefetch status. |
| [icache_preload](cache_control/icache_preload.md) | Prefetches instructions from Global Memory to the ICache. |
| [get_scalar_load_cache_mode](cache_control/get_scalar_load_cache_mode.md) | Reads the L2 Cache management strategy for the scalar load path. |
| [get_scalar_store_cache_mode](cache_control/get_scalar_store_cache_mode.md) | Reads the L2 Cache management strategy for the scalar store path. |
| [set_scalar_cache_mode](cache_control/set_scalar_cache_mode.md) | Configures the L2 Cache management strategy for the scalar load/store path. |

## Deprecated APIs

**Table 15**  Deprecated API List

| API Name | Function Description |
| --- | --- |
| [Deprecated API description](deprecated_interface/deprecated_interface.md) | Summarizes the deprecated APIs in the Tensor API used for compatibility with legacy PascalCase naming. |
| [layout::Capacity](deprecated_interface/layout_capacity_deprecated.md) | Legacy Layout capacity query API. |
| [layout::Shape](deprecated_interface/layout_shape_deprecated.md) | Legacy Layout Shape query API. |
| [layout::Stride](deprecated_interface/layout_stride_deprecated.md) | Legacy Layout Stride query API. |
| [layout::Rank](deprecated_interface/layout_rank_deprecated.md) | Legacy Layout rank query API. |
| [layout::Size](deprecated_interface/layout_size_deprecated.md) | Legacy Layout logical element count query API. |
| [layout::Get](deprecated_interface/layout_get_deprecated.md) | Legacy Layout element get API. |
| [base_tensor::Tensor](deprecated_interface/tensor_tensor_deprecated.md) | Legacy Tensor object access API. |
| [base_tensor::Engine](deprecated_interface/tensor_engine_deprecated.md) | Legacy Tensor Engine access API. |
| [base_tensor::Layout](deprecated_interface/tensor_layout_deprecated.md) | Legacy Tensor Layout access API. |
| [base_tensor::Data](deprecated_interface/tensor_data_deprecated.md) | Legacy Tensor data address access API. |
| [base_tensor::Shape](deprecated_interface/tensor_shape_deprecated.md) | Legacy Tensor Shape access API. |
| [base_tensor::Stride](deprecated_interface/tensor_stride_deprecated.md) | Legacy Tensor Stride access API. |
| [base_tensor::Size](deprecated_interface/tensor_size_deprecated.md) | Legacy Tensor logical element count query API. |
| [base_tensor::Capacity](deprecated_interface/tensor_capacity_deprecated.md) | Legacy Tensor capacity query API. |
| [base_tensor::Slice](deprecated_interface/tensor_slice_deprecated.md) | Legacy Tensor slice API. |
| [global_tensor::SetL2CacheHint](deprecated_interface/set_l2_cache_hint_deprecated.md) | Legacy Global Tensor L2 Cache configuration API. |
