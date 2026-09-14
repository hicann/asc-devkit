# Kernel Function<a name="ZH-CN_TOPIC_0000001564710756"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-20T11:05:31.708Z pushedAt=2026-08-26T03:06:51.781Z -->

A kernel function is a C/C++ function that runs in parallel on the device side. It is called by host-side code and, when called, is instantiated into multiple parallel copies that are scheduled to run on multiple AI Cores (each AI Core is abstracted as a Block). These Blocks leverage the compute power of the AI Cores to collaboratively complete the computation of an operator.

## Defining Kernel Functions<a name="zh-cn_topic_0000001447989210_section1915102519220"></a>

Observe the following rules when defining a kernel function.

-   **Using function type qualifiers and function execution space qualifiers**

    In addition to defining the kernel function in the same way as a C/C++ function declaration, you need to add an extra function type qualifier to the kernel function.

    You must use the \_\_global\_\_ function type qualifier to identify it as a kernel function that can be called by <<<...\>\>\>. Meanwhile, based on the hardware unit on which the operator actually runs, select one of the following function execution space qualifiers. For details, see [Function Execution Space Qualifiers](../../language_extension/SIMD-BuiltIn_keyword.md#section1074418132518):

    -   \_\_aicore\_\_: identifies that the kernel function is executed on the AI Core on the device side, without distinguishing the specific core type (usually used in [coupled mode](../../advanced_programming/hardware_implementation/basic_architecture.md#section1574769433)).

    -   \_\_vector\_\_: identifies that the kernel function is executed on the Vector Core on the device side, and is applicable to operators that contain only vector computation.

    -   \_\_cube\_\_: identifies that the kernel function is executed on the Cube Core on the device side, and is applicable to operators that contain only cube computation.

    -   \_\_mix\_\_: identifies that the kernel function is executed on both the Cube Core and the Vector Core, and is applicable to operators that contain fused vector and cube computation.

-   **Using variable type qualifiers**

    Pointer input parameter variables must be qualified with the variable type qualifier \_\_gm\_\_, indicating that the pointer variable points to a memory address in Global Memory.

-   **Other rules**<a name="zh-cn_topic_0000001447989210_li94421027152419"></a>

    1.  A kernel function must have a void return type.

    2.  The kernel function parameter list must comply with the [function parameter list constraints](../../../technical_appendix/cpp_standard_support/syntax_restrictions/function.md#section_kernel_function_argument_list_constraint).

-   The following is a code example:

    ```
    // Use the __global__ and __vector__ qualifiers to define kernel functions.
    __global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
    {
        ...
    }

    // Template-based definition of kernel functions is supported. An example of such a definition is provided below; it includes one type template parameter.
    template<typename T>
    __global__ __vector__ void add_kernel(__gm__ T* x, __gm__ T* y, __gm__ T* z)
    {
        ...
    }

    ```

## Calling Kernel Functions

In Ascend C operator programming, functions are strictly classified into three types based on their code execution space. The call relationships among them follow these rules:

-   Host-side execution function: runs on the host side (CPU). In addition to supporting mutual calls between functions of the same type under standard C/C++ semantics, its core role is to act as the initiator of compute tasks by delivering the execution configuration through the kernel call operator <<<...\>\>\> to call kernel functions on the device side across the boundary.

-   Kernel function: runs on the device side (NPU) and serves as the execution entry point for tasks delivered from the host side. When organizing parallel computation logic, a kernel function can call other device-side execution functions.

-   Device-side execution function: an ordinary function (other than kernel functions) that runs on the device side (NPU). Such functions are mainly used to encapsulate and reuse underlying computation logic. Their call permission is strictly limited to the device side, and they can only be called by kernel functions or device-side execution functions of the same type.

**Figure 1** Calling relationships among kernel functions, host-side execution functions, and device-side execution functions<a name="fig321723224511"></a>  
![](../../../figures/kernel_function_host_execution_function_device_execution_function_call_relationships.png "Kernel function, host-side execution function, device-side execution function call relationships")

The host side calls a kernel function using the kernel call operator <<<...>>>, as shown below:

```
kernel_name<<<numBlocks, dynUBufSize, stream>>>(argument list);
```

The parameters in <<<...\>\>\> are the execution configuration of the kernel function, which is determined by three parameters. For details, see [Kernel Function Configuration](../../language_extension/SIMD-BuiltIn_keyword.md#kernel-function-configuration):

-   numBlocks: specifies the number of cores on which the kernel function will be executed. For different hardware architectures and operator types, the setting rules of numBlocks differ. For details, see [Kernel Function Configuration](../../language_extension/SIMD-BuiltIn_keyword.md#kernel-function-configuration). Each core that executes the kernel function is assigned a logical ID, that is, block\_idx, which can be obtained in the kernel function implementation using the built-in variable [block_idx](../../language_extension/SIMD-BuiltIn_keyword.md#built-in-variables). Note that when the `__mix__` function execution space qualifier is used, the Vector Cores on the same core have the same block_idx value. For details and examples, see [Built-in Variables](../../language_extension/SIMD-BuiltIn_keyword.md#built-in-variables);

-   dynUBufSize: Dynamic Unified Buffer Size, which specifies the size of the space for dynamic UB memory allocation (UB only, excluding L1 Buffer and others), in bytes. The default value is 0;

-   stream: of the aclrtStream type. A stream is used to maintain the execution order of asynchronous operations and ensure that they are executed on the device in the order in which they are called in the application code. The default value is nullptr. For details about stream creation and other management APIs, see [*Runtime API*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).

The following kernel function named add\_custom implements the addition of two vectors. The call example is as follows:

```
// numBlocks is set to 8: Indicates that the add_kernel function is invoked on 8 cores. Each core will execute the kernel independently and in parallel.
// dynUBufSize is set to 0: Indicates that no dynamic memory space is allocated for the Unified Buffer (UB). The argument list for the kernel is x, y, z.
add_kernel<<<8, 0, stream>>>(x, y, z);

// The invocation format for a template kernel function is as follows:
// 1. Process Float32 vector addition.
add_kernel<float><<<8, 0, stream>>>(x_float, y_float, z_float);
// 2. Simply change the template parameter to int32_t to perform integer computation.
add_kernel<int32_t><<<8, 0, stream>>>(x_int, y_int, z_int);
```

The call of a kernel function is asynchronous. After the call of the kernel function is complete, control is immediately returned to the host side. You can call the following **aclrtSynchronizeStream** function to force the host-side program to wait until all kernel functions are executed.

```
aclError aclrtSynchronizeStream(aclrtStream stream);
```

For details about how to use aclrtSynchronizeStream, see [*Runtime API*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).

## Block Index Built-in Variables

After multi-core parallel compute is started through <<<numBlocks, ...>>>, multiple AI Cores execute the same kernel function code simultaneously. To prevent multiple cores from repeatedly processing the same data block, you need to partition the global data in the code.

Ascend C provides the built-in variables block_idx and block_num to help each core precisely locate the data slice it needs to process:

-   block_num: obtains the total number of logical cores allocated to the current kernel function (its value equals numBlocks configured when the kernel is called).

-   block_idx: obtains the logical core ID of the code currently being executed, with a value range of [0, block_num - 1].

In the Mix scenario (using the `__mix__` function execution space qualifier), the Vector core (AIV) also needs to use sub_block_idx to determine its position within the combination:

-   sub_block_num: obtains the number of Vector cores in the current combination (that is, the number of AIVs in each combination).

-   sub_block_idx: obtains the index of the current Vector core within the combination, with a value range of [0, sub_block_num - 1].

In the Mix scenario, the complete logical position of an AIV core is determined by block_idx and sub_block_idx together: `logic_idx = block_idx * sub_block_num + sub_block_idx`. Developers should use the [GetBlockIdx](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) and [GetSubBlockIdx](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) APIs in combination to obtain the logical position, instead of directly using the built-in variables (for more details, see the precautions in [Built-in Variables](../../language_extension/SIMD-BuiltIn_keyword.md#built-in-variables)).

The following example shows how to use block_idx and block_num to evenly distribute one-dimensional data with a total length of TOTAL_LENGTH to multiple cores for processing:

```
// Assume the total amount of data to be processed is predefined in a macro.
constexpr uint32_t TOTAL_LENGTH = 8192;

__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    // 1. Get the logical ID of the current core and the total number of logical cores participating in the computation.
    uint32_t core_idx = block_idx;
    uint32_t core_num = block_num;

    // 2. Calculate the amount of data to be processed by the current core.
    // Assume TOTAL_LENGTH is perfectly divisible by core_num.
    uint32_t data_per_core = TOTAL_LENGTH / core_num;

    // 3. Calculate the data offset of the current core in Global Memory.
    uint32_t offset = core_idx * data_per_core;

    // 4. Offset the global pointers to obtain the addresses of the data slices belonging to the current core.
    __gm__ float* x_local = x + offset;
    __gm__ float* y_local = y + offset;
    __gm__ float* z_local = z + offset;

    // 5. Subsequent logic: Each core independently processes its own data slice (e.g., move to UB, vector addition, write back, etc.).
    // op.Init(x_local, y_local, z_local, data_per_core);
    // op.Process();
}
```

Assume that we launch the preceding kernel function using add_kernel<<<8, 0, stream>>>(x, y, z); (that is, block_num = 8).

-   When the kernel with block_idx = 0 is executed, its offset = 0, and it processes the data at indexes 0 to 1023 in the global array.

-   When the kernel with block_idx = 1 is executed, its offset = 1024, and it processes the data at indexes 1024 to 2047 in the global array.

-   By analogy, the eight kernels complete the computation of 8192 data elements in full parallelism without interfering with each other. Through this sharding mechanism, block_idx implements a one-to-one mapping between software and hardware.