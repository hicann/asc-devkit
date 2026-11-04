# Memory Vector Computation Programming<a name="ZH-CN_TOPIC_0000002600000002"></a>

<!-- md-trans-meta sourceCommit=462c9a712209ad24f2d57d4b5f5dd0d416050b06 translatedAt=2026-10-28T08:43:41.316Z pushedAt=2026-11-03T07:08:03.555Z nl=2 -->

This document describes the Ascend C vector computation development method based on Unified Buffer (UB). This computation capability, referred to as Memory vector computation programming, is the most fundamental and widely used vector programming approach in Ascend C. [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) natively supports the complete UB direct programming capability. [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) introduces register computation capability, and Reg vector computation programming is recommended for higher performance.

## Basic Steps of Memory Vector Programming

The standard data flow of Memory vector computation is divided into three stages: data movement in, computation, and data movement out. The complete flow is as follows:

1. Move data from global memory to local memory through DMA;
2. Perform vector computation;
3. After computation is complete, move the results from local memory back to global memory (Global Memory) through DMA.

## Memory Vector Computation Memory Management

Memory vector computation relies on the UB in local memory to perform operations. Developers must first move the input data to the UB and then call vector computation instructions to execute the computation. The UB supports declaration in array form, which is divided into two methods: static allocation and dynamic allocation.

- Static allocation: allocates a fixed-size memory space. The space size is determined at the compilation stage and cannot be modified at runtime.

    ```c
    __global__ __vector__ void add_custom(...)
    {
        constexpr uint32_t ub_size = 256;
        __ubuf__ half static_buffer[ub_size];  // Compiler automatically aligns to 32 bytes
    }
    ```

- Dynamic allocation: you need to specify the size of the dynamic memory space through the dynamic_ub_size parameter in <<<>>>, and its size is determined at runtime.

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

Ascend NPUs provide multiple dedicated APIs for moving data between Global Memory (GM) and Unified Buffer (UB), adapting to various business scenarios. To ensure vector computation efficiency, the standard process is: data is moved from GM to UB for computation, and the computation results are then written back from UB to GM, forming a complete data link.
Developers can flexibly configure parameters to precisely control attributes such as the number of data blocks, length, and address stride, implementing continuous data movement and non-continuous (high-dimensional partitioning) data movement, fully leveraging the synergy between hardware movement and computation to improve the overall execution efficiency of vector operators.

<table>
<thead>
<tr><th>Data Path</th><th>Movement Mode</th><th>API</th><th>Applicable Scenario</th></tr>
</thead>
<tbody>
<tr>
<td rowspan="3">Global Memory -> UB</td>
<td>Continuous movement</td>
<td><code>asc_copy_gm2ub(dst, src, size)</code></td>
<td>One-dimensional data movement of continuous addresses</td>
</tr>
<tr>
<td>Non-continuous movement</td>
<td><code>asc_copy_gm2ub(dst, src, burst_count, burst_len, src_gap, dst_gap)</code></td>
<td>Non-continuous multi-block data movement, with configurable block count and stride</td>
</tr>
<tr>
<td>Unaligned movement</td>
<td><code>asc_copy_gm2ub_align(dst, src, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, l2_cache_mode, src_stride, dst_stride)</code></td>
<td>Address not 32B aligned, supports custom padding</td>
</tr>
<tr>
<td rowspan="3">UB -> Global Memory</td>
<td>Continuous movement</td>
<td><code>asc_copy_ub2gm(dst, src, size)</code></td>
<td>One-dimensional data movement with continuous addresses</td>
</tr>
<tr>
<td>Non-continuous movement</td>
<td><code>asc_copy_ub2gm(dst, src, burst_count, burst_len, src_gap, dst_gap)</code></td>
<td>Moves multiple non-continuous data blocks, with configurable block count and stride</td>
</tr>
<tr>
<td>Unaligned</td>
<td><code>asc_copy_ub2gm_align(dst, src, burst_count, burst_len, l2_cache_mode, dst_stride, src_stride)</code></td>
<td>Addresses are not 32B-aligned</td>
</tr>
</tbody>
</table>

Based on the vector movement capabilities described above, developers can select the appropriate API for different scenarios.

**Continuous movement**: To move data at continuous addresses in GM to UB, you only need to specify the byte length of the data to be moved. For example, to move the src_gm data with a shape of 256 and a data type of half into UB, you can directly call the `asc_copy_gm2ub` API:

<img src="../../../../figures/cont_copy.png" alt="Continuous data movement" width="800"/>

```c
uint32_t size = 256 * sizeof(half);
asc_copy_gm2ub(dst_ub, src_gm, size);
```

