# SIMT API Software Emulation Summary

<!-- md-trans-meta sourceCommit=9c46a8bceb97c3c8375a28ea15ab74d131841ead translatedAt=2026-11-17T04:14:10.935Z pushedAt=2026-11-17T09:52:41.794Z nl=2 -->

This section summarizes the software emulation status of each API by the directory hierarchy of the SIMT API documentation.

A non-software-emulated API is one that completes an operation using a single hardware instruction; a software-emulated API is one that is implemented by the compiler, or whose functionality is implemented by an upper-level API by concatenating multiple hardware instructions.

## Warp Functions

### LaneID Functions

| API Name | API Software Emulation |
| --- | --- |
| [laneid](../SIMT-API/Warp_functions/lane_id_functions/laneid.md) | No |
| [lanemask_eq](../SIMT-API/Warp_functions/lane_id_functions/lanemask_eq.md) | No |
| [lanemask_ge](../SIMT-API/Warp_functions/lane_id_functions/lanemask_ge.md) | No |
| [lanemask_gt](../SIMT-API/Warp_functions/lane_id_functions/lanemask_gt.md) | No |
| [lanemask_le](../SIMT-API/Warp_functions/lane_id_functions/lanemask_le.md) | No |
| [lanemask_lt](../SIMT-API/Warp_functions/lane_id_functions/lanemask_lt.md) | No |

### Warp Reduce functions

| API Name | API Software Emulation |
| --- | --- |
| [asc_reduce_add](../SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md) | No |
| [asc_reduce_max](../SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_max.md) | No |
| [asc_reduce_min](../SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_min.md) | No |

### Warp Shfl functions

| API Name | API Software Emulation |
| --- | --- |
| [asc_shfl](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl.md) | No |
| [asc_shfl_down](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down.md) | No |
| [asc_shfl_up](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_up.md) | No |
| [asc_shfl_xor](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_xor.md) | No |

### Warp Vote functions

| API Name | API Software Emulation |
| --- | --- |
| [asc_activemask](../SIMT-API/Warp_functions/Warp_vote_functions/asc_activemask.md) | No |
| [asc_all](../SIMT-API/Warp_functions/Warp_vote_functions/asc_all.md) | No |
| [asc_any](../SIMT-API/Warp_functions/Warp_vote_functions/asc_any.md) | No |
| [asc_ballot](../SIMT-API/Warp_functions/Warp_vote_functions/asc_ballot.md) | No |

## Cooperative Groups

| API Name | API Software Emulation |
| --- | --- |
| [binary_partition](../SIMT-API/cooperative_groups/binary_partition.md) | Yes |
| [tiled_partition](../SIMT-API/cooperative_groups/tiled_partition.md) | Yes |

### coalesced_group

| API Name | API Software Emulation |
| --- | --- |
| [all](../SIMT-API/cooperative_groups/coalesced_group/all.md) | Yes |
| [any](../SIMT-API/cooperative_groups/coalesced_group/any.md) | Yes |
| [ballot](../SIMT-API/cooperative_groups/coalesced_group/ballot.md) | Yes |
| [coalesced_group constructor](../SIMT-API/cooperative_groups/coalesced_group/coalesced_group_constructor.md) | Yes |
| [meta_group_rank](../SIMT-API/cooperative_groups/coalesced_group/meta_group_rank.md) | Yes |
| [meta_group_size](../SIMT-API/cooperative_groups/coalesced_group/meta_group_size.md) | Yes |
| [num_threads](../SIMT-API/cooperative_groups/coalesced_group/num_threads.md) | Yes |
| [shfl](../SIMT-API/cooperative_groups/coalesced_group/shfl.md) | Yes |
| [shfl_down](../SIMT-API/cooperative_groups/coalesced_group/shfl_down.md) | Yes |
| [shfl_up](../SIMT-API/cooperative_groups/coalesced_group/shfl_up.md) | Yes |
| [size](../SIMT-API/cooperative_groups/coalesced_group/size.md) | Yes |
| [sync](../SIMT-API/cooperative_groups/coalesced_group/sync.md) | Yes |
| [thread_rank](../SIMT-API/cooperative_groups/coalesced_group/thread_rank.md) | Yes |

### thread_block

| API Name | API Software Emulation |
| --- | --- |
| [dim_threads](../SIMT-API/cooperative_groups/thread_block/dim_threads.md) | Yes |
| [group_dim](../SIMT-API/cooperative_groups/thread_block/group_dim.md) | Yes |
| [group_index](../SIMT-API/cooperative_groups/thread_block/group_index.md) | Yes |
| [num_threads](../SIMT-API/cooperative_groups/thread_block/num_threads.md) | Yes |
| [size](../SIMT-API/cooperative_groups/thread_block/size.md) | Yes |
| [sync](../SIMT-API/cooperative_groups/thread_block/sync.md) | Yes |
| [thread_block constructor](../SIMT-API/cooperative_groups/thread_block/thread_block_constructor.md) | Yes |
| [thread_index](../SIMT-API/cooperative_groups/thread_block/thread_index.md) | Yes |
| [thread_rank](../SIMT-API/cooperative_groups/thread_block/thread_rank.md) | Yes |

### thread_block_tile

| API Name | API Software Emulation |
| --- | --- |
| [all](../SIMT-API/cooperative_groups/thread_block_tile/all.md) | Yes |
| [any](../SIMT-API/cooperative_groups/thread_block_tile/any.md) | Yes |
| [ballot](../SIMT-API/cooperative_groups/thread_block_tile/ballot.md) | Yes |
| [meta_group_rank](../SIMT-API/cooperative_groups/thread_block_tile/meta_group_rank.md) | Yes |
| [meta_group_size](../SIMT-API/cooperative_groups/thread_block_tile/meta_group_size.md) | Yes |
| [num_threads](../SIMT-API/cooperative_groups/thread_block_tile/num_threads.md) | Yes |
| [shfl](../SIMT-API/cooperative_groups/thread_block_tile/shfl.md) | Yes |
| [shfl_down](../SIMT-API/cooperative_groups/thread_block_tile/shfl_down.md) | Yes |
| [shfl_up](../SIMT-API/cooperative_groups/thread_block_tile/shfl_up.md) | Yes |
| [shfl_xor](../SIMT-API/cooperative_groups/thread_block_tile/shfl_xor.md) | Yes |
| [size](../SIMT-API/cooperative_groups/thread_block_tile/size.md) | Yes |
| [sync](../SIMT-API/cooperative_groups/thread_block_tile/sync.md) | Yes |
| [thread_block_tile constructor](../SIMT-API/cooperative_groups/thread_block_tile/thread_block_tile_constructor.md) | Yes |
| [thread_rank](../SIMT-API/cooperative_groups/thread_block_tile/thread_rank.md) | Yes |

