# Basics<a name="ZH-CN_TOPIC_0000002500468250"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:51:16.634Z pushedAt=2026-11-03T07:05:13.289Z nl=2 -->

>[!NOTE] Note
>Before learning fused operator programming, ensure that you have mastered the knowledge of [Cube Programming](../../matrix_advanced_api/basics.md).

## CV Fused Operator<a name="zh-cn_topic_0000001644411628_section204271251142411"></a>

A fused operator is formed by fusing multiple independent small operators. It is functionally equivalent to the combined small operators but usually delivers better performance. Based on your actual service requirements, you can freely fuse vector and Cube operators according to specific algorithms to achieve performance gains. Operators that combine Cube and Vector computation are collectively referred to as CV fused operators.

For example, Flash Attention, one of the most critical fused operators in large language models (LLMs), has the core implementation shown in the following figure. In the figure, the Matmul operator (Cube), Scale operator (Vector), Mask operator (Vector), and SoftMax operator (Vector) are fused into a single large operator, Flash Attention.

**Figure 1**  Flash Attention core implementation<a name="zh-cn_topic_0000001644411628_fig980125772817"></a>  
![](../../../../figures/flash_attn.png "Flash Attention core implementation")

## Scenarios and Benefits<a name="zh-cn_topic_0000001644411628_section162803512544"></a>

For vector operators and cube operators with data dependencies, you can use fused operator programming to fuse the vector operator and cube operator into a single operator kernel function, thereby gaining performance benefits. The following figure compares the execution time of independent vector and cube operators with that of a Mix fused operator, showing why developing a Mix fused operator brings performance benefits.

**Figure 2**  Execution time comparison between independent vector and cube operators and a Mix fused operator<a name="zh-cn_topic_0000001644411628_fig141121413259"></a>  
![](../../../../figures/mix_fusion_time.png "Execution time comparison between independent vector and cube operators and a Mix fused operator")

-   **Independent vector and cube operator implementation**: The result of cube computation needs to be moved to Global Memory, then moved from Global Memory to Local Memory before the vector operator computation is performed. Computation and data movement are executed serially. In addition, scheduling multiple operators increases the operator scheduling time.
-   **Fused operator implementation**: Data can be sliced, and through pipeline design, the vector computation unit and cube computation unit can perform computation in parallel. In addition, compared with unfused single operators, the operator scheduling time is reduced.

In addition to effectively improving operator performance and fully leveraging the computing power of the AI processor, fused operators offer the following benefits:

-   Reduced computation: A fused operator can combine multiple operators into one, simplifying the computation process, reducing the amount of computation, and improving computation efficiency.
-   Reduced memory usage: A fused operator can combine the intermediate results of multiple operators into one, thereby reducing memory usage and improving memory utilization.
-   Optimized data flow: A fused operator can optimize the data flow and reduce data transfer between different operators, thereby improving data processing efficiency.
-   Simplified code implementation: A fused operator can simplify code implementation, reduce the amount of code, and improve code readability and maintainability.

**In summary, the fused operator is an effective means of optimizing computation, which can improve computation efficiency and memory utilization, optimize data flow, and simplify code implementation.**

## Programming Paradigm<a name="zh-cn_topic_0000001644411628_section18752329256"></a>

Ascend C provides the **fused operator programming paradigm**, which helps developers express the data flow of a fused operator and quickly implement a custom fused operator.

The **fused operator data flow** refers to the flow of the inputs and outputs of a fused operator across storage locations. Taking a typical Cube and Vector fused operator as an example, the data flow across storage locations is shown in the following figure:

-   The output of Cube can be used as the input of Vector: GM/UB (CO2)-\>UB (VECIN)
-   The output of Vector can be used as the input of Cube: UB (VECOUT)-\>L1 Buffer (A1)-\>L0A Buffer (A2), UB (VECOUT)-\>L1 Buffer (B1)-\>L0B Buffer (B2)

![](../../../../figures/mat_pos_45.png)

The fused operator programming paradigm based on the Matmul advanced API simplifies the preceding data flow as follows:

**Figure 3**  Fused operator programming paradigm<a name="zh-cn_topic_0000001644411628_fig321783243811"></a>  
![](../../../../figures/fusion_paradigm_46.png "Fused operator programming paradigm-46")

1.  Initialize a Matmul object.
2.  Perform the internal computation of Matmul.
3.  Copy the Matmul computation result to the Vector core.
4.  Perform vector computation.
5.  Copy the output result to Global Memory.

The following is the example code of the entire process (pseudocode). For the complete sample, see [MatmulLeakyRelu](../../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api).

```
// Kernel entry function, mix scenario, AIC:AIV=1:2
__global__ __mix__(1, 2) void matmul_leakyrelu_custom(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* bias,
    __gm__ uint8_t* c, __kfc_workspace__ __gm__ uint8_t* workspace, AscendC::tiling::TCubeTiling tiling)
{
    AscendC::TPipe pipe;
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    // Set the Matmul inputs (including the left cube, right cube, and bias)
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);
    // Call matmul iterate to obtain the computation result of a [baseM, baseN] block
    while (matmulObj.template Iterate<true>())
    {
        // GetTensorC: Obtain the Matmul intermediate result;
        // LeakyReluCompute: Perform vector computation on the Matmul intermediate result;
        // DataCopy: Copy the fused computation result to GM;
    }
    matmulObj.End();
}
```
