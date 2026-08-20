# SIMT接口软仿情况汇总

本节按照SIMT API文档所在目录分级汇总各接口的软仿情况。

非软仿接口是指使用单一硬件指令完成操作的接口；软仿接口是指由编译器实现，或由上层接口通过拼接多条硬件指令实现功能的接口。

## Warp函数

### LaneID类函数

| API名 | API是否软仿 |
| --- | --- |
| [laneid](../SIMT-API/Warp_functions/lane_id_functions/laneid.md) | 否 |
| [lanemask_eq](../SIMT-API/Warp_functions/lane_id_functions/lanemask_eq.md) | 否 |
| [lanemask_ge](../SIMT-API/Warp_functions/lane_id_functions/lanemask_ge.md) | 否 |
| [lanemask_gt](../SIMT-API/Warp_functions/lane_id_functions/lanemask_gt.md) | 否 |
| [lanemask_le](../SIMT-API/Warp_functions/lane_id_functions/lanemask_le.md) | 否 |
| [lanemask_lt](../SIMT-API/Warp_functions/lane_id_functions/lanemask_lt.md) | 否 |

### Warp Reduce类函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_reduce_add](../SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md) | 否 |
| [asc_reduce_max](../SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_max.md) | 否 |
| [asc_reduce_min](../SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_min.md) | 否 |

### Warp Shfl类函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_shfl](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl.md) | 否 |
| [asc_shfl_down](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_down.md) | 否 |
| [asc_shfl_up](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_up.md) | 否 |
| [asc_shfl_xor](../SIMT-API/Warp_functions/Warp_shfl_functions/asc_shfl_xor.md) | 否 |

### Warp Vote类函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_activemask](../SIMT-API/Warp_functions/Warp_vote_functions/asc_activemask.md) | 否 |
| [asc_all](../SIMT-API/Warp_functions/Warp_vote_functions/asc_all.md) | 否 |
| [asc_any](../SIMT-API/Warp_functions/Warp_vote_functions/asc_any.md) | 否 |
| [asc_ballot](../SIMT-API/Warp_functions/Warp_vote_functions/asc_ballot.md) | 否 |

## 协作组

| API名 | API是否软仿 |
| --- | --- |
| [binary_partition](../SIMT-API/cooperative_groups/binary_partition.md) | 是 |
| [tiled_partition](../SIMT-API/cooperative_groups/tiled_partition.md) | 是 |

### coalesced_group

| API名 | API是否软仿 |
| --- | --- |
| [all](../SIMT-API/cooperative_groups/coalesced_group/all.md) | 是 |
| [any](../SIMT-API/cooperative_groups/coalesced_group/any.md) | 是 |
| [ballot](../SIMT-API/cooperative_groups/coalesced_group/ballot.md) | 是 |
| [coalesced_group构造函数](../SIMT-API/cooperative_groups/coalesced_group/coalesced_group_constructor.md) | 是 |
| [meta_group_rank](../SIMT-API/cooperative_groups/coalesced_group/meta_group_rank.md) | 是 |
| [meta_group_size](../SIMT-API/cooperative_groups/coalesced_group/meta_group_size.md) | 是 |
| [num_threads](../SIMT-API/cooperative_groups/coalesced_group/num_threads.md) | 是 |
| [shfl](../SIMT-API/cooperative_groups/coalesced_group/shfl.md) | 是 |
| [shfl_down](../SIMT-API/cooperative_groups/coalesced_group/shfl_down.md) | 是 |
| [shfl_up](../SIMT-API/cooperative_groups/coalesced_group/shfl_up.md) | 是 |
| [size](../SIMT-API/cooperative_groups/coalesced_group/size.md) | 是 |
| [sync](../SIMT-API/cooperative_groups/coalesced_group/sync.md) | 是 |
| [thread_rank](../SIMT-API/cooperative_groups/coalesced_group/thread_rank.md) | 是 |

### thread_block

| API名 | API是否软仿 |
| --- | --- |
| [dim_threads](../SIMT-API/cooperative_groups/thread_block/dim_threads.md) | 是 |
| [group_dim](../SIMT-API/cooperative_groups/thread_block/group_dim.md) | 是 |
| [group_index](../SIMT-API/cooperative_groups/thread_block/group_index.md) | 是 |
| [num_threads](../SIMT-API/cooperative_groups/thread_block/num_threads.md) | 是 |
| [size](../SIMT-API/cooperative_groups/thread_block/size.md) | 是 |
| [sync](../SIMT-API/cooperative_groups/thread_block/sync.md) | 是 |
| [thread_block构造函数](../SIMT-API/cooperative_groups/thread_block/thread_block_constructor.md) | 是 |
| [thread_index](../SIMT-API/cooperative_groups/thread_block/thread_index.md) | 是 |
| [thread_rank](../SIMT-API/cooperative_groups/thread_block/thread_rank.md) | 是 |

### thread_block_tile

| API名 | API是否软仿 |
| --- | --- |
| [all](../SIMT-API/cooperative_groups/thread_block_tile/all.md) | 是 |
| [any](../SIMT-API/cooperative_groups/thread_block_tile/any.md) | 是 |
| [ballot](../SIMT-API/cooperative_groups/thread_block_tile/ballot.md) | 是 |
| [meta_group_rank](../SIMT-API/cooperative_groups/thread_block_tile/meta_group_rank.md) | 是 |
| [meta_group_size](../SIMT-API/cooperative_groups/thread_block_tile/meta_group_size.md) | 是 |
| [num_threads](../SIMT-API/cooperative_groups/thread_block_tile/num_threads.md) | 是 |
| [shfl](../SIMT-API/cooperative_groups/thread_block_tile/shfl.md) | 是 |
| [shfl_down](../SIMT-API/cooperative_groups/thread_block_tile/shfl_down.md) | 是 |
| [shfl_up](../SIMT-API/cooperative_groups/thread_block_tile/shfl_up.md) | 是 |
| [shfl_xor](../SIMT-API/cooperative_groups/thread_block_tile/shfl_xor.md) | 是 |
| [size](../SIMT-API/cooperative_groups/thread_block_tile/size.md) | 是 |
| [sync](../SIMT-API/cooperative_groups/thread_block_tile/sync.md) | 是 |
| [thread_block_tile构造函数](../SIMT-API/cooperative_groups/thread_block_tile/thread_block_tile_constructor.md) | 是 |
| [thread_rank](../SIMT-API/cooperative_groups/thread_block_tile/thread_rank.md) | 是 |

## 原子操作

下表汇总原子操作接口在SIMD与SIMT编程场景下的软仿情况。SIMT编程场景中，接口形参不携带Unified Buffer（UB）/GM地址空间标识，编译器需先完成入参指针到UB/GM地址空间指针的转换，再完成对应的原子操作实现。是否使用接口返回值会影响部分接口的指令生成，接口软仿情况分“使用返回值”与“不使用返回值”两张表列出。

