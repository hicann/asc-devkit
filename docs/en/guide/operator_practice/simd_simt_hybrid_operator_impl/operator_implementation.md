# Operator Implementation<a name="ZH-CN_TOPIC_0000002500468256"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T14:38:51.812Z -->

>[!NOTE] Note
>The purpose of this example is to demonstrate the SIMD and SIMT mixed programming approach through a simple operator implementation, rather than to present the best practice for this operator's functionality.

The following figure shows the process of implementing a vector operator kernel function based on the SIMD and SIMT mixed programming approach.

**Figure 1** SIMD and SIMT mixed kernel function implementation process<a name="zh-cn_topic_0000002503613909_fig16061570280"></a>  
![](../../figures/simd_simt_mixed_kernel_function_implementation_process.png "SIMD and SIMT mixed kernel function implementation process")

-   Operator analysis: Analyze the input, output, mathematical expression, and computation logic of the operator.
-   Kernel function development: Define and implement the Ascend C operator entrypoint function.
-   SIMD VF function development: Define and implement the SIMD VF entrypoint function.
-   SIMT VF function development: Define and implement the SIMT VF entrypoint function.

The following uses the gather & adds operator as an example to describe the preceding steps in detail. This operator extracts 8192 data items at specified indices from a one-dimensional vector of length 100,000 and performs an add-by-1 operation on each extracted data item. For the complete code of the operator described in this example, see [SIMD and SIMT hybrid programming implementation of the gather & adds operator example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/00_introduction/simd_simt_gather_and_adds).

## Operator Analysis<a name="zh-cn_topic_0000002503613909_section438610015337"></a>

The specific steps of operator analysis are as follows:

1.  Determine the operator input and output.
    -   The gather & adds operator has two inputs: input and index. input is the raw data, and index is the index of the data to be obtained in input; the output is output.
    -   In this example, the data type supported by the operator input input is float, the data type supported by the input index is uint32\_t, and the data type of the output output is the same as that of the input input.
    -   The shape supported by the operator input is \[100000\]; the shape supported by the input index is \[8192\], and the value of the index data is in the range \[0, 100000\); the shape of the output is the same as that of the input index.
    -   The [format](../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md) supported by the operator input is ND.

2.  Determine the mathematical expression and computation logic of the operator.

    The i-th data in the output of the gather & adds operator is:

    ```
    output[i] = input[index[i]] + 1
    ```

    The computation logic is as follows:

    -   Use SIMT programming to obtain data at the specified index from the input (Global Memory) and store it to the Unified Buffer.
    -   Use SIMD programming to perform the add-1 operation on the data in the on-chip storage (Unified Buffer).
    -   Move the computation result from the Unified Buffer to the external storage (Global Memory).

    **Figure 2** Operator computation logic<a name="zh-cn_topic_0000002503613909_zh-cn_topic_0000001464091780_fig18937134755411"></a>  
    ![](../../figures/operator_computation_logic_52.png "operator computation logic 52")

    >[!NOTE] Note
    >The add-1 operation in simd\_adds can actually be implemented quickly in the simt\_gather function. The purpose of this example is to demonstrate the SIMD and SIMT mixed programming approach through a simple operator implementation, rather than to provide the best practice for this operator function.

3.  Determine the kernel function name and parameters.
    -   In this example, the kernel function is named gather\_and\_adds\_kernel.
    -   According to the analysis of the operator input and output, the kernel function has five parameters: input, index, output, inputTotalLength, and indexTotalLength. input and index are the memory addresses of the input on Global Memory, output is the memory address of the output on Global Memory, inputTotalLength is the data length of input, and indexTotalLength is the data length of index, which is also the data length of output.

4.  Determine the core splitting strategy, SIMT thread configuration, and the loop invocation count of the SIMD Reg vector computation API.

    In this example, the shape of the operator input index is 8192. You can set the number of cores to 8, with each core processing a data size of 1024.

    For the SIMT implementation, set the thread count to 1024, with each thread processing one data element. A single core needs to invoke the simt\_gather function only once to complete the gather operation.

    For the SIMD Reg vector computation implementation, the data size processed by a single core is 1024. The data length processed by a single Reg vector computation API call, oneRepeatSize, is [GetVecLen](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetVecLen.md)/sizeof\(float\), and the loop invocation count of the API, repeatTimes, is 1024/oneRepeatSize.

5.  Determine the SIMT VF function name and parameters.
    -   In this example, the SIMT VF function is named simt\_gather.
    -   According to the SIMT thread configuration strategy, the SIMT VF function has six parameters: input, index, gatherOutput, inputTotalLength, indexTotalLength, and outputTotalLength. input and index are the memory addresses of the input on Global Memory, gatherOutput is the memory address of the output on Unified Buffer, inputTotalLength is the data length of input, indexTotalLength is the data length of index, and outputTotalLength is the data length of gatherOutput on a single core.

