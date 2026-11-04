# Kernel Function<a name="ZH-CN_TOPIC_0000001564710756"></a>

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-28T08:37:53.108Z pushedAt=2026-11-03T07:08:03.552Z nl=2 -->

A kernel function (Kernel) is a C/C++ function that executes in parallel on the device side. It is called by host-side code. When called, it is instantiated into multiple parallel copies, which are scheduled to run on multiple AI Cores (each AI Core is abstracted as a Block). These Blocks leverage the computing power of AI Cores to collaboratively complete the computation of an operator.


## Kernel Definition<a name="zh-cn_topic_0000001447989210_section1915102519220"></a>

The following rules must be followed when defining a kernel.

-   **Use function type qualifiers and function execution space qualifiers**

    In addition to declaring the kernel in the same way as a C/C++ function, you must add extra function type qualifiers to the kernel.

    You must use the \_\_global\_\_ function type qualifier to identify it as a kernel that can be called by <<<...\>\>\>. Meanwhile, select one of the following function execution space qualifiers based on the hardware unit on which the operator actually runs. For details, see [Function Execution Space Qualifiers](../../language_extension/simd_builtin_keywords.md#section1074418132518):
    -   \_\_aicore\_\_: identifies that the kernel executes on the AI core on the device side without distinguishing the specific core type (usually used in [coupled mode](../../advanced_programming/hardware_implementation/basic_architecture.md#section1574769433)).
    -   \_\_vector\_\_: identifies that the kernel executes on the Vector core on the device side, applicable to operators that contain only Vector computation.
    -   \_\_cube\_\_: identifies that the kernel executes on the Cube core on the device side, applicable to operators that contain only Cube computation.
    -   \_\_mix\_\_: identifies that the kernel executes on both the Cube core and the Vector core, applicable to operators that contain fused Vector and Cube computation.

-   **Use variable type qualifiers**

    Pointer input parameter variables need to add a variable type qualifier \_\_gm\_\_, indicating that the pointer variable points to a memory address in Global Memory.

-   **Other Rules**<a name="zh-cn_topic_0000001447989210_li94421027152419"></a>
    1.  Rule: A kernel function (Kernel) must have a void return type.
    2.  Rule: The parameter list of a kernel function (Kernel) must comply with [Function Parameter List Restrictions](../../../technical_appendix/cpp_standard_support/syntax_restrictions/functions.md#section_kernel_function_argument_list_constraint).

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

In Ascend C operator programming, functions are strictly classified into three categories based on their code execution space. The call relationships among them follow these rules:

-   Host-side execution function: runs on the host (CPU). In addition to supporting mutual calls between functions of the same type under standard C/C++ semantics, its core role is to act as the initiator of computation tasks. It delivers the execution configuration through the kernel call operator <<<...\>\>\> and calls the kernel function on the device side across the boundary.
-   Kernel function: runs on the device side (NPU) and serves as the execution entry point for tasks delivered from the host side. When organizing parallel computation logic, a kernel function can call other device-side execution functions downward.
-   Device-side execution function: an ordinary function (other than the kernel function) that runs on the device side (NPU). This type of function is mainly used to encapsulate and reuse underlying computation logic. Its call permission is strictly limited to the device side, and it can only be called by a kernel function or by a device-side execution function of the same type.

**Figure 1** Call relationships among kernel functions, host-side execution functions, and device-side execution functions<a name="fig321723224511"></a><br>
![](../../../figures/kernel_call.png "Call relationships among kernel functions, host-side execution functions, and device-side execution functions")

The host side calls a kernel function using the kernel call operator <<<...>>> syntax, as shown below:

```
kernel_name<<<numBlocks, dynUBufSize, stream>>>(argument list);
```

The parameters in `<<<...>>>` are the execution configuration of the kernel function, which is determined by three parameters. For details, see [Kernel Function Configuration](../../language_extension/simd_builtin_keywords.md#kernel-configuration):

-   numBlocks: specifies the number of cores on which the kernel function will execute. For different hardware architectures and operator types, the setting rules of numBlocks differ. For details, see [Kernel Function Configuration](../../language_extension/simd_builtin_keywords.md#kernel-configuration). Each core that executes the kernel function is assigned a logical ID, that is, block\_idx, which can be obtained using the built-in variable [block_idx](../../language_extension/simd_builtin_keywords.md#built-in-variables) in the kernel function implementation. Note that when the `__mix__` function execution space qualifier is used, the block_idx values of the Vector Cores on the same core are the same. For details and examples, see [Built-in Variables](../../language_extension/simd_builtin_keywords.md#built-in-variables);
-   dynUBufSize: Dynamic Unified Buffer (UB) Size, which configures the size of the space for UB dynamic memory allocation (UB only, excluding L1 Buffer and others), in bytes. The default value is 0;
-   stream: of the aclrtStream type. stream is used to maintain the execution order of some asynchronous operations, ensuring that they are executed on the device in the order in which they are called in the application code. The default value is nullptr. For stream creation and other management APIs, see [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).

The following kernel function named add\_custom implements the addition of two vectors. The call example is as follows:

```
// numBlocks is set to 8: Indicates that the add_kernel function is invoked on 8 cores. Each core will execute the kernel independently and in parallel.
// dynUBufSize is set to 0: Indicates that no dynamic memory space is allocated for the UB. The argument list for the kernel is x, y, z.
add_kernel<<<8, 0, stream>>>(x, y, z);

// The invocation format for a template kernel function is as follows:
// 1. Process Float32 vector addition.
add_kernel<float><<<8, 0, stream>>>(x_float, y_float, z_float);
// 2. Simply change the template parameter to int32_t to perform integer computation.
add_kernel<int32_t><<<8, 0, stream>>>(x_int, y_int, z_int);
```

The call of a kernel function is asynchronous. After the call of the kernel function ends, control is immediately returned to the host. You can call the following **aclrtSynchronizeStream** function to force the host program to wait until all kernel functions have finished executing.

```
aclError aclrtSynchronizeStream(aclrtStream stream);
```

For details about how to use aclrtSynchronizeStream, see [*Runtime API*](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).

## Blocks Index Built-in Variables

After multi-core parallel computing is started through <<<numBlocks, ...>>>, multiple AI Cores execute the same kernel function (Kernel) code simultaneously. To prevent multiple cores from repeatedly processing the same data block, you need to partition the global data in the code.

Ascend C provides the built-in variables block_idx and block_num to help each core precisely locate the data slice it needs to process:
-   block_num: gets the total number of logical cores allocated to the current kernel function (Kernel) (its value equals numBlocks configured during the kernel call).
-   block_idx: gets the logical core ID that is currently executing the code, with a value range of [0, block_num - 1].

In the Mix scenario (using the `__mix__` function execution space qualifier), the Vector core (AIV) also needs to use sub_block_idx to determine its position within the combination:
-   sub_block_num: gets the number of Vector cores in the current combination (that is, the number of AIVs in each combination).
-   sub_block_idx: gets the index of the current Vector core within the combination, with a value range of [0, sub_block_num - 1].

In the Mix scenario, the complete logical position of an AIV core is determined by block_idx and sub_block_idx together: `logic_idx = block_idx * sub_block_num + sub_block_idx`. Developers should use the [GetBlockIdx](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) and [GetSubBlockIdx](../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) APIs in combination to obtain the logical position, rather than directly using the built-in variables (for more details, see the notes in [built-in variables](../../language_extension/simd_builtin_keywords.md#built-in-variables)).

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
Assume that we launch the preceding kernel function (Kernel) by using add_kernel<<<8, 0, stream>>>(x, y, z); (that is, block_num = 8).
-   When the core with block_idx = 0 executes, its offset = 0, and it processes the data with indexes from 0 to 1023 in the global array.
-   When the core with block_idx = 1 executes, its offset = 1024, and it processes the data with indexes from 1024 to 2047 in the global array.
-   By analogy, the eight cores complete the computation of 8192 data elements in full parallel without interfering with each other. Through this sharding mechanism, block_idx implements a one-to-one mapping between software and hardware.
