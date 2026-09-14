# CPU Domain Twin Debugging<a name="ZH-CN_TOPIC_0000001664002637"></a>

<!-- md-trans-meta sourceCommit=84d4771c758afa4058a229c863ddc58cf59e3dd3 translatedAt=2026-08-26T11:01:37.347Z pushedAt=2026-08-31T06:49:16.340Z -->

This section describes the methods for CPU domain debugging: verifying kernel functions on the CPU side, gdb debugging, and printing using the **printf** command. SIMT programming scenarios are not supported currently.

>[!NOTE] Note
>During CPU debugging, configure log-related environment variables to record the program execution process and exception information, which helps developers perform functional debugging.
>For usage constraints and detailed descriptions of environment variables, see the "Auxiliary Functions \> Logging" section in [*Environment Variable Reference*](https://www.hiascend.com/document/redirect/CannCommunityEnvRef).

## Verifying Kernel Functions on the CPU Side<a name="section1227643165914"></a>

On non-Ascend devices, developers can use the CPU simulation environment to develop and test operators first, and then use Ascend devices for accelerated computation once everything is ready. In the [Compilation and Execution](../../compilation_and_execution/compilation_and_execution.md) section, we have introduced the compilation and execution of the operator kernel program in the NPU domain. Compared with the execution logic of the NPU domain operator, CPU domain debugging compiles the operator kernel program as a host program. In this case, the operator kernel program links the CPU debugging library and executes the generated executable file to complete the CPU domain execution verification of the operator. The CPU-side program can be debugged step by step using the general-purpose GDB debugging tool to precisely verify whether the program execution flow meets expectations.

**Figure 1**  Comparison of kernel function execution logic between the CPU domain and NPU domain<a name="fig39851716019"></a>  
![](../../../figures/kernel_function_execution_logic_comparison_between_cpu_and_npu_domains.png "Comparison of kernel function execution logic between the CPU domain and NPU domain")

It is recommended to use the CMake compilation method, which enables the CPU domain twin debugging feature quickly with minimal modifications.

1.  To enable CPU domain debugging, include the **cpu\_debug\_launch.h** header file.

    In CPU debug mode, the BiSheng Compiler escapes the process of calling the kernel function through <<<\>\>\>, implementing the kernel function call in the CPU domain. The related call functions are defined in "cpu\_debug\_launch.h". In the source file that calls the kernel function using the <<<\>\>\> syntax, include the required header file as follows:

    ```
    #ifdef ASCENDC_CPU_DEBUG
    #include "cpu_debug_launch.h"
    #endif
    ```

2.  Pass the variables *CMAKE\_ASC\_RUN\_MODE* and *CMAKE\_ASC\_ARCHITECTURES* during the CMake configuration phase to enable CPU domain compilation. The command example is as follows:

    ```
    cmake -B build -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201
    ```

    cpu indicates enabling CPU domain compilation, and the value after dav- is the NPU architecture version number. Fill it in according to the actual situation by referring to the [mapping table](../../language_extension/SIMD-BuiltIn_keyword.md#table65291052154114).

    For other CMakeLists.txt project configurations, see [Compilation Using CMake](../../compilation_and_execution/operator_compilation/ai_core_operator_compilation_basic_usage.md#ZH-CN_TOPIC_0000002428982142).

>[!NOTE] Note
>To unify the CPU domain and NPU domain code, the framework adapts only some acl APIs in the CPU domain. When using the CPU domain debugging feature, developers can use only the following acl APIs, and linking the **ascendcl library** by users is not supported:
>- APIs with actual functions, supported for CPU domain calls
>    - aclDataTypeSize, aclFloat16ToFloat, aclFloatToFloat16.
>    - aclrtMalloc, aclrtFree, aclrtMallocHost, aclrtFreeHost, aclrtMemset, aclrtMemsetAsync, aclrtMemcpy, aclrtMemcpyAsync, aclrtMemcpy2d, aclrtMemcpy2dAsync, aclrtCreateContext, aclrtDestroyContext.
>- APIs without actual functions, implemented as stubs
>    - Profiling data collection
>        aclprofInit, aclprofSetConfig, aclprofStart, aclprofStop, aclprofFinalize.
>    - System configuration
>        aclInit, aclFinalize, aclrtGetVersion.
>    - Runtime management
>        aclrtSetDevice, aclrtResetDevice, aclrtCreateStream, aclrtCreateStreamWithConfig, aclrtDestroyStream, aclrtDestroyStreamForce, aclrtSynchronizeStream, aclrtCreateContext, aclrtDestroyContext.

> [!CAUTION] Caution
> In CPU twin debugging mode, the compiler does not recognize the [SIMD built-in keywords](../../language_extension/SIMD-BuiltIn_keyword.md) and [SIMT built-in keywords](../../language_extension/SIMT-BuiltIn_keyword.md) provided by Ascend C, including:
> - **Function execution space qualifiers**: such as `__aicore__`, `__global__`, `__host__`.
> - **Function marker macros**: such as `__simd_vf__`, `__simd_callee__`, `__simt_vf__`.
> - **Address space qualifiers**: such as `__gm__`, `__ubuf__`.
>
> These keywords are strongly related to device-side execution and do not need to take effect in CPU twin debugging mode. Therefore, the system framework defines these keywords as empty.
> Due to this empty definition, **function signatures that are distinguished only by built-in keywords are treated as identical**, which causes redefinition compilation errors.
>
> Typical problem scenarios include:
> 1. **A device-side function has the same name as a host-side function**
>
> <blockquote>
> In the following example, the `add` function is distinguished only by `__aicore__` for the execution domain. In CPU twin debugging mode, the two declarations conflict:
> 	<pre><code class="language-asc">__aicore__ inline void add(int x, int y);  // Device side
> void add(int x, int y);                    // Host side (equivalent to __host__)</code></pre>
> </blockquote>
>
> 2. **Parameter types differ only in address space qualifiers.**
>
> <blockquote>
> In the following example, the two device-side functions differ only in the address space qualifiers of the pointer:
> 	<pre><code class="language-asc">__aicore__ inline void process(__gm__ void* data, int size);
> __aicore__ inline void process(__ubuf__ void* data, int size); </code></pre>
> </blockquote>
>
> **Pay special attention in twin debugging scenarios**: Do not define functions that are distinguished only by built-in keywords.

## gdb Debugging<a name="section13838280458"></a>

You can use **gdb** to debug the operator computation precision step by step. Since CPU debugging has been converted to multi-process debugging, each core starts an independent child process. Therefore, gdb needs to be switched to child process debugging. For the coupled architecture, each AI Core starts one child process. For the decoupled architecture, each AI Core starts three child processes by default: one cube process and two vector processes.

-   Debugging a single child process

    Start gdb. In the example, add\_custom\_cpu is the operator executable file in the CPU domain. For details, see [Modifying and Executing the One-Click Compilation and Execution Script](../../appendix/completing_kernel_launch_based_on_sample_project.md#section188001652105215). Set **run-mode** in the one-click compilation and execution script to **cpu** to compile and generate the operator executable file in the CPU domain.

    After gdb starts, first set it to follow child processes, and then set a breakpoint. The execution will stop in the child process. However, this method only stops in the first child process that hits the breakpoint. The remaining child processes and the parent process continue to execute until they exit. Operators that involve inter-core synchronization cannot be debugged using this method.

    ```
    gdb --args add_custom_cpu  // Start gdb. add_custom_cpu is the operator executable file.
    (gdb) set follow-fork-mode child
    ```

-   Debugging multiple child processes

    If inter-core synchronization is involved, you need to debug multiple child processes simultaneously.

    After gdb starts, first set the debug mode to debug only one process and suspend other processes. The command is as follows:

    ```
    (gdb) set detach-on-fork off
    ```

    The command for viewing the current debug mode is as follows:

    ```
    (gdb) show detach-on-fork
    ```

    To interrupt the gdb program, use the event capture method, that is, the gdb program captures the fork event and interrupts. In this way, the gdb program can be interrupted each time a child process is started. The setting command is as follows:

    ```
    (gdb) catch fork
    ```

    After executing r, you can view the current process information:

    ```
    (gdb) info inferiors
      Num  Description
    * 1    process 19613
    ```

    As we can see, when fork is executed for the first time, the program stops at the fork position of the parent process, and the child process has not been generated yet.

    After executing c, run info inferiors again. You can see that the first child process has been started.

    ```
    (gdb) info inferiors
      Num  Description 
    * 1    process 19613
      2    process 19626
    ```

    At this point, you can switch to the second process, that is, the first child process, and set a breakpoint for debugging. The parent process is in the paused state:

    ```
    (gdb) inferior 2
    [Switching to inferior 2 [process 19626] ($HOME/demo)]
    (gdb) info inferiors
      Num  Description
      1    process 19613
    * 2    process 19626
    ```

    Note that the number following inferior is the sequence number of the process, not the process ID.

    If synchronous blocking occurs, you can switch back to the parent process to continue generating child processes, then switch to a new child process for debugging. After the synchronization condition is met, switch back to the first child process to continue execution.

The following is a sample command for debugging a single child process:

```
gdb --args add_custom_cpu
set follow-fork-mode child
break add_custom.cpp:45
run
list
backtrace
print i
break add_custom.cpp:56
continue
display xLocal
quit
```

## Printing with the printf Command<a name="section1829231615466"></a>

Write **printf(...)** directly in the code to observe the output values. The sample code is as follows:

```
printf("xLocal size: %d\n", xLocal.GetSize()); 
printf("tileLength: %d\n", tileLength);
```
