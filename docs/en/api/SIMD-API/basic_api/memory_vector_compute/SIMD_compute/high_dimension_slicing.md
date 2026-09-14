# High-Dimensional Slicing

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T14:55:23.538Z -->

> [!NOTE]
>
> This section explains the **tensor high-dimensional slicing computation** APIs in the **vector computation basic APIs**. If you do not need to use such APIs, you can skip this section.
>
> In the following text, repeatTime, dataBlockStride, repeatStride, and mask are generic descriptions, and their names may not exactly correspond to the parameter names in specific instructions.
>
> For example, the dataBlockStride parameter, which specifies the address step between different DataBlocks within a single iteration, corresponds to the dstBlkStride and srcBlkStride parameters in unary APIs, and to the dstBlkStride, src0BlkStride, and src1BlkStride parameters in binary APIs.
>
> You can find the description of the parameter meanings in the parameter description of a specific API.

**High-dimensional slicing APIs** support the repeatTime, stride, and mask parameters, which can control the iterative execution of instructions, the address interval of operands, and the mask within an iteration, making the functions more flexible.

Take the Exp API as an example. The API call form is as follows:

```cpp
Exp(dst, src, mask, repeatTime, {dstBlkStride, ..., dstRepStride, ...});
```

## Iteration Control

In each iteration, the vector computation unit reads **8** **DataBlocks** from the UB (each DataBlock has contiguous internal addresses and is 32 bytes in length) for computation, and writes the results to the corresponding 8 DataBlocks. The following figure shows the Exp computation of the 8 DataBlocks in a single iteration.

**Figure 1**  Exp computation of the 8 DataBlocks in a single iteration  
![](../../../../figures/single_iter_exp.png)<a id="figure-1-exp-computation-of-the-8-datablocks-in-a-single-iteration"></a>

The vector computation API allows developers to configure the number of iterations through **repeatTime**, thereby controlling the multiple-iteration execution of an instruction. Assume that repeatTime is set to 2. The vector computation unit performs two iterations and computes 2\*8 (8 DataBlocks per iteration) \* 32 bytes = 512 bytes of results. If the data type is half, 256 elements are computed. The following figure shows the Exp computation of two iterations. Due to hardware limitations, **repeatTime cannot exceed 255.**

**Figure 2**  Exp computation of two iterations  
![](../../../../figures/two_iter_exp.png)<a id="figure-2-exp-computation-of-two-iterations"></a>

## Mask Configuration

For data within the same iteration, you can use the **mask** parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows how the mask controls, in bit-by-bit mode, which elements participate in the computation when performing the Abs computation. 1 indicates that the element participates in the computation, and 0 indicates that it does not. For details about the mask, see [Mask](mask.md#mask).

**Figure 3**  Mask operation using the **mask** parameter (using the int32 data type as an example)  
![](../../../../figures/mask_param_int32.png)<a id="figure-3-mask-operation-using-the-mask-parameter-int32-data-type-as-an-example"></a>

## Address Stride Configuration

The vector computation unit also supports vector computation with intervals, which is configured through **dataBlockStride** (the address step between different DataBlocks within a single iteration) and **repeatStride** (the address step of the same DataBlock between adjacent iterations).

- **dataBlockStride**<a id="datablockstride"></a>

    dataBlockStride refers to the address step between different DataBlocks within a single iteration. Its value cannot exceed the UB size limit.

    - For continuous computation, set dataBlockStride to 1 to process the eight DataBlocks within the same iteration continuously.
    - For non-continuous computation, set dataBlockStride to a value greater than 1 (for example, 2). In this case, an interval of one DataBlock occurs between different DataBlocks within the same iteration when data is read, as shown in the following figure.

    **Figure 4**  Examples of different dataBlockStride values  
    ![](../../../../figures/data_block_stride.png)<a id="fig-4-examples-of-different-datablockstride-values"></a>

    **Note: dstBlkStride does not support being set to 0. Setting it to 0 may cause undefined behavior. If srcBlkStride is set to 0, the source operand always reuses the first DataBlock.**

    The following figure shows an example of reading and writing the source operand and destination operand in the UB space within a single iteration. In this example, dataBlockStride of the source operand is set to 2, indicating that the address step between different DataBlocks within a single iteration is two DataBlocks. dataBlockStride of the destination operand is set to 1, indicating that the addresses are continuous within a single iteration.

    **Figure 5**  Diagram of a non-continuous scenario within a single iteration  
    ![](../../../../figures/single_iter_non_continuous.png)<a id="fig-5-diagram-of-a-non-continuous-scenario-within-a-single-iteration"></a>

- **repeatStride**<a id="repeatstride"></a>

    repeatStride is the address step of the same DataBlock between adjacent iterations.

    - **Continuous computation scenario:** Assume that a Tensor is defined for both the destination operand and the source operand (that is, the addresses overlap), and repeatStride is set to 8. In this case, the vector computation unit reads eight consecutive DataBlocks in the first iteration and the next eight consecutive DataBlocks in the second iteration. After multiple iterations, the computation of all input data is completed.

        ![](../../../../figures/repeat_stride1.png)

    - **Non-continuous computation scenario:** When repeatStride is greater than 8 (for example, 10), the data read by the vector computation unit between adjacent iterations is non-contiguous in address, with an interval of two DataBlocks.

        ![](../../../../figures/repeat_stride2.png)

    - **Repeated computation scenario:** When repeatStride is set to 0, the vector computation unit repeatedly reads and computes the first eight consecutive DataBlocks.

        ![](../../../../figures/repeat_stride3.png)

    - **Partially repeated computation:** When repeatStride is greater than 0 and less than 8, part of the data between adjacent iterations is repeatedly read and computed by the vector computation unit. This scenario generally does not apply to common use cases.

        ![](../../../../figures/repeat_stride4.png)

    When repeatTime is greater than 1 and multiple iterations are required to complete the vector computation, developers can set the value of repeatStride appropriately based on different usage scenarios. The following figure shows a schematic diagram of a non-continuous scenario across multiple iterations. In the example, the **repeatStride** of both the source operand and the destination operand is set to 9, indicating that the interval between the start addresses of the same DataBlock in adjacent iterations is 9 DataBlocks. The same DataBlock means that the DataBlock is at the same position within the iteration. For example, in the following figure, src1 and src9 are in adjacent iterations and are both at the position of the first DataBlock within the iteration, and the interval between them is the value of **repeatStride**.

    **Figure 6** Schematic diagram of a non-continuous scenario across multiple iterations  
    ![](../../../../figures/multi_iter_non_continuous.png)<a id="figure-6-schematic-diagram-of-a-non-continuous-scenario-across-multiple-iterations"></a>
