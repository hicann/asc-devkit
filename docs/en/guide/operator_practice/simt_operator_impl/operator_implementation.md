# Operator Implementation<a name="ZH-CN_TOPIC_0000002532228181"></a>

<!-- md-trans-meta sourceCommit=71cb0b86f0588b6afb35a9f75dffa49dd3a77c97 translatedAt=2026-08-26T14:44:00.469Z -->

This section uses the Gather operator as an example to describe the basic process of SIMT operator implementation, as shown in the following figure:

![](../../figures/tik2_vector_programming_process.png)

-   Operator analysis and kernel function definition: Specify the operator input and output, and analyze the scheme for setting the maximum thread count. Specify the kernel function name and input/output parameters of the operator, determine the dynamic parameter space size, and configure the maximum thread count.
-   Host-side thread partitioning calculation: Calculate and set parameters such as gridDim and blockDim based on the shape information of the input data.
-   Kernel-side operator implementation: Implement the computation logic within a single thread.

The preceding steps are described in detail below. For the complete operator implementation, see [Gather Operator Implementation Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/00_introduction/01_gather/general_gather).

## Operator Analysis and Kernel Function Definition<a name="zh-cn_topic_0000002475260740_section725024573711"></a>

The operator analysis procedure is as follows:

1.  Specify the function and computation logic of the operator.

    The gather operator obtains the data of specified index rows from an input tensor. Specifically, it obtains m rows of data at specified indices from the two-dimensional tensor input of shape M \* N, where the row indices of these m rows are specified by the input index. The formula for calculating the data in the i-th row of the operator output is as follows:

    ```
    output[i] = input[index[i]]
    ```

2.  Specify the input and output of the operator.
    -   The gather operator has two inputs, input and index, and one output, output.
    -   In this sample, the data type of the operator input input supports float, half, and int32\_t. The data type of index is uint32\_t. The data type of the operator output is the same as that of the input.
    -   Each thread processes one row of data. The length of each row in\_width and the total number of rows to be processed index\_total\_length need to be passed in to ensure that trailing threads do not perform invalid operations.
    -   In this sample, the operator does not require a large number of temporary variables. Therefore, the maximum thread count of the kernel function is directly set to 2048.