6.  Determine the SIMD VF function name and parameters
    -   In this example, the SIMD VF function is named simd\_adds.
    -   According to the above SIMD strategy, the SIMD VF function has five parameters: output, input, count, oneRepeatSize, and repeatTimes. output is the memory address of the output on Unified Buffer, input is the memory address of the input on Unified Buffer, count is the total data size processed by a single core, oneRepeatSize is the data size processed in a single loop, and repeatTimes is the loop invocation count of the Reg vector computation API.

Based on the above analysis, the design specification of the Ascend C gather & adds operator is as follows:

-   Operator type (OpType): Gather\_Adds
-   Operator input and output:

    **Table 1**  gather & adds operator input and output specifications

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
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1064913716498"><a name="p1064913716498"></a><a name="p1064913716498"></a>input</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p13649143754912"><a name="p13649143754912"></a><a name="p13649143754912"></a>100000</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p6649153794916"><a name="p6649153794916"></a><a name="p6649153794916"></a>float</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p964993718492"><a name="p964993718492"></a><a name="p964993718492"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p5649637154912"><a name="p5649637154912"></a><a name="p5649637154912"></a>index (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p364953724916"><a name="p364953724916"></a><a name="p364953724916"></a>8192</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p96498374494"><a name="p96498374494"></a><a name="p96498374494"></a>uint32_t</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1264913764917"><a name="p1264913764917"></a><a name="p1264913764917"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p176491137124912"><a name="p176491137124912"></a><a name="p176491137124912"></a>output (output)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p7649183734912"><a name="p7649183734912"></a><a name="p7649183734912"></a>8192</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p1164913712492"><a name="p1164913712492"></a><a name="p1164913712492"></a>float</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p156491037174920"><a name="p156491037174920"></a><a name="p156491037174920"></a>ND</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Core count: 8
-   SIMT thread count: 1024
-   Kernel function name: gather\_and\_adds\_kernel
-   SIMT VF function name: simt\_gather
-   SIMD VF function name: simd\_adds
-   Operator implementation file name: gather\_and\_adds.asc

## Kernel Function Definition and Implementation<a name="zh-cn_topic_0000002503613909_section638730113317"></a>

Define the kernel function according to the rules described in the [kernel function](../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md).

1.  Function prototype definition

    In this example, the function is named `gather_and_adds_kernel` (the kernel function name can be customized). According to the above analysis, the function prototype is defined as follows:

    ```
    __global__ __vector__ void gather_and_adds_kernel(__gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t inputTotalLength, uint32_t indexTotalLength)
    {
    }
    ```