**表1** 原子操作（使用返回值）

| API名 | 非软仿数据类型 | 软仿数据类型 |
| --- | --- | --- |
| [asc_atomic_add](../SIMT-API/atomic_operations/asc_atomic_add.md) | int32_t、uint32_t、int64_t、uint64_t | float、half2、bfloat16x2_t |
| [asc_atomic_and](../SIMT-API/atomic_operations/asc_atomic_and.md) | - | int32_t、uint32_t、int64_t、uint64_t |
| [asc_atomic_cas](../SIMT-API/atomic_operations/asc_atomic_cas.md) | uint32_t、uint64_t | int32_t、int64_t、float、half2、bfloat16x2_t |
| [asc_atomic_dec](../SIMT-API/atomic_operations/asc_atomic_dec.md) | - | uint32_t、uint64_t |
| [asc_atomic_exch](../SIMT-API/atomic_operations/asc_atomic_exch.md) | uint32_t、uint64_t | int32_t、int64_t、float、half2、bfloat16x2_t |
| [asc_atomic_inc](../SIMT-API/atomic_operations/asc_atomic_inc.md) | - | uint32_t、uint64_t |
| [asc_atomic_max](../SIMT-API/atomic_operations/asc_atomic_max.md) | uint32_t、int32_t、int64_t、uint64_t | float、half2、bfloat16x2_t |
| [asc_atomic_min](../SIMT-API/atomic_operations/asc_atomic_min.md) | uint32_t、int32_t、int64_t、uint64_t | float、half2、bfloat16x2_t |
| [asc_atomic_or](../SIMT-API/atomic_operations/asc_atomic_or.md) | - | int32_t、uint32_t、int64_t、uint64_t |
| [asc_atomic_sub](../SIMT-API/atomic_operations/asc_atomic_sub.md) | - | int32_t、uint32_t、float、int64_t、uint64_t、half2、bfloat16x2_t |
| [asc_atomic_xor](../SIMT-API/atomic_operations/asc_atomic_xor.md) | - | int32_t、uint32_t、int64_t、uint64_t |

**表2** 原子操作（不使用返回值）

| API名 | 非软仿数据类型 | 软仿数据类型 |
| --- | --- | --- |
| [asc_atomic_add](../SIMT-API/atomic_operations/asc_atomic_add.md) | int32_t、uint32_t、int64_t、uint64_t、float、half、bfloat16_t、half2、bfloat16x2_t | - |
| [asc_atomic_and](../SIMT-API/atomic_operations/asc_atomic_and.md) | - | int32_t、uint32_t、int64_t、uint64_t |
| [asc_atomic_cas](../SIMT-API/atomic_operations/asc_atomic_cas.md) | uint32_t、uint64_t | int32_t、int64_t、float、half2、bfloat16x2_t |
| [asc_atomic_dec](../SIMT-API/atomic_operations/asc_atomic_dec.md) | - | uint32_t、uint64_t |
| [asc_atomic_exch](../SIMT-API/atomic_operations/asc_atomic_exch.md) | uint32_t、uint64_t | int32_t、int64_t、float、half2、bfloat16x2_t |
| [asc_atomic_inc](../SIMT-API/atomic_operations/asc_atomic_inc.md) | - | uint32_t、uint64_t |
| [asc_atomic_max](../SIMT-API/atomic_operations/asc_atomic_max.md) | int32_t、uint32_t、int64_t、uint64_t、float、half、bfloat16_t、half2、bfloat16x2_t | - |
| [asc_atomic_min](../SIMT-API/atomic_operations/asc_atomic_min.md) | int32_t、uint32_t、int64_t、uint64_t、float、half、bfloat16_t、half2、bfloat16x2_t | - |
| [asc_atomic_or](../SIMT-API/atomic_operations/asc_atomic_or.md) | - | int32_t、uint32_t、int64_t、uint64_t |
| [asc_atomic_sub](../SIMT-API/atomic_operations/asc_atomic_sub.md) | half、bfloat16_t | int32_t、uint32_t、float、int64_t、uint64_t、half2、bfloat16x2_t |
| [asc_atomic_xor](../SIMT-API/atomic_operations/asc_atomic_xor.md) | - | int32_t、uint32_t、int64_t、uint64_t |

## 同步与内存栅栏

### 内存栅栏接口

| API名 | API是否软仿 |
| --- | --- |
| [asc_threadfence](../SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md) | 否 |
| [asc_threadfence_block](../SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_block.md) | 否 |

### 同步接口

| API名 | API是否软仿 |
| --- | --- |
| [asc_syncthreads](../SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md) | 否 |

## 地址空间谓词函数

| API名 | API是否软仿 |
| --- | --- |
| [__isGlobal](../SIMT-API/address_space_predicate_functions/__isGlobal.md) | 否 |
| [__isLocal](../SIMT-API/address_space_predicate_functions/__isLocal.md) | 否 |
| [__isUbuf](../SIMT-API/address_space_predicate_functions/__isUbuf.md) | 否 |

## 地址空间转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__cvta_generic_to_global](../SIMT-API/address_space_conversion_functions/__cvta_generic_to_global.md) | 否 |
| [__cvta_generic_to_local](../SIMT-API/address_space_conversion_functions/__cvta_generic_to_local.md) | 否 |
| [__cvta_generic_to_ubuf](../SIMT-API/address_space_conversion_functions/__cvta_generic_to_ubuf.md) | 否 |
| [__cvta_global_to_generic](../SIMT-API/address_space_conversion_functions/__cvta_global_to_generic.md) | 否 |
| [__cvta_local_to_generic](../SIMT-API/address_space_conversion_functions/__cvta_local_to_generic.md) | 否 |
| [__cvta_ubuf_to_generic](../SIMT-API/address_space_conversion_functions/__cvta_ubuf_to_generic.md) | 否 |

## 数学函数

### bfloat16类型

#### bfloat16x2类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [h2ceil](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2ceil.md) | 否 |
| [h2cos](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2cos.md) | 是 |
| [h2exp](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp.md) | 是 |
| [h2exp10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10.md) | 是 |
| [h2exp2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2.md) | 是 |
| [h2floor](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2floor.md) | 否 |
| [h2log](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log.md) | 是 |
| [h2log10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10.md) | 是 |
| [h2log2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2.md) | 是 |
| [h2rcp](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rcp.md) | 是 |
| [h2rint](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rint.md) | 否 |
| [h2rsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt.md) | 是 |
| [h2sin](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2sin.md) | 是 |
| [h2sqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt.md) | 是 |
| [h2tanh](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh.md) | 是 |
| [h2trunc](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2trunc.md) | 是 |

