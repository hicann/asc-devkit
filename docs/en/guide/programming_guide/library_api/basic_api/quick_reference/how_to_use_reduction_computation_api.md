# How to Use Reduction Computation APIs<a name="ZH-CN_TOPIC_0000002271364328"></a>

<!-- md-trans-meta sourceCommit=305969706ea1d3db04aa6a98519b00ea0bcbb3ba translatedAt=2026-08-26T12:22:26.639Z pushedAt=2026-09-03T12:13:30.703Z -->

A reduction instruction reduces a data set to a single value or a smaller set. Based on the data range of the reduction operation, reduction instructions are classified into the following types. For details, see the [reduction instruction schematic diagram](#zh-cn_topic_0000002267504648_fig16735034112710):

-   **ReduceMax**/**ReduceMin**/**ReduceSum**: Performs a reduction operation on all input data to obtain the maximum value and its index, the minimum value and its index, or the data sum.
-   **ReduceRepeat**: Performs a reduction operation on the input data within each repeat. The template parameter `ReduceType` specifies whether to obtain the maximum value, minimum value, or summation, and the result is the maximum value and its index, the minimum value and its index, or the data sum within each repeat. When an index is returned, it is the index within the repeat.
-   **ReduceDataBlock**: Performs a reduction operation on the input data within each DataBlock. The template parameter `ReduceType` specifies whether to obtain the maximum value, minimum value, or summation, and the result is the maximum value, minimum value, or data sum within each DataBlock.
-   **ReducePairElem**: Performs a reduction operation on each pair of adjacent (odd-even) elements. Currently, only summation specified by the template parameter `ReduceType::SUM` is supported. For example, for (a1, a2, a3, a4, a5, a6...), the result after reduction is (a1+a2, a3+a4, a5+a6, ......).

**Figure 1** Reduction instruction schematic diagram<a name="zh-cn_topic_0000002267504648_fig16735034112710"></a>  
![](../../../../figures/Reduce_summary.png "Reduction instruction schematic diagram")

For reduction instructions, as with other basic APIs, a **tensor high-dimensional sharding computation** API is also provided. It fully leverages hardware advantages and allows developers to control the **iterative execution** of instructions and the **address stride** of operands, making the functionality more flexible. However, the units and constraints of specific parameters differ slightly from those of the [basic APIs](../interface_classification/high_dimensional_sharding_api.md). The differences are described below.

-   **mask**: Controls the elements that participate in the computation within each iteration. The usage of the mask parameter is the same as the general usage of basic APIs.
-   **repeatTime**: Number of iterations. Developers configure the number of iterations through **repeatTime** to control the multiple iterative executions of the instruction.
    -   For **ReduceMax**/**ReduceMin**/**ReduceSum**, when **repeatTime** exceeds 255, the API handles it internally, so **repeatTime** supports a larger value range, as long as it does not exceed the maximum value of int32_t.
    -   **ReduceRepeat**/**ReduceDataBlock**/**ReducePairElem** are the same as other basic APIs in that **repeatTime** must not exceed 255.
-   **repeatStride**: indicates the address stride between adjacent iterations.
    -   The destination operand of the **ReduceMax**/**ReduceMin**/**ReduceSum** instruction is reduced to a single maximum value/minimum value/sum, so its destination operand does not support configuring **repeatStride**. Only the source operand supports **repeatStride**, and its meaning and unit (datablock) are consistent with the general description of basic APIs.
    -   For **ReduceRepeat**/**ReduceDataBlock**/**ReducePairElem**, both the source operand and the destination operand support configuring **repeatStride**. The meaning and unit (DataBlock) of the source operand's **repeatStride** are consistent with the general description of basic APIs. The meaning and unit of the destination operand's **repeatStride** differ from the general description of basic APIs, because after reduction, the length of the destination operand becomes shorter. For example, after **ReduceRepeat** reduction, each repeat is merged into a single value, so the interval between iterations can no longer use one DataBlock as the unit, but instead uses the length after one repeat is reduced as the unit.

-   **dataBlockStride**: indicates the address stride of datablocks within a single iteration.
    -   The destination operand of the **ReduceMax**/**ReduceMin**/**ReduceSum** instruction is reduced to a single maximum value/minimum value/sum, so its destination operand does not support configuring **dataBlockStride**. The source operand also does not support **dataBlockStride**.
    -   For **ReduceRepeat**/**ReduceDataBlock**/**ReducePairElem**, the source operand supports configuring **dataBlockStride**, and its meaning and unit (DataBlock) are consistent with the general description of basic APIs. The destination operand does not support **dataBlockStride**, because after reduction, the length of the destination operand becomes shorter. For example, after **ReduceRepeat** reduction, each repeat is merged into a single value, so the concepts of DataBlock within an iteration and address interval no longer exist.
