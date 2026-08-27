# SIMT Stack Overflow Scenarios and Optimization Example

## Overview

This example uses SIMT stack overflow caused by defining large array variables in a kernel and by recursive function calls to describe how to locate and optimize stack overflow issues in Ascend C SIMT programming.

This example contains two cases:

- Case 1: A large array in stack space causes stack overflow. You can increase the stack space through a configuration file, but using UB to store a large amount of temporary data is recommended.
- Case 2: When recursive functions are used, an excessively large recursion depth causes stack overflow. You need to control the recursion depth or use an iterative algorithm instead of a recursive algorithm.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── stack_overflow
│   ├── CMakeLists.txt          // Example build script
│   ├── stack_overflow.asc      // SIMT stack overflow reproduction and optimization example
│   ├── acl.json                // aclInit configuration file used to configure the SIMT stack size
│   ├── README_en.md
│   └── README.md
```

## Example Description

This example contains two typical stack overflow scenarios, and reproduces and optimizes SIMT stack overflow issues respectively.

**Case description**:

| Case   | Core Issue                              | Corresponding Kernel                                                      | Solution                                             |
|--------|-----------------------------------------|---------------------------------------------------------------------------|------------------------------------------------------|
| Case 1 | Storing excessively large data on stack | simt_stack_overflow                                                       | Increase the stack size through a configuration file |
| Case 2 | Excessive recursive function call depth | simt_recursive_stack_overflow/simt_recursive_stack_overflow_optimized     | Use an iterative algorithm instead of recursion      |

> **Stack overflow issue description**
>
> The default stack space of SIMT threads is limited. When a large thread-local array is defined in a kernel function, or when the recursive call depth is too large, the required SIMT stack space keeps increasing. If the runtime stack requirement exceeds the configured upper limit, SIMT stack overflow is triggered. For details about stack space, see [Memory Hierarchy](../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simt_programming/memory_hierarchy.md).

**Example specifications**:

<table>
<tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">SimtStackOverflow</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
<tr><td align="center">input</td><td align="center">[2048]</td><td align="center">int32_t</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[2048]</td><td align="center">float</td></tr>
<tr><td rowspan="1" align="center">Kernel Name</td><td colspan="3" align="center">simt_stack_overflow/simt_recursive_stack_overflow/simt_recursive_stack_overflow_optimized</td></tr>
</table>

## Example Implementation

This section analyzes two types of SIMT stack overflow issues: large local arrays and recursive calls. Each case includes an overflow scenario and an optimized scenario, and provides the corresponding core implementation, error locating method, and optimization suggestions.

---

### Case 1: Large Local Array Stack Overflow and Stack Size Configuration

**Example goal**: Reproduce SIMT stack overflow caused by using a large array in stack space, and verify the fix by configuring a larger SIMT stack space.

**Core implementation**: Use the kernel function `simt_stack_overflow` to allocate the large array `float local_buffer[320]` on the stack of each thread. The array occupies `320 * 4B = 1280B` per thread.

```cpp
__global__ void simt_stack_overflow(int32_t* input, float* output, uint32_t total_length)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= total_length) {
        return;
    }
    float local_buffer[BUFFER_SIZE];

    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        local_buffer[i] = i + 1;
    }

    float sum = 1.0f;
    for (uint32_t i = 0; i < input[idx]; i++) {
        sum *= local_buffer[i];
    }
    output[idx] = sum;
}
```

**Execution result**:

- Accuracy exception occurs, and `[Failed] Case accuracy verification failed!` is displayed.

**Initial issue locating through log information**:

- On the host side, call `aclGetRecentErrMsg()` after the `aclrtSynchronizeStream()` API to obtain the error information on the device side. The implementation is as follows:

    ```cpp
    const char* err = aclGetRecentErrMsg();
    if (err != nullptr) {
        fprintf(stderr, "[Host] kernel error: %s\n", err);
    } else {
        printf("[Host] kernel completed.\n");
    }
    ```

    Compile and run the example again. The printed logs are as follows:

    ```text
    The extend info: errcode:(354) errorStr: The VEC SIMT stack overflows. Possible cause: The local variable is too large or there are too many local variables. subErrType: 0x4.
    ...
    [DFX_INFO]AI Core kernel execution failed, device_id=0, stream_id=61, report_stream_id=61, task_id=0, flip_num=0, fault kernel_name=_Z19simt_stack_overflowPiPfj, fault kernel info ext=_Z19simt_stack_overflowPiPfj, program id=0, hash=136201840501799437.[FUNC:GetError][FILE:stream.cc][LINE:1750]
    ...
    ```

    The key error information in the printed logs is `errorStr: The VEC SIMT stack overflows`, indicating that stack overflow occurs during kernel execution and the kernel exits abnormally.

**Verifying the stack overflow issue through compile options**

- To verify stack overflow, add `--cce-res-usage` to the compile options to obtain the stack space usage calculated by the compiler. Recompile the example. The following compile log is displayed:

    ```text
    [BISHENG] Function properties for _Z19simt_stack_overflowPiPfj_simt_entry: Stack size: 1280 bytes, Used register number: 16
    ```

    `Stack size: 1280 bytes` in the log indicates that the stack space required by the kernel variables is `1280B`. The default SIMT stack space is `1152B`, confirming that stack overflow occurs.

**Code issue analysis**:

- After re-examining the code, the other function calls, calculation logic, and variable definitions are not complex. Only the large array `float local_buffer[320]` defined on the stack occupies 1280B, which is the most likely cause of stack overflow.

**Solution**:

- Solution 1: If a large array variable is suitable for UB space, move it to UB to reduce stack usage and allow sharing between threads.
- Solution 2: If a large amount of temporary data must be stored in stack space, increase the SIMT stack space to meet the stack space requirement of the kernel and resolve the stack overflow issue.

   You can configure the SIMT stack size through the `simt_stack_size` field in the `aclInit("acl.json")` API. The following is a code example:

    ```cpp
    aclInit("acl.json");
    ```

    ```json
    {
        "StackSize": {
            "simt_stack_size": 1280,
            "simt_divergence_stack_size": 512
        }
    }
    ```

    For more configuration details, see the "SIMT operator stack space size configuration example" section in the [aclInit](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit) API. The stack space size configuration field is `simt_stack_size`, and `simt_stack_size` specifies the stack space size of each thread of the SIMT operator.

    > [!NOTE]Note
    > The default SIMT stack space is 1152B. In this example, the stack space of each SIMT thread is configured to 1280B. The maximum number of threads per Thread Block is 2048, and the maximum number of physical cores that can concurrently execute Thread Blocks in the test environment of this example is 72. Therefore, compared with the default configuration, the additional GM space occupied is `(1280B - 1152B) * 2048 * 72 = 18MB`.

**Conclusion**:

- Optimize large arrays: When implementing a kernel, avoid placing large arrays, large structs, and similar data on the thread stack. You can store large arrays and large structs in UB space to reduce stack usage.
- Increase SIMT stack space: Configuring the SIMT stack size sharply increases GM usage, so evaluate it together with the service scale.

### Case 2: Recursive Stack Overflow and Iterative Optimization

**Example goal**: Reproduce SIMT stack overflow caused by recursive calls, and verify the fix with an iterative implementation.

**Core implementation**: Use a recursive algorithm to calculate factorial.

```cpp
__aicore__ float recursive_stack_stage(int32_t value)
{
    if (value < 1) {
        return 1.0f;
    }
    return recursive_stack_stage(value - 1) * value;
}