3.  Specify the function name and parameters.
    -   Customize the kernel function name. In this sample, the kernel function is named gather\_custom.
    -   By analyzing the operator input and output, use template parameters to support different input/output data types.

        <a name="table4135123919714"></a>
        <table><thead align="left"><tr id="row6135439176"><th class="cellrowborder" valign="top" width="31.619999999999997%" id="mcps1.1.4.1.1"><p id="p213510395713"><a name="p213510395713"></a><a name="p213510395713"></a>Template Parameter Name</p>
        </th>
        <th class="cellrowborder" valign="top" width="29.2%" id="mcps1.1.4.1.2"><p id="p531020292080"><a name="p531020292080"></a><a name="p531020292080"></a>Template Parameter Type</p>
        </th>
        <th class="cellrowborder" valign="top" width="39.18%" id="mcps1.1.4.1.3"><p id="p1713553915713"><a name="p1713553915713"></a><a name="p1713553915713"></a>Parameter Definition</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row413593915712"><td class="cellrowborder" valign="top" width="31.619999999999997%" headers="mcps1.1.4.1.1 "><p id="p163062813811"><a name="p163062813811"></a><a name="p163062813811"></a>type_data</p>
        </td>
        <td class="cellrowborder" valign="top" width="29.2%" headers="mcps1.1.4.1.2 "><p id="p1431172912819"><a name="p1431172912819"></a><a name="p1431172912819"></a>typename</p>
        </td>
        <td class="cellrowborder" valign="top" width="39.18%" headers="mcps1.1.4.1.3 "><p id="p121352396712"><a name="p121352396712"></a><a name="p121352396712"></a>Data type of the input/output</p>
        </td>
        </tr>
        <tr id="row513513393711"><td class="cellrowborder" valign="top" width="31.619999999999997%" headers="mcps1.1.4.1.1 "><p id="p1233817169818"><a name="p1233817169818"></a><a name="p1233817169818"></a>type_idx</p>
        </td>
        <td class="cellrowborder" valign="top" width="29.2%" headers="mcps1.1.4.1.2 "><p id="p03119291987"><a name="p03119291987"></a><a name="p03119291987"></a>typename</p>
        </td>
        <td class="cellrowborder" valign="top" width="39.18%" headers="mcps1.1.4.1.3 "><p id="p013513917710"><a name="p013513917710"></a><a name="p013513917710"></a>Data type of index</p>
        </td>
        </tr>
        </tbody>
        </table>

        The function input parameters are defined as follows:

        <a name="table1353353310716"></a>
        <table><thead align="left"><tr id="row125311633871"><th class="cellrowborder" valign="top" width="23.362336233623363%" id="mcps1.1.4.1.1"><p id="p115315333718"><a name="p115315333718"></a><a name="p115315333718"></a>Parameter</p>
        </th>
        <th class="cellrowborder" valign="top" width="17.72177217721772%" id="mcps1.1.4.1.2"><p id="p135316331477"><a name="p135316331477"></a><a name="p135316331477"></a>Type</p>
        </th>
        <th class="cellrowborder" valign="top" width="58.91589158915891%" id="mcps1.1.4.1.3"><p id="p753119332073"><a name="p753119332073"></a><a name="p753119332073"></a>Definition</p>
        </th>
        </tr>
        </thead>
        <tbody><tr id="row2053118339712"><td class="cellrowborder" valign="top" width="23.362336233623363%" headers="mcps1.1.4.1.1 "><p id="p153111335710"><a name="p153111335710"></a><a name="p153111335710"></a>input</p>
        </td>
        <td class="cellrowborder" valign="top" width="17.72177217721772%" headers="mcps1.1.4.1.2 "><p id="p1753117331577"><a name="p1753117331577"></a><a name="p1753117331577"></a>type_data*</p>
        </td>
        <td class="cellrowborder" valign="top" width="58.91589158915891%" headers="mcps1.1.4.1.3 "><p id="p1531173318711"><a name="p1531173318711"></a><a name="p1531173318711"></a>Memory address of the input data in <span id="ph39787110114"><a name="ph39787110114"></a><a name="ph39787110114"></a>Global Memory</span></p>
        </td>
        </tr>
        <tr id="row8532833771"><td class="cellrowborder" valign="top" width="23.362336233623363%" headers="mcps1.1.4.1.1 "><p id="p1753218339717"><a name="p1753218339717"></a><a name="p1753218339717"></a>index</p>
        </td>
        <td class="cellrowborder" valign="top" width="17.72177217721772%" headers="mcps1.1.4.1.2 "><p id="p11532203318712"><a name="p11532203318712"></a><a name="p11532203318712"></a>type_idx*</p>
        </td>
        <td class="cellrowborder" valign="top" width="58.91589158915891%" headers="mcps1.1.4.1.3 "><p id="p15632163816119"><a name="p15632163816119"></a><a name="p15632163816119"></a>Memory address of the index data in <span id="ph122519152272"><a name="ph122519152272"></a><a name="ph122519152272"></a>Global Memory</span></p>
        </td>
        </tr>
        <tr id="row153213335719"><td class="cellrowborder" valign="top" width="23.362336233623363%" headers="mcps1.1.4.1.1 "><p id="p75321337715"><a name="p75321337715"></a><a name="p75321337715"></a>gather_output</p>
        </td>
        <td class="cellrowborder" valign="top" width="17.72177217721772%" headers="mcps1.1.4.1.2 "><p id="p7555285100"><a name="p7555285100"></a><a name="p7555285100"></a>type_data*</p>
        </td>
        <td class="cellrowborder" valign="top" width="58.91589158915891%" headers="mcps1.1.4.1.3 "><p id="p175321633673"><a name="p175321633673"></a><a name="p175321633673"></a>Memory address of the output data in <span id="ph15278142492719"><a name="ph15278142492719"></a><a name="ph15278142492719"></a>Global Memory</span></p>
        </td>
        </tr>
        <tr id="row1153216339719"><td class="cellrowborder" valign="top" width="23.362336233623363%" headers="mcps1.1.4.1.1 "><p id="p65328331978"><a name="p65328331978"></a><a name="p65328331978"></a>in_width</p>
        </td>
        <td class="cellrowborder" valign="top" width="17.72177217721772%" headers="mcps1.1.4.1.2 "><p id="p81986171107"><a name="p81986171107"></a><a name="p81986171107"></a>uint32_t</p>
        </td>
        <td class="cellrowborder" valign="top" width="58.91589158915891%" headers="mcps1.1.4.1.3 "><p id="p17532133310715"><a name="p17532133310715"></a><a name="p17532133310715"></a>Length of the second dimension of the input data (column width)</p>
        </td>
        </tr>
        <tr id="row1953314331071"><td class="cellrowborder" valign="top" width="23.362336233623363%" headers="mcps1.1.4.1.1 "><p id="p1553263313718"><a name="p1553263313718"></a><a name="p1553263313718"></a>index_total_length</p>
        </td>
        <td class="cellrowborder" valign="top" width="17.72177217721772%" headers="mcps1.1.4.1.2 "><p id="p12930182111011"><a name="p12930182111011"></a><a name="p12930182111011"></a>uint32_t</p>
        </td>
        <td class="cellrowborder" valign="top" width="58.91589158915891%" headers="mcps1.1.4.1.3 "><p id="p65331033278"><a name="p65331033278"></a><a name="p65331033278"></a>Total length of the index data</p>
        </td>
        </tr>
        </tbody>
        </table>