## Atomic Operations

The following tables summarize the software emulation status of atomic operation APIs in SIMD and SIMT programming scenarios. In the SIMT programming scenario, the API parameters do not carry Unified Buffer (UB)/GM address space identifiers. The compiler must first convert the input pointer to a UB/GM address space pointer, and then complete the corresponding atomic operation implementation. Whether the API return value is used affects instruction generation for some APIs. The software emulation status is therefore listed in two tables: "With Return Value" and "Without Return Value".

**Table 1** Atomic operations (with return value)

| API Name | Non-Software-Emulated Data Types | Software-Emulated Data Types |
| --- | --- | --- |
| [asc_atomic_add](../SIMT-API/atomic_operations/asc_atomic_add.md) | int32_t, uint32_t, int64_t, uint64_t | float, half2, bfloat16x2_t |
| [asc_atomic_and](../SIMT-API/atomic_operations/asc_atomic_and.md) | - | int32_t, uint32_t, int64_t, uint64_t |
| [asc_atomic_cas](../SIMT-API/atomic_operations/asc_atomic_cas.md) | uint32_t, uint64_t | int32_t, int64_t, float, half2, bfloat16x2_t |
| [asc_atomic_dec](../SIMT-API/atomic_operations/asc_atomic_dec.md) | - | uint32_t, uint64_t |
| [asc_atomic_exch](../SIMT-API/atomic_operations/asc_atomic_exch.md) | uint32_t, uint64_t | int32_t, int64_t, float, half2, bfloat16x2_t |
| [asc_atomic_inc](../SIMT-API/atomic_operations/asc_atomic_inc.md) | - | uint32_t, uint64_t |
| [asc_atomic_max](../SIMT-API/atomic_operations/asc_atomic_max.md) | uint32_t, int32_t, int64_t, uint64_t | float, half2, bfloat16x2_t |
| [asc_atomic_min](../SIMT-API/atomic_operations/asc_atomic_min.md) | uint32_t, int32_t, int64_t, uint64_t | float, half2, bfloat16x2_t |
| [asc_atomic_or](../SIMT-API/atomic_operations/asc_atomic_or.md) | - | int32_t, uint32_t, int64_t, uint64_t |
| [asc_atomic_sub](../SIMT-API/atomic_operations/asc_atomic_sub.md) | - | int32_t, uint32_t, float, int64_t, uint64_t, half2, bfloat16x2_t |
| [asc_atomic_xor](../SIMT-API/atomic_operations/asc_atomic_xor.md) | - | int32_t, uint32_t, int64_t, uint64_t |

**Table 2** Atomic operations (without return value)

| API Name | Non-Software-Emulated Data Types | Software-Emulated Data Types |
| --- | --- | --- |
| [asc_atomic_add](../SIMT-API/atomic_operations/asc_atomic_add.md) | int32_t, uint32_t, int64_t, uint64_t, float, half, bfloat16_t, half2, bfloat16x2_t | - |
| [asc_atomic_and](../SIMT-API/atomic_operations/asc_atomic_and.md) | - | int32_t, uint32_t, int64_t, uint64_t |
| [asc_atomic_cas](../SIMT-API/atomic_operations/asc_atomic_cas.md) | uint32_t, uint64_t | int32_t, int64_t, float, half2, bfloat16x2_t |
| [asc_atomic_dec](../SIMT-API/atomic_operations/asc_atomic_dec.md) | - | uint32_t, uint64_t |
| [asc_atomic_exch](../SIMT-API/atomic_operations/asc_atomic_exch.md) | uint32_t, uint64_t | int32_t, int64_t, float, half2, bfloat16x2_t |
| [asc_atomic_inc](../SIMT-API/atomic_operations/asc_atomic_inc.md) | - | uint32_t, uint64_t |
| [asc_atomic_max](../SIMT-API/atomic_operations/asc_atomic_max.md) | int32_t, uint32_t, int64_t, uint64_t, float, half, bfloat16_t, half2, bfloat16x2_t | - |
| [asc_atomic_min](../SIMT-API/atomic_operations/asc_atomic_min.md) | int32_t, uint32_t, int64_t, uint64_t, float, half, bfloat16_t, half2, bfloat16x2_t | - |
| [asc_atomic_or](../SIMT-API/atomic_operations/asc_atomic_or.md) | - | int32_t, uint32_t, int64_t, uint64_t |
| [asc_atomic_sub](../SIMT-API/atomic_operations/asc_atomic_sub.md) | half, bfloat16_t | int32_t, uint32_t, float, int64_t, uint64_t, half2, bfloat16x2_t |
| [asc_atomic_xor](../SIMT-API/atomic_operations/asc_atomic_xor.md) | - | int32_t, uint32_t, int64_t, uint64_t |

## Synchronization and Memory Fence

### Memory Fence APIs

| API Name | API Software Emulation |
| --- | --- |
| [asc_threadfence](../SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md) | No |
| [asc_threadfence_block](../SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_block.md) | No |

### Synchronization API

| API Name | API Software Emulation |
| --- | --- |
| [asc_syncthreads](../SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md) | No |

## Address Space Predicate Functions

| API Name | API Software Emulation |
| --- | --- |
| [__isGlobal](../SIMT-API/address_space_predicate_functions/__isGlobal.md) | No |
| [__isLocal](../SIMT-API/address_space_predicate_functions/__isLocal.md) | No |
| [__isUbuf](../SIMT-API/address_space_predicate_functions/__isUbuf.md) | No |

## Address Space Conversion Functions

| API Name | API Software Emulation |
| --- | --- |
| [__cvta_generic_to_global](../SIMT-API/address_space_conversion_functions/__cvta_generic_to_global.md) | No |
| [__cvta_generic_to_local](../SIMT-API/address_space_conversion_functions/__cvta_generic_to_local.md) | No |
| [__cvta_generic_to_ubuf](../SIMT-API/address_space_conversion_functions/__cvta_generic_to_ubuf.md) | No |
| [__cvta_global_to_generic](../SIMT-API/address_space_conversion_functions/__cvta_global_to_generic.md) | No |
| [__cvta_local_to_generic](../SIMT-API/address_space_conversion_functions/__cvta_local_to_generic.md) | No |
| [__cvta_ubuf_to_generic](../SIMT-API/address_space_conversion_functions/__cvta_ubuf_to_generic.md) | No |

## Math Functions

### bfloat16 Type

