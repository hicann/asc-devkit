# TPipe and TQue Programming Paradigm<a name="ZH-CN_TOPIC_0000002465655445"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-28T09:11:06.040Z pushedAt=2026-11-03T07:08:03.562Z nl=2 -->

The previous chapter described the underlying design principles of the TPipe-TQue programming model: based on the classic C/C++ queue pipeline concept, an operator task is split into multiple pipeline stages, TQue queues are used to resolve read-after-write (RAW) data dependencies, AllocTensor/FreeTensor are used to resolve write-after-read (WAR) memory reuse conflicts, and underlying synchronization instructions are used to ensure timing safety for asynchronous parallelism across multiple units.

This chapter consolidates the principles described above into a standardized programming paradigm that can be directly coded. The previous chapter answered **why it is designed this way**, while this chapter focuses on **how to write code according to this design**. The essence of the TPipe-TQue programming paradigm is the engineering implementation of the queue pipeline concept on the AI Core. It encapsulates complex underlying logic such as pipeline splitting, queue synchronization, memory lifecycle management, and asynchronous parallel scheduling into a unified, fixed development process, helping developers quickly write standardized, pipelined, and high-performance operator kernels.

This paradigm defines a standardized, fixed process for implementing operator kernels. By strictly following this paradigm, developers can quickly build a stable and efficient operator code framework without manually writing underlying synchronization logic or handling data dependencies and timing blocking on their own.

From the perspective of hardware execution, the vector unit, cube unit, and data transfer unit inside the AI Core natively support asynchronous parallelism and pipeline execution. The TPipe-TQue programming paradigm fully aligns with this hardware characteristic, abstracting the multi-stage pipeline behavior of the hardware into a unified software-level **copy-in — compute — copy-out** three-stage pipeline development model.

The core logic of pipeline parallelism can be intuitively understood through a schematic diagram: a complete operator task is split into multiple ordered sub-stages, with different hardware units responsible for different operations. The same data slice must flow serially through each stage to ensure valid data dependencies, while different data slices can be processed simultaneously at different stages, achieving overlapped pipeline throughput. This is exactly consistent with the execution logic of classic C/C++ pipeline programs.

**Figure 1**  Pipeline parallelism schematic diagram<a name="fig3986112422"></a>  
![](../../../../figures/pipe_parallel.png "Pipeline parallelism schematic diagram")

Based on the discussion in the previous chapter, it can be concluded that the Ascend C pipeline system is jointly supported by two core modules:
- **[TPipe](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/TPipe.md)** resource manager: uniformly manages system memory resources and events used for synchronization.
- **[TQue](../../../../../api/SIMD-API/basic_api/resource_management/TQue/TQue.md)** queue: completes synchronization and communication between Stage tasks.

Based on this mechanism, Ascend C standardizes the pipeline programming paradigms for three types of operators: vector, cube, and fused. The following sections elaborate on each paradigm in combination with hardware characteristics and engineering examples.