#### bfloat16x2类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__hbequx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbequx2.md) | 是 |
| [__hbeqx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbeqx2.md) | 是 |
| [__hbgeux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgeux2.md) | 是 |
| [__hbgex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgex2.md) | 是 |
| [__hbgtux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtux2.md) | 是 |
| [__hbgtx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbgtx2.md) | 是 |
| [__hbleux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbleux2.md) | 是 |
| [__hblex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hblex2.md) | 是 |
| [__hbltux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltux2.md) | 是 |
| [__hbltx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbltx2.md) | 是 |
| [__hbneux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbneux2.md) | 是 |
| [__hbnex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hbnex2.md) | 是 |
| [__hequx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2.md) | 是 |
| [__hequx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hequx2_mask.md) | 是 |
| [__heqx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2.md) | 是 |
| [__heqx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__heqx2_mask.md) | 是 |
| [__hgeux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2.md) | 是 |
| [__hgeux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgeux2_mask.md) | 是 |
| [__hgex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2.md) | 是 |
| [__hgex2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgex2_mask.md) | 是 |
| [__hgtux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2.md) | 是 |
| [__hgtux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtux2_mask.md) | 是 |
| [__hgtx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2.md) | 是 |
| [__hgtx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hgtx2_mask.md) | 是 |
| [__hleux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2.md) | 是 |
| [__hleux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hleux2_mask.md) | 是 |
| [__hlex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2.md) | 是 |
| [__hlex2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hlex2_mask.md) | 是 |
| [__hltux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2.md) | 是 |
| [__hltux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltux2_mask.md) | 是 |
| [__hltx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2.md) | 是 |
| [__hltx2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hltx2_mask.md) | 是 |
| [__hmaxx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2.md) | 否 |
| [__hmaxx2_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hmaxx2_nan.md) | 是 |
| [__hminx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2.md) | 否 |
| [__hminx2_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hminx2_nan.md) | 是 |
| [__hneux2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2.md) | 是 |
| [__hneux2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hneux2_mask.md) | 是 |
| [__hnex2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2.md) | 是 |
| [__hnex2_mask](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__hnex2_mask.md) | 是 |
| [__isnanx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_compare_functions/__isnanx2.md) | 是 |

#### bfloat16x2类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habsx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__habsx2.md) | 否 |
| [__haddx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2.md) | 否 |
| [__hcmadd](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hcmadd.md) | 是 |
| [__hdivx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2.md) | 否 |
| [__hfmax2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2.md) | 否 |
| [__hfmax2_relu](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hfmax2_relu.md) | 是 |
| [__hmulx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hmulx2.md) | 否 |
| [__hnegx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hnegx2.md) | 否 |
| [__hsubx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hsubx2.md) | 否 |

#### bfloat16类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [hceil](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hceil.md) | 否 |
| [hcos](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hcos.md) | 是 |
| [hexp](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp.md) | 是 |
| [hexp10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp10.md) | 是 |
| [hexp2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp2.md) | 是 |
| [hfloor](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hfloor.md) | 否 |
| [hlog](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog.md) | 是 |
| [hlog10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog10.md) | 是 |
| [hlog2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog2.md) | 是 |
| [hrcp](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrcp.md) | 是 |
| [hrint](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrint.md) | 否 |
| [hrsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt.md) | 是 |
| [hsin](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsin.md) | 是 |
| [hsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsqrt.md) | 是 |
| [htanh](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/htanh.md) | 是 |
| [htrunc](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/htrunc.md) | 是 |

#### bfloat16类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__heq](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__heq.md) | 否 |
| [__hequ](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hequ.md) | 否 |
| [__hge](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hge.md) | 否 |
| [__hgeu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hgeu.md) | 否 |
| [__hgt](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hgt.md) | 否 |
| [__hgtu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hgtu.md) | 否 |
| [__hisinf](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hisinf.md) | 否 |
| [__hisnan](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hisnan.md) | 否 |
| [__hle](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hle.md) | 否 |
| [__hleu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hleu.md) | 否 |
| [__hlt](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hlt.md) | 否 |
| [__hltu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hltu.md) | 否 |
| [__hmax](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmax.md) | 是 |
| [__hmax_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmax_nan.md) | 是 |
| [__hmin](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmin.md) | 是 |
| [__hmin_nan](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hmin_nan.md) | 是 |
| [__hne](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hne.md) | 否 |
| [__hneu](../SIMT-API/math_functions/bfloat16_type/bfloat16_compare_functions/__hneu.md) | 否 |

#### bfloat16类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habs](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs.md) | 是 |
| [__hadd](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd.md) | 否 |
| [__hdiv](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv.md) | 否 |
| [__hfma](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma.md) | 否 |
| [__hfma_relu](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hfma_relu.md) | 是 |
| [__hmul](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul.md) | 否 |
| [__hneg](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hneg.md) | 否 |
| [__hsub](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hsub.md) | 否 |

