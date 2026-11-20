# Memory Vector Computation Programming<a name="ZH-CN_TOPIC_0000002600000002"></a>

<!-- md-trans-meta sourceCommit=fea3eb525c3418cb979b51bf12361b10de4163fd translatedAt=2026-11-09T11:05:26.788Z pushedAt=2026-11-19T06:32:19.962Z nl=2 -->

This document describes the Ascend C vector computation development method based on the Unified Buffer (UB). This computation capability, abbreviated as Memory vector computation programming, is the most basic and most widely used vector programming method in Ascend C. [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) natively supports the complete UB direct programming capability. [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) introduces the register computation capability, and Reg vector computation programming is recommended for higher performance.

## Basic Steps for Memory Vector Programming

The standard data flow of Memory vector computation consists of three stages: data movement in, computation, and data movement out. The complete process is as follows:

1. Move data from global memory to local memory through DMA.
2. Perform vector computation.
3. After the computation is complete, move the result from local memory back to global memory through DMA.

## Memory Vector Computation Memory Management

Memory vector computation relies on the UB in local memory to perform operations. Developers must first move the input data to the UB and then call vector computation instructions to execute the computation. The UB supports declaration in the form of arrays, which can be statically or dynamically allocated.

- Static allocation: allocates a fixed-size memory space. The space size is determined at the compilation stage and cannot be modified at runtime.

    ```c
    __global__ __vector__ void add_custom(...)
    {
        constexpr uint32_t ub_size = 256;
        __ubuf__ half static_buffer[ub_size];  // Compiler automatically aligns to 32 bytes
    }
    ```

- Dynamic allocation: you need to specify the size of the dynamic memory through the dynamic_ub_size parameter in <<<>>>, and its size is determined at runtime.

    ```c
    // Device-side: declare dynamic shared memory
    __global__ __vector__ void add_custom(...)
    {
        extern __ubuf__ half dynamic_buf[];
        // ...
    }

    // Host-side: specify dynamic shared memory size via third parameter in <<<>>>
    int32_t main(int argc, char const *argv[])
    {
        // ...
        uint32_t dyn_ubuf_size = 1024 * sizeof(char);
        add_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(...);
        // ...
    }
    ```

## Memory Data Movement

Ascend NPUs provide multiple sets of dedicated APIs to implement data movement between global memory (GM) and the UB, adapting to various business scenarios. To ensure vector computation efficiency, the standard process is as follows: data is moved from GM to the UB for computation, and the computation result is then written back from the UB to GM, forming a complete data link.
Developers can flexibly configure parameters to precisely control attributes such as the number of data blocks, length, and address stride, implementing contiguous data movement and non-contiguous (high-dimensional sharding) data movement. This fully leverages the synergy between hardware movement and computation to improve the overall execution efficiency of vector operators.

<!-- npu="950" id1 -->
This section uses [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) as an example to describe the data movement APIs.

<table>
<thead>
<tr><th>Data Path</th><th>Movement Mode</th><th>API</th><th>Scenario</th></tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Global Memory -> UB</td>
<td>Contiguous movement</td>
<td><code>asc_copy_gm2ub(dst, src, size)</code><br><code>asc_copy_gm2ub_align(dst, src, size)</code></td>
<td>Movement of a single segment of contiguous data, with a length that may not be 32B-aligned</td>
</tr>
<tr>
<td>High-dimensional sharding movement</td>
<td><code>asc_copy_gm2ub(dst, src, burst_count, burst_len, src_stride, dst_stride)</code><br><code>asc_copy_gm2ub_align(dst, src, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, l2_cache_mode, src_stride, dst_stride)</code></td>
<td>Moves multiple data blocks, with configurable block count, length, and first-address stride. The <code>_align</code> APIs additionally allow configuring left/right padding, padding mode, and L2 cache policy.</td>
</tr>
<tr>
<td rowspan="2">UB -> Global Memory</td>
<td>Contiguous movement</td>
<td><code>asc_copy_ub2gm(dst, src, size)</code><br><code>asc_copy_ub2gm_align(dst, src, size)</code></td>
<td>Moves a single contiguous data segment, whose length may not be 32B-aligned.</td>
</tr>
<tr>
<td>High-dimensional sharding movement</td>
<td><code>asc_copy_ub2gm(dst, src, burst_count, burst_len, dst_stride, src_stride)</code><br><code>asc_copy_ub2gm_align(dst, src, burst_count, burst_len, l2_cache_mode, dst_stride, src_stride)</code></td>
<td>Moves multiple data blocks, with configurable block count, length, and first-address stride. The <code>_align</code> APIs additionally allow configuring the L2 cache policy.</td>
</tr>
</tbody>
</table>