#### bfloat16x2 Math Functions

| API Name | API Software Emulation |
| --- | --- |
| [h2ceil](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2ceil.md) | No |
| [h2cos](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2cos.md) | Yes |
| [h2exp](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp.md) | Yes |
| [h2exp10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10.md) | Yes |
| [h2exp2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2.md) | Yes |
| [h2floor](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2floor.md) | No |
| [h2log](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log.md) | Yes |
| [h2log10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10.md) | Yes |
| [h2log2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2.md) | Yes |
| [h2rcp](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rcp.md) | Yes |
| [h2rint](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rint.md) | No |
| [h2rsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt.md) | Yes |
| [h2sin](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2sin.md) | Yes |
| [h2sqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt.md) | Yes |
| [h2tanh](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh.md) | Yes |
| [h2trunc](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2trunc.md) | Yes |

#### bfloat16x2 Comparison Functions

| API Name | API Software Emulation |
| --- | --- |
| [__hbequx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbequx2.md) | Yes |
| [__hbeqx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbeqx2.md) | Yes |
| [__hbgeux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgeux2.md) | Yes |
| [__hbgex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgex2.md) | Yes |
| [__hbgtux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtux2.md) | Yes |
| [__hbgtx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtx2.md) | Yes |
| [__hbleux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbleux2.md) | Yes |
| [__hblex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hblex2.md) | Yes |
| [__hbltux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltux2.md) | Yes |
| [__hbltx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltx2.md) | Yes |
| [__hbneux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbneux2.md) | Yes |
| [__hbnex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbnex2.md) | Yes |
| [__hequx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2.md) | Yes |
| [__hequx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2_mask.md) | Yes |
| [__heqx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2.md) | Yes |
| [__heqx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2_mask.md) | Yes |
| [__hgeux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2.md) | Yes |
| [__hgeux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2_mask.md) | Yes |
| [__hgex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2.md) | Yes |
| [__hgex2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2_mask.md) | Yes |
| [__hgtux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2.md) | Yes |
| [__hgtux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2_mask.md) | Yes |
| [__hgtx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2.md) | Yes |
| [__hgtx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2_mask.md) | Yes |
| [__hleux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2.md) | Yes |
| [__hleux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2_mask.md) | Yes |
| [__hlex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2.md) | Yes |
| [__hlex2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2_mask.md) | Yes |
| [__hltux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2.md) | Yes |
| [__hltux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2_mask.md) | Yes |
| [__hltx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2.md) | Yes |
| [__hltx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2_mask.md) | Yes |
| [__hmaxx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2.md) | No |
| [__hmaxx2_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2_nan.md) | Yes |
| [__hminx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2.md) | No |
| [__hminx2_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2_nan.md) | Yes |
| [__hneux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2.md) | Yes |
| [__hneux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2_mask.md) | Yes |
| [__hnex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2.md) | Yes |
| [__hnex2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2_mask.md) | Yes |
| [__isnanx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__isnanx2.md) | Yes |

#### bfloat16x2 Arithmetic Functions

| API Name | API Software Emulation |
| --- | --- |
| [__habsx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__habsx2.md) | No |
| [__haddx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2.md) | No |
| [__hcmadd](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hcmadd.md) | Yes |
| [__hdivx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2.md) | No |
| [__hfmax2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2.md) | No |
| [__hfmax2_relu](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_relu.md) | Yes |
| [__hmulx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hmulx2.md) | No |
| [__hnegx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hnegx2.md) | No |
| [__hsubx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hsubx2.md) | No |

#### bfloat16 Math Functions

| API Name | API Software Emulation |
| --- | --- |
| [hceil](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hceil.md) | No |
| [hcos](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hcos.md) | Yes |
| [hexp](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp.md) | Yes |
| [hexp10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp10.md) | Yes |
| [hexp2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp2.md) | Yes |
| [hfloor](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hfloor.md) | No |
| [hlog](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog.md) | Yes |
| [hlog10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog10.md) | Yes |
| [hlog2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog2.md) | Yes |
| [hrcp](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrcp.md) | Yes |
| [hrint](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrint.md) | No |
| [hrsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt.md) | Yes |
| [hsin](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsin.md) | Yes |
| [hsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsqrt.md) | Yes |
| [htanh](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/htanh.md) | Yes |
| [htrunc](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/htrunc.md) | Yes |

#### bfloat16 Comparison Functions

| API Name | API Software Emulation |
| --- | --- |
| [__heq](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__heq.md) | No |
| [__hequ](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hequ.md) | No |
| [__hge](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hge.md) | No |
| [__hgeu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hgeu.md) | No |
| [__hgt](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hgt.md) | No |
| [__hgtu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hgtu.md) | No |
| [__hisinf](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hisinf.md) | No |
| [__hisnan](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hisnan.md) | No |
| [__hle](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hle.md) | No |
| [__hleu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hleu.md) | No |
| [__hlt](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hlt.md) | No |
| [__hltu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hltu.md) | No |
| [__hmax](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmax.md) | Yes |
| [__hmax_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmax_nan.md) | Yes |
| [__hmin](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmin.md) | Yes |
| [__hmin_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmin_nan.md) | Yes |
| [__hne](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hne.md) | No |
| [__hneu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hneu.md) | No |

#### bfloat16 Arithmetic Functions

| API Name | API Software Emulation |
| --- | --- |
| [__habs](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs.md) | Yes |
| [__hadd](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd.md) | No |
| [__hdiv](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv.md) | No |
| [__hfma](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma.md) | No |
| [__hfma_relu](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_relu.md) | Yes |
| [__hmul](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul.md) | No |
| [__hneg](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hneg.md) | No |
| [__hsub](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hsub.md) | No |

#### bfloat16 Precision Conversion Functions

