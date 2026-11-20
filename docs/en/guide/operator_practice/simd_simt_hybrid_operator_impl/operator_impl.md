# Operator Implementation

<!-- md-trans-meta sourceCommit=fcfc9d3c5c94ebc7869767a984d4f85589421dc0 translatedAt=2026-11-09T10:39:39.689Z pushedAt=2026-11-19T06:32:19.813Z nl=2 -->

The following uses the gather & adds operator as an example to describe the implementation of a SIMD and SIMT hybrid operator. For the complete code, see [SIMD and SIMT Hybrid Programming Implementation of the gather&adds Operator Sample](../../../../../examples/05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds).

> [!NOTE] Note 
> This sample is intended only to demonstrate the SIMD and SIMT hybrid programming approach through a simple operator implementation, and serves only as a feature introduction.

## Operator Function

The gather & adds operators extract 8192 data items from the one-dimensional vector input of length 100000 at the positions specified by index, and adds 1 to each extracted data item. The *i*-th data item in the operator output is:

```cpp
output[i] = input[index[i]] + 1
```

The computation process can be split into two stages: gather and adds. In the gather stage, data is read discretely from input according to index, and the results are written to the Unified Buffer (UB). In the adds stage, 1 is added to the contiguous intermediate data in the UB, and the results are written back to the UB in place. Finally, the result data is moved to output in the GM.

The gather stage involves index mapping and discrete access, and uses SIMT to express the data access logic of each thread. The adds stage processes contiguous data and uses SIMD to perform vector computation.

## Operator Analysis

**Figure 1**  gather & adds operator overall execution pipeline  
![](../../figures/gather_adds_overall_pipeline.png "gather & adds operator overall execution pipeline")

The overall execution pipeline of the gather & adds operators is shown in Figure 1. **simt\_gather** and **simd\_adds** both belong to the vector pipeline and are executed in call order. Moving the adds result from UB to GM belongs to the MTE3 move-out pipeline. Because the vector pipeline and the MTE3 move-out pipeline execute asynchronously in parallel, the MTE3 move-out operation may read UB data before the vector pipeline finishes writing the adds result. Therefore, a synchronization instruction must be inserted before the move-out to ensure that the adds result is written to UB before the move-out starts.

**Figure 2**  gather & adds operator execution flow  
![](../../figures/gather_adds_execution_flow.png "gather & adds operator execution flow")

The detailed execution flow of the gather & adds operator is shown in Figure 2. Based on this flow, the implementation of the gather & adds operators is analyzed in the order of input and output, kernel function parameters, execution configuration, and VF function parameters.

**Input and output analysis:** The gather & adds operator has two inputs, input and index, and one output, output. input is the original data, index indicates the data positions to be read from input, and output stores the result after reading and adding 1.

In this sample, the data type of input is float, and its shape is \[100000\]. The data type of index is uint32\_t, its shape is \[8192\], and its value range is \[0, 100000\). The data type of output is the same as that of input, and its shape is the same as that of index. The operator input and output [format](../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout.md) is ND.

**Kernel function name and parameters:** Based on the input, output, and execution flow described above, the kernel function in this sample is named **gather\_and\_adds\_kernel**, and its function prototype is as follows.

```cpp
__global__ __vector__ void gather_and_adds_kernel(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t input_total_length,
    uint32_t index_total_length)
```