All the preceding APIs require the UB start address to be 32B-aligned, and the movement length must meet the byte-alignment requirement of the data type. When the length is not an integer multiple of 32B, moving data from GM to UB writes padding data into the destination UB, so space must be reserved for the padded write range. Moving data from UB to GM reads the source UB over a 32B-aligned range, so a valid space must be reserved for the read range, but only the valid data is written to GM.

**Contiguous movement**: To move data from contiguous addresses in GM to UB, you only need to specify the byte length of the data to be moved. The following figure moves 256B of contiguous data. Taking the half type as an example, there are 128 elements in total. You can call the `asc_copy_gm2ub` or `asc_copy_gm2ub_align` API. Choose either of the following two calling methods:

<img src="../../../../../api/figures/asc_copy_gm2ub_continuous.png" alt="Schematic diagram of contiguous movement" width="800"/>

```c
uint32_t size = 128 * sizeof(half);
// Option 1
asc_copy_gm2ub(dst_ub, src_gm, size);
// Option 2
asc_copy_gm2ub_align(dst_ub, src_gm, size);
```

>[!NOTE] Note
>These contiguous movement APIs are mainly intended to improve the ease of data movement for users. They are provided at the software layer and implemented through the high-dimensional sharding APIs at the underlying layer.

**High-dimensional sharding movement**: To move multiple non-contiguous data segments from GM to UB, you can use the high-dimensional sharding mode to configure parameters such as the number of data blocks, the length of a single block, and the stride between blocks. The following figure moves two data blocks, each 96B, with a 32B stride between the two data blocks in GM and a 64B stride in UB. A total of 192B of valid data is moved, and the destination UB must reserve at least 256B of space.

<img src="../../../../../api/figures/asc_copy_gm2ub_highdim_split.png" alt="Schematic diagram of high-dimensional sharding movement" width="800"/>

`burst_len`, `src_stride`, and `dst_stride` are all in bytes. The stride indicates the address stride between the start addresses of adjacent data blocks. The parameters corresponding to the preceding figure are as follows:

```c
uint16_t burst_count = 2;
uint16_t burst_len = 96;
// Source burst starts are 96B + 32B apart.
uint16_t src_stride = 96 + 32;
// Destination burst starts are 96B + 64B apart.
uint16_t dst_stride = 96 + 64;

asc_copy_gm2ub(dst_ub, src_gm, burst_count, burst_len, src_stride, dst_stride);
```

**Alignment padding and fill value configuration for high-dimensional sharding movement**: Alignment padding is used to meet the 32B alignment requirement on the UB side, and fill value configuration is used to specify the value written to the padding area. Common APIs pad data that is not 32B-aligned in the default manner. When you need to explicitly configure the left/right padding or the fill mode, you can use the high-dimensional sharding prototype of `asc_copy_gm2ub_align`. The UB start address must be 32B-aligned, and the movement length must meet the byte alignment requirement of the data type.

Taking the movement of three data blocks each 48B long as an example, set both `left_padding_num` and `right_padding_num` to 0, and select the padding mode through `dst_stride`:

In Compact mode, set `dst_stride` to `burst_len`, that is, 48B. The start offsets of the three data blocks in UB are 0B, 48B, and 96B in sequence. Only 16B is padded at the end of the 144B valid data, and the destination UB must reserve at least 160B of space.

<img src="../../../../../api/figures/asc_copy_gm2ub_align_compact_padding.png" alt="Schematic diagram of 32-byte padding in Compact mode" width="800"/>

In Normal mode, set `dst_stride` to a value that is not equal to `burst_len` and satisfies 32B alignment, which is 64B in this example. The start offsets of the three data blocks in UB are 0B, 64B, and 128B in sequence, and each data block is padded with 16B. The destination UB must reserve at least 192B of space.