| API Name | API Software Emulation |
| --- | --- |
| [__bfloat1622float2](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat1622float2.md) | Yes |
| [__bfloat162bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat162.md) | Yes |
| [__bfloat162bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rd.md) | No |
| [__bfloat162bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rn.md) | No |
| [__bfloat162bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rna.md) | No |
| [__bfloat162bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_ru.md) | No |
| [__bfloat162bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rz.md) | No |
| [__bfloat162float](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162float.md) | Yes |
| [__bfloat162int_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rd.md) | No |
| [__bfloat162int_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rn.md) | No |
| [__bfloat162int_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rna.md) | No |
| [__bfloat162int_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_ru.md) | No |
| [__bfloat162int_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rz.md) | No |
| [__bfloat162ll_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rd.md) | Yes |
| [__bfloat162ll_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rn.md) | Yes |
| [__bfloat162ll_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rna.md) | Yes |
| [__bfloat162ll_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_ru.md) | Yes |
| [__bfloat162ll_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rz.md) | Yes |
| [__bfloat162uint_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rd.md) | No |
| [__bfloat162uint_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rn.md) | No |
| [__bfloat162uint_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rna.md) | No |
| [__bfloat162uint_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_ru.md) | No |
| [__bfloat162uint_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rz.md) | No |
| [__bfloat162ull_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rd.md) | Yes |
| [__bfloat162ull_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rn.md) | Yes |
| [__bfloat162ull_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rna.md) | Yes |
| [__bfloat162ull_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_ru.md) | Yes |
| [__bfloat162ull_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rz.md) | Yes |
| [__float22bfloat162_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd.md) | No |
| [__float22bfloat162_rd_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd_sat.md) | No |
| [__float22bfloat162_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn.md) | No |
| [__float22bfloat162_rn_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn_sat.md) | No |
| [__float22bfloat162_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna.md) | No |
| [__float22bfloat162_rna_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna_sat.md) | No |
| [__float22bfloat162_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru.md) | No |
| [__float22bfloat162_ru_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru_sat.md) | No |
| [__float22bfloat162_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz.md) | No |
| [__float22bfloat162_rz_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz_sat.md) | No |
| [__float2bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16.md) | No |
| [__float2bfloat162_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat162_rn.md) | Yes |
| [__float2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd.md) | No |
| [__float2bfloat16_rd_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd_sat.md) | No |
| [__float2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn.md) | No |
| [__float2bfloat16_rn_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn_sat.md) | No |
| [__float2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna.md) | No |
| [__float2bfloat16_rna_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna_sat.md) | No |
| [__float2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru.md) | No |
| [__float2bfloat16_ru_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru_sat.md) | No |
| [__float2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz.md) | No |
| [__float2bfloat16_rz_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz_sat.md) | No |
| [__floats2bfloat162_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__floats2bfloat162_rn.md) | Yes |
| [__half2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rd.md) | No |
| [__half2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rn.md) | No |
| [__half2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rna.md) | No |
| [__half2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_ru.md) | No |
| [__half2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rz.md) | No |
| [__halves2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__halves2bfloat162.md) | Yes |
| [__high2bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat16.md) | No |
| [__high2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat162.md) | Yes |
| [__high2float](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2float.md) | No |
| [__highs2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__highs2bfloat162.md) | Yes |
| [__int2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rd.md) | No |
| [__int2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rn.md) | No |
| [__int2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rna.md) | No |
| [__int2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_ru.md) | No |
| [__int2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rz.md) | No |
| [__ll2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rd.md) | Yes |
| [__ll2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rn.md) | Yes |
| [__ll2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rna.md) | Yes |
| [__ll2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_ru.md) | Yes |
| [__ll2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rz.md) | Yes |
| [__low2bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat16.md) | No |
| [__low2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat162.md) | Yes |
| [__low2float](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2float.md) | No |
| [__lowhigh2highlow](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lowhigh2highlow.md) | Yes |
| [__lows2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lows2bfloat162.md) | Yes |
| [__uint2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rd.md) | No |
| [__uint2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rn.md) | No |
| [__uint2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rna.md) | No |
| [__uint2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_ru.md) | No |
| [__uint2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rz.md) | No |
| [__ull2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rd.md) | Yes |
| [__ull2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rn.md) | Yes |
| [__ull2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rna.md) | Yes |
| [__ull2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_ru.md) | Yes |
| [__ull2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rz.md) | Yes |
| [__ushort_as_bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort_as_bfloat16.md) | Yes |

### float Math Functions

