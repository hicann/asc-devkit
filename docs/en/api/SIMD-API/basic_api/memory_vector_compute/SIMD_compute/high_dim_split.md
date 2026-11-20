# High-Dimensional Split

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-05T01:55:52.984Z pushedAt=2026-11-18T01:25:30.748Z nl=2 -->

> [!NOTE] Note
>
> This section explains the **tensor high-dimensional split computation** APIs in the **vector computation basic APIs**. If you do not need to use such APIs, you can skip this section.
>
> In the following text, repeatTime, dataBlockStride, repeatStride, and mask are generic descriptions, and their names do not necessarily correspond exactly to the parameter names in specific instructions.
>
> For example, the dataBlockStride parameter, which is the address stride between different DataBlocks within a single iteration, corresponds to the dstBlkStride and srcBlkStride parameters in unary APIs, and to the dstBlkStride, src0BlkStride, and src1BlkStride parameters in binary APIs.
>
> You can find the description of the parameter meanings in the parameter description of the specific API.

The **high-dimensional split APIs** support the repeatTime, stride, and mask parameters, which control the iterative execution of instructions, the address interval of operands, and the mask within an iteration, providing more flexible functionality.

Take the Exp API as an example. The API is called as follows:

```cpp
Exp(dst, src, mask, repeatTime, {dstBlkStride, ..., dstRepStride, ...});
```

## Iteration Control

In each iteration, the vector computation unit reads **8** **DataBlocks** from the Unified Buffer (UB) (each DataBlock has contiguous internal addresses and is 32 bytes long), performs the computation, and writes the results to the corresponding 8 DataBlocks. The following figure shows the Exp computation on the 8 DataBlocks in a single iteration.

**Figure 1**  Diagram of Exp computation on the 8 DataBlocks in a single iteration  
![](../../../../figures/single_iter_exp.png)<a id="figure-1-diagram-of-exp-computation-on-the-8-datablocks-in-a-single-iteration"></a>

The vector computation API allows you to configure the number of iterations through **repeatTime**, thereby controlling the repeated execution of the instruction. If repeatTime is set to 2, the vector computation unit performs 2 iterations and computes a result of 2\*8 (8 DataBlocks per iteration) \* 32 bytes = 512 bytes. If the data type is half, 256 elements are computed. The following figure shows the Exp computation over 2 iterations. Due to hardware limitations, **repeatTime cannot exceed 255.**

**Figure 2**  Exp computation over 2 iterations  
![](../../../../figures/two_iter_exp.png)<a id="figure-2-exp-computation-over-2-iterations"></a>

## Mask Configuration

For data in the same iteration, you can use the **mask** parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows how the mask bit-by-bit mode controls which elements participate in the computation during an Abs computation, where 1 indicates participation and 0 indicates non-participation. For details about the mask, see [mask](mask.md#mask).

**Figure 3**  Schematic diagram of mask operation using the **mask** parameter (using the int32 data type as an example)  
![](../../../../figures/mask_param_int32.png)<a id="figure-3-schematic-diagram-of-mask-operation-using-the-mask-parameter-using-the-int32-data-type-as-an-example"></a>

## Address Stride Configuration

The vector computation unit also supports vector computation with intervals, configured through **dataBlockStride** (the address step between different DataBlocks within a single iteration) and **repeatStride** (the address step of the same DataBlock between adjacent iterations).

- **dataBlockStride**<a id="datablockstride"></a>

    dataBlockStride is the address step between different DataBlocks within a single iteration. Its value cannot exceed the UB size limit.

    - For continuous computation, set dataBlockStride to 1 to process the 8 DataBlocks within the same iteration continuously.
    - For non-continuous computation, when dataBlockStride is greater than 1 (for example, 2), an interval of one DataBlock appears between different DataBlocks within the same iteration when reading data, as shown in the following figure.

    **Figure 4**  Example of different dataBlockStride values  
    ![](../../../../figures/data_block_stride.png)<a id="figure-4-example-of-different-datablockstride-values"></a>

    **Note: dstBlkStride does not support being set to 0. Setting it to 0 may cause undefined behavior. If srcBlkStride=0, the source operand always reuses the first DataBlock.**

    The following figure shows an example of reading and writing the source operand and destination operand in the UB space within a single iteration. In the example, the source operand's dataBlockStride is set to 2, indicating that the address step between different DataBlocks within a single iteration is 2 DataBlocks; the destination operand's dataBlockStride is set to 1, indicating that the addresses are continuous within a single iteration.

    **Figure 5**  Non-continuous scenario within a single iteration  
    ![](../../../../figures/single_iter_non_continuous.png)<a id="figure-5-non-continuous-scenario-within-a-single-iteration"></a>

- **repeatStride**<a id="repeatstride"></a>

    repeatStride is the address step between the same DataBlock in adjacent iterations.

    - **Continuous computation scenario:** Assume that a Tensor is defined for both the destination operand and the source operand (that is, the addresses overlap), and repeatStride is set to 8. In this case, the vector computation unit reads eight consecutive DataBlocks in the first iteration and the next eight consecutive DataBlocks in the second iteration. After multiple iterations, computation of all input data is completed.

        ![](../../../../figures/repeat_stride1.png)

    - **Non-continuous computation scenario:** When repeatStride is greater than 8 (for example, 10), the data read by the vector computation unit in adjacent iterations is non-contiguous in address, with an interval of two DataBlocks.

        ![](../../../../figures/repeat_stride2.png)

    - **Repeated computation scenario:** When repeatStride is set to 0, the vector computation unit repeatedly reads and computes the first eight consecutive DataBlocks.

        ![](../../../../figures/repeat_stride3.png)

    - **Partially repeated computation:** When repeatStride is greater than 0 and less than 8, part of the data in adjacent iterations is repeatedly read and computed by the vector computation unit. This scenario is generally not involved.

        ![](../../../../figures/repeat_stride4.png)

    When repeatTime is greater than 1 and multiple iterations are required to complete the vector computation, you can set repeatStride appropriately based on different usage scenarios. The following figure shows a diagram of a non-contiguous scenario across multiple iterations. In this example, **repeatStride** of both the source operand and the destination operand is set to 9, indicating that the interval between the start addresses of the same DataBlock in adjacent iterations is 9 DataBlocks. The same DataBlock refers to a DataBlock at the same position within an iteration. For example, src1 and src9 in the following figure are in adjacent iterations and are both the first DataBlock in their respective iterations, so the interval between them is the value of **repeatStride**.

    **Figure 6**  Diagram of non-contiguous scenarios across multiple iterations  
    ![](../../../../figures/multi_iter_non_continuous.png)<a id="figure-6-diagram-of-non-contiguous-scenarios-across-multiple-iterations"></a>