> 📌 This API is mainly intended to improve the ease of data movement for users. It is provided at the software layer and implemented at the underlying layer through the high-dimensional partitioning API.

**Non-continuous movement**: To move multiple non-continuous data segments in GM into the same UB space, you can use the high-dimensional partitioning mode to configure parameters such as the number of data blocks, the length of a single block, and the stride between blocks. Taking src_gm with a shape of 256 and a data type of half as an example, the corresponding movement logic and code implementation are as follows:


<img src="../../../../figures/discont_copy.png" alt="Non-continuous movement" width="800"/>

```c
// Number of consecutive data blocks: 16 rows of data, transfer every alternate row to UB, need to transfer 8 rows
uint16_t burst_count = 2;
// Length of single data block (in 32B units): value is 8
uint16_t burst_len = 8;
// Stride from previous block to next block in source data (in 32B units): interval of two rows between blocks, value is 1
uint16_t src_stride = 1;
// Stride from previous block to next block in destination data (in 32B units): data blocks are consecutive, value is 2
uint16_t dst_stride = 2;

asc_copy_gm2ub(dst_ub, src_gm, burst_count, burst_len, src_stride, dst_stride);
```

> 📌 stride (the interval from the head of the previous data block to the head of the next data block) is introduced by this API starting from [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md). [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) uses gap (the difference between the end address of the previous data block and the start address of the next data block).

**Unaligned movement**: The basic high-dimensional partitioning API requires that both the data length and the address stride in GM be aligned to 32 bytes, which cannot adapt to unaligned scenarios. For example, after changing the data to src_gm with a shape of (16, 200) and a type of half, asc_copy_gm2ub cannot implement row-by-row movement. For this scenario, Ascend C provides the `asc_copy_gm2ub_align` unaligned movement API, which supports configuring the data block length and address stride in bytes, breaking the 32-byte alignment restriction.

<img src="../../../../figures/unaligned_in_cap.png" alt="Unaligned move-in capability" width="800"/>

Taking the preceding figure as an example, two data blocks are moved from GM to UB. Each blockLen is 54B, the stride between adjacent data blocks of the source operand is 1B, and the stride between adjacent data blocks of the destination operand is 32B.
Two and three half-type elements are padded on the left and right sides of blockLen, respectively. In this case, blockLen + leftPadding + rightPadding = 54B + 2 * 2B + 3 * 2B = 64B, which satisfies the 32B alignment.
```c
// Number of consecutive data blocks: transfer two consecutive data blocks
uint16_t burst_count = 2;
// Length of single data block (in bytes): value is 8
uint16_t burst_len = 8;
// Stride from previous block to next block in source data: source data stride unit is 1B
uint16_t src_stride = 1;
// Stride from previous block to next block in destination data: UB operation constraint address must be 32B aligned, value is 32 / 32
uint16_t dst_stride = 32 / 32;
// Left padding of transfer result (element count): value is 4 / sizeof(half) = 2
uint8_t left_padding_num = 2;
// Right padding of transfer result (element count): value is 6 / sizeof(half) = 3
uint8_t right_padding_num = 3;

asc_copy_gm2ub_align(dst, src, burst_count, burst_len, left_padding_num, right_padding_num, false, asc_load_l2_cache_mode::NORMAL_FIRST_VICTIM, src_stride, dst_stride);
```
> 📌 stride (the interval from the head of the previous data block to the head of the next data block) is introduced by this API starting from [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md). [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) uses gap (the difference between the end address of the previous data block and the start address of the next data block).

The preceding describes the logic of moving data from GM to UB. Moving data from UB to GM can be implemented using APIs of the same series. For more information about movement APIs, see [Vector Data Movement](../../../../../api/SIMD-API/c_api/vector_datamove/vector_datamove.md).

## Memory Data Computation

Memory vector computation includes two modes: continuous computation and high-dimensional partitioning computation. It also supports the mask function, which flexibly controls the data range involved in computation. The following uses the addition API as an example and illustrates the differences between the two modes with diagrams: continuous computation is simple to use and is suitable for continuous data operations on one-dimensional tensors; high-dimensional partitioning computation is more flexible and supports iterative computation and custom address strides. Developers can select a mode based on their business scenarios.

<img src="../../../../figures/mem_vec_c.png" alt="Memory vector computation mode diagram" width="800"/>

### High-Dimensional Partitioning Computation

The high-dimensional partitioning capability mainly supports setting parameters such as `repeat_time`, `stride`, and `mask` for computation APIs, enabling precise control over the iterative execution of instructions, the address stride of operands, and the mask within each iteration.

#### Iteration Control

In a single iteration, the vector computation unit reads eight consecutive DataBlocks (32 bytes each) from the UB, and writes the computation results to the eight corresponding DataBlocks in the destination UB.