| API Name | API Software Emulation |
| --- | --- |
| [__fdividef](../SIMT-API/math_functions/float_math_functions/__fdividef.md) | No |
| [__saturatef](../SIMT-API/math_functions/float_math_functions/__saturatef.md) | Yes |
| [acosf](../SIMT-API/math_functions/float_math_functions/acosf.md) | Yes |
| [acoshf](../SIMT-API/math_functions/float_math_functions/acoshf.md) | Yes |
| [asinf](../SIMT-API/math_functions/float_math_functions/asinf.md) | Yes |
| [asinhf](../SIMT-API/math_functions/float_math_functions/asinhf.md) | Yes |
| [atan2f](../SIMT-API/math_functions/float_math_functions/atan2f.md) | Yes |
| [atanf](../SIMT-API/math_functions/float_math_functions/atanf.md) | Yes |
| [atanhf](../SIMT-API/math_functions/float_math_functions/atanhf.md) | Yes |
| [cbrtf](../SIMT-API/math_functions/float_math_functions/cbrtf.md) | Yes |
| [ceilf](../SIMT-API/math_functions/float_math_functions/ceilf.md) | No |
| [copysignf](../SIMT-API/math_functions/float_math_functions/copysignf.md) | Yes |
| [cosf](../SIMT-API/math_functions/float_math_functions/cosf.md) | Yes |
| [coshf](../SIMT-API/math_functions/float_math_functions/coshf.md) | Yes |
| [cospif](../SIMT-API/math_functions/float_math_functions/cospif.md) | Yes |
| [cyl_bessel_i0f](../SIMT-API/math_functions/float_math_functions/cyl_bessel_i0f.md) | Yes |
| [cyl_bessel_i1f](../SIMT-API/math_functions/float_math_functions/cyl_bessel_i1f.md) | Yes |
| [erfcf](../SIMT-API/math_functions/float_math_functions/erfcf.md) | Yes |
| [erfcinvf](../SIMT-API/math_functions/float_math_functions/erfcinvf.md) | Yes |
| [erfcxf](../SIMT-API/math_functions/float_math_functions/erfcxf.md) | Yes |
| [erff](../SIMT-API/math_functions/float_math_functions/erff.md) | Yes |
| [erfinvf](../SIMT-API/math_functions/float_math_functions/erfinvf.md) | Yes |
| [exp10f](../SIMT-API/math_functions/float_math_functions/exp10f.md) | No |
| [exp2f](../SIMT-API/math_functions/float_math_functions/exp2f.md) | No |
| [expf](../SIMT-API/math_functions/float_math_functions/expf.md) | No |
| [expm1f](../SIMT-API/math_functions/float_math_functions/expm1f.md) | Yes |
| [fabsf](../SIMT-API/math_functions/float_math_functions/fabsf.md) | No |
| [fdimf](../SIMT-API/math_functions/float_math_functions/fdimf.md) | Yes |
| [fdividef](../SIMT-API/math_functions/float_math_functions/fdividef.md) | Yes |
| [floorf](../SIMT-API/math_functions/float_math_functions/floorf.md) | No |
| [fmaf](../SIMT-API/math_functions/float_math_functions/fmaf.md) | No |
| [fmaxf](../SIMT-API/math_functions/float_math_functions/fmaxf.md) | Yes |
| [fminf](../SIMT-API/math_functions/float_math_functions/fminf.md) | Yes |
| [fmodf](../SIMT-API/math_functions/float_math_functions/fmodf.md) | Yes |
| [frexpf](../SIMT-API/math_functions/float_math_functions/frexpf.md) | Yes |
| [hypotf](../SIMT-API/math_functions/float_math_functions/hypotf.md) | Yes |
| [ilogbf](../SIMT-API/math_functions/float_math_functions/ilogbf.md) | Yes |
| [isfinite](../SIMT-API/math_functions/float_math_functions/isfinite.md) | No |
| [isinf](../SIMT-API/math_functions/float_math_functions/isinf.md) | No |
| [isnan](../SIMT-API/math_functions/float_math_functions/isnan.md) | No |
| [j0f](../SIMT-API/math_functions/float_math_functions/j0f.md) | Yes |
| [j1f](../SIMT-API/math_functions/float_math_functions/j1f.md) | Yes |
| [jnf](../SIMT-API/math_functions/float_math_functions/jnf.md) | Yes |
| [ldexpf](../SIMT-API/math_functions/float_math_functions/ldexpf.md) | Yes |
| [lgammaf](../SIMT-API/math_functions/float_math_functions/lgammaf.md) | Yes |
| [llrintf](../SIMT-API/math_functions/float_math_functions/llrintf.md) | Yes |
| [llroundf](../SIMT-API/math_functions/float_math_functions/llroundf.md) | Yes |
| [log10f](../SIMT-API/math_functions/float_math_functions/log10f.md) | Yes |
| [log1pf](../SIMT-API/math_functions/float_math_functions/log1pf.md) | Yes |
| [log2f](../SIMT-API/math_functions/float_math_functions/log2f.md) | Yes |
| [logbf](../SIMT-API/math_functions/float_math_functions/logbf.md) | Yes |
| [logf](../SIMT-API/math_functions/float_math_functions/logf.md) | Yes |
| [lrintf](../SIMT-API/math_functions/float_math_functions/lrintf.md) | Yes |
| [lroundf](../SIMT-API/math_functions/float_math_functions/lroundf.md) | Yes |
| [modff](../SIMT-API/math_functions/float_math_functions/modff.md) | Yes |
| [nearbyintf](../SIMT-API/math_functions/float_math_functions/nearbyintf.md) | Yes |
| [nextafterf](../SIMT-API/math_functions/float_math_functions/nextafterf.md) | Yes |
| [norm3df](../SIMT-API/math_functions/float_math_functions/norm3df.md) | Yes |
| [norm4df](../SIMT-API/math_functions/float_math_functions/norm4df.md) | Yes |
| [normcdff](../SIMT-API/math_functions/float_math_functions/normcdff.md) | Yes |
| [normcdfinvf](../SIMT-API/math_functions/float_math_functions/normcdfinvf.md) | Yes |
| [normf](../SIMT-API/math_functions/float_math_functions/normf.md) | Yes |
| [powf](../SIMT-API/math_functions/float_math_functions/powf.md) | No |
| [rcbrtf](../SIMT-API/math_functions/float_math_functions/rcbrtf.md) | Yes |
| [remainderf](../SIMT-API/math_functions/float_math_functions/remainderf.md) | Yes |
| [remquof](../SIMT-API/math_functions/float_math_functions/remquof.md) | Yes |
| [rhypotf](../SIMT-API/math_functions/float_math_functions/rhypotf.md) | Yes |
| [rintf](../SIMT-API/math_functions/float_math_functions/rintf.md) | Yes |
| [rnorm3df](../SIMT-API/math_functions/float_math_functions/rnorm3df.md) | Yes |
| [rnorm4df](../SIMT-API/math_functions/float_math_functions/rnorm4df.md) | Yes |
| [rnormf](../SIMT-API/math_functions/float_math_functions/rnormf.md) | Yes |
| [roundf](../SIMT-API/math_functions/float_math_functions/roundf.md) | No |
| [rsqrtf](../SIMT-API/math_functions/float_math_functions/rsqrtf.md) | Yes |
| [scalblnf](../SIMT-API/math_functions/float_math_functions/scalblnf.md) | Yes |
| [scalbnf](../SIMT-API/math_functions/float_math_functions/scalbnf.md) | Yes |
| [signbit](../SIMT-API/math_functions/float_math_functions/signbit.md) | Yes |
| [sincosf](../SIMT-API/math_functions/float_math_functions/sincosf.md) | Yes |
| [sincospif](../SIMT-API/math_functions/float_math_functions/sincospif.md) | Yes |
| [sinf](../SIMT-API/math_functions/float_math_functions/sinf.md) | Yes |
| [sinhf](../SIMT-API/math_functions/float_math_functions/sinhf.md) | Yes |
| [sinpif](../SIMT-API/math_functions/float_math_functions/sinpif.md) | Yes |
| [sqrtf](../SIMT-API/math_functions/float_math_functions/sqrtf.md) | No |
| [tanf](../SIMT-API/math_functions/float_math_functions/tanf.md) | Yes |
| [tanhf](../SIMT-API/math_functions/float_math_functions/tanhf.md) | Yes |
| [tanpif](../SIMT-API/math_functions/float_math_functions/tanpif.md) | Yes |
| [tgammaf](../SIMT-API/math_functions/float_math_functions/tgammaf.md) | Yes |
| [truncf](../SIMT-API/math_functions/float_math_functions/truncf.md) | No |
| [y0f](../SIMT-API/math_functions/float_math_functions/y0f.md) | Yes |
| [y1f](../SIMT-API/math_functions/float_math_functions/y1f.md) | Yes |
| [ynf](../SIMT-API/math_functions/float_math_functions/ynf.md) | Yes |

### fp8 Types

#### fp8 Conversion Functions

| API Name | API Software Emulation |
| --- | --- |
| [__asc_cvt_float2_to_fp8x2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float2_to_fp8x2.md) | Yes |
| [__e4m3x22float2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__e4m3x22float2.md) | No |
| [__e5m2x22float2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__e5m2x22float2.md) | No |
| [__float22hif82_rh](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rh.md) | No |
| [__float22hif82_rh_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rh_sat.md) | No |
| [__float22hif82_rna](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna.md) | No |
| [__float22hif82_rna_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna_sat.md) | No |
| [__half22hif82_rh](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh.md) | No |
| [__half22hif82_rh_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh_sat.md) | No |
| [__half22hif82_rna](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna.md) | No |
| [__half22hif82_rna_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna_sat.md) | No |
| [__hif822float2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__hif822float2.md) | No |
| [__hif822half2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__hif822half2.md) | No |

