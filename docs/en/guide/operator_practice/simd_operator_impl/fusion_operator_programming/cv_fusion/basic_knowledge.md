# Basic Knowledge<a name="ZH-CN_TOPIC_0000002500468250"></a>

<!-- md-trans-meta sourceCommit=564901738b931c84398ad97ee43f557ae89a6b27 translatedAt=2026-08-26T13:49:53.456Z -->

>[!NOTE] Note
>Before learning fused operator programming, ensure that you have mastered the knowledge of [matrix programming](../../matrix_advanced_api/basic_knowledge.md).

## CV Fused Operator<a name="zh-cn_topic_0000001644411628_section204271251142411"></a>

A fused operator is formed by fusing multiple independent small operators. Its functionality is equivalent to that of the multiple small operators, and its performance is usually better than that of the independent small operators. Based on actual business scenarios, you can freely fuse Vector and Cube operators according to specific algorithms to achieve performance gains. Operators that fuse Cube computation and Vector computation are collectively referred to as CV fused operators.

For example, Flash Attention, one of the most core fused operators in large language models (LLMs), has the following core implementation. In the figure, the Matmul operator (Cube), Scale operator (Vector), Mask operator (Vector), and SoftMax operator (Vector) are fused into a single large operator, Flash Attention.

**Figure 1**  Flash Attention core implementation<a name="zh-cn_topic_0000001644411628_fig980125772817"></a>  
![](../../../../figures/flash_attention_core_implementation.png "Flash Attention core implementation")

## Scenarios and Benefits<a name="zh-cn_topic_0000001644411628_section162803512544"></a>

For vector operators and matrix operators with data dependencies, you can fuse them through fused operator programming and carry them in a single operator Kernel function, thereby gaining performance benefits. The following figure compares the execution time of independent vector operators and matrix operators with that of Mix fused operators, showing why developing Mix fused operators brings performance benefits.

**Figure 2**  Execution time comparison between independent vector operators and matrix operators and Mix fused operators<a name="zh-cn_topic_0000001644411628_fig141121413259"></a>  
![](../../../../figures/independent_vector_operators_and_matrix_operators_mix_fused_operators_execution_time_comparison.png "Execution time comparison between independent vector operators and matrix operators and Mix fused operators")

-   **Independent vector operator and matrix operator implementation**: The result after matrix computation needs to be moved to Global Memory, then moved from Global Memory to Local Memory before the vector operator computation is performed. Both computation and data movement are executed serially. In addition, the scheduling and execution of multiple operators increases the operator scheduling time.
-   **Fused operator implementation**: Data can be sliced, and through pipeline design, the vector computation unit and matrix computation unit can perform parallel computation. In addition, compared with unfused single operators, the operator scheduling time is reduced.

In addition to effectively improving operator performance and fully leveraging the computing power of AI processors, fused operators offer the following benefits:

-   Reduced computation: A fused operator can combine multiple operators into one, simplifying the computation process, reducing the amount of computation, and improving computation efficiency.
-   Reduced memory usage: A fused operator can combine the intermediate results of multiple operators into one, thereby reducing memory usage and improving memory utilization.
-   Optimized data flow: A fused operator can optimize the data flow and reduce data transfer between different operators, thereby improving data processing efficiency.
-   Simplified code implementation: A fused operator can simplify code implementation, reduce the amount of code, and improve code readability and maintainability.

**In summary, a fused operator is an effective means of optimizing computation, which can improve computational efficiency and memory utilization, optimize data flow, and simplify code implementation.**

## Programming Paradigm<a name="zh-cn_topic_0000001644411628_section18752329256"></a>

Ascend C provides the **fused operator programming paradigm**, which helps developers express the data flow of fused operators and quickly implement custom fused operators.

The **fused operator data flow** refers to the flow of the inputs and outputs of a fused operator across storage locations. Taking a typical Cube and Vector fused operator as an example, the data flow between logical locations is shown in the following figure:

-   The Cube output can be used as the Vector input: CO2-\>VECIN
-   The Vector output can be used as the Cube input: VECOUT-\>A1-\>A2, VECOUT-\>B1-\>B2

![](../../../../figures/matrix_programming_logical_position_diagram_45.png)

The fused operator programming paradigm based on the Matmul advanced API simplifies the preceding data flow as follows:

**Figure 3**  Fused operator programming paradigm<a name="zh-cn_topic_0000001644411628_fig321783243811"></a>  
![](../../../../figures/fused_operator_programming_paradigm_46.png "fused_operator_programming_paradigm_46")

1.  Initialize a Matmul object.
2.  Perform the internal computation of Matmul.
3.  Copy the Matmul computation result to the Vector core.
4.  Perform vector computation.
5.  Copy the output result to Global Memory.

The following shows the sample code (pseudocode) of the entire process. For the complete sample, see [MatmulLeakyRelu](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/00_introduction/03_fusion_operation/matmul_leakyrelu_advanced_api).

```
// Kernel entry function, mix scenario, AIC:AIV=1:2
__global__ __mix__(1, 2) void matmul_leakyrelu_custom(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* bias,
    __gm__ uint8_t* c, __kfc_workspace__ __gm__ uint8_t* workspace, AscendC::tiling::TCubeTiling tiling)
{
    AscendC::TPipe pipe;
    // ...
    REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tiling);
    // Set the Matmul inputs (including the left matrix, right matrix, and bias)
    matmulObj.SetTensorA(aGlobal);
    matmulObj.SetTensorB(bGlobal);
    matmulObj.SetBias(biasGlobal);
    // Call matmul iterate to obtain the computation result of a [baseM, baseN] block
    while (matmulObj.template Iterate<true>())
    {
        // GetTensorC: Obtain the Matmul intermediate result;
        // LeakyReluCompute: Perform vector computation on the Matmul intermediate result;
        // DataCopy: Copy the fused computation result out to GM;
    }
    matmulObj.End();
}
```