4.  Specify the dynamic parameter configuration scheme for the SIMT kernel function, such as gridDim and blockDim.
    -   This sample uses a uniform partitioning scheme. Based on the available number of cores and the maximum thread count, it calculates and adjusts gridDim (the number of thread blocks to be started) and blockDim (the number of threads to be started in a thread block), while ensuring that gridDim does not exceed 65535 and blockDim does not exceed the maximum thread count of 2048.
    -   The operator implementation logic does not require dynamic UB space.

Based on the preceding analysis, the design specifications of the SIMT Gather operator are as follows:

-   Operator type (OpType): Gather
-   Operator input/output:

    **Table 1**  Gather operator input/output specifications

    <a name="table4934296305"></a>
    <table><thead align="left"><tr id="row59358913304"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p5503181819300"><a name="p5503181819300"></a><a name="p5503181819300"></a><strong id="b1850331853010"><a name="b1850331853010"></a><a name="b1850331853010"></a>name</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1550381833017"><a name="p1550381833017"></a><a name="p1550381833017"></a><strong id="b7503171811309"><a name="b7503171811309"></a><a name="b7503171811309"></a>shape</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1950391883014"><a name="p1950391883014"></a><a name="p1950391883014"></a><strong id="b2503111803020"><a name="b2503111803020"></a><a name="b2503111803020"></a>data type</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p14503218133015"><a name="p14503218133015"></a><a name="p14503218133015"></a><strong id="b8503141818301"><a name="b8503141818301"></a><a name="b8503141818301"></a>format</strong></p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p236113468393"><a name="p236113468393"></a><a name="p236113468393"></a>input</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p153614462390"><a name="p153614462390"></a><a name="p153614462390"></a>(M, N)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p10361154623914"><a name="p10361154623914"></a><a name="p10361154623914"></a>float/half/int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p193611746103911"><a name="p193611746103911"></a><a name="p193611746103911"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p17361164612395"><a name="p17361164612395"></a><a name="p17361164612395"></a>index</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p11361134612397"><a name="p11361134612397"></a><a name="p11361134612397"></a>(m), m &lt; M</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p13619467395"><a name="p13619467395"></a><a name="p13619467395"></a>uint32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p5361446193918"><a name="p5361446193918"></a><a name="p5361446193918"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p9361124603911"><a name="p9361124603911"></a><a name="p9361124603911"></a>output</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p103619462395"><a name="p103619462395"></a><a name="p103619462395"></a>(m, N)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p2361144653915"><a name="p2361144653915"></a><a name="p2361144653915"></a>float/half/int32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p11361124673912"><a name="p11361124673912"></a><a name="p11361124673912"></a>ND</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Kernel function name: gather\_custom