### half Type

#### half2 Math Library Functions

| API Name | API Software Emulation |
| --- | --- |
| [h2ceil](../SIMT-API/math_functions/half_type/half2_math_functions/h2ceil.md) | Yes |
| [h2cos](../SIMT-API/math_functions/half_type/half2_math_functions/h2cos.md) | Yes |
| [h2exp](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp.md) | Yes |
| [h2exp10](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp10.md) | Yes |
| [h2exp2](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp2.md) | Yes |
| [h2floor](../SIMT-API/math_functions/half_type/half2_math_functions/h2floor.md) | Yes |
| [h2log](../SIMT-API/math_functions/half_type/half2_math_functions/h2log.md) | Yes |
| [h2log10](../SIMT-API/math_functions/half_type/half2_math_functions/h2log10.md) | Yes |
| [h2log2](../SIMT-API/math_functions/half_type/half2_math_functions/h2log2.md) | Yes |
| [h2rcp](../SIMT-API/math_functions/half_type/half2_math_functions/h2rcp.md) | Yes |
| [h2rint](../SIMT-API/math_functions/half_type/half2_math_functions/h2rint.md) | Yes |
| [h2rsqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2rsqrt.md) | Yes |
| [h2sin](../SIMT-API/math_functions/half_type/half2_math_functions/h2sin.md) | Yes |
| [h2sqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2sqrt.md) | No |
| [h2tanh](../SIMT-API/math_functions/half_type/half2_math_functions/h2tanh.md) | Yes |
| [h2trunc](../SIMT-API/math_functions/half_type/half2_math_functions/h2trunc.md) | Yes |

#### Half2 Comparison Functions

| API Name | API Software Emulation |
| --- | --- |
| [__hbequx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbequx2.md) | No |
| [__hbeqx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbeqx2.md) | No |
| [__hbgeux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgeux2.md) | No |
| [__hbgex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgex2.md) | No |
| [__hbgtux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgtux2.md) | No |
| [__hbgtx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgtx2.md) | No |
| [__hbleux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbleux2.md) | No |
| [__hblex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hblex2.md) | No |
| [__hbltux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbltux2.md) | No |
| [__hbltx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbltx2.md) | No |
| [__hbneux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbneux2.md) | No |
| [__hbnex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbnex2.md) | No |
| [__hequx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hequx2.md) | No |
| [__hequx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hequx2_mask.md) | No |
| [__heqx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__heqx2.md) | No |
| [__heqx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__heqx2_mask.md) | No |
| [__hgeux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgeux2.md) | No |
| [__hgeux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgeux2_mask.md) | No |
| [__hgex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgex2.md) | No |
| [__hgex2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgex2_mask.md) | No |
| [__hgtux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtux2.md) | No |
| [__hgtux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtux2_mask.md) | No |
| [__hgtx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtx2.md) | No |
| [__hgtx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtx2_mask.md) | No |
| [__hleux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hleux2.md) | No |
| [__hleux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hleux2_mask.md) | No |
| [__hlex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hlex2.md) | No |
| [__hlex2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hlex2_mask.md) | No |
| [__hltux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltux2.md) | No |
| [__hltux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltux2_mask.md) | No |
| [__hltx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltx2.md) | No |
| [__hltx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltx2_mask.md) | No |
| [__hmaxx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hmaxx2.md) | No |
| [__hmaxx2_nan](../SIMT-API/math_functions/half_type/half2_compare_functions/__hmaxx2_nan.md) | Yes |
| [__hminx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hminx2.md) | No |
| [__hminx2_nan](../SIMT-API/math_functions/half_type/half2_compare_functions/__hminx2_nan.md) | Yes |
| [__hneux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hneux2.md) | No |
| [__hneux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hneux2_mask.md) | No |
| [__hnex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hnex2.md) | No |
| [__hnex2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hnex2_mask.md) | No |
| [__isnanx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__isnanx2.md) | No |

#### half2 Arithmetic Functions

| API Name | API Software Emulation |
| --- | --- |
| [__habsx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__habsx2.md) | No |
| [__haddx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__haddx2.md) | No |
| [__hcmadd](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hcmadd.md) | Yes |
| [__hdivx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hdivx2.md) | No |
| [__hfmax2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hfmax2.md) | No |
| [__hfmax2_relu](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hfmax2_relu.md) | Yes |
| [__hmulx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hmulx2.md) | No |
| [__hnegx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hnegx2.md) | No |
| [__hsubx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hsubx2.md) | No |

#### half Math Functions

| API Name | API Software Emulation |
| --- | --- |
| [hceil](../SIMT-API/math_functions/half_type/half_math_functions/hceil.md) | Yes |
| [hcos](../SIMT-API/math_functions/half_type/half_math_functions/hcos.md) | Yes |
| [hexp](../SIMT-API/math_functions/half_type/half_math_functions/hexp.md) | No |
| [hexp10](../SIMT-API/math_functions/half_type/half_math_functions/hexp10.md) | Yes |
| [hexp2](../SIMT-API/math_functions/half_type/half_math_functions/hexp2.md) | Yes |
| [hfloor](../SIMT-API/math_functions/half_type/half_math_functions/hfloor.md) | Yes |
| [hlog](../SIMT-API/math_functions/half_type/half_math_functions/hlog.md) | No |
| [hlog10](../SIMT-API/math_functions/half_type/half_math_functions/hlog10.md) | Yes |
| [hlog2](../SIMT-API/math_functions/half_type/half_math_functions/hlog2.md) | Yes |
| [hrcp](../SIMT-API/math_functions/half_type/half_math_functions/hrcp.md) | Yes |
| [hrint](../SIMT-API/math_functions/half_type/half_math_functions/hrint.md) | No |
| [hrsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hrsqrt.md) | Yes |
| [hsin](../SIMT-API/math_functions/half_type/half_math_functions/hsin.md) | Yes |
| [hsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hsqrt.md) | No |
| [htanh](../SIMT-API/math_functions/half_type/half_math_functions/htanh.md) | Yes |
| [htrunc](../SIMT-API/math_functions/half_type/half_math_functions/htrunc.md) | Yes |

#### Half Type Comparison Functions