2.  Start the SIMT VF function `simt_gather` to obtain the data at the specified indexes from `input`.

    1.  Calculate the data size to be processed by a single core. The total data size is `indexTotalLength`. Divide it by the number of cores to obtain the data size to be processed by a single core.

        ```
        uint32_t indexTotalLengthPerBlock = indexTotalLength / AscendC::GetBlockNum();
        ```

    2.  Use the [Alloc](../../../api/SIMD-API/basic_api/resource_management/LocalMemAllocator/Alloc.md) API to apply for Unified Buffer memory space, and use this Tensor as the output of the `simt_gather` function.
    3.  Use the [asc_vf_call](../../../api/SIMT-API/SIMD_SIMT_hybrid_programming_intro/extended_syntax/kernel_function_config_147.md) API to start the SIMT_VF function `simt_gather`. The first parameter is a [dim3](../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md#li1136665405) structure, which represents the three-dimensional thread hierarchy. In this example, it is initialized to `dim3(1024)`, using the one-dimensional definition with a total of 1024 threads.

    ```
    constexpr uint32_t THREAD_COUNT = 1024;
    
    __global__ __vector__ void gather_and_adds_kernel(__gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t inputTotalLength, uint32_t indexTotalLength)
    {
        // Define the UB memory allocation object
        AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    
        // Calculate the data size to be processed by a single core
        uint32_t indexTotalLengthPerBlock = indexTotalLength / AscendC::GetBlockNum();
        // Allocate UB memory as the output of simt_gather
        AscendC::LocalTensor<float> gatherOutput = ubAllocator.Alloc<float>(indexTotalLengthPerBlock);
        // 1. Call the simt function to obtain the 1024 data items at the specified indexes
        asc_vf_call<simt_gather>(dim3(THREAD_COUNT), input, index,
                                 (__ubuf__ float *)gatherOutput.GetPhyAddr(),
                                 inputTotalLength,
                                 indexTotalLength,
                                 indexTotalLengthPerBlock);
    
        // 2. Call the SIMD function to perform the add-1 operation
        ...
    
        // 3. Move the data to GM
        ...
    }
    ```

3.  Start the SIMD VF function simd\_adds to perform the add-1 computation on the data in the Unified Buffer.

    1.  Use the [Alloc](../../../api/SIMD-API/basic_api/resource_management/LocalMemAllocator/Alloc.md) API to allocate Unified Buffer memory space, and use this Tensor as the output of the simt\_adds function.
    2.  Use the [GetVecLen](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetVecLen.md) API to divide by the length of a single data item to compute oneRepeatSize, the amount of data processed by the Reg vector computation API in a single call. Divide indexTotalLengthPerBlock, the amount of data to be processed by a single core, by oneRepeatSize to compute the loop invocation count of the Reg vector computation API.
    3.  Use the [asc_vf_call](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) API to start the SIMD VF function simd\_adds.

    ```
    __global__ __vector__ void gather_and_adds_kernel(__gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t inputTotalLength, uint32_t indexTotalLength)
    {
        // 1. Call the simt function to obtain the 1024 data items at the specified indexes
        ...
    
        // Allocate UB as the output of simd_adds
        AscendC::LocalTensor<float> addsOutput = ubAllocator.Alloc<float>(indexTotalLengthPerBlock);
        // Calculate the amount of data processed by a single call to the Reg vector computation API.
        constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
        // Calculate the number of loop iterations for the Reg vector computation API.
        uint16_t repeatTimes = (indexTotalLengthPerBlock + oneRepeatSize - 1) / oneRepeatSize;
        // 2. Call the SIMD function to perform the plus-1 operation.
        asc_vf_call<simd_adds>((__ubuf__ float *)addsOutput.GetPhyAddr(),
            (__ubuf__ float *)gatherOutput.GetPhyAddr(), indexTotalLengthPerBlock, oneRepeatSize, repeatTimes);
    
        // Rely on PIPE_V and PIPE_MTE3 for pipeline synchronization.
        AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(0);
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(0);
    
        // 3. Move the data to GM.
        ...
    }
    ```

4.  Use the [DataCopy](../../../api/SIMD-API/basic_api/data_move_guide/overview/data_movement_concept.md) API to move the result data to Global Memory.

    ```
    __global__ __vector__ void gather_and_adds_kernel(__gm__ float* input, __gm__ uint32_t* index, __gm__ float* output, uint32_t inputTotalLength, uint32_t indexTotalLength)
    {
        // 1. Call the simt function to obtain the 1024 data items at the specified indexes
        ...
    
        // 2. Call the SIMD function to perform the plus-1 operation.
        ...
    
        // 3. Move the data to GM.
        // Define a GlobalTensor object for data transfer.
        AscendC::GlobalTensor<float> outputGlobalTensor;
        // Initialize the GlobalTensor address based on the block offset.
        outputGlobalTensor.SetGlobalBuffer(output + indexTotalLengthPerBlock * AscendC::GetBlockIdx());
        // Call the data transfer API to move data to GM.
        AscendC::DataCopy(outputGlobalTensor, addsOutput, indexTotalLengthPerBlock);
    }
    ```

## SIMT VF Function Definition and Implementation<a name="zh-cn_topic_0000002503613909_section10423482111"></a>

1.  Define the function prototype.

    According to the above analysis of the SIMT VF function parameters, define the SIMT VF function prototype. Use the [\_\_simt\_vf\_\_](../../programming_guide/language_extension/simd_and_simt_hybrid_programming_builtin_keyword.md#section1009183746521) function type qualifier to identify the SIMT VF kernel function entry so that it can be called by asc\_vf\_call.

    >[!NOTE] Note
    >In SIMT programming, \_\_launch\_bounds\_\_\(thread\_num\) is an optional configuration used to specify the maximum number of threads launched by the kernel function at compile time (if not configured, thread\_num defaults to 1024). Note the following when using it: thread\_num \>= x \* y \* z (that is, the first parameter of asc\_vf\_call: dim3\{x, y, z\}), and the value range of thread\_num is 1 to 2048. The maximum number of threads determines the number of registers that can be allocated to each thread. For the specific correspondence, see [Table 5](../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md#table1715318510594). Registers are used to store local variables in a thread. If the number of local variables exceeds the number of registers, issues such as stack overflow are likely to occur.

    ```
    constexpr uint32_t THREAD_COUNT = 1024;
    
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_gather(
        __gm__ float* input,
        __gm__ uint32_t* index,
        __ubuf__ float* gatherOutput,
        uint32_t inputTotalLength,
        uint32_t indexTotalLength,
        uint32_t outputTotalLength)
    {
    }
    ```

2.  Implement the function.

    The simt\_gather function obtains data at the specified index from the input (Global Memory). Based on the above data partitioning strategy, first calculate the index of the data to be processed by the thread, and then store the data to the Unified Buffer through an assignment operation.

    In this example, the number of cores is set to 8, the thread hierarchy is \{1024, 1, 1\}, and the total data size is 8192 (8 \* 1024). Each thread processes only one data element. The logic for calculating the index of the data to be processed in index is: current core id \* number of threads per core + current thread id. The code is as follows:

    ```
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    ```

    [blockIdx](../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md#li1676053814914) is used to obtain the current core id. [blockDim](../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md#blockdim) is used to obtain the three-dimensional thread hierarchy \{x, y, z\}, which is \{1024, 1, 1\} in this example. Since the second and third dimensions are both 1, a one-dimensional hierarchy is used, so the thread count can be written as blockDim.x. [threadIdx](../../programming_guide/language_extension/SIMT-BuiltIn_keyword.md#li7760123814919) is used to obtain the three-dimensional thread index \{x, y, z\}. In this example, only the first dimension x is used, and the current thread id can be obtained through threadIdx.x.

    ```
    __simt_vf__ __launch_bounds__(THREAD_COUNT) inline void simt_gather(
        __gm__ float* input,
        __gm__ uint32_t* index,
        __ubuf__ float* gatherOutput,
        uint32_t inputTotalLength,
        uint32_t indexTotalLength,
        uint32_t outputTotalLength)
    {
        // Exception check to prevent out-of-bounds access
        if (threadIdx.x >= outputTotalLength) {
            return;
        }
    
        // Calculate the index of the data to be processed by the thread in the input index
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        // Check for exceptions to prevent out-of-bounds access.
        if (idx >= indexTotalLength) {
            return;
        }
    
        // Obtain the index of the data to be read by the thread from the input.
        uint32_t gatherIdx = index[idx];
        // Check for exceptions to prevent out-of-bounds access.
        if (gatherIdx >= inputTotalLength) {
            return;
        }
    
        // Store the data at index gatherIdx in input to the UB.
        gatherOutput[threadIdx.x] = input[gatherIdx];
    }
    ```

## SIMD VF Function Definition and Implementation<a name="zh-cn_topic_0000002503613909_section194351011182319"></a>

1.  Define the function prototype.

    According to the above parameter analysis of the SIMD VF function, define the SIMD VF function prototype. Use the \_\_simd\_vf\_\_ function type qualifier to identify the SIMD VF entrypoint function so that it can be called by the asc\_vf\_call keyword.

    ```
    __simd_vf__ inline void simd_adds(__ubuf__ float *output, __ubuf__ float *input,
        uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
    }
    ```

2.  Call the Reg vector computation API in a loop for repeatTimes times to complete the add-1 operation.

    1.  Use the [continuous aligned load](../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_continuous.md) API to move data from the Unified Buffer to the Reg vector computation basic unit [RegTensor](../../../api/SIMD-API/basic_api/reg_vector_compute/register_data_types/RegTensor.md).
    2.  Use the [Adds](../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Adds.md) API to complete the add-1 operation on the data.
    3.  Use the [continuous aligned store](../../../api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_continuous.md) API to move data from RegTensor to the Unified Buffer.

    ```
    constexpr float ADDS_ADDEND = 1.0f;
    
    __simd_vf__ inline void simd_adds(__ubuf__ float *output, __ubuf__ float *input,
        uint32_t count, uint32_t oneRepeatSize, uint16_t repeatTimes)
    {
        // Initialize the Reg vector computation unit, source operand
        AscendC::Reg::RegTensor<float> srcReg0;
        // Initialize the Reg vector computation unit, destination operand
        AscendC::Reg::RegTensor<float> dstReg0;
        // Initialize the Reg vector computation mask register
        AscendC::Reg::MaskReg maskReg;
    
        for (uint16_t i = 0; i < repeatTimes; i++) {
            // Copy data from UB to the Reg vector computation basic unit.
            maskReg = AscendC::Reg::UpdateMask<float>(count);
            AscendC::Reg::LoadAlign(srcReg0, input + i * oneRepeatSize);
            // Call the Adds API to perform the add-by-1 operation.
            AscendC::Reg::Adds(dstReg0, srcReg0, ADDS_ADDEND, maskReg);
            // Copy data from the Reg vector computation basic unit to UB.
            AscendC::Reg::StoreAlign(output + i * oneRepeatSize, dstReg0, maskReg);
        }
    }
    ```
