# Coupled Mode<a name="ZH-CN_TOPIC_0000002500468248"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-26T13:47:01.988Z -->

>[!NOTE] Note
>This section provides programming guidance for matrix multiplication using basic APIs in [coupled mode](../../../programming_guide/advanced_programming/hardware_implementation/basic_architecture.md#section1574769433).

## Programming Paradigm<a name="zh-cn_topic_0000002135641293_section12567050132819"></a>

The Cube programming paradigm divides the operator implementation process into five basic tasks: CopyIn, Split, Compute, Aggregate, and CopyOut. CopyIn is responsible for the copy-in operation, Split for the data splitting operation, Compute for the Cube instruction computation operation, Aggregate for the data aggregation operation, and CopyOut for the copy-out operation.

**Figure 1**  Basic task design of matrix programming<a name="zh-cn_topic_0000002135641293_fig382344282315"></a>  
![](../../../figures/matrix_programming_basic_task_design.png "Basic task design of matrix programming")

The interaction process and flowchart between specific tasks are as follows.

1.  Stage1: CopyIn task.
    1.  Use the [DataCopy](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) API to copy GlobalTensor data to LocalTensor.
    2.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to place the LocalTensor into the Queue of A1/B1.

2.  Stage2: Split task.
    1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to fetch the LocalTensor from A1/B1.
    2.  Use the [LoadData](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) API to move the LocalTensor from A1/B1 to A2/B2.
    3.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to enqueue the computation result LocalTensor into the Queue of A2/B2.

3.  Stage3: Compute task.
    1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to dequeue the LocalTensor from A2/B2.
    2.  Use the [Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) API to complete the Cube computation.
    3.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to enqueue the computation result LocalTensor into the Queue of CO1.

4.  Stage4: Aggregate task.
    1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to dequeue the LocalTensor from CO1.
    2.  Use the Ascend C API to copy the result matrix to CO2.
    3.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to enqueue the computation result LocalTensor into the Queue of CO2.

5.  Stage5: CopyOut task.
    1.  Use the [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) API to obtain a LocalTensor from the Queue of CO2.
    2.  Use the [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/gm_and_ub_data_movement.md) API to copy the LocalTensor to the GlobalTensor.

**Figure 2**  Matrix programming Queue<a name="zh-cn_topic_0000002135641293_fig1561614192617"></a>  
![](../../../figures/matrix_programming_queue.png "Matrix programming Queue")

## Development Process<a name="zh-cn_topic_0000002135641293_section220853764417"></a>

The following figure shows the process of implementing a matrix operator based on Ascend C.

**Figure 3**  Matrix operator implementation process<a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001463907248_fig11771195614212"></a>  
![](../../../figures/matrix_operator_implementation_process.png "Matrix operator implementation process")

-   Operator analysis: Analyze the mathematical expression, inputs, outputs, and computation logic of the operator, and identify the Ascend C APIs to be called.
-   Kernel function definition: Define the entry function of the Ascend C operator.
-   Implement the operator class based on the matrix programming paradigm: Complete the internal implementation of the kernel function, and call the private member functions CopyIn, SplitA, SplitB, Compute, Aggregate, and CopyOut to perform the five-stage pipeline operations of the matrix operator.

The following uses the Matmul operator as an example to describe the preceding steps in detail. The code framework of the Matmul operator is as follows.

```
#include "kernel_operator.h"

// Implement the operator class based on the programming paradigm
class KernelMatmul {
public:
    __aicore__ inline void Init(GM_ADDR a, GM_ADDR b, GM_ADDR c)
    {
        // ...
    }
    __aicore__ inline void Process()
    {
        CopyIn();
        SplitA();
        AscendC::LocalTensor<half> b1Local = inQueueB1.DeQue<half>();
        AscendC::LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
        AscendC::LocalTensor<float> c2Local = outQueueCO2.AllocTensor<float>();
        // split matrix b into 2 parts, [32, 16] and [32, 16]
        for (int i = 0; i < 2; ++i) {
            SplitB(b1Local, i);
            Compute(a2Local);
            Aggregate(c2Local, i);
        }
        inQueueB1.FreeTensor(b1Local);
        inQueueA2.FreeTensor(a2Local);
        outQueueCO2.EnQue<float>(c2Local);
        CopyOut();
    }
private:
    __aicore__ inline void CopyIn()
    {
        // ...
    }
    __aicore__ inline void SplitA()
    {
        // ...
    }
    __aicore__ inline void SplitB(const LocalTensor<half>& b1Local, const int bSplitIdx)
    {
        // ...
    }
    __aicore__ inline void Compute(const LocalTensor<half>& a2Local)
    {
        // ...
    }
    __aicore__ inline void Aggregate(const LocalTensor<float>& c2Local, const int bSplitIdx)
    {
        // ...
    }
    __aicore__ inline void CopyOut()
    {
        // ...
    }
private:
    // ...

};

//Define the kernel function
extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c)
{
    KernelMatmul op;
    op.Init(a, b, c);
    op.Process();
}
```

## Operator Analysis<a name="zh-cn_topic_0000002135641293_section11569817102912"></a>

Before developing the operator code, you need to analyze the mathematical expression, input, output, and computation logic of the operator, and identify the Ascend C APIs to be called.

1.  Identify the mathematical expression and computation logic of the operator.

    The Matmul operator performs matrix multiplication. Its mathematical expression is as follows: matrix a of shape \[m, k\] is multiplied by matrix b of shape \[k, n\] to obtain matrix c of shape \[m, n\]. For simplicity, set m = k = n = 32.

    ```
    c = a * b
    ```

    Note that when the data to be processed is too large, you need to split the data and move it to A2 and B2 in blocks, perform computation separately, and then aggregate the results. The computation logic below is intended to demonstrate the Split and Aggregate stages. Determine whether splitting and aggregation are required based on the actual size of the data to be processed.

    The computation logic is as follows:

    1.  Move the input data matrices a and b to Local Memory A1 and B1, respectively.
    2.  Move matrix a from A1 to A2. To achieve partial parallelism, split matrix b into part1 and part2, both of shape \[k, n / 2\], and then move them to B2 in blocks.
    3.  Perform matrix multiplication on matrix a with part1 and part2 of matrix b, respectively, to obtain part1 and part2 of matrix c, both of shape \[m, n / 2\]. The computation results are stored in CO1.
    4.  Copy part1 and part2 of matrix c to CO2 for merging.
    5.  Move the merged output data out of CO2.

2.  Identify the inputs and output.
    -   The Matmul operator has two inputs, a and b, and one output, c.
    -   In this sample, the operator input supports the half (float16) data type, and the Operator Output uses the float32 data type.
    -   The shapes of matrices a, b, and c are all \[32, 32\].
    -   The data format supported by the operator input and output is ND.

3.  Determine the kernel function name and parameters.
    -   You can customize the kernel function name. In this sample, the kernel function is named matmul\_custom.
    -   Based on the analysis of the operator inputs and output, the kernel function has three parameters: a, b, and c. a and b are the memory addresses of the inputs in Global Memory, and c is the memory address of the output in Global Memory.

4.  Analyze constraints.

    Because the hardware architecture imposes format constraints on the input and output of Cube computations, a format conversion process must be added to the operator implementation.

    -   When moving matrices a and b to A1 and B1, convert matrices a and b from the ND format to the NZ format.
    -   When moving matrix a from A1 to A2, convert matrix a from the NZ format to the ZZ format; when moving matrix b from B1 to B2, convert matrix b from the NZ format to the ZN format.
    -   When moving the computation result out of CO2, convert matrix c from the NZ format to the ND format.
    -   For details about data layout formats, see [Data Layout Format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md).

5.  Determine the APIs required for operator implementation.
    -   To implement data movement between external storage and internal storage, see the data movement APIs in the Ascend C API Reference. For details, see [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/overview/data_movement_concept.md).
    -   To implement matrix data format conversion, see the data conversion APIs in the Ascend C API Reference. For details, see [LoadData](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md).
    -   The Cube computation process involves matrix multiplication. See the Cube computation APIs in the Ascend C API Reference. For details, see [Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md).
    -   The Tensor data structures used in the computation are managed by Queue. APIs such as [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) and [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) are used.

Based on the preceding analysis, the computation flowchart and design specifications of the Ascend C Matmul operator are as follows:

**Figure 4** Computation flowchart of the Matmul operator<a name="zh-cn_topic_0000002135641293_fig5645174474016"></a>  
![](../../../figures/matmul_operator_computation_flowchart.png "Computation flowchart of the Matmul operator")

**Table 1** Design specifications of the Ascend C Matmul operator

<a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_table164881913121819"></a>
<table><tbody><tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row1848911314188"><th class="firstcol" valign="top" id="mcps1.2.6.1.1"><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p54891613141818"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p54891613141818"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p54891613141818"></a>Operator Type (OpType)</p></th>
<td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.1.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1748971311817"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1748971311817"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1748971311817"></a>Matmul</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_row10188125911198"><th class="firstcol" rowspan="3" valign="top" width="19.82%" id="mcps1.2.6.2.1"><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p17188125981915"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p17188125981915"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p17188125981915"></a>Operator Input</p></th>
<td class="cellrowborder" valign="top" width="19.17%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p11189145916192"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p11189145916192"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p11189145916192"></a><strong id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b12391113055716"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b12391113055716"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b12391113055716"></a>Name</strong></p></td>
<td class="cellrowborder" valign="top" width="16.39%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p918935961919"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p918935961919"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p918935961919"></a><strong id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b9405203015715"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b9405203015715"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b9405203015715"></a>Shape</strong></p></td>
<td class="cellrowborder" valign="top" width="15.09%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1618925919195"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1618925919195"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1618925919195"></a><strong id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b4418183055720"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b4418183055720"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b4418183055720"></a>Data Type</strong></p></td>
<td class="cellrowborder" valign="top" width="29.53%" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p61891259191918"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p61891259191918"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p61891259191918"></a><strong id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b64181730165715"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b64181730165715"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_b64181730165715"></a>Format</strong></p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row19489161331815"><td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p134891113131814"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p134891113131814"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p134891113131814"></a>a</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1283525118245"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1283525118245"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p1283525118245"></a>(m, k) = (32, 32)</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p12624946102416"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p12624946102416"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p12624946102416"></a>half</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p173195184235"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p173195184235"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p173195184235"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row1348951391811"><td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p13489191311184"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p13489191311184"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p13489191311184"></a>b</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p10959231592"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p10959231592"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p10959231592"></a>(k, n) = (32, 32)</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p76241246152416"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p76241246152416"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p76241246152416"></a>half</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.6.2.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p2795442191816"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p2795442191816"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p2795442191816"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row2489171310187"><th class="firstcol" valign="top" width="19.82%" id="mcps1.2.6.5.1"><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p4489913171810"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p4489913171810"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p4489913171810"></a>Operator Output</p></th>
<td class="cellrowborder" valign="top" width="19.17%" headers="mcps1.2.6.5.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p34899136188"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p34899136188"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p34899136188"></a>c</p></td>
<td class="cellrowborder" valign="top" width="16.39%" headers="mcps1.2.6.5.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p191831924155919"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p191831924155919"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p191831924155919"></a>(m, n) = (32, 32)</p></td>
<td class="cellrowborder" valign="top" width="15.09%" headers="mcps1.2.6.5.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p4624546192418"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p4624546192418"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p4624546192418"></a>float32</p></td>
<td class="cellrowborder" valign="top" width="29.53%" headers="mcps1.2.6.5.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1752074519175"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1752074519175"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1752074519175"></a>ND</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_row628260192410"><th class="firstcol" valign="top" id="mcps1.2.6.6.1"><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p22821601246"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p22821601246"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p22821601246"></a>Kernel Function Name</p></th>
<td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.6.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1359311404260"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1359311404260"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p1359311404260"></a>matmul_custom</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_row24890138181"><th class="firstcol" rowspan="4" valign="top" id="mcps1.2.6.7.1"><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p64894131189"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p64894131189"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_zh-cn_topic_0229825559_zh-cn_topic_0229823837_zh-cn_topic_0211294710_p64894131189"></a>Main APIs Used</p></th>
<td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.7.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p193715912115"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p193715912115"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p193715912115"></a>DataCopy: data movement API</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_row13483131172111"><td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.7.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p172254862115"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p172254862115"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p172254862115"></a>LoadData: matrix data format conversion API</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_row9293141202412"><td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.7.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p2293241142412"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p2293241142412"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p2293241142412"></a>Mmad: Cube computation API</p></td>
</tr>
<tr id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_row1036344318916"><td class="cellrowborder" colspan="4" valign="top" headers="mcps1.2.6.7.1 "><p id="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p4174848399"><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p4174848399"></a><a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001514387805_p4174848399"></a>Interfaces such as EnQue and DeQue: Queue management interfaces</p></td>
</tr>
</tbody>
</table>

## Kernel Function Definition<a name="zh-cn_topic_0000002135641293_section434251315304"></a>

Define the kernel function according to the rules described in [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md).

1.  Define the function prototype.

    In this sample, the function is named matmul\_custom (the kernel function name can be customized). Based on the analysis of the operator input and output in [Operator Analysis](#zh-cn_topic_0000002135641293_section11569817102912), three parameters a, b, and c are determined, where a and b are input memory and c is output memory. According to the kernel function rules in [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md), the function prototype is defined as follows: use the \_\_global\_\_ function type qualifier to identify it as a kernel function that can be invoked with <<<\>\>\>; use the \_\_aicore\_\_ function type qualifier to identify that the kernel function runs on the AI Core on the device side; for convenience, use the GM\_ADDR macro to modify the input parameters. For the definition of the GM\_ADDR macro, see [Kernel Function](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md).

    ```
    extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c)
    {
    }
    ```

2.  Call the Init and Process functions of the operator class.

    The Init function of the operator class completes memory initialization-related work, and the Process function implements the core computation logic of the operator. For details, see [Operator Class Implementation](#zh-cn_topic_0000002135641293_section1882915463510).

    ```
    extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c)
    {
        KernelMatmul op;
        op.Init(a, b, c);
        op.Process();
    }
    ```

3.  Encapsulate the kernel function to obtain the matmul\_custom\_do function for convenient invocation by the main program. \#ifndef ASCENDC\_CPU\_DEBUG indicates that this encapsulation function is used only when compiling and running the operator on the NPU side. When compiling and running the operator on the CPU side, the matmul\_custom function can be called directly. According to [Kernel Function Definition and Invocation](../../../programming_guide/programming_model/ai_core_simd_programming/kernel_function.md#zh-cn_topic_0000001447989210_section1915102519220), when invoking the kernel function, in addition to passing parameters a, b, and c, you also need to pass numBlocks (the number of cores on which the kernel function runs), the dynamic UB size (set to 0 when no dynamic UB is required), and stream (the stream that maintains the execution order of asynchronous operations in the application) to specify the execution configuration of the kernel function.

    ```
    #ifndef ASCENDC_CPU_DEBUG
    // call of kernel function
    void matmul_custom_do(uint32_t numBlocks, uint32_t dynUBufSize, void* stream, uint8_t* a, uint8_t* b, uint8_t* c)
    {
        matmul_custom<<<numBlocks, dynUBufSize, stream>>>(a, b, c);
    }
    #endif
    ```

## Operator Class Implementation<a name="zh-cn_topic_0000002135641293_section1882915463510"></a>

As described in the previous section, the kernel function calls the Init and Process functions of the operator class. This section describes how to implement the operator class based on the programming paradigm. For details about the matrix programming paradigm, see [programming paradigm](#zh-cn_topic_0000002135641293_section12567050132819).

The operator class mainly contains the public Init function for initialization, the core Process function, and some private members used in the implementation. The KernelMatmul operator class is defined as follows:

```
class KernelMatmul {
public:
    __aicore__ inline KernelMatmul(){}
    // Initialization function that performs memory initialization operations.
    __aicore__ inline void Init(GM_ADDR a, GM_ADDR b, GM_ADDR c){}
    // Core processing function that implements the operator logic.
    // Calls the private member functions CopyIn, SplitA, SplitB, Compute, Aggregate, and CopyOut to implement the five-stage pipeline of the matrix operator.
    __aicore__ inline void Process(){}

private:
    __aicore__ inline void CopyND2NZ(const LocalTensor<half>& dst, const GlobalTensor<half>& src, const uint16_t height, const uint16_t width){}
    // Copy-in function that implements the CopyIn stage of the programming paradigm. Called by the Process function.
    __aicore__ inline void CopyIn(){}
    // Copy-in function that implements the Split stage of the programming paradigm. Called by the Process function.
    __aicore__ inline void SplitA(){}
    // Copy-in function that implements the Split stage of the programming paradigm. Called twice by the Process function in a loop to move the two parts of matrix b respectively.
    __aicore__ inline void SplitB(const LocalTensor<half>& b1Local, const int bSplitIdx){}
    // Computation function that implements the Compute stage of the programming paradigm. Called twice by the Process function in a loop to compute the two parts of matrix c respectively.
    __aicore__ inline void Compute(const LocalTensor<half>& a2Local){}
    // Move-out function that completes the Aggregate stage in the programming paradigm. It is called twice by the Process function to move out the two parts of matrix c.
    __aicore__ inline void Aggregate(const LocalTensor<float>& c2Local, const int bSplitIdx){}
    // Move-out function that completes the CopyOut stage in the programming paradigm. It is called by the Process function.
    __aicore__ inline void CopyOut(){}

private:
    AscendC::TPipe pipe;  // Pipe memory management object that manages the memory of the Queue.
    AscendC::TQue<AscendC::TPosition::A1, 1> inQueueA1;  // Queue for input data, with TPosition set to A1.
    AscendC::TQue<AscendC::TPosition::A2, 1> inQueueA2;  // Queue for input data, with TPosition set to A2.
    AscendC::TQue<AscendC::TPosition::B1, 1> inQueueB1;  // Queue for input data, with TPosition set to B1.
    AscendC::TQue<AscendC::TPosition::B2, 2> inQueueB2;  // Queue for input data, with TPosition set to B2.
    AscendC::TQue<AscendC::TPosition::CO1, 2> outQueueCO1;  // Queue for output data, with TPosition set to CO1.
    AscendC::TQue<AscendC::TPosition::CO2, 1> outQueueCO2;  // Queue for output data, with TPosition set to CO2.
    // Object that manages the Global Memory addresses of the input and output, where aGM and bGM are inputs and cGM is the output.
    AscendC::GlobalTensor<half> aGM, bGM;
    AscendC::GlobalTensor<float> cGM;

    uint16_t m = 32;
    uint16_t n = 32;
    uint16_t k = 32;
    uint16_t aSize, bSize, cSize, mBlocks, nBlocks, kBlocks;
};
```

**KernelMatmul Constructor Implementation**

The constructor initializes the private member variables. The specific code is as follows:

```
__aicore__ inline KernelMatmul()
{
    aSize = m * k;
    bSize = k * n;
    cSize = m * n;
    mBlocks = m / 16;
    nBlocks = n / 16;
    kBlocks = k / 16;
}
```

The shape of matrix a is [m, k], the shape of matrix b is [k, n], and the shape of matrix c is [m, n]. In this example, m, n, and k are all set to 32.

aSize, bSize, and cSize are the numbers of elements in matrices a, b, and c, respectively.

mBlocks, nBlocks, and kBlocks are the numbers of fractals occupied by m, n, and k. For the half type, the shape of one fractal is 16 \* 16. The formula for calculating blocks is as follows:

-   mBlocks = m / 16
-   nBlocks = n / 16
-   kBlocks = k / 16

For details about fractals, see [Data Layout Format](../../../technical_appendix/concepts_and_terms/neural_networks_and_operators/data_layout_format.md).

**Init Function Implementation**

The Init function mainly performs the following tasks:

-   Set the Global Memory addresses of the input and output Global Tensors.

    Take setting the memory offset address of input a in Global Memory as an example:

    ```
    aGM.SetGlobalBuffer((__gm__ half*)a);
    ```

    Note that because the input parameters of the Init function in this sample are uniformly set to uint8\_t\*, they need to be cast to the specific data type \(\_\_gm\_\_ half\*\) before the offset is applied.

-   Allocate memory for the input and output Queues through the Pipe memory management object.

    For example, to allocate memory for the input data queue inQueueB2, use the following code segment:

    ```
    pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half) / 2);
    ```

    In this sample, matrix b is split into two parts. When allocating memory for inQueueB2, two memory blocks need to be requested, each half the size of matrix b. The memory initialization of outQueueCO1 follows the same approach.

The specific initialization function code is as follows:

```
__aicore__ inline void Init(GM_ADDR a, GM_ADDR b, GM_ADDR c)
{
    aGM.SetGlobalBuffer((__gm__ half*)a);
    bGM.SetGlobalBuffer((__gm__ half*)b);
    cGM.SetGlobalBuffer((__gm__ float*)c);
    pipe.InitBuffer(inQueueA1, 1, aSize * sizeof(half));
    pipe.InitBuffer(inQueueA2, 1, aSize * sizeof(half));
    pipe.InitBuffer(inQueueB1, 1, bSize * sizeof(half));
    pipe.InitBuffer(inQueueB2, 2, bSize * sizeof(half) / 2);
    pipe.InitBuffer(outQueueCO1, 2, cSize * sizeof(float) / 2);
    pipe.InitBuffer(outQueueCO2, 1, cSize * sizeof(float));
}
```

**Process Function Implementation**

Based on the matrix programming paradigm, the kernel function implementation is divided into five basic stages: CopyIn, Split, Compute, Aggregate, and CopyOut. The Split, Compute, and Aggregate stages need to distinguish between matrices a and b. In the Process function, these functions are called as follows.

```
__aicore__ inline void Process()
{
    CopyIn();
    SplitA();
    AscendC::LocalTensor<half> b1Local = inQueueB1.DeQue<half>();
    AscendC::LocalTensor<half> a2Local = inQueueA2.DeQue<half>();
    AscendC::LocalTensor<float> c2Local = outQueueCO2.AllocTensor<float>();
    // split matrix b into 2 parts, [32, 16] and [32, 16]
    for (int i = 0; i < 2; ++i) {
        SplitB(b1Local, i);
        Compute(a2Local);
        Aggregate(c2Local, i);
    }
    inQueueB1.FreeTensor(b1Local);
    inQueueA2.FreeTensor(a2Local);
    outQueueCO2.EnQue<float>(c2Local);
    CopyOut();
}
```

Within the two loops, SplitB needs to move the b matrices of the two parts from inQueueB1 separately, Compute needs to calculate the multiplication of the a matrix and the b matrices of the two parts separately, and Aggregate needs to move the c matrices of the two parts separately. The data flow of the five stages is illustrated as follows:

**Figure 5**  Data flow illustration<a name="zh-cn_topic_0000002135641293_fig203166439417"></a>  
![](../../../figures/data_flow_illustration.png "Data flow illustration")

Splitting the b matrix enables partial parallelism. The pipeline parallelism illustration of this sample is as follows:

**Figure 6**  Parallelism illustration<a name="zh-cn_topic_0000002135641293_fig1620145114425"></a>  
![](../../../figures/parallelism_illustration.png "Parallelism illustration")

1.  Stage 1: Implement the CopyIn function.

    1.  Use [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) to apply for a1Local and b1Local from the queues of A1 and B1.
    2.  Use the [DataCopy](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) API to move matrices a and b to Local Memory, and convert their data layout from ND to NZ at the same time.

        A single DataCopy instruction moves height\*16 numbers, and the loop is executed width/16 times. The parameters of DataCopy are set as follows:

        -   blockCount is set to height, and the movement is performed height times in total.
        -   blockLen is set to 1, and 16 numbers of the half type are moved at a time.
        -   Set srcStride to width/16 - 1. The source matrix skips one row for each block moved.
        -   Set dstStride to 0. Each block of the destination matrix is stored contiguously in memory.
        -   In each loop iteration, the start address of the source matrix moves by 16 elements, and the start address of the destination matrix moves by 16\*height elements.

        The format conversion is illustrated as follows. The first loop moves the blue part, and the second loop moves the green part. In the figure, width is 32, occupying two fractals, and height is 32, occupying two fractals, so a total of four 16\*16 fractals are moved.

        **Figure 7**  ND-to-NZ conversion illustration<a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001464069644_fig155702534417"></a>  
        ![](../../../figures/nd_to_nz.png "ND-to-NZ conversion illustration")

        Note: The ND-to-NZ format conversion described above is only an example. Developers can choose an appropriate conversion method based on the actual situation.

    3.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to place a1Local and b1Local into the Queues of A1 and B1, respectively.

    The specific code is as follows:

    ```
    __aicore__ inline void CopyND2NZ(const LocalTensor<half>& dst, const GlobalTensor<half>& src, const uint16_t height, const uint16_t width)
    {
        for (int i = 0; i < width / 16; ++i) {
            int srcOffset = i * 16;
            int dstOffset = i * 16 * height;
            AscendC::DataCopy(dst[dstOffset], src[srcOffset], { height, 1, uint16_t(width / 16 - 1), 0 });
        }
    }
    __aicore__ inline void CopyIn()
    {
        AscendC::LocalTensor<half> a1Local = inQueueA1.AllocTensor<half>();
        AscendC::LocalTensor<half> b1Local = inQueueB1.AllocTensor<half>();
        CopyND2NZ(a1Local, aGM, m, k);
        CopyND2NZ(b1Local, bGM, k, n);
        inQueueA1.EnQue(a1Local);
        inQueueB1.EnQue(b1Local);
    }
    ```

2.  Stage2: SplitA function implementation.
    1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to take a1Local from the Queue of A1.
    2.  Use [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) to apply for a2Local from the Queue of A2.
    3.  Use [LoadData](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) to move matrix a to A2, and convert matrix a from the NZ format to the ZZ format.

        The following figure shows the data movement and format conversion. In the figure, k is 32, occupying kBlocks (k/16 = 2) fractals; m is 32, occupying mBlocks (m/16 = 2) fractals. A total of four 16\*16 fractals are moved. In this example, one LoadData call moves two 16\*16 fractals, and LoadData is called twice in a loop. The first loop moves the two blue fractals, and the second loop moves the two green fractals.

        In a single loop, the parameters of LoadData (which moves two fractals in this example, either the blue part or the green part) are set as follows:

        -   repeatTimes indicates the number of iterations for data processing. Because each LoadData iteration processes one fractal, it can also be understood as the number of fractals to be moved. In this example, it is the number of fractals along the k axis, which is set to kBlocks, indicating that kBlocks fractals are moved.
        -   srcStride indicates the interval between the start addresses of source operand fractals in adjacent iterations. Take moving the blue fractals as an example: in the source operand matrix on the left of the following figure, the interval between the start addresses of the first blue fractal and the second blue fractal is mBlocks fractals, so it is set to mBlocks here.
        -   dstGap uses the default value, and the two fractals in the destination matrix are stored contiguously.
        -   ifTranspose is set to false. Each fractal is in the Z format both before and after the movement, and the transpose function is not enabled.
        -   In each loop iteration, the start address of the source matrix is offset by 16\*16, and the start address of the destination matrix is offset by 16\*k.

        **Figure 8**  NZ-to-ZZ format conversion<a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001464069644_fig1046153194220"></a>  
        ![](../../../figures/nz_to_zz.png "NZ-to-ZZ format conversion")

    4.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to place the computation result a2Local into the Queue of A2.

        The specific code is as follows:

        ```
            __aicore__ inline void SplitA()
            {
                int srcOffset = 0;
                int dstOffset = 0;
                AscendC::LocalTensor<half> a1Local = inQueueA1.DeQue<half>();
                AscendC::LocalTensor<half> a2Local = inQueueA2.AllocTensor<half>();
        
                // transform nz to zz
                for (int i = 0; i < mBlocks; ++i) {
                    AscendC::LoadData2DParams loadDataParams;
                    loadDataParams.repeatTimes = kBlocks;
                    loadDataParams.srcStride = mBlocks;
                    loadDataParams.ifTranspose = false;
        
                    AscendC::LoadData(a2Local[dstOffset], a1Local[srcOffset], loadDataParams);
        
                    srcOffset += 16 * 16;
                    dstOffset += k * 16;
                }
        
                inQueueA2.EnQue<half>(a2Local);
                inQueueA1.FreeTensor(a1Local);
            }
        ```

3.  Stage2: SplitB function implementation.

    1.  SplitB requires two parameters: b1Local, which is obtained from the Queue of B1 using [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md), and the loop iteration variable index.
    2.  Use [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) to allocate b2Local from the Queue of B2.
    3.  Use [LoadData](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/matrix_computation_load.md) to move the b matrix to B2, and convert it from the NZ format to the ZN format at the same time.

        The following figure shows the movement and format conversion: in the figure, k is 32, occupying kBlocks (k/16=2) fractals; n is 32, occupying nBlocks (n/16=2) fractals. A total of four 16\*16 fractals are moved. In this example, one LoadData call moves two 16\*16 fractals, and LoadData is called twice in a loop. The first loop moves the two fractals in blue, and the second loop moves the two fractals in green.

        In a single loop, the parameters of LoadData (which moves two fractals in this example, either the blue part or the green part) are set as follows:

        -   repeatTimes indicates the number of iterations for data processing. Because each iteration of LoadData processes one fractal, it can also be understood as the number of fractals to be moved. In this example, it is the number of fractals along the k axis, which is set to kBlocks, indicating that kBlocks fractals are moved.
        -   srcStride is the interval between the start addresses of the source operand fractals in adjacent iterations. Take moving the blue fractals as an example: in the source operand matrix on the left of the following figure, the interval between the start addresses of the first blue fractal and the second blue fractal is one fractal, so it is set to 1 here, meaning that the two fractals of the source matrix are stored contiguously.
        -   dstGap uses the default value 0, and the two fractals of the destination matrix are stored contiguously.
        -   ifTranspose is set to true. Each fractal is in Z format before movement and needs to be in N format after movement, so the transpose function must be enabled.
        -   In each loop iteration, the start address of the source matrix needs to be offset by k\*n/2.

        **Figure 9**  NZ-to-ZN format conversion<a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001464069644_fig5456545436"></a>  
        ![](../../../figures/nz_to_zn.png "NZ-to-ZN format conversion")

    4.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to place the computation result b2Local into the Queue of B2.

    The specific code is as follows:

    ```
        __aicore__ inline void SplitB(const AscendC::LocalTensor<half>& b1Local, const int bSplitIdx)
        {
            AscendC::LocalTensor<half> b2Local = inQueueB2.AllocTensor<half>();
    
            // transform nz to zn
            AscendC::LoadData2DParams loadDataParams;
            loadDataParams.repeatTimes = kBlocks;
            loadDataParams.srcStride = 1;
            loadDataParams.ifTranspose = true;
    
            AscendC::LoadData(b2Local, b1Local[bSplitIdx * bSize / 2], loadDataParams);
    
            inQueueB2.EnQue<half>(b2Local);
        }
    ```

4.  Stage3: Implement the Compute function to complete the core cube computation.

    1.  The Compute function needs to receive the parameter a2Local, which is obtained from the Queue of A2 using [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md).
    2.  Use [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md) to apply for c1Local from the Queue of CO1.
    3.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to obtain b2Local from B2.
    4.  Use [Mmad](../../../../api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute/Mmad.md) to complete the matrix multiplication computation.
    5.  Use [EnQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/EnQue.md) to place the computation result c1Local into the Queue of CO1.

    The specific code is as follows:

    ```
        __aicore__ inline void Compute(const AscendC::LocalTensor<half>& a2Local)
        {
            AscendC::LocalTensor<half> b2Local = inQueueB2.DeQue<half>();
            AscendC::LocalTensor<float> c1Local = outQueueCO1.AllocTensor<float>();
    
            AscendC::MmadParams mmadParams;
            mmadParams.m = m;
            mmadParams.n = n / 2;
            mmadParams.k = k;
            AscendC::Mmad(c1Local, a2Local, b2Local, mmadParams);
    
            outQueueCO1.EnQue<float>(c1Local);
            inQueueB2.FreeTensor(b2Local);
        }
    ```

5.  Stage4: Implement the Aggregate function to complete the data aggregation operation.

    1.  Aggregate requires two parameters: c2Local, which is applied for from the Queue of CO2 using [AllocTensor](../../../../api/SIMD-API/basic_api/resource_management/TQue/AllocTensor.md), and the loop iteration variable index.
    2.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to take c1Local out of CO1.
    3.  Use [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/gm_and_ub_data_movement.md) to move the result matrix from CO1 to CO2.

        The DataCopy parameters are set as follows:

        -   Set blockCount to 1 and blockLen to 2 to move two fractals consecutively without format conversion.
        -   Set blockMode to BlockMode::BLOCK\_MODE\_MATRIX, indicating that the data needs to be moved by fractal.
        -   The offset of the first address of c2Local is set to index \* cSize / 2.

    The specific code is as follows:

    ```
        __aicore__ inline void Aggregate(const AscendC::LocalTensor<float>& c2Local, const int bSplitIdx)
        {
            AscendC::LocalTensor<float> c1Local = outQueueCO1.DeQue<float>();
    
            AscendC::DataCopyParams dataCopyParams;
            dataCopyParams.blockCount = 1;
            dataCopyParams.blockLen = 2;
            AscendC::DataCopyEnhancedParams enhancedParams;
            enhancedParams.blockMode = AscendC::BlockMode::BLOCK_MODE_MATRIX;
            AscendC::DataCopy(c2Local[bSplitIdx * cSize / 2], c1Local, dataCopyParams, enhancedParams);
    
            outQueueCO1.FreeTensor(c1Local);
        }
    ```

6.  Stage5: CopyOut function implementation.

    1.  Use [DeQue](../../../../api/SIMD-API/basic_api/resource_management/TQue/DeQue.md) to take c2Local out of CO2.
    2.  Use [DataCopy](../../../../api/SIMD-API/basic_api/data_move_guide/gm_and_ub_data_movement.md) to move the result matrix from CO2 to Global Memory, and convert the format from NZ to ND at the same time.

        Each loop iteration moves one fractal, moving m\*16 numbers. The DataCopy parameters are described as follows:

        -   blockCount is set to m, moving m times in total.
        -   blockLen is set to 2. A DataCopy instruction moves 2 blocks at a time, with 16 numbers in each block.
        -   srcStride is set to 0, meaning there is no gap between two moves.
        -   dstStride is set to \(nBlocks - 1\) \* 2, meaning there is a gap of 2 blocks between two moves.
        -   In each loop iteration, the destination matrix is offset by 16, and the source matrix is offset by m\*16.

        The format conversion is illustrated as follows. The first loop moves the blue data, and the second loop moves the green data.

        **Figure 10**  NZ-to-ND format conversion<a name="zh-cn_topic_0000002135641293_zh-cn_topic_0000001464069644_fig58491033114314"></a>  
        ![](../../../figures/nz_to_nd.png "NZ-to-ND format conversion")

    The specific code is as follows:

    ```
        __aicore__ inline void CopyOut()
        {
            AscendC::LocalTensor<float> c2Local = outQueueCO2.DeQue<float>();
    
            // transform nz to nd
            for (int i = 0; i < nBlocks; ++i) {
                AscendC::DataCopy(cGM[i * 16], c2Local[i * m * 16], { m, 2, 0, uint16_t((nBlocks - 1) * 2) });
            }
    
            outQueueCO2.FreeTensor(c2Local);
        }
    ```