__global__ void simt_recursive_stack_overflow(int32_t* input, float* output, uint32_t total_length)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= total_length) {
        return;
    }
    output[idx] = recursive_stack_stage(input[idx]);
}
```

**Execution result**:

- Accuracy exception occurs, and `[Failed] Case accuracy verification failed!` is displayed.

**Initial issue locating through log information**:

- On the host side, call `aclGetRecentErrMsg()` after the `aclrtSynchronizeStream()` API to obtain the error information on the device side. The implementation is as follows:

    ```cpp
    const char* err = aclGetRecentErrMsg();
    if (err != nullptr) {
        fprintf(stderr, "[Host] kernel error: %s\n", err);
    } else {
        printf("[Host] kernel completed.\n");
    }
    ```

    Compile and run the example again. The printed logs are as follows:

    ```text
    The extend info: errcode:(356) errorStr: VEC SIMT DVG stack overflows, which may be caused by too many conditional branches or too manay nested loops. subErrType: 0x4.
    ...
    [DFX_INFO]AI Core kernel execution failed, device_id=0, stream_id=61, report_stream_id=61, task_id=0, flip_num=0, fault kernel_name=_Z29simt_recursive_stack_overflowPiPfj, fault kernel info ext=_Z29simt_recursive_stack_overflowPiPfj, program id=0, hash=136201840501799437.
    ...
    ```

    The key error information in the printed logs is `errorStr: VEC SIMT DVG stack overflows`, indicating that divergence stack overflow occurs during kernel execution and the kernel exits abnormally.

**Confirming the issue by simplifying the code**:

- Delete the condition branch in the recursive function. The code is as follows:

    ```cpp
    __aicore__ float recursive_stack_stage(int32_t value)
    {
        return recursive_stack_stage(value - 1) * value;
    }
    ```

    Compile and run the example again. The printed logs are as follows:

    ```text
    The extend info: errcode:(354) errorStr: The VEC SIMT stack overflows. Possible cause: The local variable is too large or there are too many local variables. subErrType: 0x4.
    ...
    [DFX_INFO]AI Core kernel execution failed, device_id=0, stream_id=61, report_stream_id=61, task_id=0, flip_num=0, fault kernel_name=_Z29simt_recursive_stack_overflowPiPfj, fault kernel info ext=_Z29simt_recursive_stack_overflowPiPfj, program id=0, hash=11904994319956742437.[FUNC:GetError][FILE:stream.cc][LINE:1750]
    ...
    ```

    The key error information in the printed logs is `errorStr: The VEC SIMT stack overflows`, indicating that stack overflow occurs during kernel execution and the kernel exits abnormally.

**Code issue analysis**:

- After re-examining the code, the kernel does not contain much logic except the recursive condition check and recursive call. Therefore, the `if` branches are nested in the recursive scenario, causing divergence stack overflow.
- When the condition branch in the recursive function is deleted, stack overflow occurs. This indicates that the recursive scenario causes both the general stack and the divergence stack to overflow.

**Solution**:

- Solution 1: Change the recursive algorithm to an iterative algorithm to reduce the requirements for SIMT divergence stack space and SIMT stack space.

    Use an iterative algorithm to calculate factorial. The code is as follows:

    ```cpp
    __aicore__ float recursive_stack_stage_optimized(int32_t value)
    {
        float result = 1.0f;
        for (int32_t i = 2; i <= value; ++i) {
            result *= i;
        }
        return result;
    }

    __global__ void simt_recursive_stack_overflow_optimized(int32_t* input, float* output, uint32_t total_length)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= total_length) {
            return;
        }
        output[idx] = recursive_stack_stage_optimized(input[idx]);
    }
    ```

- Solution 2: If recursive calls must be used, increase the SIMT divergence stack space and stack space to meet the kernel requirements for SIMT divergence stack space and SIMT stack space, while strictly controlling the recursion depth.

    For more configuration details, see the "SIMT operator stack space size configuration example" section in the [aclInit](https://hiascend.com/document/redirect/CannCommunityruntimeapiaclinit) API. The SIMT divergence stack space configuration field is `simt_divergence_stack_size`, and the stack space size configuration field of each thread of the SIMT operator is `simt_stack_size`.

> [!NOTE]Note
> Recursive calls require excessive stack space. Recursive calls are not recommended in actual development scenarios.

**Conclusion**:

- Optimize recursive algorithms: When implementing a kernel, avoid recursive calls. Prefer changing recursive methods to iterative methods to reduce stack space and divergence stack usage.
- Increase SIMT divergence stack space and stack space: Configuring the SIMT divergence stack size and stack size increases GM usage, so evaluate it together with the service scale.

## Optimization Suggestions

| Scenario | Root Cause | Optimization Method |
|----------|------------|---------------------|
| Large local array | A large thread-local array is defined in the kernel | Prefer moving it to UB space or reducing the local array size. If a large stack space is required, customize the SIMT stack size (not recommended) |
| Recursive call | Excessive recursion depth causes stack frames to accumulate continuously | Prefer an iterative implementation. Increase the SIMT divergence stack size and stack size (not recommended) |
| Excessive temporary variables | Too many temporary variables are defined | Reduce temporary variables. Use address references instead of temporary variables where possible to reduce register usage |
| Excessive branches | The function call hierarchy is too deep | Optimize branch processing logic and reduce branch jumps within a warp |
| Complex struct | A complex struct is passed in | For complex structs, plan UB or GM memory to store struct data and avoid passing excessive amounts of data |

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure environment variables.

    Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

    ```bash
    source ${install_path}/cann/set_env.sh
    ```

  > **Note**: `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Build and run the example.

    ```bash
    SCENARIO_NUM=1                       # Select the execution scenario, options 1-4
    mkdir -p build && cd build;          # Create and enter the build directory
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
    ./demo                               # Run the example
    ```

**Build options**:

| Option                    | Valid Value | Description |
|---------------------------|-------------|-------------|
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510`  | NPU architecture. This example supports only dav-3510 (Ascend 950PR/Ascend 950DT) |
| `SCENARIO_NUM`            | `1-4`       | Example scenario number. The default value is 1. 1: overflow scenario of Case 1; 2: optimized scenario of Case 1; 3: overflow scenario of Case 2; 4: optimized scenario of Case 2 |

**Execution result example**:

- For the stack overflow scenario of Case 1, the following error information appears in the logs:

    ```text
    The extend info: errcode:(354) errorStr: The VEC SIMT stack overflows. Possible cause: The local variable is too large or there are too many local variables. subErrType: 0x4.
    ```

- For the stack overflow scenario of Case 2, the following error information appears in the logs:

    ```text
    The extend info: errcode:(356) errorStr: VEC SIMT DVG stack overflows, which may be caused by too many conditional branches or too manay nested loops. subErrType: 0x4.
    ```

- For the optimization verification scenarios of Case 1 and Case 2, the following information is displayed after successful execution:

    ```text
    test pass!
    ```