Currently, SIMD and SIMT hybrid programming uses only the Vector Core (AIV core). The kernel function can use [\_\_vector\_\_](../../programming_guide/language_extension/simd_builtin_keywords.md#section1074418132518) to indicate that only the AIV core is started.

Here, **input\_total\_length** and **index\_total\_length** represent the lengths of input and index, respectively. In the gather stage, input must be accessed based on index. **input\_total\_length** is used to check whether the input access position stored in index is out of bounds, and **index\_total\_length** is used to check whether the position at which the thread accesses index is out of bounds.

**Execution configuration:** Specify the SIMT thread configuration and core partitioning strategy.

In this example, each SIMT thread processes one data element. The number of threads per core is set to 1024, meaning each core processes 1024 data elements. The length of the operator input index is 8192, so the number of cores is 8192 / 1024 = 8. Each core calls the simt\_gather function once to complete the gather operation on that core.

**SIMT VF function name and parameters:** Based on the SIMT thread configuration strategy, the SIMT VF function in this example is named **simt\_gather**. Its parameters are described as follows.

| Parameter | Type | Description |
|:---|:---|:---|
| input | \_\_gm\_\_ float\* | GM address of the input |
| index | \_\_gm\_\_ uint32\_t\* | GM address of the input index |
| local\_output | \_\_ubuf\_\_ float\* | Output address of the SIMT gather stage |
| input\_total\_length | uint32\_t | Data length of input |
| index\_total\_length | uint32\_t | Data length of index |
| output\_total\_length | uint32\_t | Data length of local\_output on a single core |

**SIMD VF function name and parameters:** In this example, the SIMD VF function is named **simd\_adds**. Its parameters are described as follows.

| Parameter | Type | Description |
|:---|:---|:---|
| output | \_\_ubuf\_\_ float\* | Output address of the SIMD adds stage |
| input | \_\_ubuf\_\_ float\* | Input address of the SIMD adds stage |
| count | uint32\_t | Total number of data elements processed by a single core |
| one\_repeat\_size | uint32\_t | Number of float elements processed per round |
| repeat\_times | uint16\_t | Number of loops required to process count elements |

Based on the preceding analysis, the design specifications of the Ascend C gather & adds operators are as follows:

**Table 1**  gather & adds operator design specifications

<table>
<tr><td rowspan="1" align="center">Operator Type (OpType)</td><td colspan="4" align="center">Gather_Adds</td></tr>
<tr><td rowspan="3" align="center">Operator input</td><td align="center">Name</td><td align="center">Shape</td><td align="center">Data Type</td><td align="center">Format</td></tr>
<tr><td align="center">input</td><td align="center">100000</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">index</td><td align="center">8192</td><td align="center">uint32_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">operator output</td><td align="center">output</td><td align="center">8192</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">kernel function name</td><td colspan="4" align="center"><code>gather_and_adds_kernel</code></td></tr>
<tr><td rowspan="1" align="center">SIMT VF function name</td><td colspan="4" align="center"><code>simt_gather</code></td></tr>
<tr><td rowspan="1" align="center">SIMD VF function name</td><td colspan="4" align="center"><code>simd_adds</code></td></tr>
</table>

## Kernel Function Definition and Implementation

Define the kernel function according to the rules described in [Kernel Function](../../programming_guide/advanced_programming/advanced_ai_core_programming_model/simd_simt_hybrid_programming/kernel_and_vf_functions.md#zh-cn_topic_0000002571578013_section156822920311). In this sample, the kernel function **gather\_and\_adds\_kernel** is defined as follows:

```cpp
__global__ __vector__ void gather_and_adds_kernel(
    __gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t input_total_length,
    uint32_t index_total_length)
{
    ...
    // 1. Call the SIMT function to obtain the data at the specified index.
    asc_vf_call<simt_gather>(
        dim3(THREAD_COUNT), input, index, local_output, input_total_length,
        index_total_length, index_total_length_per_block);

    ...
    // 2. Call the SIMD function to perform the add-1 operation.
    asc_vf_call<simd_adds>(
        local_output, local_output, index_total_length_per_block, one_repeat_size, repeat_times);

    // Synchronize the PIPE_V and PIPE_MTE3 pipelines.
    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);

    // 3. Move the data from UB to GM.
    asc_copy_ub2gm_align(output + index_total_length_per_block * block_idx, local_output, 1,
        index_total_length_per_block * sizeof(float), asc_store_l2_cache_mode::NORMAL_FIRST_VICTIM, 0, 0);
}
```

The kernel function starts the SIMT VF function simt\_gather and the SIMD VF function simd\_adds in sequence: simt\_gather reads the discrete data from GM into UB, and simd\_adds performs the add-1 computation on the gather result in UB and writes the result back to UB in place.

For the result **local\_output** computed and written to UB based on SIMD programming, before calling the [asc\_copy\_ub2gm\_align](../../../api/SIMD-API/c_api/vector_datamove/asc_copy_ub2gm_align/asc_copy_ub2gm_align.md) API to move it out to GM, you need to use the [asc\_sync\_notify](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_notify.md)/[asc\_sync\_wait](../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_sync_wait.md) API to control the synchronization between the vector computation pipeline and the MTE3 move-out pipeline, ensuring that the adds computation result is written to UB before the move-out is started.

## SIMT VF Implementation

Based on the preceding analysis of the SIMT VF function parameters, define the SIMT VF function prototype as follows:

```cpp
constexpr uint32_t THREAD_COUNT = 1024;

__simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_gather(
    __gm__ float* input, __gm__ uint32_t* index, __ubuf__ float* local_output, uint32_t input_total_length,
    uint32_t index_total_length, uint32_t output_total_length)
{
    ...
    // Compute the element position in the global index array based on the current core ID and thread ID.
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    ...
    // Read the index of the data that the thread should obtain in the input.
    uint32_t gather_idx = index[idx];
    ...
    // Store the data at index gather_idx in input to UB.
    local_output[threadIdx.x] = input[gather_idx];
}
```

> [!NOTE] Note 
> In SIMT programming, **\_\_launch\_bounds\_\_\(thread\_num\)** is an optional configuration used to specify the maximum number of threads for launching the kernel function at compile time (if not configured, **thread\_num** defaults to **1024**). Note the following when using it: **thread\_num** \>= x \* y \* z (that is, the first parameter of **asc\_vf\_call**: **dim3\{x, y, z\})**, and the value range of the thread count **thread\_num** is 1 to 2048. The maximum thread count determines the number of registers that can be allocated to each thread. For the specific mapping, see [Table 5](../../programming_guide/language_extension/simt_builtin_keywords.md#table1715318510594). Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as stack overflow are likely to occur.

Use the [\_\_simt\_vf\_\_](../../programming_guide/language_extension/simd_simt_hybrid_builtin_keywords.md#__simt_vf__) function type qualifier to mark the SIMT VF function entry so that it can be called by [asc\_vf\_call](../../../api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config.md#asc_vf_call-call).

The simt\_gather function obtains data at a specified index from the input. In the function, the index of the data to be processed by the thread is first computed, and then the data is stored to UB through an assignment operation.

In this example, the number of cores is set to 8, the thread hierarchy is \{1024, 1, 1\}, and the total data amount is 8192 (8 \* 1024). Each thread processes one index element. The global index position is computed as follows:

```cpp
uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
```

Here, [blockIdx](../../programming_guide/language_extension/simt_builtin_keywords.md#li1676053814914) is used to obtain the current core ID. [blockDim](../../programming_guide/language_extension/simt_builtin_keywords.md#li076017381191) is used to obtain the three-dimensional thread hierarchy \{x, y, z\}, which is \{1024, 1, 1\} in this example. Since the second and third dimensions are both 1, a one-dimensional hierarchy is used, so the thread count can be written as **blockDim.x**. [threadIdx](../../programming_guide/language_extension/simt_builtin_keywords.md#li7760123814919) is used to obtain the three-dimensional thread index \{x, y, z\}. In this example, only the first dimension x is used, and the current thread ID can be obtained through **threadIdx.x**.

## SIMD VF Implementation

Based on the preceding parameter analysis of the SIMD VF function, define the SIMD VF function prototype as follows:

```cpp
constexpr float ADDS_ADDEND = 1.0f;

__simd_vf__ inline void simd_adds(
    __ubuf__ float* output, __ubuf__ float* input, uint32_t count, uint32_t one_repeat_size, uint16_t repeat_times)
{
    // Initialize the SIMD vector data register, source operand.
    vector_float src_reg0;
    // Initialize the SIMD vector data register, destination operand.
    vector_float dst_reg0;
    // Initialize the SIMD vector computation flag register.
    vector_bool mask_reg;

    for (uint16_t i = 0; i < repeat_times; i++) {
        mask_reg = asc_update_mask_b32(count);
        // Move data from UB to the SIMD vector data register.
        asc_loadalign(src_reg0, input + i * one_repeat_size);
        // Call the asc_add_scalar API to add 1.
        asc_add_scalar(dst_reg0, src_reg0, ADDS_ADDEND, mask_reg);
        // Move data from the SIMD vector data register to UB.
        asc_storealign(output + i * one_repeat_size, dst_reg0, mask_reg);
    }
}
```

Use the **\_\_simd\_vf\_\_** function type qualifier to identify the SIMD VF function entry so that it can be called by [asc\_vf\_call](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md).

The simd\_adds function adds 1 to the data in UB. In the function, the loop runs **repeat\_times** times to process the data of a single core. The input is the gather result written to UB in the SIMT stage, and the output is the UB output after adding 1. In this sample, both the input and output are passed as **local\_output**. **one\_repeat\_size** indicates the number of float elements processed in each round, and **repeat\_times** indicates the number of loop iterations required to process count elements.

In the loop, the [asc_loadalign](../../../api/SIMD-API/c_api/reg_compute/load/asc_loadalign.md) API is used to move data from the UB to the SIMD vector data registers, the [asc_add_scalar](../../../api/SIMD-API/c_api/reg_compute/reg_arith/asc_add_scalar.md) API is used to add 1 to the data, and the [asc_storealign](../../../api/SIMD-API/c_api/reg_compute/store/asc_storealign.md) API is used to move the data from the SIMD vector data registers back to the UB.