<img src="../../../../figures/db_iter_c.png" alt="DataBlock iteration diagram" width="800"/>

If repeat_time (number of iterations) is set to 2, the unit performs two rounds of iteration, processing a total of 2 x 8 x 32 bytes = 512 bytes. If the data type is half (2 bytes per element), this corresponds to processing 256 elements.

> 📌 Hardware constraint: repeat_time ranges from 1 to 255. This constraint applies to all products of [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md) and [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).

<img src="../../../../figures/iter2_exp_c.png" alt="Exp computation with two iterations" width="800"/>

#### Address Stride Configuration

The vector computation unit also supports vector computation with strides, which is configured through `data_block_stride` (the address stride between different DataBlocks within a single iteration) and `repeat_stride` (the address stride of the same DataBlock between adjacent iterations). `data_block_stride` refers to the address stride between different DataBlocks within a single iteration. Its value cannot exceed the UB size limit.
For continuous computation, `data_block_stride` is set to 1, and the 8 DataBlocks within the same iteration are processed continuously.
For non-continuous computation, `data_block_stride` is greater than 1 (for example, 2), and a gap of one DataBlock appears between different DataBlocks within the same iteration when reading data, as shown in the following figure.

<img src="../../../../figures/db_stride_c.png" alt="data_block_stride example" width="800"/>

> 📌 If dst_block_stride == 0, it is equivalent to dst_block_stride = 1; if src_block_stride = 0, the source operand always reuses the first DataBlock.

The following figure shows an example of reading and writing the source operand and destination operand in the UB space within a single iteration. In the example, the `data_block_stride` of the source operand is set to 2, indicating that the address stride between different DataBlocks within a single iteration is 2 DataBlocks; the `data_block_stride` of the destination operand is set to 1, indicating that the addresses are continuous within a single iteration.

<img src="../../../../figures/iter_rw_c.png" alt="Example of reading and writing source and destination operands within a single iteration" width="800"/>

Here, **repeat_stride** refers to the address stride of the same DataBlock between adjacent iterations. You can set `repeat_stride` to flexibly control different scenarios.

| Scenario | repeat_stride Value | Data Read Characteristics | Application Scenario |
|------|------------------|-------------|--------|
| Continuous computation | 8 | Each iteration reads 8 consecutive data_blocks to complete the computation of all input data | <img src="../../../../figures/rep_cont_c.png" alt="repeat_stride continuous computation scenario" width="600"/> |
| Non-continuous computation | >8 (for example, 10) | There is a data_block gap between adjacent iterations, and the addresses are not continuous | <img src="../../../../figures/rep_discont_c.png" alt="repeat_stride non-continuous computation scenario" width="600"/> |
| Repeated computation | 0 | The first 8 consecutive data_blocks are repeatedly read and computed | <img src="../../../../figures/rep_repeat_c.png" alt="repeat_stride repeated computation scenario" width="600"/> |
| Partially repeated computation | (0, 8) | Part of the data is repeatedly read between adjacent iterations, which is generally not involved in common scenarios | <img src="../../../../figures/rep_partial_c.png" alt="repeat_stride partially repeated computation" width="600"/> |

When `repeat_time`>1, the computation is completed through multiple iterations. `repeat_stride` indicates the start address stride (in DataBlocks) of the DataBlock at the same position between adjacent iterations. For example, when `repeat_stride` = 9, the first DataBlock of the first iteration and the first DataBlock of the second iteration are 9 DataBlocks apart.

<img src="../../../../figures/multi_iter_c.png" alt="Schematic diagram of the non-continuous scenario with multiple iterations" width="800"/>

Configure `repeat_stride` and `data_block_stride` through the C-style interface to implement the multiple iterations described above.
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
#### Mask Settings

For data within the same iteration, you can use the mask parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows the schematic diagram of controlling which elements participate in the computation bit by bit using the mask bit-by-bit mode during Abs computation, where 1 indicates participation in the computation and 0 indicates non-participation.

<img src="../../../../figures/mask_op_c.png" alt="Mask operation diagram" width="800"/>

Each mask bit corresponds to the position of one element in the data. Valid bits and invalid bits are used to mark elements, enabling fine-grained on/off control over data operations.
The mask is a fixed-width numeric value. The rules are as follows: a valid bit (value 1) indicates that the corresponding element participates in the computation, and an invalid bit (value 0) indicates that the corresponding element is masked and does not participate in the operation.