#### bfloat16类型精度转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__bfloat1622float2](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat1622float2.md) | 是 |
| [__bfloat162bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat162.md) | 是 |
| [__bfloat162bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rd.md) | 否 |
| [__bfloat162bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rn.md) | 否 |
| [__bfloat162bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rna.md) | 否 |
| [__bfloat162bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_ru.md) | 否 |
| [__bfloat162bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162bfloat16_rz.md) | 否 |
| [__bfloat162float](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162float.md) | 是 |
| [__bfloat162int_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rd.md) | 否 |
| [__bfloat162int_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rn.md) | 否 |
| [__bfloat162int_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rna.md) | 否 |
| [__bfloat162int_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_ru.md) | 否 |
| [__bfloat162int_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162int_rz.md) | 否 |
| [__bfloat162ll_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rd.md) | 是 |
| [__bfloat162ll_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rn.md) | 是 |
| [__bfloat162ll_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rna.md) | 是 |
| [__bfloat162ll_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_ru.md) | 是 |
| [__bfloat162ll_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ll_rz.md) | 是 |
| [__bfloat162uint_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rd.md) | 否 |
| [__bfloat162uint_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rn.md) | 否 |
| [__bfloat162uint_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rna.md) | 否 |
| [__bfloat162uint_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_ru.md) | 否 |
| [__bfloat162uint_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162uint_rz.md) | 否 |
| [__bfloat162ull_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rd.md) | 是 |
| [__bfloat162ull_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rn.md) | 是 |
| [__bfloat162ull_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rna.md) | 是 |
| [__bfloat162ull_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_ru.md) | 是 |
| [__bfloat162ull_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__bfloat162ull_rz.md) | 是 |
| [__float22bfloat162_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd.md) | 否 |
| [__float22bfloat162_rd_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rd_sat.md) | 否 |
| [__float22bfloat162_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn.md) | 否 |
| [__float22bfloat162_rn_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rn_sat.md) | 否 |
| [__float22bfloat162_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna.md) | 否 |
| [__float22bfloat162_rna_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rna_sat.md) | 否 |
| [__float22bfloat162_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru.md) | 否 |
| [__float22bfloat162_ru_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_ru_sat.md) | 否 |
| [__float22bfloat162_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz.md) | 否 |
| [__float22bfloat162_rz_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float22bfloat162_rz_sat.md) | 否 |
| [__float2bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16.md) | 否 |
| [__float2bfloat162_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat162_rn.md) | 是 |
| [__float2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd.md) | 否 |
| [__float2bfloat16_rd_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rd_sat.md) | 否 |
| [__float2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn.md) | 否 |
| [__float2bfloat16_rn_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rn_sat.md) | 否 |
| [__float2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna.md) | 否 |
| [__float2bfloat16_rna_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rna_sat.md) | 否 |
| [__float2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru.md) | 否 |
| [__float2bfloat16_ru_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_ru_sat.md) | 否 |
| [__float2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz.md) | 否 |
| [__float2bfloat16_rz_sat](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__float2bfloat16_rz_sat.md) | 否 |
| [__floats2bfloat162_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__floats2bfloat162_rn.md) | 是 |
| [__half2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rd.md) | 否 |
| [__half2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rn.md) | 否 |
| [__half2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rna.md) | 否 |
| [__half2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_ru.md) | 否 |
| [__half2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__half2bfloat16_rz.md) | 否 |
| [__halves2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__halves2bfloat162.md) | 是 |
| [__high2bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat16.md) | 否 |
| [__high2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2bfloat162.md) | 是 |
| [__high2float](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__high2float.md) | 否 |
| [__highs2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__highs2bfloat162.md) | 是 |
| [__int2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rd.md) | 否 |
| [__int2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rn.md) | 否 |
| [__int2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rna.md) | 否 |
| [__int2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_ru.md) | 否 |
| [__int2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__int2bfloat16_rz.md) | 否 |
| [__ll2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rd.md) | 是 |
| [__ll2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rn.md) | 是 |
| [__ll2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rna.md) | 是 |
| [__ll2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_ru.md) | 是 |
| [__ll2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ll2bfloat16_rz.md) | 是 |
| [__low2bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat16.md) | 否 |
| [__low2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2bfloat162.md) | 是 |
| [__low2float](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__low2float.md) | 否 |
| [__lowhigh2highlow](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lowhigh2highlow.md) | 是 |
| [__lows2bfloat162](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__lows2bfloat162.md) | 是 |
| [__uint2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rd.md) | 否 |
| [__uint2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rn.md) | 否 |
| [__uint2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rna.md) | 否 |
| [__uint2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_ru.md) | 否 |
| [__uint2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__uint2bfloat16_rz.md) | 否 |
| [__ull2bfloat16_rd](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rd.md) | 是 |
| [__ull2bfloat16_rn](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rn.md) | 是 |
| [__ull2bfloat16_rna](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rna.md) | 是 |
| [__ull2bfloat16_ru](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_ru.md) | 是 |
| [__ull2bfloat16_rz](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ull2bfloat16_rz.md) | 是 |
| [__ushort_as_bfloat16](../SIMT-API/math_functions/bfloat16_type/bfloat16_precision_conversion_functions/__ushort_as_bfloat16.md) | 是 |