| API Name | API Software Emulation |
| --- | --- |
| [__heq](../SIMT-API/math_functions/half_type/half_compare_functions/__heq.md) | No |
| [__hequ](../SIMT-API/math_functions/half_type/half_compare_functions/__hequ.md) | No |
| [__hge](../SIMT-API/math_functions/half_type/half_compare_functions/__hge.md) | No |
| [__hgeu](../SIMT-API/math_functions/half_type/half_compare_functions/__hgeu.md) | No |
| [__hgt](../SIMT-API/math_functions/half_type/half_compare_functions/__hgt.md) | No |
| [__hgtu](../SIMT-API/math_functions/half_type/half_compare_functions/__hgtu.md) | No |
| [__hisinf](../SIMT-API/math_functions/half_type/half_compare_functions/__hisinf.md) | No |
| [__hisnan](../SIMT-API/math_functions/half_type/half_compare_functions/__hisnan.md) | No |
| [__hle](../SIMT-API/math_functions/half_type/half_compare_functions/__hle.md) | No |
| [__hleu](../SIMT-API/math_functions/half_type/half_compare_functions/__hleu.md) | No |
| [__hlt](../SIMT-API/math_functions/half_type/half_compare_functions/__hlt.md) | No |
| [__hltu](../SIMT-API/math_functions/half_type/half_compare_functions/__hltu.md) | No |
| [__hmax](../SIMT-API/math_functions/half_type/half_compare_functions/__hmax.md) | Yes |
| [__hmax_nan](../SIMT-API/math_functions/half_type/half_compare_functions/__hmax_nan.md) | Yes |
| [__hmin](../SIMT-API/math_functions/half_type/half_compare_functions/__hmin.md) | Yes |
| [__hmin_nan](../SIMT-API/math_functions/half_type/half_compare_functions/__hmin_nan.md) | Yes |
| [__hne](../SIMT-API/math_functions/half_type/half_compare_functions/__hne.md) | No |
| [__hneu](../SIMT-API/math_functions/half_type/half_compare_functions/__hneu.md) | No |

#### half Arithmetic Functions

| API Name | API Software Emulation |
| --- | --- |
| [__habs](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__habs.md) | Yes |
| [__hadd](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hadd.md) | No |
| [__hdiv](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hdiv.md) | No |
| [__hfma](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma.md) | No |
| [__hfma_relu](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma_relu.md) | Yes |
| [__hmul](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hmul.md) | No |
| [__hneg](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hneg.md) | No |
| [__hsub](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hsub.md) | No |

#### half Precision Conversion Functions

| API Name | API Software Emulation |
| --- | --- |
| [__bfloat162half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd.md) | No |
| [__bfloat162half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd_sat.md) | No |
| [__bfloat162half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn.md) | No |
| [__bfloat162half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn_sat.md) | No |
| [__bfloat162half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna.md) | No |
| [__bfloat162half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna_sat.md) | No |
| [__bfloat162half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru.md) | No |
| [__bfloat162half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru_sat.md) | No |
| [__bfloat162half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz.md) | No |
| [__bfloat162half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz_sat.md) | No |
| [__float22half2_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rd.md) | No |
| [__float22half2_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rd_sat.md) | No |
| [__float22half2_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rn.md) | No |
| [__float22half2_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rn_sat.md) | No |
| [__float22half2_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rna.md) | No |
| [__float22half2_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rna_sat.md) | No |
| [__float22half2_ro](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ro.md) | No |
| [__float22half2_ro_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ro_sat.md) | No |
| [__float22half2_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ru.md) | No |
| [__float22half2_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ru_sat.md) | No |
| [__float22half2_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rz.md) | No |
| [__float22half2_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rz_sat.md) | No |
| [__float2half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half.md) | No |
| [__float2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rd.md) | No |
| [__float2half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rd_sat.md) | No |
| [__float2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rn.md) | No |
| [__float2half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rn_sat.md) | No |
| [__float2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rna.md) | No |
| [__float2half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rna_sat.md) | No |
| [__float2half_ro](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ro.md) | No |
| [__float2half_ro_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ro_sat.md) | No |
| [__float2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ru.md) | Yes |
| [__float2half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ru_sat.md) | No |
| [__float2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rz.md) | No |
| [__float2half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rz_sat.md) | No |
| [__floats2half2_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__floats2half2_rn.md) | Yes |
| [__half22float2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half22float2.md) | No |
| [__half2float](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2float.md) | Yes |
| [__half2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rd.md) | No |
| [__half2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rn.md) | No |
| [__half2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rna.md) | No |
| [__half2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_ru.md) | No |
| [__half2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rz.md) | No |
| [__half2int_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rd.md) | No |
| [__half2int_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rn.md) | No |
| [__half2int_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rna.md) | No |
| [__half2int_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_ru.md) | No |
| [__half2int_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rz.md) | No |
| [__half2ll_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rd.md) | Yes |
| [__half2ll_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rn.md) | Yes |
| [__half2ll_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rna.md) | Yes |
| [__half2ll_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_ru.md) | Yes |
| [__half2ll_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rz.md) | Yes |
| [__half2uint_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rd.md) | No |
| [__half2uint_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rn.md) | No |
| [__half2uint_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rna.md) | No |
| [__half2uint_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_ru.md) | No |
| [__half2uint_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rz.md) | No |
| [__half2ull_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rd.md) | Yes |
| [__half2ull_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rn.md) | Yes |
| [__half2ull_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rna.md) | Yes |
| [__half2ull_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_ru.md) | Yes |
| [__half2ull_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rz.md) | Yes |
| [__halves2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__halves2half2.md) | Yes |
| [__high2float](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__high2float.md) | No |
| [__high2half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__high2half.md) | No |
| [__high2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__high2half2.md) | Yes |
| [__highs2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__highs2half2.md) | Yes |
| [__int2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rd.md) | No |
| [__int2half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rd_sat.md) | No |
| [__int2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rn.md) | No |
| [__int2half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rn_sat.md) | No |
| [__int2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rna.md) | No |
| [__int2half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rna_sat.md) | No |
| [__int2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_ru.md) | No |
| [__int2half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_ru_sat.md) | No |
| [__int2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rz.md) | No |
| [__int2half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rz_sat.md) | No |
| [__ll2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rd.md) | Yes |
| [__ll2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rn.md) | Yes |
| [__ll2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rna.md) | Yes |
| [__ll2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_ru.md) | Yes |
| [__ll2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rz.md) | Yes |
| [__low2float](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__low2float.md) | No |
| [__low2half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__low2half.md) | Yes |
| [__low2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__low2half2.md) | Yes |
| [__lowhigh2highlow](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__lowhigh2highlow.md) | Yes |
| [__lows2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__lows2half2.md) | Yes |
| [__uint2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rd.md) | No |
| [__uint2half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rd_sat.md) | No |
| [__uint2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rn.md) | No |
| [__uint2half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rn_sat.md) | No |
| [__uint2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rna.md) | No |
| [__uint2half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rna_sat.md) | No |
| [__uint2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_ru.md) | No |
| [__uint2half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_ru_sat.md) | No |
| [__uint2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rz.md) | No |
| [__uint2half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rz_sat.md) | No |
| [__ull2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rd.md) | Yes |
| [__ull2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rn.md) | Yes |
| [__ull2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rna.md) | Yes |
| [__ull2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_ru.md) | Yes |
| [__ull2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rz.md) | Yes |
| [__ushort_as_half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ushort_as_half.md) | Yes |

