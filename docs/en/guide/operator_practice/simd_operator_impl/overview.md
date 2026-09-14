# Overview<a name="ZH-CN_TOPIC_0000002500548086"></a>

<!-- md-trans-meta sourceCommit=34c6d5a10e0caa815cf22c2db83ff5447311b0e0 translatedAt=2026-08-26T13:27:16.995Z pushedAt=2026-09-11T11:01:19.059Z -->

The operator implementation in Ascend C mainly consists of two parts:

-   Tiling implementation on the host side

    Because the internal storage of the AI Core on the NPU cannot fully accommodate all input and output data of an operator, a portion of the input data needs to be moved in for computation and then moved out each time, followed by moving in the next portion of input data for computation. This process is called Tiling. The algorithm for splitting data is called the Tiling algorithm or Tiling strategy. The computation program that determines the parameters related to the data splitting algorithm (such as the block size moved each time and the total number of loops) based on information such as the operator shape is called the Tiling implementation, also known as the Tiling Function. Since the Tiling implementation performs only scalar computations, which the AI Core is not good at, it is separated out and executed on the host-side CPU.

-   Kernel implementation on the device side

    The Kernel implementation is the operator kernel function implementation. Inside the Kernel function, the Tiling information is obtained by parsing the Tiling structure passed from the host side, and the process of moving data in and out of Local Memory is controlled based on the Tiling information. The operator logic is implemented by calling APIs for computation, data movement, memory management, and task synchronization. Its core logic is essentially computation-intensive tasks that need to be executed on the NPU.

This chapter describes the Tiling and Kernel implementation of operators in three typical scenarios: vector programming, matrix programming, and fused operator programming. It is a specific application of the typical [programming paradigm](../../programming_guide/programming_model/ai_core_simd_programming/overview.md) mentioned above, and also introduces more programming details and API usage methods.