### float类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [__fdividef](../SIMT-API/math_functions/float_math_functions/__fdividef.md) | 否 |
| [__saturatef](../SIMT-API/math_functions/float_math_functions/__saturatef.md) | 是 |
| [acosf](../SIMT-API/math_functions/float_math_functions/acosf.md) | 是 |
| [acoshf](../SIMT-API/math_functions/float_math_functions/acoshf.md) | 是 |
| [asinf](../SIMT-API/math_functions/float_math_functions/asinf.md) | 是 |
| [asinhf](../SIMT-API/math_functions/float_math_functions/asinhf.md) | 是 |
| [atan2f](../SIMT-API/math_functions/float_math_functions/atan2f.md) | 是 |
| [atanf](../SIMT-API/math_functions/float_math_functions/atanf.md) | 是 |
| [atanhf](../SIMT-API/math_functions/float_math_functions/atanhf.md) | 是 |
| [cbrtf](../SIMT-API/math_functions/float_math_functions/cbrtf.md) | 是 |
| [ceilf](../SIMT-API/math_functions/float_math_functions/ceilf.md) | 否 |
| [copysignf](../SIMT-API/math_functions/float_math_functions/copysignf.md) | 是 |
| [cosf](../SIMT-API/math_functions/float_math_functions/cosf.md) | 是 |
| [coshf](../SIMT-API/math_functions/float_math_functions/coshf.md) | 是 |
| [cospif](../SIMT-API/math_functions/float_math_functions/cospif.md) | 是 |
| [cyl_bessel_i0f](../SIMT-API/math_functions/float_math_functions/cyl_bessel_i0f.md) | 是 |
| [cyl_bessel_i1f](../SIMT-API/math_functions/float_math_functions/cyl_bessel_i1f.md) | 是 |
| [erfcf](../SIMT-API/math_functions/float_math_functions/erfcf.md) | 是 |
| [erfcinvf](../SIMT-API/math_functions/float_math_functions/erfcinvf.md) | 是 |
| [erfcxf](../SIMT-API/math_functions/float_math_functions/erfcxf.md) | 是 |
| [erff](../SIMT-API/math_functions/float_math_functions/erff.md) | 是 |
| [erfinvf](../SIMT-API/math_functions/float_math_functions/erfinvf.md) | 是 |
| [exp10f](../SIMT-API/math_functions/float_math_functions/exp10f.md) | 否 |
| [exp2f](../SIMT-API/math_functions/float_math_functions/exp2f.md) | 否 |
| [expf](../SIMT-API/math_functions/float_math_functions/expf.md) | 否 |
| [expm1f](../SIMT-API/math_functions/float_math_functions/expm1f.md) | 是 |
| [fabsf](../SIMT-API/math_functions/float_math_functions/fabsf.md) | 否 |
| [fdimf](../SIMT-API/math_functions/float_math_functions/fdimf.md) | 是 |
| [fdividef](../SIMT-API/math_functions/float_math_functions/fdividef.md) | 是 |
| [floorf](../SIMT-API/math_functions/float_math_functions/floorf.md) | 否 |
| [fmaf](../SIMT-API/math_functions/float_math_functions/fmaf.md) | 否 |
| [fmaxf](../SIMT-API/math_functions/float_math_functions/fmaxf.md) | 是 |
| [fminf](../SIMT-API/math_functions/float_math_functions/fminf.md) | 是 |
| [fmodf](../SIMT-API/math_functions/float_math_functions/fmodf.md) | 是 |
| [frexpf](../SIMT-API/math_functions/float_math_functions/frexpf.md) | 是 |
| [hypotf](../SIMT-API/math_functions/float_math_functions/hypotf.md) | 是 |
| [ilogbf](../SIMT-API/math_functions/float_math_functions/ilogbf.md) | 是 |
| [isfinite](../SIMT-API/math_functions/float_math_functions/isfinite.md) | 否 |
| [isinf](../SIMT-API/math_functions/float_math_functions/isinf.md) | 否 |
| [isnan](../SIMT-API/math_functions/float_math_functions/isnan.md) | 否 |
| [j0f](../SIMT-API/math_functions/float_math_functions/j0f.md) | 是 |
| [j1f](../SIMT-API/math_functions/float_math_functions/j1f.md) | 是 |
| [jnf](../SIMT-API/math_functions/float_math_functions/jnf.md) | 是 |
| [ldexpf](../SIMT-API/math_functions/float_math_functions/ldexpf.md) | 是 |
| [lgammaf](../SIMT-API/math_functions/float_math_functions/lgammaf.md) | 是 |
| [llrintf](../SIMT-API/math_functions/float_math_functions/llrintf.md) | 是 |
| [llroundf](../SIMT-API/math_functions/float_math_functions/llroundf.md) | 是 |
| [log10f](../SIMT-API/math_functions/float_math_functions/log10f.md) | 是 |
| [log1pf](../SIMT-API/math_functions/float_math_functions/log1pf.md) | 是 |
| [log2f](../SIMT-API/math_functions/float_math_functions/log2f.md) | 是 |
| [logbf](../SIMT-API/math_functions/float_math_functions/logbf.md) | 是 |
| [logf](../SIMT-API/math_functions/float_math_functions/logf.md) | 是 |
| [lrintf](../SIMT-API/math_functions/float_math_functions/lrintf.md) | 是 |
| [lroundf](../SIMT-API/math_functions/float_math_functions/lroundf.md) | 是 |
| [modff](../SIMT-API/math_functions/float_math_functions/modff.md) | 是 |
| [nearbyintf](../SIMT-API/math_functions/float_math_functions/nearbyintf.md) | 是 |
| [nextafterf](../SIMT-API/math_functions/float_math_functions/nextafterf.md) | 是 |
| [norm3df](../SIMT-API/math_functions/float_math_functions/norm3df.md) | 是 |
| [norm4df](../SIMT-API/math_functions/float_math_functions/norm4df.md) | 是 |
| [normcdff](../SIMT-API/math_functions/float_math_functions/normcdff.md) | 是 |
| [normcdfinvf](../SIMT-API/math_functions/float_math_functions/normcdfinvf.md) | 是 |
| [normf](../SIMT-API/math_functions/float_math_functions/normf.md) | 是 |
| [powf](../SIMT-API/math_functions/float_math_functions/powf.md) | 否 |
| [rcbrtf](../SIMT-API/math_functions/float_math_functions/rcbrtf.md) | 是 |
| [remainderf](../SIMT-API/math_functions/float_math_functions/remainderf.md) | 是 |
| [remquof](../SIMT-API/math_functions/float_math_functions/remquof.md) | 是 |
| [rhypotf](../SIMT-API/math_functions/float_math_functions/rhypotf.md) | 是 |
| [rintf](../SIMT-API/math_functions/float_math_functions/rintf.md) | 是 |
| [rnorm3df](../SIMT-API/math_functions/float_math_functions/rnorm3df.md) | 是 |
| [rnorm4df](../SIMT-API/math_functions/float_math_functions/rnorm4df.md) | 是 |
| [rnormf](../SIMT-API/math_functions/float_math_functions/rnormf.md) | 是 |
| [roundf](../SIMT-API/math_functions/float_math_functions/roundf.md) | 否 |
| [rsqrtf](../SIMT-API/math_functions/float_math_functions/rsqrtf.md) | 是 |
| [scalblnf](../SIMT-API/math_functions/float_math_functions/scalblnf.md) | 是 |
| [scalbnf](../SIMT-API/math_functions/float_math_functions/scalbnf.md) | 是 |
| [signbit](../SIMT-API/math_functions/float_math_functions/signbit.md) | 是 |
| [sincosf](../SIMT-API/math_functions/float_math_functions/sincosf.md) | 是 |
| [sincospif](../SIMT-API/math_functions/float_math_functions/sincospif.md) | 是 |
| [sinf](../SIMT-API/math_functions/float_math_functions/sinf.md) | 是 |
| [sinhf](../SIMT-API/math_functions/float_math_functions/sinhf.md) | 是 |
| [sinpif](../SIMT-API/math_functions/float_math_functions/sinpif.md) | 是 |
| [sqrtf](../SIMT-API/math_functions/float_math_functions/sqrtf.md) | 否 |
| [tanf](../SIMT-API/math_functions/float_math_functions/tanf.md) | 是 |
| [tanhf](../SIMT-API/math_functions/float_math_functions/tanhf.md) | 是 |
| [tanpif](../SIMT-API/math_functions/float_math_functions/tanpif.md) | 是 |
| [tgammaf](../SIMT-API/math_functions/float_math_functions/tgammaf.md) | 是 |
| [truncf](../SIMT-API/math_functions/float_math_functions/truncf.md) | 否 |
| [y0f](../SIMT-API/math_functions/float_math_functions/y0f.md) | 是 |
| [y1f](../SIMT-API/math_functions/float_math_functions/y1f.md) | 是 |
| [ynf](../SIMT-API/math_functions/float_math_functions/ynf.md) | 是 |

### fp8类型

#### fp8数据转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__asc_cvt_float2_to_fp8x2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__asc_cvt_float2_to_fp8x2.md) | 是 |
| [__e4m3x22float2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__e4m3x22float2.md) | 否 |
| [__e5m2x22float2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__e5m2x22float2.md) | 否 |
| [__float22hif82_rh](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rh.md) | 否 |
| [__float22hif82_rh_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rh_sat.md) | 否 |
| [__float22hif82_rna](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna.md) | 否 |
| [__float22hif82_rna_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__float22hif82_rna_sat.md) | 否 |
| [__half22hif82_rh](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh.md) | 否 |
| [__half22hif82_rh_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rh_sat.md) | 否 |
| [__half22hif82_rna](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna.md) | 否 |
| [__half22hif82_rna_sat](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__half22hif82_rna_sat.md) | 否 |
| [__hif822float2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__hif822float2.md) | 否 |
| [__hif822half2](../SIMT-API/math_functions/fp8_type/fp8_conversion_functions/__hif822half2.md) | 否 |

### half类型