<img src="../../../../../api/figures/asc_copy_gm2ub_align_normal_padding.png" alt="Schematic diagram of 32-byte padding in Normal mode" width="800"/>

In the following example, `src_gm`, `dst_ub_compact`, and `dst_ub_normal` are all half-type pointers, and the two destination UBs reserve the space described above respectively. The source data is arranged contiguously, and `src_stride` is 48B. By setting `enable_constant_pad` to `true`, the padding areas in the figure are filled with half-type 0:

```c
uint32_t burst_count = 3;
uint32_t burst_len = 48;
uint64_t src_stride = 48;
uint8_t left_padding_num = 0;
uint8_t right_padding_num = 0;

asc_set_copy_pad_val(static_cast<half>(0));
// Compact: pad only the end of all three bursts, using at least 160B of UB.
asc_copy_gm2ub_align(dst_ub_compact, src_gm, burst_count, burst_len,
                    left_padding_num, right_padding_num, true,
                    asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, src_stride, 48);
// Normal: pad each burst separately, using at least 192B of UB.
asc_copy_gm2ub_align(dst_ub_normal, src_gm, burst_count, burst_len,
                    left_padding_num, right_padding_num, true,
                    asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, src_stride, 64);
```

>[!NOTE] Note
>Compact mode requires both the left and right padding to be 0. Normal mode supports explicitly configuring the left and right padding. When both the left and right padding are 0, `enable_constant_pad` is used to select the filling method for the padding area: when it is `true`, the value configured by `asc_set_copy_pad_val` is used; when it is `false`, the hardware automatically fills dummy data. When either the left or right padding is non-zero, `enable_constant_pad` does not take effect, and the padding value must be configured in advance through [asc_set_copy_pad_val](../../../../../api/SIMD-API/c_api/vector_datamove/asc_set_copy_pad_val.md). The left and right padding areas and the area on the right that is further padded for 32B alignment all use this value.

The above describes the logic of moving data from GM to UB. Moving data from UB to GM can be implemented using APIs of the same series. For more information about movement APIs, see [Vector Data Movement](../../../../../api/SIMD-API/c_api/vector_datamove/vector_datamove.md).

<!-- end id1 -->

## Memory Data Computation

Memory vector computation includes two modes: contiguous computation and high-dimensional sharding computation. It also supports the mask function, which flexibly controls the data range involved in computation.
The following uses the addition API as an example and combines schematic diagrams to explain the differences between the two modes: contiguous computation is simple to use and is suitable for contiguous data operations on one-dimensional tensors; high-dimensional sharding computation is more flexible and supports iterative operations and custom address strides. Developers can select the appropriate mode based on their business scenarios.

<img src="../../../../figures/mem_vec_c.png" alt="Schematic diagram of Memory vector computation modes" width="800"/>

### High-Dimensional Sharding Computation

The high-dimensional sharding capability mainly supports setting parameters such as `repeat_time`, `stride`, and `mask` for computation APIs, enabling precise control over the iterative execution of instructions, the address stride of operands, and the mask within each iteration.

#### Iteration Control

In a single iteration, the vector computation unit reads eight contiguous DataBlocks (32 bytes each) from UB, and writes the operation results to the eight corresponding DataBlocks in the destination UB.

<img src="../../../../figures/db_iter_c.png" alt="Schematic diagram of DataBlock iteration" width="800"/>

If repeat_time (number of iterations) is set to 2, the unit executes two iterations, and the total amount of data processed is 2 × 8 × 32 bytes = 512 bytes. If the data type is half (2 bytes/element), this corresponds to processing 256 elements.

> 📌 Hardware constraint: repeat_time ranges from 1 to 255. This constraint applies to all products of [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) and [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).

<img src="../../../../figures/iter2_exp_c.png" alt="Exp computation with 2 iterations" width="800"/>

#### Address Stride Configuration

