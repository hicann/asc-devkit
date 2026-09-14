# Heterogeneous System<a name="ZH-CN_TOPIC_0000002394061301"></a>

<!-- md-trans-meta sourceCommit=198cbfdf29f72f78c8de735d88a5244dc258f2e7 translatedAt=2026-08-20T10:57:32.244Z pushedAt=2026-08-26T03:07:20.946Z -->

A heterogeneous system based on Ascend processors typically consists of a CPU and an Ascend NPU. The CPU and its memory are referred to as the host and host memory, while the NPU and its memory are referred to as the device and device memory.

An application based on Ascend typically consists of two parts: one part runs on the host CPU and is programmed using standard C/C++, and the other part runs on the NPU and is written in the Ascend C programming language. The code running on the NPU is called a [kernel function](./ai_core_simd_programming/kernel function.md), which must be called by the host code for execution. In addition, the host needs to use the CANN Runtime API to perform the following operations: copying data between host memory and device memory, launching the kernel function on the NPU, and waiting for the kernel function to complete execution. The host code and the kernel function can be written in the same .asc file and compiled heterogeneously by the BiSheng Compiler.

To improve compute efficiency, an NPU usually has multiple compute cores executing concurrently, with each core generally processing different data. Each compute core of the NPU is called an AI Core (the compute core of the AI processor). Traditionally, AI Cores follow the Single Instruction Multiple Data (SIMD) model, which achieves parallel compute by operating on multiple data elements with a single instruction. Starting from the Ascend 950PR/Ascend 950DT architecture, as a supplement to SIMD (mainly used to assist in the development of discrete vector operators), AI Cores also support the Single Instruction Multiple Thread (SIMT) model, which drives multiple threads to execute in parallel with a single instruction.

The following figure illustrates the process in which the code running on the host CPU dispatches the kernel function to the AI Core by calling the CANN Runtime API. Because host memory and device memory have independent memory spaces, the Runtime API must be explicitly called for data transfer. The typical process is as follows: the host code first transfers the input data from host memory to device memory, then launches the kernel function on the device and waits for its execution to complete, and finally copies the computation results from device memory back to host memory.


**Figure 1**  Kernel scheduling diagram<a name="fig573916102285"></a>  
<img src="../../figures/kernel_scheduling_diagram_2.png" alt="Kernel scheduling diagram" width="520px">


> [!NOTE] Note
> For details about task/stream and other Runtime management, see [*Runtime APIs*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).