#### half2类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [h2ceil](../SIMT-API/math_functions/half_type/half2_math_functions/h2ceil.md) | 是 |
| [h2cos](../SIMT-API/math_functions/half_type/half2_math_functions/h2cos.md) | 是 |
| [h2exp](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp.md) | 是 |
| [h2exp10](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp10.md) | 是 |
| [h2exp2](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp2.md) | 是 |
| [h2floor](../SIMT-API/math_functions/half_type/half2_math_functions/h2floor.md) | 是 |
| [h2log](../SIMT-API/math_functions/half_type/half2_math_functions/h2log.md) | 是 |
| [h2log10](../SIMT-API/math_functions/half_type/half2_math_functions/h2log10.md) | 是 |
| [h2log2](../SIMT-API/math_functions/half_type/half2_math_functions/h2log2.md) | 是 |
| [h2rcp](../SIMT-API/math_functions/half_type/half2_math_functions/h2rcp.md) | 是 |
| [h2rint](../SIMT-API/math_functions/half_type/half2_math_functions/h2rint.md) | 是 |
| [h2rsqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2rsqrt.md) | 是 |
| [h2sin](../SIMT-API/math_functions/half_type/half2_math_functions/h2sin.md) | 是 |
| [h2sqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2sqrt.md) | 否 |
| [h2tanh](../SIMT-API/math_functions/half_type/half2_math_functions/h2tanh.md) | 是 |
| [h2trunc](../SIMT-API/math_functions/half_type/half2_math_functions/h2trunc.md) | 是 |

#### half2类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__hbequx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbequx2.md) | 否 |
| [__hbeqx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbeqx2.md) | 否 |
| [__hbgeux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgeux2.md) | 否 |
| [__hbgex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgex2.md) | 否 |
| [__hbgtux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgtux2.md) | 否 |
| [__hbgtx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbgtx2.md) | 否 |
| [__hbleux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbleux2.md) | 否 |
| [__hblex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hblex2.md) | 否 |
| [__hbltux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbltux2.md) | 否 |
| [__hbltx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbltx2.md) | 否 |
| [__hbneux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbneux2.md) | 否 |
| [__hbnex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hbnex2.md) | 否 |
| [__hequx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hequx2.md) | 否 |
| [__hequx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hequx2_mask.md) | 否 |
| [__heqx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__heqx2.md) | 否 |
| [__heqx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__heqx2_mask.md) | 否 |
| [__hgeux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgeux2.md) | 否 |
| [__hgeux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgeux2_mask.md) | 否 |
| [__hgex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgex2.md) | 否 |
| [__hgex2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgex2_mask.md) | 否 |
| [__hgtux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtux2.md) | 否 |
| [__hgtux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtux2_mask.md) | 否 |
| [__hgtx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtx2.md) | 否 |
| [__hgtx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hgtx2_mask.md) | 否 |
| [__hleux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hleux2.md) | 否 |
| [__hleux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hleux2_mask.md) | 否 |
| [__hlex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hlex2.md) | 否 |
| [__hlex2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hlex2_mask.md) | 否 |
| [__hltux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltux2.md) | 否 |
| [__hltux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltux2_mask.md) | 否 |
| [__hltx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltx2.md) | 否 |
| [__hltx2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hltx2_mask.md) | 否 |
| [__hmaxx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hmaxx2.md) | 否 |
| [__hmaxx2_nan](../SIMT-API/math_functions/half_type/half2_compare_functions/__hmaxx2_nan.md) | 是 |
| [__hminx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hminx2.md) | 否 |
| [__hminx2_nan](../SIMT-API/math_functions/half_type/half2_compare_functions/__hminx2_nan.md) | 是 |
| [__hneux2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hneux2.md) | 否 |
| [__hneux2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hneux2_mask.md) | 否 |
| [__hnex2](../SIMT-API/math_functions/half_type/half2_compare_functions/__hnex2.md) | 否 |
| [__hnex2_mask](../SIMT-API/math_functions/half_type/half2_compare_functions/__hnex2_mask.md) | 否 |
| [__isnanx2](../SIMT-API/math_functions/half_type/half2_compare_functions/__isnanx2.md) | 否 |

#### half2类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habsx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__habsx2.md) | 否 |
| [__haddx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__haddx2.md) | 否 |
| [__hcmadd](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hcmadd.md) | 是 |
| [__hdivx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hdivx2.md) | 否 |
| [__hfmax2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hfmax2.md) | 否 |
| [__hfmax2_relu](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hfmax2_relu.md) | 是 |
| [__hmulx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hmulx2.md) | 否 |
| [__hnegx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hnegx2.md) | 否 |
| [__hsubx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hsubx2.md) | 否 |

#### half类型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [hceil](../SIMT-API/math_functions/half_type/half_math_functions/hceil.md) | 是 |
| [hcos](../SIMT-API/math_functions/half_type/half_math_functions/hcos.md) | 是 |
| [hexp](../SIMT-API/math_functions/half_type/half_math_functions/hexp.md) | 否 |
| [hexp10](../SIMT-API/math_functions/half_type/half_math_functions/hexp10.md) | 是 |
| [hexp2](../SIMT-API/math_functions/half_type/half_math_functions/hexp2.md) | 是 |
| [hfloor](../SIMT-API/math_functions/half_type/half_math_functions/hfloor.md) | 是 |
| [hlog](../SIMT-API/math_functions/half_type/half_math_functions/hlog.md) | 否 |
| [hlog10](../SIMT-API/math_functions/half_type/half_math_functions/hlog10.md) | 是 |
| [hlog2](../SIMT-API/math_functions/half_type/half_math_functions/hlog2.md) | 是 |
| [hrcp](../SIMT-API/math_functions/half_type/half_math_functions/hrcp.md) | 是 |
| [hrint](../SIMT-API/math_functions/half_type/half_math_functions/hrint.md) | 否 |
| [hrsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hrsqrt.md) | 是 |
| [hsin](../SIMT-API/math_functions/half_type/half_math_functions/hsin.md) | 是 |
| [hsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hsqrt.md) | 否 |
| [htanh](../SIMT-API/math_functions/half_type/half_math_functions/htanh.md) | 是 |
| [htrunc](../SIMT-API/math_functions/half_type/half_math_functions/htrunc.md) | 是 |

#### half类型比较函数