The vector computation unit also supports vector computation with strides, which is configured through `data_block_stride` (the address stride between different DataBlocks within a single iteration) and `repeat_stride` (the address stride of the same DataBlock between adjacent iterations). `data_block_stride` refers to the address stride between different DataBlocks within a single iteration. Its value cannot exceed the UB size limit.
For contiguous computation, `data_block_stride` is set to 1, and the 8 DataBlocks within the same iteration are processed contiguously.
For non-contiguous computation, `data_block_stride` is greater than 1 (for example, 2), and a gap of one DataBlock appears between different DataBlocks within the same iteration when data is read, as shown in the following figure.

<img src="../../../../figures/db_stride_c.png" alt="data_block_stride example" width="800"/>

> 📌 If dst_block_stride == 0, it is equivalent to dst_block_stride = 1. If src_block_stride = 0, the source operand always reuses the first DataBlock.

The following figure shows an example of reading and writing the source operand and destination operand in the UB space within a single iteration. In this example, `data_block_stride` of the source operand is set to 2, indicating that the address stride between different DataBlocks within a single iteration is 2 DataBlocks. `data_block_stride` of the destination operand is set to 1, indicating that the addresses are contiguous within a single iteration.

<img src="../../../../figures/iter_rw_c.png" alt="Example of reading and writing source and destination operands within a single iteration" width="800"/>

Here, **repeat_stride** refers to the address stride of the same DataBlock between adjacent iterations. You can flexibly control different scenarios by setting `repeat_stride`.

| Scenario | repeat_stride Value | Data Read Characteristics | Applicable Scenario |
|------|------------------|-------------|--------|
| Continuous computation | 8 | Each iteration reads 8 consecutive data_blocks to complete the computation of all input data | <img src="../../../../figures/rep_cont_c.png" alt="Contiguous computation scenario with repeat_stride" width="600"/> |
| Non-contiguous computation | >8 (for example, 10) | There is a data_block gap between adjacent iterations, and the addresses are non-contiguous | <img src="../../../../figures/rep_discont_c.png" alt="Non-contiguous computation scenario with repeat_stride" width="600"/> |
| Repeated computation | 0 | The first 8 consecutive data_blocks are read and computed repeatedly | <img src="../../../../figures/rep_repeat_c.png" alt="Repeated computation scenario with repeat_stride" width="600"/> |
| Partially repeated computation | (0, 8) | Part of the data is read repeatedly between adjacent iterations, which is generally not involved in common scenarios | <img src="../../../../figures/rep_partial_c.png" alt="Partially repeated computation with repeat_stride" width="600"/> |

When `repeat_time`>1, the computation is completed through multiple iterations. `repeat_stride` indicates the start address interval (in dataBlocks) between DataBlocks at the same position in adjacent iterations. For example, when `repeat_stride` = 9, the first DataBlock of the first iteration and the first DataBlock of the second iteration are 9 dataBlocks apart.

<img src="../../../../figures/multi_iter_c.png" alt="Schematic diagram of non-contiguous multi-iteration scenario" width="800"/>

Configure `repeat_stride` and `data_block_stride` through the C-style API to implement the multi-iteration functionality described above.

```c
// In the above example with repeat interval of 9, the following configuration achieves the corresponding operation
uint8_t repeat_time = 1;
uint8_t dst_block_stride = 1;
uint8_t src0_block_stride = 1;
uint8_t src1_block_stride = 1;
uint8_t dst_repeat_stride = 9;
uint8_t src0_repeat_stride = 9;
uint8_t src1_repeat_stride = 9;

asc_add(z, x, y, repeat_time, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride, src0_repeat_stride, src1_repeat_stride);
```

#### Mask Setting

For data within the same iteration, you can use the mask parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows the schematic diagram of controlling which elements participate in the computation bit by bit using the mask bit-by-bit mode during Abs computation, where 1 indicates participation in the computation and 0 indicates non-participation.

<img src="../../../../figures/mask_op_c.png" alt="Schematic diagram of mask operation" width="800"/>

Each mask bit corresponds to the position of an element in the data. Valid bits and invalid bits are used to mark elements, enabling fine-grained on/off control over data operations.
The mask is a fixed-width value with the following rules: a valid bit (value 1) indicates that the corresponding element participates in the computation, and an invalid bit (value 0) indicates that the corresponding element is masked and does not participate in the operation.

