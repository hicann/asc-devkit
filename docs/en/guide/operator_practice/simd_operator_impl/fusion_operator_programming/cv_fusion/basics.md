# Basic Knowledge<a name="ZH-CN_TOPIC_0000002500468250"></a>

<!-- md-trans-meta sourceCommit=12a14b50a4f8b34df0b2db2cb19ae1e957b3bde2 translatedAt=2026-11-09T10:28:14.601Z pushedAt=2026-11-19T06:32:19.725Z nl=2 -->

>[!NOTE] Note
>Before learning fused operator programming, ensure that you have mastered the knowledge of [Cube Programming](../../matrix_advanced_api/basics.md).

## CV Fused Operator<a name="zh-cn_topic_0000001644411628_section204271251142411"></a>

A fused operator is formed by fusing multiple independent small operators. Its functionality is equivalent to that of multiple small operators, and its performance is usually better than that of independent small operators. Based on actual business scenario requirements, users can freely fuse vector and cube operators according to specific algorithms to achieve performance gains. Operators that fuse cube computation and vector computation are collectively referred to as CV fused operators.

For example, for Flash Attention, one of the most core fused operators in LLM large models, its core implementation is shown in the following figure. The Matmul operator (cube), Scale operator (vector), Mask operator (vector), and SoftMax operator (vector) in the figure are fused into one large operator, Flash Attention.

**Figure 1**  Flash Attention core implementation<a name="zh-cn_topic_0000001644411628_fig980125772817"></a>  
![](../../../../figures/flash_attn.png "Flash Attention core implementation")

## Scenarios and Benefits<a name="zh-cn_topic_0000001644411628_section162803512544"></a>

For vector operators and cube operators with data dependencies, you can use the fused operator programming approach to fuse the vector operators and cube operators into a single operator kernel function, thereby achieving performance gains. The following figure compares the execution time of independent vector operators and cube operators with that of a Mix fused operator, showing why developing a Mix fused operator brings performance gains.

**Figure 2**  Execution time comparison between independent vector operators and cube operators and a Mix fused operator<a name="zh-cn_topic_0000001644411628_fig141121413259"></a>  
![](../../../../figures/mix_fusion_time.png "Execution time comparison between independent vector operators and cube operators and a Mix fused operator")

-   **Independent vector operators and cube operators**: The result of the cube computation must be moved to Global Memory, then moved from Global Memory to Local Memory before the vector operator computation is performed. The computation and data movement are executed serially. In addition, scheduling multiple operators increases the operator scheduling time.
-   **Fused operator implementation**: Data can be sliced, and a pipelined design enables the vector computation unit and the cube computation unit to perform computation in parallel. In addition, compared with unfused single operators, the fused operator scheduling time is reduced.

In addition to effectively improving operator performance and fully leveraging the computing power of the AI processor, fused operators offer the following benefits:

-   Reduced computation: A fused operator merges multiple operators into one, simplifying the computation process, reducing the amount of computation, and improving computation efficiency.
-   Reduced memory usage: A fused operator merges the intermediate results of multiple operators into one, thereby reducing memory usage and improving memory utilization.
-   Optimized data flow: A fused operator optimizes the data flow and reduces data transfer between different operators, thereby improving data processing efficiency.
-   Simplified code implementation: A fused operator simplifies code implementation, reduces the amount of code, and improves code readability and maintainability.

**In summary, fusing operators is an effective means of optimizing computation, improving computational efficiency and memory utilization, optimizing data flow, and simplifying code implementation.**

## Programming Paradigm<a name="zh-cn_topic_0000001644411628_section18752329256"></a>

Ascend C provides the **fused operator programming paradigm**, which allows developers to express the data flow of fused operators based on this paradigm and quickly implement custom fused operators.

The **fused operator data flow** refers to the flow of the inputs and outputs of a fused operator across storage locations. Taking a typical fused cube and vector operator as an example, the data flow between storage locations is shown in the following figure:

-   The output of Cube can be used as the input of Vector: GM/UB (CO2) -\>UB (VECIN)
-   The output of Vector can be used as the input of Cube: UB (VECOUT) -\>L1 Buffer (A1) -\>L0A Buffer (A2), UB (VECOUT) -\>L1 Buffer (B1) -\>L0B Buffer (B2)

![](../../../../figures/mat_pos_45.png)

The fused operator programming paradigm based on the Matmul advanced API simplifies the preceding data flow into the following steps:

**Figure 3** Fused operator programming paradigm<a name="zh-cn_topic_0000001644411628_fig321783243811"></a>  
![](../../../../figures/fusion_paradigm_46.png "Fused operator programming paradigm-46")

1.  Initialize a Matmul object.
2.  Perform the internal computation of Matmul.
3.  Copy the Matmul computation result to the Vector core.
4.  Perform vector computation.
5.  Copy the output result to Global Memory.

The following shows the example code (pseudocode) of the entire process. For the complete sample, see [MatmulLeakyRelu](../../../../../../../examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api).

```
// Kernel function entry, mix scenario, AIC:AIV=1:2
__global__ __mix__(1, 2) void matmul_leakyrelu_custom(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* bias,
    __gm__ uint8_t* c, __kfc_workspace__ __gm__ uint8_t* workspace, AscendC::tiling::TCubeTiling tiling)
{
    AscendC::TPipe pipe;
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    // Set the Matmul inputs (including the left cube, right cube, and bias).
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);
    // Call matmul iterate to obtain a block of [baseM, baseN] computation results.
    while (matmulObj.template Iterate<true>())
    {
        // GetTensorC: Obtain the Matmul intermediate result;
        // LeakyReluCompute: Perform vector computation on the Matmul intermediate result;
        // DataCopy: Copy the fused computation result to GM;
    }
    matmulObj.End();
}
```