-   **[Vector programming paradigm](./tpipe_tque_paradigm.md#section116515238815)**
-   **[Cube programming paradigm](./tpipe_tque_paradigm.md#section8213173433312)**
-   **[Fused operator programming paradigm](./tpipe_tque_paradigm.md#section57815481855)**

## Vector Programming Paradigm<a name="section116515238815"></a>

![](../../../../figures/mat_pos.png)

As shown in the preceding figure, the vector programming paradigm divides the operator implementation process into three basic tasks: CopyIn, Compute, and CopyOut.

- **CopyIn** is responsible for data copy-in: it moves input data from Global Memory to Local Memory (VECIN indicates the storage location of the data copied in for vector computation), and enqueues the data after the copy-in is complete;
- **Compute** is responsible for vector instruction computation: after dequeuing the data, it performs computation on Local Memory, and enqueues the result after the computation is complete;
- **CopyOut** is responsible for data copy-out: it retrieves the computation result from the queue and moves it from Local Memory (VECOUT indicates the storage location for data moved out by vector computation) back to Global Memory.

Both VECIN and VECOUT described above belong to the TPosition (logical position) concept. Ascend C expresses physical storage at different levels through the abstract TPosition, replacing the direct on-chip physical storage concept and hiding the hardware architecture. In addition to VECIN and VECOUT, VECCALC is also used in vector programming, typically when defining temporary variables.

From a programming perspective, the specific process (as shown in the following pseudocode) and the flowchart are as follows:

![](../../../../figures/vec_queue.png)

```
AscendC::TPipe pipe;                                // Create a global resource manager.
AscendC::TQue<AscendC::TPosition::VecIn, 1> queIn;  // Create a CopyIn-stage queue.
AscendC::TQue<AscendC::TPosition::VecOut, 1> queOut;// Create a CopyOut-stage queue.
// Init stage
pipe.InitBuffer(queIn, 2, 1024);                    // Enable DoubleBuffer to split the data to be processed into two parts for pipeline parallelism.
pipe.InitBuffer(queOut, 2, 1024);
for-loop {
    // CopyIn stage
    {
        auto tensor = queIn.AllocTensor<half>();   // Allocate resources from the queue, with a length of 1024.
        AscendC::DataCopy(tensor, gm, 1024);       // Copy data from GM to the Unified Buffer (UB, VECIN).
        queIn.EnQue(tensor);
    }
    // Compute stage
    {
        auto tensor = queIn.DeQue<half>();
        auto tensorOut = queOut.AllocTensor<half>();
        AscendC::Abs(tensorOut, tensor, 1024);    // Perform computation.
        queIn.FreeTensor(tensor);
        queOut.EnQue(tensorOut);
    }
    // CopyOut stage
    {
        auto tensor = queOut.DeQue<half>();
        AscendC::DataCopy(gmOut, tensor, 1024);   // Copy data from UB (VECOUT) to GM
        queOut.FreeTensor(tensor);                // Release resources
    }
}
```

The memory, events, and other resources used for data transfer between tasks are managed by TPipe. As shown in the following memory management diagram, TPipe provides the queue memory initialization capability through the [InitBuffer](../../../../../api/SIMD-API/basic_api/resource_management/TPipe/InitBuffer.md) API, allowing developers to allocate memory for a specified queue.

After the queue memory is initialized, call [AllocTensor](../../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) to allocate memory for a LocalTensor when memory is needed. When the LocalTensor is no longer used after the related computation is complete, call [FreeTensor](../../../../../api/SIMD-API/basic_api/resource_management/TQue/FreeTensor.md) to reclaim its memory.

**Figure 2** Memory management diagram<a name="fig375042942717"></a>  
![](../../../../figures/mem_mgmt.png "Memory management diagram")

The memory for temporary variables used during programming is also managed by TPipe. Temporary variables can use the TBuf data structure to allocate storage space at a specified TPosition. Memory allocated through TBuf can only be used for computation and cannot be enqueued to or dequeued from a queue. For details about the API usage, see [TBuf](../../../../../api/SIMD-API/basic_api/resource_management/TBuf/TBuf.md).

By following the preceding paradigm, you can implement parallel processing of data on a single core. The data to be processed is divided into multiple slices, and each parallel task processes all slices in sequence. The arrows between tasks indicate data dependencies. For example, Compute can start processing a slice only after CopyIn finishes processing that slice.

**Figure 3** Pipeline task diagram<a name="fig1556061818199"></a>  
![](../../../../figures/pipe_task.png "Pipeline task diagram")

The following figure shows the runtime sequence of the pipeline tasks in the preceding figure. As shown in the figure, for the same data slice, CopyIn, Compute, and CopyOut are dependent on each other and must be executed serially. For different data slices, multiple tasks can be processed in parallel at the same time, thereby improving overall performance through task-level parallelism.

**Figure 4** Pipeline task runtime diagram<a name="fig123244111202"></a>  
![](../../../../figures/pipe_task_run.png "Pipeline task runtime diagram")

## Cube Programming Paradigm<a name="section8213173433312"></a>

The following figure shows a typical data flow of cube computation:

![](../../../../figures/mat_pos_2.png)

Consistent with vector programming, cube programming also uses logical positions (TPosition) to express the data flow. The main TPosition definitions used are as follows:

- A1: logical memory used to store the left cube, physically corresponding to the L1 Buffer of the AI Core.
- **B1**: logical memory for storing the right cube, physically corresponding to the L1 Buffer of the AI Core.
- C1: logical memory for storing Bias data, physically corresponding to the L1 Buffer or UB of the AI Core.
- **A2**: logical memory for storing a small block of the left cube (split to fit the L0A Buffer capacity), physically corresponding to the L0A Buffer of the AI Core.
- **B2**: logical memory for storing a small block of the right cube (split to fit the L0B Buffer capacity), physically corresponding to the L0B Buffer of the AI Core.
- **C2**: logical memory for storing a small block of Bias data (split to fit the BT Buffer capacity), physically corresponding to the BT Buffer or L0C Buffer of the AI Core.
- CO1: logical memory used to store small blocks of cube computation results (such as the split result blocks), physically corresponding to the L0C Buffer of the AI Core.
- CO2: logical memory used to store the final cube computation results, physically corresponding to Global Memory or the UB of the AI Core.
- VECIN: logical memory used to store the input data for vector computation, physically corresponding to the UB of the AI Core.
- VECCALC: logical memory used to store temporary variables for vector computation, physically corresponding to the UB of the AI Core.
- VECOUT: logical memory used to store the output data of vector computation, physically corresponding to the UB of the AI Core.

The cube computation process can also be essentially summarized into three stages: CopyIn, Compute, and CopyOut. Because the process is relatively complex, the Matmul high-order API provides high-order encapsulation, which effectively simplifies the programming paradigm.

![](../../../../figures/mat_pos_3.png)

As shown in the figure: the CopyIn stage corresponds to the [SetTensorA](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA.md), [SetTensorB](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorB.md), and [SetBias](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias.md) APIs; the Compute stage corresponds to the [Iterate](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/Iterate.md) API; and the CopyOut stage corresponds to the [GetTensorC](../../../../../api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/GetTensorC.md) API. An example is as follows:

```
// Create a Matmul object. Pass in the parameter type information of A, B, C, and Bias.
// The type information is defined by MatmulType, including the logical memory location, data format, and data type.
typedef MatmulType<TPosition::GM, CubeFormat::ND, half> aType;
typedef MatmulType<TPosition::GM, CubeFormat::ND, half> bType;
typedef MatmulType<TPosition::GM, CubeFormat::ND, float> cType;
typedef MatmulType<TPosition::GM, CubeFormat::ND, float> biasType;
Matmul<aType, bType, cType, biasType> mm;

REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling); // Initialize
// CopyIn stage: Move data from GM to Local Memory
mm.SetTensorA(gm_a);    // Set left cube A
mm.SetTensorB(gm_b);    // Set right cube B
mm.SetBias(gm_bias);    // Set the bias
// Compute stage: Perform cube multiplication
while (mm.Iterate()) {
    // CopyOut stage: Move data from Local Memory to GM
    mm.GetTensorC(gm_c);
}
// End the cube multiplication operation
mm.End();
```

## Fused Operator Programming Paradigm<a name="section57815481855"></a>

Operators that support mixed vector and cube computation are called fused operators. Ascend C provides the **fused operator programming paradigm**, which helps developers express the data flow of fused operators based on this paradigm and quickly implement custom fused operators.

The **fused operator data flow** refers to the flow of its inputs and outputs among storage locations. Taking a typical Cube-and-Vector fused operator as an example, the data flow among logical positions is shown in the following figure (Bias is omitted for simplicity):

- The Cube output can be used as the Vector input: CO2→VECIN
- The Vector output can also be used as the Cube input: VECOUT→A1→A2, VECOUT→B1→B2

![](../../../../figures/mat_pos_4.png)

The fused operator programming paradigm based on the Matmul advanced API simplifies the preceding data flow into the following steps:

**Figure 5** Fused operator programming paradigm<a name="fig321783243811"></a>  
![](../../../../figures/fusion_paradigm.png "Fused operator programming paradigm")

1. Initialize the Matmul object and move the input data from Global Memory to the Cube core.
2. Execute the internal Matmul computation.
3. Copy the Matmul computation result to the Vector core.
4. Perform vector computation.
5. Copy the output result back to Global Memory.

The example code is as follows (pseudocode):

```
template<typename aType, typename bType, typename cType, typename biasType>
__aicore__ inline void MatmulLeakyKernel<aType, bType, cType, biasType>::Process()
{
    // Step 1: Initialize the MatMul object and move the input data from Global Memory to the Cube core
    uint32_t computeRound = 0;
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj);
    matmulObj.Init(&tiling);
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);

    while (matmulObj.template Iterate<true>()) { // Step 2: Execute the internal MatMul computation.
        // Step 3: Move the MatMul computation result to the Vector core.
        reluOutLocal = reluOutQueue_.AllocTensor<cType>();
        matmulObj.template GetTensorC<true>(reluOutLocal, false, true);
        // Step 4: Execute the Vector computation.
        AscendC::LeakyRelu(reluOutLocal, reluOutLocal, (cType)alpha, tiling.baseM * tiling.baseN);
        reluOutQueue_.EnQue(reluOutLocal);
        // Step 5: Move the output result back to Global Memory.
        reluOutQueue_.DeQue<cType>();
        ...
        AscendC::DataCopy(cGlobal[startOffset], reluOutLocal, copyParam);
        reluOutQueue_.FreeTensor(reluOutLocal);

        computeRound++;
    }
    matmulObj.End();
}
```

## Summary
This chapter fully builds on the queue pipeline fundamentals introduced in the previous chapter, turning the theoretical stage pipeline decomposition, data dependencies, and synchronization into a unified engineering programming paradigm for Ascend C. Whether it is a simple vector operator, a high-density cube operator, or a complex fused operator, its essence is a different form of the classic C/C++ queue pipeline model implemented on AI Core heterogeneous hardware: through standardized three-stage pipelining, queue-based data synchronization, and unified resource management, the underlying complex parallel timing and memory conflict issues are hidden, allowing developers to efficiently develop high-performance operators with a unified, concise, and reusable pipeline mindset.
