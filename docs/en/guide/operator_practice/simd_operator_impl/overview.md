# Overview<a name="ZH-CN_TOPIC_0000002500548086"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-09T10:34:04.915Z pushedAt=2026-11-19T06:32:19.762Z nl=2 -->

The operator implementation in Ascend C mainly consists of two parts:

-   Tiling implementation on the host side

    Because the internal storage of the AI Core on the NPU cannot fully accommodate all input and output data of an operator, a portion of the input data needs to be moved in for computation and then moved out each time, followed by moving in the next portion of input data for computation. This process is called tiling. The algorithm for splitting data is called the tiling algorithm or tiling strategy. The computation program that determines the parameters related to the data splitting algorithm (such as the block size moved each time and the total number of loops) based on information such as the operator shape is called the tiling implementation, also known as the tiling function. Since the tiling implementation involves only scalar computation, which the AI Core is not good at, it is separated out and executed on the host-side CPU.

-   Kernel implementation on the device side

    In the operator kernel function implementation, the tiling information is obtained by parsing the tiling structure passed from the host side, and the process of moving data in and out of Local Memory is controlled based on the tiling information. The operator logic is implemented by calling APIs for computation, data movement, memory management, and task synchronization. Its core logic is essentially computation-intensive tasks, which need to be executed on the NPU.

This chapter describes the tiling and kernel function implementation of operators in three typical scenarios: vector programming, cube programming, and fused operator programming. It is a concrete application of the typical [programming paradigm](../../programming_guide/programming_model/ai_core_simd_programming/overview.md) described above, and also introduces more programming details and the usage of APIs.
