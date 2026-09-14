# Operator Precision Issue During Kernel Function Run Verification<a name="ZH-CN_TOPIC_0000001664188149"></a>

<!-- md-trans-meta sourceCommit=3ee506c06c64594337c3b6b8c8af85192bd95f75 translatedAt=2026-08-26T11:22:24.474Z pushedAt=2026-08-31T11:27:33.600Z -->

## Symptoms<a name="section151611254194612"></a>

During NPU domain run verification of the operator, the actual data is inconsistent with the ground truth data, indicating a precision issue in the operator.

## Root Cause<a name="section417961104715"></a>

A precision issue in an operator is generally caused by incorrect implementation logic of the operator.

## Troubleshooting<a name="section166318242419"></a>

Ascend C provides the twin debugging feature, which locates logic issues in the operator implementation through CPU domain functional verification, gdb single-step debugging, and printf value printing. This sample only demonstrates possible scenarios to facilitate the demonstration of the locating steps. In actual use, debug based on your code.

1.  Perform CPU domain functional verification and check whether any error is reported in the log.

    Refer to [Completing Kernel Launch Based on a Sample Project](../completing_kernel_launch_based_on_sample_project.md) to write the CPU-side run verification code and perform run verification. You will find that the precision comparison in the CPU domain is also inconsistent.

    Check the console log for error messages. You can search for the keyword "failed". For example, the error example in the following figure indicates that the error occurs where the LeakyRelu API is called in the code.

    ```
    leakyrelu_custom_cpu: /usr/local/Ascend/CANN-7.0/x86_64-linux/tikcpp/tikcfw/interface/kernel_operator_vec_binary_scalar_intf.h:447: void AscendC::LeakyRelu(const AscendC::LocalTensor<T>&, const AscendC::LocalTensor<T>&, const T&, const int32_t&) [with T = float16::Fp16T; int32_t = int]: Assertion `false && "check vlrelu instr failed"' failed
    ```

    The preceding error log generally only helps locate the code line where the error occurs, but cannot identify the specific error. Next, use gdb debugging or printf printing to further pinpoint the error.

2.  Perform gdb debugging. The following sample shows how to start the CPU-side running program of the leakyrelu operator. This sample program directly throws an exception. Run it directly in gdb and view the call stack information to analyze and locate the issue. In other scenarios, you can use basic gdb operations such as setting breakpoints for debugging. For details about debugging Ascend C programs with gdb, see [CPU Domain Twin Debugging](../../debugging_and_tuning/functional_debug/cpu_domain_twin_debugging.md).
    1.  Use gdb to start the program to be debugged and enter the gdb interface for debugging.

        ```
        gdb leakyrelu_custom_cpu
        ```

    2.  Debug a single child process.

        ```
        (gdb) set follow-fork-mode child
        ```

    3.  Run the program.

        ```
        (gdb) r
        ```

    4.  Use bt to view the program call stack.

        ```
        (gdb) bt
        ```

    5.  View the stack information of the specific layer and print the values of specific variables. In this example, tileLength is printed as 1024, which means that 1024 half-type numbers need to be processed in this program, with a size of 1024\*sizeof\(half\)=2048 bytes. For the value of the input tensor xLocal, dataLen indicates that the size of the LocalTensor is 1024 bytes, so only 1024 bytes of data can be computed. It can be seen that the lengths of the two do not match, which helps locate the issue.

        ```
        (gdb) f 5
        #5  0x000055555555d364 in KernelLeakyRelu::Compute (this=0x7fffffffd7d0, progress=0) at /root/AscendC_DemoCode-master/precision-error/vector/leakyrelu_custom.cpp:59
        59              LeakyRelu(yLocal, xLocal, scalar, tileLength);
        (gdb) p tileLength
        $1 = 1024
        (gdb) p xLocal
        $1 = {<AscendC::BaseTensor<float16::Fp16T>> = {<No data fields>}, address_ = {logicPos = 9 '\t', bufferHandle = 0x7fffffffd930 "\003\005\377\377", dataLen = 1024,bufferAddr = 0,absAddr = ...}
        ```

3.  Perform printf printing. Add variable printing at appropriate locations. The sample code is as follows:

    ```
    printf("xLocal size: %d\n", xLocal.GetSize());
    printf("tileLength: %d\n", tileLength);
    ```

    The following console log output is displayed: tileLength is printed as 1024, which means that 1024 half-type numbers need to be processed in this program. The size of the input tensor xLocal is 512, which means that only 512 half-type numbers can be computed. This shows that the lengths of the two do not match, which helps locate the issue.

    ```
    xLocal size: 512
    tileLength: 1024
    ```
