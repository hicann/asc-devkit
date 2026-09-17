# Basic Vector Operator<a name="ZH-CN_TOPIC_0000002532228157"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T14:00:45.154Z -->

The following figure shows the implementation flow of the basic vector operator kernel function based on Ascend C.

**Figure 1**  Vector operator kernel function implementation flow<a name="zh-cn_topic_0000002201157438_fig16061570280"></a>  
![](../../../figures/vector_operator_kernel_function_implementation_flow.png "Vector operator kernel function implementation flow")

-   Operator analysis: Analyze the mathematical expression, input, output, and computation logic of the operator, and identify the Ascend C APIs to be called.
-   Kernel function definition: Define the entry function of the Ascend C operator.
-   Implement the operator class according to the [vector programming paradigm](../../../programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md#section116515238815): Complete the internal implementation of the kernel function, including three basic tasks: CopyIn, Compute, and CopyOut.

The following uses an Add operator as an example to describe the preceding steps in detail. In this example, the input is of the half data type, the last dimension of the shape is 32-byte aligned, the operator runs on a single core, and the computation is completed at one time.

## Operator Analysis<a name="zh-cn_topic_0000002201157438_section4870456573"></a>

The operator analysis procedure is as follows:

1.  Clarify the mathematical expression and computation logic of the operator.

    The mathematical expression of the Add operator is:

    ```
    z = x + y
    ```

    The computation logic is as follows: the operands of the [vector computation APIs](../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md) provided by Ascend C are all [LocalTensor](../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor.md). The input data must first be moved from the external storage (Global Memory) to the on-chip storage (Unified Buffer). Then, the computation API is used to add the two input parameters to obtain the final result, which is then moved back to the external storage. The computation logic of the Ascend C Add operator is shown in the following figure.

    **Figure 2**  Operator computation logic<a name="zh-cn_topic_0000002201157438_zh-cn_topic_0000001464091780_fig18937134755411"></a>  
    ![](../../../figures/operator_computation_logic.png "Operator computation logic")

2.  Clarify the input and output.
    -   The Add operator has two inputs: x and y, and one output: z.
    -   In this example, the operator input supports the half (float16) data type, and the output data type is the same as the input data type.
    -   The operator input supports the shape (1, 2048), and the output shape is the same as the input shape.
    -   The [format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md) supported by the operator input is ND.

3.  Determine the kernel function name and parameters.
    -   You can customize the kernel function name. In this example, the kernel function is named vec\_add\_custom.
    -   According to the analysis of the operator input and output, the kernel function has three parameters: x, y, and z. x and y are the memory addresses of the inputs in Global Memory, and z is the memory address of the output in Global Memory.

4.  Determine the APIs required for operator implementation.
    -   The implementation involves data movement between external storage and internal storage. Check the data movement APIs in the Ascend C API Reference. You need to use [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/overview/data_movement_concept.md) to implement data movement.
    -   This example involves only the addition operation of vector computation. Check the [vector computation](../../../../api/SIMD-API/basic_api/memory_vector_compute/memory_vector_computation.md) APIs in the Ascend C API Reference. A preliminary analysis shows that you can use the basic arithmetic Add API [Add](../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md) to implement x+y.
    -   Use [Queue](../../../../api/SIMD-API/basic_api/resource_management/TQue/tque_introduction.md) to manage the Tensor data structure used in computation. Specifically, use APIs such as [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) and [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md).

Based on the preceding analysis, the design specifications of the Ascend C Add operator are as follows:

-   Operator type (OpType): Add
-   Operator input and output:

    **Table 1**  Input and output specifications of the Add operator

    <a name="table4934296305"></a>
    <table><thead align="left"><tr id="row59358913304"><th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.1"><p id="p5503181819300"><a name="p5503181819300"></a><a name="p5503181819300"></a><strong id="b1850331853010"><a name="b1850331853010"></a><a name="b1850331853010"></a>name</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.2"><p id="p1550381833017"><a name="p1550381833017"></a><a name="p1550381833017"></a><strong id="b7503171811309"><a name="b7503171811309"></a><a name="b7503171811309"></a>shape</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.3"><p id="p1950391883014"><a name="p1950391883014"></a><a name="p1950391883014"></a><strong id="b2503111803020"><a name="b2503111803020"></a><a name="b2503111803020"></a>data type</strong></p>
    </th>
    <th class="cellrowborder" valign="top" width="25%" id="mcps1.2.5.1.4"><p id="p14503218133015"><a name="p14503218133015"></a><a name="p14503218133015"></a><strong id="b8503141818301"><a name="b8503141818301"></a><a name="b8503141818301"></a>format</strong></p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row393589203016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1950331810308"><a name="p1950331810308"></a><a name="p1950331810308"></a>x (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1950321863013"><a name="p1950321863013"></a><a name="p1950321863013"></a>(1, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p135031118173010"><a name="p135031118173010"></a><a name="p135031118173010"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p19503131815305"><a name="p19503131815305"></a><a name="p19503131815305"></a>ND</p>
    </td>
    </tr>
    <tr id="row6935119173013"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p75031182305"><a name="p75031182305"></a><a name="p75031182305"></a>y (input)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p16503111873010"><a name="p16503111873010"></a><a name="p16503111873010"></a>(1, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p45031818103018"><a name="p45031818103018"></a><a name="p45031818103018"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503918103012"><a name="p1503918103012"></a><a name="p1503918103012"></a>ND</p>
    </td>
    </tr>
    <tr id="row59354943016"><td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.1 "><p id="p1450316186305"><a name="p1450316186305"></a><a name="p1450316186305"></a>z (output)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.2 "><p id="p1150361853017"><a name="p1150361853017"></a><a name="p1150361853017"></a>(1, 2048)</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.3 "><p id="p15503918173017"><a name="p15503918173017"></a><a name="p15503918173017"></a>half</p>
    </td>
    <td class="cellrowborder" valign="top" width="25%" headers="mcps1.2.5.1.4 "><p id="p1503101813012"><a name="p1503101813012"></a><a name="p1503101813012"></a>ND</p>
    </td>
    </tr>
    </tbody>
    </table>

-   Kernel function name: vec\_add\_custom
-   Main APIs used:
    -   DataCopy: data movement API
    -   Add: basic vector arithmetic API
    -   EnQue, DeQue, and other APIs: queue management APIs

-   Operator implementation file name: vector_add.asc

## Kernel Function Definition<a name="zh-cn_topic_0000002201157438_section58723515714"></a>

Define the kernel function according to the rules described in [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md).

1.  Define the function prototype.

    In this example, the function is named vector_add_custom (the kernel function name can be customized). According to the analysis of the operator input and output in [Operator Analysis](#zh-cn_topic_0000002201157438_section4870456573), three parameters x, y, and z are determined, where x and y are input memory and z is output memory. According to the rules in [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md#zh-cn_topic_0000001447989210_section1915102519220), the function prototype is defined as follows: use the __global__ function type qualifier to identify it as a kernel function that can be called by <<<>>>; use the __vector__ function type qualifier to identify that the kernel function is executed on the Vector Core of the device-side AI Core; for convenience, use the GM_ADDR macro to modify the input parameters. For the definition of the GM_ADDR macro, see [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md#zh-cn_topic_0000001447989210_li94421027152419).

    ```
    __global__ __vector__ void vector_add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
    {
    }
    ```

2.  Call the Init and Process functions of the operator class.

    The Init function of the operator class completes the memory initialization related work, and the Process function implements the core logic of the operator implementation. For details, see [Operator Class Implementation](#zh-cn_topic_0000002201157438_section10423482111).

    ```
    __global__ __vector__ void vector_add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z)
    {
        AscendC::TPipe pipe;
        KernelAdd op;
        op.Init(x, y, z, &pipe);
        op.Process();
    }
    ```

3.  According to the [Kernel Function Definition and Invocation](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md#zh-cn_topic_0000001447989210_section1915102519220) section, when calling the kernel function, in addition to the parameters x, y, and z, you also need to pass numBlocks (the number of cores on which the kernel function is executed), the dynamic UB size (set to 0 when no dynamic UB is required), and stream (the stream that maintains the execution order of asynchronous operations in the application) to specify the execution configuration of the kernel function.

    ```
    vector_add_custom<<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice);
    ```

## Operator Class Implementation<a name="zh-cn_topic_0000002201157438_section10423482111"></a>

As described in the previous section, the kernel function calls the Init and Process functions of the operator class. This section details how to implement the operator class based on the programming paradigm.

The design of the Add operator implementation flow based on the vector programming paradigm is as follows. For details about the vector programming paradigm, see [Vector Programming Paradigm](../../../programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md#section116515238815). For the Add operator implementation flow obtained after the design, see [Figure 3 Add operator implementation flow](#zh-cn_topic_0000002201157438_fig4134406304):

-   The Add operator implementation flow consists of three basic tasks: CopyIn, Compute, and CopyOut. The CopyIn task is responsible for moving the input tensors xGm and yGm from Global Memory to Local Memory and storing them in xLocal and yLocal, respectively. The Compute task is responsible for performing the addition operation on xLocal and yLocal and storing the computation result in zLocal. The CopyOut task is responsible for moving the output data from zLocal to the output tensor zGm in Global Memory.
-   The CopyIn and Compute tasks are synchronized through the VECIN queues inQueueX and inQueueY, and the Compute and CopyOut tasks are synchronized through the VECOUT queue outQueueZ.
-   The memory used for inter-task interaction and the memory for temporary variables are uniformly managed by the [Pipe memory management object](../../../../api/SIMD-API/basic_api/resource_management/TPipe/TPipe.md).

**Figure 3**  Add operator implementation process<a name="zh-cn_topic_0000002201157438_fig4134406304"></a>  
![](../../../figures/add_operator_implementation_process.png "Add operator implementation process")

The operator class mainly implements the preceding flow. It includes the public Init function for initialization and the core processing function Process, which calls the three basic tasks shown in the preceding figure. It also includes some private members used in the operator implementation, such as the GlobalTensor objects (xGm, yGm, and zGm) and the VECIN and VECOUT queues shown in the preceding figure. The specific members of the KernelAdd operator class are as follows:

```
class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    // Initialization function that completes memory initialization related operations.
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AscendC::TPipe* pipeIn){}
    // Core processing function that implements the operator logic and calls the private member functions CopyIn, Compute, and CopyOut to complete the three-stage pipeline operations of the vector operator.
    __aicore__ inline void Process(){}

private:
    // Copy-in function that completes the CopyIn stage and is called by the core Process function.
    __aicore__ inline void CopyIn(){}
    // Compute function that completes the Compute stage and is called by the core Process function.
    __aicore__ inline void Compute(){}
    // Copy-out function that completes the CopyOut stage and is called by the core Process function.
    __aicore__ inline void CopyOut(){}

private:
    AscendC::TPipe* pipe;  // Pipe memory management object.
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueX;  // Input data queue management object, with TPosition set to VECIN.
    AscendC::TQue<AscendC::TPosition::VECIN, 1> inQueueY;  // Input data queue management object, with TPosition set to VECIN.
    AscendC::TQue<AscendC::TPosition::VECOUT, 1> outQueueZ;  // Output data queue management object, with TPosition set to VECOUT.
    AscendC::GlobalTensor<half> xGm;  // Object that manages the Global Memory addresses of the input and output, where xGm and yGm are inputs and zGm is the output.
    AscendC::GlobalTensor<half> yGm;
    AscendC::GlobalTensor<half> zGm;
};
```

The initialization function mainly completes the following:

-   Set the Global Memory addresses of the input and output Global Tensors.

    In this example, the allocation scheme is as follows: the total data length TOTAL\_LENGTH is 1 \* 2048. Use the [SetGlobalBuffer](../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/SetGlobalBuffer.md) API of the GlobalTensor class to set the start address and length of the Global Memory on this core.

    ```
    xGm.SetGlobalBuffer((__gm__ half *)x, TOTAL_LENGTH);
    ```

-   Allocate memory for the input and output queues through the [Pipe memory management object](../../../../api/SIMD-API/basic_api/resource_management/TPipe/TPipe.md).

    For example, to allocate memory for the queue of input x, use the following code segment:

    ```
    pipe->InitBuffer(inQueueX, 1, TOTAL_LENGTH * sizeof(half))
    ```

The specific initialization function code is as follows:

```
constexpr int32_t TOTAL_LENGTH = 1 * 2048;  // Total data length
__aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, AscendC::TPipe* pipeIn)
{
    pipe = pipeIn;    
    // Set the start address and length of the Global Memory
    xGm.SetGlobalBuffer((__gm__ half*)x, TOTAL_LENGTH);
    yGm.SetGlobalBuffer((__gm__ half*)y, TOTAL_LENGTH);
    zGm.SetGlobalBuffer((__gm__ half*)z, TOTAL_LENGTH);

    // Allocate memory for the input and output queues through the Pipe memory management object
    pipe->InitBuffer(inQueueX, 1, TOTAL_LENGTH * sizeof(half));
    pipe->InitBuffer(inQueueY, 1, TOTAL_LENGTH * sizeof(half));
    pipe->InitBuffer(outQueueZ, 1, TOTAL_LENGTH * sizeof(half));
}
```

Based on the vector programming paradigm, the kernel function implementation is divided into three basic tasks: CopyIn, Compute, and CopyOut. Then, according to the algorithm analysis in the programming paradigm, the entire computation is split into three stages. You write the code for each stage separately. For the three-stage flow diagram, see [Figure 3](#zh-cn_topic_0000002201157438_fig4134406304). The specific flow is as follows:

<a id="copyin-implementation"></a>
1.  Stage 1: CopyIn implementation.

    1.  Use [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/overview/data_movement_concept.md) to copy data from GlobalTensor to LocalTensor.
    2.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to put LocalTensor into the VECIN queue.

    ```
    // Allocate memory for LocalTensor from Que
    AscendC::LocalTensor<half> xLocal = inQueueX.AllocTensor<half>();
    AscendC::LocalTensor<half> yLocal = inQueueY.AllocTensor<half>();
    // Copy data from GlobalTensor to LocalTensor
    AscendC::DataCopy(xLocal, xGm, TOTAL_LENGTH);
    AscendC::DataCopy(yLocal, yGm, TOTAL_LENGTH);
    // Put LocalTensor into the VECIN queue
    inQueueX.EnQue(xLocal);
    inQueueY.EnQue(yLocal);
    ```


2.  Stage2: Compute implementation.

    <a id="copyout-implementation"></a>
    1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to take LocalTensor out of VECIN.
    2.  Use the Ascend C API [Add](../../../../api/SIMD-API/basic_api/memory_vector_compute/basic_arithmetic/Add.md) to complete the vector computation.
    3.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to put the LocalTensor of the computation result into the VECOUT queue.
    4.  Use [FreeTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/FreeTensor.md) to release the LocalTensor that is no longer in use.

    ```
    // Obtain Input from the VECIN queue
    AscendC::LocalTensor<half> xLocal = inQueueX.DeQue<half>();
    AscendC::LocalTensor<half> yLocal = inQueueY.DeQue<half>();
    AscendC::LocalTensor<half> zLocal = outQueueZ.AllocTensor<half>();
    // Call the Add operator for computation
    AscendC::Add(zLocal, xLocal, yLocal, TOTAL_LENGTH);
    // Place the computation result LocalTensor into the VECOUT queue
    outQueueZ.EnQue<half>(zLocal);
    // Release the LocalTensor
    inQueueX.FreeTensor(xLocal);
    inQueueY.FreeTensor(yLocal);
    ```

3.  Stage 3: CopyOut implementation.

    1.  Use the [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) API to obtain the LocalTensor from the VECOUT queue.
    2.  Use the [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/overview/data_movement_concept.md) API to copy the LocalTensor to the GlobalTensor.
    3.  Use [FreeTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/FreeTensor.md) to reclaim the LocalTensor that is no longer in use.

    ```
    // Obtain the computation result from the VECOUT queue
    AscendC::LocalTensor<half> zLocal = outQueueZ.DeQue<half>();
    // Copy the computation result from LocalTensor to GlobalTensor.
    AscendC::DataCopy(zGm, zLocal, TOTAL_LENGTH);
    // Release LocalTensor.
    outQueueZ.FreeTensor(zLocal);
    ```