The mask setting provides two modes: Counter mode and Normal mode. Normal mode further includes continuous mode and bit-by-bit mode.
Counter mode applies to scenarios of continuous element computation. In this mode, only the lower 64 bits of the mask register are valid and are used to indicate the total number of elements participating in the computation. The API automatically ignores repeat_time, and the system automatically matches the number of iterations based on the number of elements. For example, when computing 200 elements of the half type, due to the 32B alignment constraint of the UB, 256 element spaces need to be allocated. In this case, using Counter mode can precisely process the actual number of elements and avoid redundant computation.

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

In Normal computation mode (the default mode; if no mode switch occurs, no repeated configuration is required), the MASK register is used as the mask for each Repeat. The mask parameter can be set through `asc_set_vector_mask` to control the number of elements participating in the computation within a single iteration. Only high-dimensional partitioning supports this mode, which is distinguished at the API level into continuous mode within a single iteration and bit-by-bit mode within a single iteration.

- Continuous mode within a single iteration: indicates that the first n continuous elements participate in the computation. The value range is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed within each iteration.

    > **Mask range derivation**: The Vector computation unit processes a fixed 32B × 8 block = 256B of data in a single iteration. Therefore, the maximum mask value = 256B / sizeof(DataType). When the operand is 16-bit (sizeof = 2B), mask ∈ [0, 128] (256/2 = 128); when the operand is 32-bit (sizeof = 4B), mask ∈ [0, 64] (256/4 = 64); when the operand is 64-bit (sizeof = 8B), mask ∈ [0, 32] (256/8 = 32). This derivation rule can be generalized to other data types (for example, bfloat16 = 16 bit → mask upper limit 128, fp32 = 32 bit → mask upper limit 64).

    In continuous mode, if the mask exceeds the range, it is calculated as the maximum mask value for the corresponding data type. mask = 0 indicates that the instruction does not execute the computation operation, and the API is treated as a NOP (no operation). If a negative value is set, undefined behavior may occur.

    In this mode, you can specify that the first n consecutive elements are computed in a single iteration, while other data does not participate in the computation. For example, for src data (shape(256), data type half), the data needs to be computed in 2 iterations (128 elements per iteration). By setting mask = 100, the elements in the range [0, 99] are computed in the first iteration, and the elements in the range [128, 227] participate in the computation in the second iteration, while other elements do not participate in the computation.

    <img src="../../../../figures/iter_cont.png" alt="continuous computation within a single iteration" width="600"/>

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

- Bit-by-bit mode within a single iteration: You can control which elements participate in the computation on a bit-by-bit basis. A bit value of 1 indicates participation in the computation, and 0 indicates non-participation. It is divided into maskHigh (high mask) and maskLow (low mask). The value range of the parameters is related to the data type of the operand. Different data types allow different maximum numbers of elements to be processed in each iteration. When the operand is 16-bit, maskLow and maskHigh ∈ [0, 264-1], and they cannot be 0 at the same time. When the operand is 32-bit, maskHigh is 0, and maskLow ∈ (0, 264-1]. When the operand is 64-bit, maskHigh is 0, and maskLow ∈ (0, 232-1].

    In this mode, you can specify the values of maskHigh and maskLow to handle various mask operations. For interleaved computation, set the mask to the 01010101... or 10101010... pattern.
    Taking src data (shape(256), data type half) as an example, by setting a bit-by-bit mask, you can select the values of specific bits to participate in the computation in a single iteration:

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

### Continuous Computation

Continuous computation is a simplified interface provided at the software layer. When using it, you do not need to configure a mask; you can directly perform operations on N consecutive data elements in the source operand and write the results consecutively to the destination operand. It is mainly used for batch computation of one-dimensional data. Taking addition as an example, to perform addition on a vector of length 4096, call it as follows:
```c
asc_add(z, x, y, 4096); // 4096: number of consecutive elements involved in addition.
```

## Memory Vector Computation Synchronization Control

The MTE2 movement unit, vector computation unit, and other units inside the AI Core run asynchronously in parallel. When multiple units read from and write to the same storage resource, data dependencies arise. Therefore, Memory vector computation must rely on pipeline synchronization APIs to ensure that the process runs correctly. The Memory vector computation process is simpler than cube computation and consists of three stages: data movement in, computation, and data movement out, which correspond to the PIPE_MTE2, PIPE_V, and PIPE_MTE3 pipelines respectively. The execution order must be constrained through intra-core synchronization APIs to ensure serial execution of the process. On products with [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the `asc_lock` and `asc_unlock` APIs can be used to implement synchronization control across the three stages. The following is a complete example of vector addition:

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

By combining the data movement, computation, and synchronization capabilities described above, you can develop a complete Memory vector computation operator based on the C language. For a complete project example, see [c_api_add sample](../../../../../../../examples/02_simd_c_api/00_introduction/01_add/c_api_delicacy_async_add).