The kernel function is defined as follows:

```
constexpr uint32_t MAX_THREAD_COUNT = 2048;

template <typename type_data, typename type_idx>
__global__ __launch_bounds__(MAX_THREAD_COUNT) void gather_custom(
    type_data* input,
    type_idx* index,
    type_data* gather_output,
    uint32_t in_width,
    uint32_t index_total_length)
```

>[!NOTE] Note
>When defining a kernel function, use \_\_launch\_bounds\_\_\(MAX\_THREAD\_COUNT\) to specify the maximum thread count. The maximum thread count ranges from 1 to 2048. A larger maximum thread count allows more threads to be enabled and delivers better performance, but reduces the number of internal registers available to each thread. If not set, the default maximum thread count is 1024. In the preceding analysis, it has been determined that the computation does not require many registers, so the maximum thread count is set to 2048. In actual operator development, adjust this value based on the specific operator implementation.

## Host-Side Thread Partitioning Calculation<a name="zh-cn_topic_0000002475260740_section10423482111"></a>

This sample uses a simple uniform partitioning scheme to describe how to calculate dynamic partitioning parameters.

1.  Set the initial gridDim.

    If gridDim is set to a value smaller than the actual number of AIV cores, idle cores are wasted. Therefore, set the initial gridDim to the actual number of AIV cores on the current chip. The method for obtaining the number of AIV cores is as follows:

    ```
    uint32_t real_core_num = 0;
    const auto& platformInfoMgr = platform_ascendc::PlatformAscendCManager::GetInstance();
    real_core_num = platformInfoMgr->GetCoreNumAiv();
    blocks_per_grid = real_core_num; // blocks_per_grid is the initial gridDim
    ```

2.  Calculate blockDim.

    Calculate blockDim, the number of threads enabled in a thread block, based on the length of the input index (index_total_length) and the initial gridDim.

    ```
    // threads_per_block is the blockDim value
    threads_per_block = (index_total_length + blocks_per_grid - 1) / blocks_per_grid;
    ```

3.  Adjust blockDim.

    If blockDim exceeds the maximum thread count limit, adjust blockDim to the maximum thread count.

    ```
    if (threads_per_block > MAX_THREAD_COUNT) {
        threads_per_block = MAX_THREAD_COUNT;
    }
    ```

4.  Adjust gridDim.

    Recalculate gridDim to ensure that gridDim \* blockDim ≥ index\_total\_length, that is, ensure that all enabled threads can process the data of the specified number of rows.

    ```
    blocks_per_grid = (index_total_length + threads_per_block - 1) / threads_per_block;
    ```

The complete partitioning calculation code is as follows:

```
constexpr uint32_t MAX_THREAD_COUNT = 2048;
constexpr uint32_t MAX_BLOCK_COUNT = 65535;

bool block_split(uint32_t index_total_length, uint32_t &blocks_per_grid, uint32_t &threads_per_block) {
    uint32_t real_core_num = 0;
    const auto& platformInfoMgr = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (platformInfoMgr == nullptr) {
        std::cout << "[ERROR] Get platform info failed, please check device status."<< std::endl;
        return false;
    }
    real_core_num = platformInfoMgr->GetCoreNumAiv();
    blocks_per_grid = real_core_num;
    threads_per_block = (index_total_length + blocks_per_grid -1) / blocks_per_grid;
    if (threads_per_block > MAX_THREAD_COUNT) {
        threads_per_block = MAX_THREAD_COUNT;
        blocks_per_grid = (index_total_length + threads_per_block - 1) / threads_per_block;
        if (blocks_per_grid > MAX_BLOCK_COUNT) {
            std::cout << "[ERROR] index_total_length: "<< index_total_length << " can not be bigger than "
                << MAX_THREAD_COUNT * MAX_BLOCK_COUNT<< "."<< std::endl;
            return false;
        }
    }
    return true;
}
```

## Kernel-side Operator Implementation<a name="zh-cn_topic_0000002475260740_section11287940131811"></a>

1.  Obtain the position offset of the current thread based on the uniform partitioning algorithm.

    In this operator, only the first dimension of thread dimensions such as gridDim and blockDim is used. Therefore, only the x-dimension information needs to be considered when calculating the offset. As shown in the following code, threadIdx indicates the index of a thread within its thread block, blockDim indicates the number of threads configured in a thread block, and blockIdx indicates the index of the thread block.

    ```
    // Calculate the thread index.
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    ```

2.  Based on the thread index, obtain the row index of the data to be processed by the current thread, calculate the corresponding input/output position offsets, and implement the gathering of the entire row of data.

    ```
    uint32_t in_row = index[out_row];
    int input_idx = in_row * in_width;
    int output_idx = out_row * in_width;
    for (int32_t col = 0; col < in_width; col++) {
        gather_output[output_idx] = input[input_idx];
        input_idx += 1;
        output_idx += 1;
    }
    ```

The complete kernel function code is as follows:

```
constexpr uint32_t MAX_THREAD_COUNT = 2048;
constexpr uint32_t MAX_BLOCK_COUNT = 65535;

template <typename type_data, typename type_idx>
__global__ __launch_bounds__(MAX_THREAD_COUNT) void gather_custom(
    type_data* input,
    type_idx* index,
    type_data* gather_output,
    uint32_t in_width,
    uint32_t index_total_length)
{
    // Calculate global thread ID
    int32_t out_row = blockIdx.x * blockDim.x + threadIdx.x;
    // Maps to the row index of output tensor
    if (out_row >= index_total_length) {
        return;
    }
    // Single thread processes entire row (all columns) - enables coalesced memory access
    uint32_t in_row = index[out_row];
    int input_idx = in_row * in_width;
    int output_idx = out_row * in_width;
    for (int32_t col = 0; col < in_width; col++) {
        gather_output[output_idx] = input[input_idx];
        input_idx += 1;
        output_idx += 1;
    }
}
```

## Execution Verification<a name="zh-cn_topic_0000002475260740_section085962118107"></a>

After the kernel function, that is, the operator kernel program, is developed, you can write the kernel function invocation program on the Host side to call the operator from the host-side application for execution verification.

The key code on the host side is as follows:

```
std::vector<float> gather(std::vector<float>& input, const uint32_t* in_shape, std::vector<uint32_t>& index)
{
    ...
    // Calculate the partitioning parameters.
   
    uint32_t blocks_per_grid = 0;
    uint32_t threads_per_block = 0;
    block_split(index_total_length, blocks_per_grid, threads_per_block);
    
    ...
    // This operator does not require dynamic UB memory.
    uint32_t dyn_ubuf_size = 0;  // No need to alloc dynamic memory.
    
    // Call the kernel function with the kernel invocation operator <<<...>>> to perform the specified operation.
    
    gather_custom<<<blocks_per_grid, threads_per_block, dyn_ubuf_size, stream>>>(
              input_device, index_device, output_device, in_shape[1], index_total_length);
    ...
}
```