Mask configuration provides two modes: Counter mode and Normal mode. Normal mode further includes contiguous mode and bit-by-bit mode.
Counter mode applies to scenarios involving computation on contiguous elements. In this mode, only the lower 64 bits of the mask register are valid and are used to indicate the total number of elements participating in the computation. The API automatically ignores repeat_time, and the system automatically matches the number of iterations based on the number of elements. For example, when computing 200 half-type elements, due to the 32B alignment constraint of UB, 256 elements of space must be requested. In this case, using Counter mode can precisely process the actual number of elements and avoid redundant computation.

<img src="../../../../figures/count_mode.png" alt="Counter mode computation" width="600"/>

```c
__ubuf__ half dst_ub[256];
__ubuf__ half src0_ub[256];
__ubuf__ half src1_ub[256];
half scalar_value = 1.0;
// 1. Direct transfer processing (temporarily omitted)
// 2. Use high-dimensional segmentation API for computation
// 2.1 Set mask counter mode
asc_set_mask_count();
// 2.2 Set Mask value to 200, indicating this computation processes 200 elements
asc_set_vector_mask(0, static_cast<uint64_t>(200)); 
asc_add_scalar(dst_ub, src0_ub, scalar_value, 1, 1, 1, 1, 8, 8, 8);

// 3. Default needs to reset to Normal mode, if subsequent still requires Counter mode then no need to reset to Normal mode
asc_set_mask_normal();
```

In Normal computation mode (the default mode; no reconfiguration is required if no mode switch occurs), the MASK register is used as the mask for each Repeat. The mask parameter can be set through `asc_set_vector_mask` to control the number of elements participating in the computation within a single iteration. Only high-dimensional sharding supports this mode, which is distinguished at the API level into contiguous mode within a single iteration and bit-by-bit mode within a single iteration.

- Continuous mode within a single iteration: indicates that the first n consecutive elements participate in the computation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration.

    > **Mask range derivation**: The Vector compute unit processes a fixed 32B x 8 block = 256B of data in a single iteration. Therefore, the maximum mask value = 256B / sizeof(DataType). When the operand is 16-bit (sizeof = 2B), mask is in [0, 128] (256/2 = 128). When the operand is 32-bit (sizeof = 4B), mask is in [0, 64] (256/4 = 64). When the operand is 64-bit (sizeof = 8B), mask is in [0, 32] (256/8 = 32). This derivation rule can be generalized to other data types (for example, bfloat16 = 16 bit -> mask upper limit 128, fp32 = 32 bit -> mask upper limit 64).

    In contiguous mode, when the mask exceeds the range, it is computed based on the maximum mask value of the corresponding data type. mask = 0 indicates that the instruction does not perform the computation operation, and the API is treated as a NOP (no operation). Setting a negative value may produce undefined behavior.

    In this mode, you can specify that the first n contiguous elements within a single iteration participate in the computation, while other data does not participate. For example, for src data (shape(256), data type half), the data needs to be computed through 2 iterations (128 elements per iteration). By setting mask = 100, the first iteration computes elements in the range [0, 99], and the second iteration computes elements in the range [128, 227], while other elements do not participate in the computation.

    <img src="../../../../figures/iter_cont.png" alt="contiguous computation within a single iteration" width="600"/>

    ```c
    __ubuf__ half dst_ub[256];
    __ubuf__ half src0_ub[256];
    __ubuf__ half src1_ub[256];
    half scalar_value = 1.0;

    // 1. Data transfer (temporarily omitted)
    // 2. Use high-dimensional segmentation API for computation
    // 2.1 Set mask Normal mode
    asc_set_mask_normal();
    // 2.2 Set Mask value to 100, indicating single iteration computes first 100 elements
    asc_set_vector_mask(0, static_cast<uint64_t>(100)); 
    asc_adds(dst_ub, src0_ub, scalar_value, 2, 1, 1, 1, 8, 8, 8);
    ```