| API名 | API是否软仿 |
| --- | --- |
| [__heq](../SIMT-API/math_functions/half_type/half_compare_functions/__heq.md) | 否 |
| [__hequ](../SIMT-API/math_functions/half_type/half_compare_functions/__hequ.md) | 否 |
| [__hge](../SIMT-API/math_functions/half_type/half_compare_functions/__hge.md) | 否 |
| [__hgeu](../SIMT-API/math_functions/half_type/half_compare_functions/__hgeu.md) | 否 |
| [__hgt](../SIMT-API/math_functions/half_type/half_compare_functions/__hgt.md) | 否 |
| [__hgtu](../SIMT-API/math_functions/half_type/half_compare_functions/__hgtu.md) | 否 |
| [__hisinf](../SIMT-API/math_functions/half_type/half_compare_functions/__hisinf.md) | 否 |
| [__hisnan](../SIMT-API/math_functions/half_type/half_compare_functions/__hisnan.md) | 否 |
| [__hle](../SIMT-API/math_functions/half_type/half_compare_functions/__hle.md) | 否 |
| [__hleu](../SIMT-API/math_functions/half_type/half_compare_functions/__hleu.md) | 否 |
| [__hlt](../SIMT-API/math_functions/half_type/half_compare_functions/__hlt.md) | 否 |
| [__hltu](../SIMT-API/math_functions/half_type/half_compare_functions/__hltu.md) | 否 |
| [__hmax](../SIMT-API/math_functions/half_type/half_compare_functions/__hmax.md) | 是 |
| [__hmax_nan](../SIMT-API/math_functions/half_type/half_compare_functions/__hmax_nan.md) | 是 |
| [__hmin](../SIMT-API/math_functions/half_type/half_compare_functions/__hmin.md) | 是 |
| [__hmin_nan](../SIMT-API/math_functions/half_type/half_compare_functions/__hmin_nan.md) | 是 |
| [__hne](../SIMT-API/math_functions/half_type/half_compare_functions/__hne.md) | 否 |
| [__hneu](../SIMT-API/math_functions/half_type/half_compare_functions/__hneu.md) | 否 |

#### half类型算术函数

| API名 | API是否软仿 |
| --- | --- |
| [__habs](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__habs.md) | 是 |
| [__hadd](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hadd.md) | 否 |
| [__hdiv](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hdiv.md) | 否 |
| [__hfma](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma.md) | 否 |
| [__hfma_relu](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hfma_relu.md) | 是 |
| [__hmul](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hmul.md) | 否 |
| [__hneg](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hneg.md) | 否 |
| [__hsub](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hsub.md) | 否 |

#### half类型精度转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__bfloat162half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd.md) | 否 |
| [__bfloat162half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rd_sat.md) | 否 |
| [__bfloat162half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn.md) | 否 |
| [__bfloat162half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rn_sat.md) | 否 |
| [__bfloat162half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna.md) | 否 |
| [__bfloat162half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rna_sat.md) | 否 |
| [__bfloat162half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru.md) | 否 |
| [__bfloat162half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_ru_sat.md) | 否 |
| [__bfloat162half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz.md) | 否 |
| [__bfloat162half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__bfloat162half_rz_sat.md) | 否 |
| [__float22half2_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rd.md) | 否 |
| [__float22half2_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rd_sat.md) | 否 |
| [__float22half2_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rn.md) | 否 |
| [__float22half2_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rn_sat.md) | 否 |
| [__float22half2_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rna.md) | 否 |
| [__float22half2_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rna_sat.md) | 否 |
| [__float22half2_ro](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ro.md) | 否 |
| [__float22half2_ro_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ro_sat.md) | 否 |
| [__float22half2_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ru.md) | 否 |
| [__float22half2_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_ru_sat.md) | 否 |
| [__float22half2_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rz.md) | 否 |
| [__float22half2_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float22half2_rz_sat.md) | 否 |
| [__float2half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half.md) | 否 |
| [__float2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rd.md) | 否 |
| [__float2half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rd_sat.md) | 否 |
| [__float2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rn.md) | 否 |
| [__float2half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rn_sat.md) | 否 |
| [__float2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rna.md) | 否 |
| [__float2half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rna_sat.md) | 否 |
| [__float2half_ro](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ro.md) | 否 |
| [__float2half_ro_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ro_sat.md) | 否 |
| [__float2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ru.md) | 是 |
| [__float2half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_ru_sat.md) | 否 |
| [__float2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rz.md) | 否 |
| [__float2half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__float2half_rz_sat.md) | 否 |
| [__floats2half2_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__floats2half2_rn.md) | 是 |
| [__half22float2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half22float2.md) | 否 |
| [__half2float](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2float.md) | 是 |
| [__half2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rd.md) | 否 |
| [__half2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rn.md) | 否 |
| [__half2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rna.md) | 否 |
| [__half2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_ru.md) | 否 |
| [__half2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2half_rz.md) | 否 |
| [__half2int_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rd.md) | 否 |
| [__half2int_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rn.md) | 否 |
| [__half2int_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rna.md) | 否 |
| [__half2int_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_ru.md) | 否 |
| [__half2int_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2int_rz.md) | 否 |
| [__half2ll_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rd.md) | 是 |
| [__half2ll_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rn.md) | 是 |
| [__half2ll_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rna.md) | 是 |
| [__half2ll_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_ru.md) | 是 |
| [__half2ll_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ll_rz.md) | 是 |
| [__half2uint_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rd.md) | 否 |
| [__half2uint_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rn.md) | 否 |
| [__half2uint_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rna.md) | 否 |
| [__half2uint_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_ru.md) | 否 |
| [__half2uint_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rz.md) | 否 |
| [__half2ull_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rd.md) | 是 |
| [__half2ull_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rn.md) | 是 |
| [__half2ull_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rna.md) | 是 |
| [__half2ull_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_ru.md) | 是 |
| [__half2ull_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2ull_rz.md) | 是 |
| [__halves2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__halves2half2.md) | 是 |
| [__high2float](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__high2float.md) | 否 |
| [__high2half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__high2half.md) | 否 |
| [__high2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__high2half2.md) | 是 |
| [__highs2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__highs2half2.md) | 是 |
| [__int2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rd.md) | 否 |
| [__int2half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rd_sat.md) | 否 |
| [__int2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rn.md) | 否 |
| [__int2half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rn_sat.md) | 否 |
| [__int2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rna.md) | 否 |
| [__int2half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rna_sat.md) | 否 |
| [__int2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_ru.md) | 否 |
| [__int2half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_ru_sat.md) | 否 |
| [__int2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rz.md) | 否 |
| [__int2half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__int2half_rz_sat.md) | 否 |
| [__ll2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rd.md) | 是 |
| [__ll2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rn.md) | 是 |
| [__ll2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rna.md) | 是 |
| [__ll2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_ru.md) | 是 |
| [__ll2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ll2half_rz.md) | 是 |
| [__low2float](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__low2float.md) | 否 |
| [__low2half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__low2half.md) | 是 |
| [__low2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__low2half2.md) | 是 |
| [__lowhigh2highlow](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__lowhigh2highlow.md) | 是 |
| [__lows2half2](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__lows2half2.md) | 是 |
| [__uint2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rd.md) | 否 |
| [__uint2half_rd_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rd_sat.md) | 否 |
| [__uint2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rn.md) | 否 |
| [__uint2half_rn_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rn_sat.md) | 否 |
| [__uint2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rna.md) | 否 |
| [__uint2half_rna_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rna_sat.md) | 否 |
| [__uint2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_ru.md) | 否 |
| [__uint2half_ru_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_ru_sat.md) | 否 |
| [__uint2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rz.md) | 否 |
| [__uint2half_rz_sat](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rz_sat.md) | 否 |
| [__ull2half_rd](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rd.md) | 是 |
| [__ull2half_rn](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rn.md) | 是 |
| [__ull2half_rna](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rna.md) | 是 |
| [__ull2half_ru](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_ru.md) | 是 |
| [__ull2half_rz](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ull2half_rz.md) | 是 |
| [__ushort_as_half](../SIMT-API/math_functions/half_type/half_precision_conversion_functions/__ushort_as_half.md) | 是 |