### Data Type Conversion

#### Type Conversion Functions

| API Name | API Software Emulation |
| --- | --- |
| [__float2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rd.md) | No |
| [__float2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rn.md) | No |
| [__float2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rna.md) | No |
| [__float2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_ru.md) | No |
| [__float2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rz.md) | No |
| [__float2int_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rd.md) | No |
| [__float2int_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rn.md) | No |
| [__float2int_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rna.md) | No |
| [__float2int_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_ru.md) | No |
| [__float2int_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rz.md) | No |
| [__float2ll_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rd.md) | No |
| [__float2ll_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rn.md) | No |
| [__float2ll_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rna.md) | No |
| [__float2ll_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_ru.md) | No |
| [__float2ll_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rz.md) | No |
| [__float2uint_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rd.md) | No |
| [__float2uint_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rn.md) | No |
| [__float2uint_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rna.md) | No |
| [__float2uint_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_ru.md) | No |
| [__float2uint_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rz.md) | No |
| [__float2ull_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rd.md) | No |
| [__float2ull_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rn.md) | No |
| [__float2ull_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rna.md) | No |
| [__float2ull_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_ru.md) | No |
| [__float2ull_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rz.md) | No |
| [__float_as_int](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float_as_int.md) | Yes |
| [__float_as_uint](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float_as_uint.md) | Yes |
| [__int2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rd.md) | No |
| [__int2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rn.md) | No |
| [__int2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rna.md) | No |
| [__int2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_ru.md) | No |
| [__int2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rz.md) | No |
| [__int_as_float](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int_as_float.md) | Yes |
| [__ll2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rd.md) | No |
| [__ll2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rn.md) | No |
| [__ll2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rna.md) | No |
| [__ll2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_ru.md) | No |
| [__ll2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rz.md) | No |
| [__uint2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rd.md) | No |
| [__uint2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rn.md) | No |
| [__uint2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rna.md) | No |
| [__uint2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_ru.md) | No |
| [__uint2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rz.md) | No |
| [__uint_as_float](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint_as_float.md) | Yes |
| [__ull2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rd.md) | No |
| [__ull2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rn.md) | No |
| [__ull2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rna.md) | No |
| [__ull2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_ru.md) | No |
| [__ull2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rz.md) | No |

### Integer Math Library Functions

| API Name | API Software Emulation |
| --- | --- |
| [__brev](../SIMT-API/math_functions/integer_math_functions/__brev.md) | No |
| [__byte_perm](../SIMT-API/math_functions/integer_math_functions/__byte_perm.md) | No |
| [__clz](../SIMT-API/math_functions/integer_math_functions/__clz.md) | No |
| [__ffs](../SIMT-API/math_functions/integer_math_functions/__ffs.md) | No |
| [__hadd](../SIMT-API/math_functions/integer_math_functions/__hadd.md) | No |
| [__mul24](../SIMT-API/math_functions/integer_math_functions/__mul24.md) | No |
| [__mul64hi](../SIMT-API/math_functions/integer_math_functions/__mul64hi.md) | No |
| [__mul_i32toi64](../SIMT-API/math_functions/integer_math_functions/__mul_i32toi64.md) | No |
| [__mulhi](../SIMT-API/math_functions/integer_math_functions/__mulhi.md) | No |
| [__popc](../SIMT-API/math_functions/integer_math_functions/__popc.md) | No |
| [__rhadd](../SIMT-API/math_functions/integer_math_functions/__rhadd.md) | No |
| [__sad](../SIMT-API/math_functions/integer_math_functions/__sad.md) | No |
| [__uhadd](../SIMT-API/math_functions/integer_math_functions/__uhadd.md) | No |
| [__umul24](../SIMT-API/math_functions/integer_math_functions/__umul24.md) | No |
| [__umul64hi](../SIMT-API/math_functions/integer_math_functions/__umul64hi.md) | No |
| [__umulhi](../SIMT-API/math_functions/integer_math_functions/__umulhi.md) | No |
| [__urhadd](../SIMT-API/math_functions/integer_math_functions/__urhadd.md) | No |
| [__usad](../SIMT-API/math_functions/integer_math_functions/__usad.md) | No |
| [labs](../SIMT-API/math_functions/integer_math_functions/labs.md) | Yes |
| [llabs](../SIMT-API/math_functions/integer_math_functions/llabs.md) | Yes |
| [llmax](../SIMT-API/math_functions/integer_math_functions/llmax.md) | Yes |
| [llmin](../SIMT-API/math_functions/integer_math_functions/llmin.md) | Yes |
| [max](../SIMT-API/math_functions/integer_math_functions/max.md) | No |
| [min](../SIMT-API/math_functions/integer_math_functions/min.md) | No |
| [ullmax](../SIMT-API/math_functions/integer_math_functions/ullmax.md) | Yes |
| [ullmin](../SIMT-API/math_functions/integer_math_functions/ullmin.md) | Yes |
| [umax](../SIMT-API/math_functions/integer_math_functions/umax.md) | No |
| [umin](../SIMT-API/math_functions/integer_math_functions/umin.md) | No |

## Memory Access Functions

| API Name | API Software Emulation |
| --- | --- |
| [asc_dcci_entire](../SIMT-API/memory_access_functions/asc_dcci_entire.md) | No |
| [asc_dcci_single](../SIMT-API/memory_access_functions/asc_dcci_single.md) | No |
| [asc_ldca](../SIMT-API/memory_access_functions/asc_ldca.md) | No |
| [asc_ldcg](../SIMT-API/memory_access_functions/asc_ldcg.md) | No |
| [asc_stcg](../SIMT-API/memory_access_functions/asc_stcg.md) | No |
| [asc_stwt](../SIMT-API/memory_access_functions/asc_stwt.md) | No |