- Bit-by-bit mode within a single iteration: You can control which elements participate in computation on a bit-by-bit basis. A bit value of 1 indicates participation in computation, and 0 indicates non-participation. It is divided into maskHigh (high-order mask) and maskLow (low-order mask). The value range of the parameters is related to the data type of the operand. Different data types allow different maximum numbers of elements to be processed within each iteration. When the operand is 16-bit, maskLow and maskHigh ∈ [0, 264-1], and they cannot both be 0. When the operand is 32-bit, maskHigh is 0 and maskLow ∈ (0, 264-1]. When the operand is 64-bit, maskHigh is 0 and maskLow ∈ (0, 232-1].

    In this mode, you can specify the values of maskHigh and maskLow to handle various mask operations. For interleaved computation, set mask to a pattern such as 01010101... or 10101010....
    Taking src data (with a shape of 256 and a data type of half) as an example, by setting a bit-by-bit mask, you can select the values of specific bits to participate in computation within a single iteration:

    <img src="../../../../figures/iter_bit.png" alt="bit-by-bit computation within a single iteration" width="600"/>

    ```c
    __ubuf__ half dst_ub[256];
    __ubuf__ half src0_ub[256];
    __ubuf__ half src1_ub[256];

    // 1. Data transfer (temporarily omitted)
    // 2. Use high-dimensional segmentation API for computation
    // 2.1 Set mask Normal mode
    asc_set_mask_normal();
    // 2.2 Set Mask to bit-wise mode (0101...), implement interleaved computation
    asc_set_vector_mask(0x000000000000AAAA, 0x0000000000000000); 
    asc_add(dst_ub, src0_ub, src1_ub, 2, 1, 1, 1, 8, 8, 8);
    ```

### Contiguous Computation

Contiguous computation is a simplified API provided by the software layer. When using it, you do not need to configure a mask. You can directly perform operations on N contiguous data elements in the source operand and write the results contiguously to the destination operand. It is mainly used for batch computation of one-dimensional data.
Taking addition as an example, to perform addition on a vector of length 4096, call it as follows:

```c
asc_add(z, x, y, 4096); // 4096: number of consecutive elements involved in addition.
```

## Synchronization Control for Memory Vector Computation

The MTE2 movement unit, vector computation unit, and other units inside the AI Core run asynchronously in parallel. When multiple units read and write the same storage resource, data dependencies arise. Therefore, Memory vector computation must rely on pipeline synchronization APIs to ensure normal execution. The Memory vector computation process is simpler than cube computation and is divided into three stages: data movement in, computation, and data movement out, corresponding to the pipelines PIPE_MTE2, PIPE_V, and PIPE_MTE3, respectively. The execution order must be constrained through intra-core synchronization APIs to ensure serial execution. In products with [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), you can use the `asc_lock` and `asc_unlock` APIs to implement synchronization control of the three stages.

For the synchronization principles of `asc_lock`/`asc_unlock`, as well as data dependencies and Buffer reuse in double-buffering scenarios, see [Implementing Multi-Pipeline Synchronization Based on Mutex](../../../advanced_programming/advanced_kernel_programming/intra_core_sync/mutex_multi_pipeline_sync.md).

The following is a complete example of vector addition:

```c
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    uint8_t mutex_id = 1;
    // ...
    // 1. Step 1: Data transfer in, execution pipeline is PIPE_MTE2
    asc_lock(PIPE_MTE2, mutex_id);
    asc_copy_gm2ub_align(x_local, x_gm, 1, 48 * sizeof(half), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_copy_gm2ub_align(y_local, y_gm, 1, 48 * sizeof(half), 0, 0, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_unlock(PIPE_MTE2, mutex_id);

    // 2. Step 2: Compute, execution pipeline is PIPE_V
    asc_lock(PIPE_V, mutex_id);
    asc_add(z, x, y, 4096);
    asc_unlock(PIPE_V, mutex_id);
    
    // 3. Step 3: Data transfer out, execution pipeline is PIPE_MTE3
    asc_lock(PIPE_MTE3, mutex_id);
    asc_copy_ub2gm_align(z_gm, z_local, 1, 48 * sizeof(int8_t), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
    asc_unlock(PIPE_MTE3, mutex_id);
}
```

## Memory Vector Computation Example

By combining the data movement, computation, and synchronization capabilities described above, you can develop a complete memory vector computation operator based on the C language. For a complete project example, see the [c_api_add sample](../../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add).
