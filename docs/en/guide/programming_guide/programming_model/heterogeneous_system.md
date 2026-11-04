# Heterogeneous System<a name="ZH-CN_TOPIC_0000002394061301"></a>

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-28T08:32:09.353Z pushedAt=2026-11-03T07:08:03.547Z nl=2 -->

A heterogeneous system based on Ascend processors typically consists of a CPU and an Ascend NPU. The CPU and its memory are referred to as the host and host memory, while the NPU and its memory are referred to as the device and device memory.

An Ascend-based application typically consists of two parts: one part runs on the host CPU and is programmed in standard C/C++, and the other part runs on the NPU and is written in the Ascend C programming language. The code running on the NPU is called a [kernel function](./ai_core_simd_programming/kernel_function.md), which must be invoked by the host code for execution. In addition, the host needs to use the CANN Runtime API to perform the following operations: copying data between host memory and device memory, launching the kernel function (Kernel) on the NPU, and waiting for the kernel function (Kernel) to complete execution. The host code and the kernel function (Kernel) can be written in the same .asc file and compiled heterogeneously by the BiSheng Compiler.

To improve computing efficiency, an NPU usually has multiple computing cores executing concurrently, with each core generally processing different data. Each computing core of the NPU is called an AI Core (the computing core of the AI processor). Traditionally, the AI Core follows the SIMD (Single Instruction Multiple Data) model, which uses a single instruction to operate on multiple data elements simultaneously to achieve parallel computing. Starting from the Ascend 950PR&950DT product architecture, as a supplement to SIMD (mainly used to assist the development of discrete vector operators), the AI Core also supports the SIMT (Single Instruction Multiple Thread) model, which uses a single instruction to drive multiple threads to execute in parallel.

The following figure illustrates the process in which the code running on the host CPU dispatches the kernel function (Kernel) to the AI Core by calling the CANN Runtime API. Because host memory and device memory have independent memory spaces, the Runtime API must be explicitly called for data transfer. The typical process is as follows: the host code first transfers the input data from host memory to device memory, then launches the kernel function (Kernel) on the device and waits for its execution to complete, and finally copies the computation results from device memory back to host memory.


**Figure 1**  Kernel function scheduling diagram<a name="fig573916102285"></a><br>
<img src="../../figures/kernel_sched_2.png" alt="Kernel function scheduling diagram" width="520px">


>[!NOTE] Note
> For details about Runtime management such as Task and Stream, see [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).