### 数据类型转换

#### 类型转换函数

| API名 | API是否软仿 |
| --- | --- |
| [__float2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rd.md) | 否 |
| [__float2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rn.md) | 否 |
| [__float2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rna.md) | 否 |
| [__float2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_ru.md) | 否 |
| [__float2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2float_rz.md) | 否 |
| [__float2int_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rd.md) | 否 |
| [__float2int_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rn.md) | 否 |
| [__float2int_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rna.md) | 否 |
| [__float2int_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_ru.md) | 否 |
| [__float2int_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2int_rz.md) | 否 |
| [__float2ll_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rd.md) | 否 |
| [__float2ll_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rn.md) | 否 |
| [__float2ll_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rna.md) | 否 |
| [__float2ll_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_ru.md) | 否 |
| [__float2ll_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ll_rz.md) | 否 |
| [__float2uint_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rd.md) | 否 |
| [__float2uint_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rn.md) | 否 |
| [__float2uint_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rna.md) | 否 |
| [__float2uint_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_ru.md) | 否 |
| [__float2uint_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2uint_rz.md) | 否 |
| [__float2ull_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rd.md) | 否 |
| [__float2ull_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rn.md) | 否 |
| [__float2ull_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rna.md) | 否 |
| [__float2ull_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_ru.md) | 否 |
| [__float2ull_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float2ull_rz.md) | 否 |
| [__float_as_int](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float_as_int.md) | 是 |
| [__float_as_uint](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__float_as_uint.md) | 是 |
| [__int2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rd.md) | 否 |
| [__int2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rn.md) | 否 |
| [__int2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rna.md) | 否 |
| [__int2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_ru.md) | 否 |
| [__int2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int2float_rz.md) | 否 |
| [__int_as_float](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__int_as_float.md) | 是 |
| [__ll2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rd.md) | 否 |
| [__ll2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rn.md) | 否 |
| [__ll2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rna.md) | 否 |
| [__ll2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_ru.md) | 否 |
| [__ll2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ll2float_rz.md) | 否 |
| [__uint2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rd.md) | 否 |
| [__uint2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rn.md) | 否 |
| [__uint2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rna.md) | 否 |
| [__uint2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_ru.md) | 否 |
| [__uint2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint2float_rz.md) | 否 |
| [__uint_as_float](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__uint_as_float.md) | 是 |
| [__ull2float_rd](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rd.md) | 否 |
| [__ull2float_rn](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rn.md) | 否 |
| [__ull2float_rna](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rna.md) | 否 |
| [__ull2float_ru](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_ru.md) | 否 |
| [__ull2float_rz](../SIMT-API/math_functions/data_type_conversion/type_conversion_functions/__ull2float_rz.md) | 否 |

### 整型数学库函数

| API名 | API是否软仿 |
| --- | --- |
| [__brev](../SIMT-API/math_functions/integer_math_functions/__brev.md) | 否 |
| [__byte_perm](../SIMT-API/math_functions/integer_math_functions/__byte_perm.md) | 否 |
| [__clz](../SIMT-API/math_functions/integer_math_functions/__clz.md) | 否 |
| [__ffs](../SIMT-API/math_functions/integer_math_functions/__ffs.md) | 否 |
| [__hadd](../SIMT-API/math_functions/integer_math_functions/__hadd.md) | 否 |
| [__mul24](../SIMT-API/math_functions/integer_math_functions/__mul24.md) | 否 |
| [__mul64hi](../SIMT-API/math_functions/integer_math_functions/__mul64hi.md) | 否 |
| [__mul_i32toi64](../SIMT-API/math_functions/integer_math_functions/__mul_i32toi64.md) | 否 |
| [__mulhi](../SIMT-API/math_functions/integer_math_functions/__mulhi.md) | 否 |
| [__popc](../SIMT-API/math_functions/integer_math_functions/__popc.md) | 否 |
| [__rhadd](../SIMT-API/math_functions/integer_math_functions/__rhadd.md) | 否 |
| [__sad](../SIMT-API/math_functions/integer_math_functions/__sad.md) | 否 |
| [__uhadd](../SIMT-API/math_functions/integer_math_functions/__uhadd.md) | 否 |
| [__umul24](../SIMT-API/math_functions/integer_math_functions/__umul24.md) | 否 |
| [__umul64hi](../SIMT-API/math_functions/integer_math_functions/__umul64hi.md) | 否 |
| [__umulhi](../SIMT-API/math_functions/integer_math_functions/__umulhi.md) | 否 |
| [__urhadd](../SIMT-API/math_functions/integer_math_functions/__urhadd.md) | 否 |
| [__usad](../SIMT-API/math_functions/integer_math_functions/__usad.md) | 否 |
| [labs](../SIMT-API/math_functions/integer_math_functions/labs.md) | 是 |
| [llabs](../SIMT-API/math_functions/integer_math_functions/llabs.md) | 是 |
| [llmax](../SIMT-API/math_functions/integer_math_functions/llmax.md) | 是 |
| [llmin](../SIMT-API/math_functions/integer_math_functions/llmin.md) | 是 |
| [max](../SIMT-API/math_functions/integer_math_functions/max.md) | 否 |
| [min](../SIMT-API/math_functions/integer_math_functions/min.md) | 否 |
| [ullmax](../SIMT-API/math_functions/integer_math_functions/ullmax.md) | 是 |
| [ullmin](../SIMT-API/math_functions/integer_math_functions/ullmin.md) | 是 |
| [umax](../SIMT-API/math_functions/integer_math_functions/umax.md) | 否 |
| [umin](../SIMT-API/math_functions/integer_math_functions/umin.md) | 否 |

## 访存函数

| API名 | API是否软仿 |
| --- | --- |
| [asc_dcci_entire](../SIMT-API/memory_access_functions/asc_dcci_entire.md) | 否 |
| [asc_dcci_single](../SIMT-API/memory_access_functions/asc_dcci_single.md) | 否 |
| [asc_ldca](../SIMT-API/memory_access_functions/asc_ldca.md) | 否 |
| [asc_ldcg](../SIMT-API/memory_access_functions/asc_ldcg.md) | 否 |
| [asc_stcg](../SIMT-API/memory_access_functions/asc_stcg.md) | 否 |
| [asc_stwt](../SIMT-API/memory_access_functions/asc_stwt.md) | 否 |
