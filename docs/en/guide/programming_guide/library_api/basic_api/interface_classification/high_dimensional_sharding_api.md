# High-Dimensional Sharding API<a name="ZH-CN_TOPIC_0000002271261208"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T12:24:05.536Z pushedAt=2026-09-06T02:44:21.920Z -->

> [!NOTE] Note
> - This section explains the **tensor high-dimensional sharding computation** APIs in **basic vector computation APIs**. If you do not need to use these APIs, you can skip this section.
> - In the following content, **repeatTime**, **dataBlockStride**, **repeatStride**, and **mask** are generic descriptions, and their names may not exactly correspond to the parameter names in specific instructions.
>     For example, the **dataBlockStride** parameter, which indicates the address step between different datablocks within a single iteration, corresponds to the **dstBlkStride** and **srcBlkStride** parameters in unary APIs, and to the **dstBlkStride**, **src0BlkStride**, and **src1BlkStride** parameters in binary APIs.
>     You can find the description of the parameter meanings in the parameter description of specific APIs.

The **tensor high-dimensional sharding computation** APIs can fully leverage hardware advantages, allowing developers to control the **iterative execution** of instructions and the **address interval** of operands, providing more flexible functionality.

Vector computation is performed by the Vector compute unit. Both the source operands and destination operands of vector computation are stored in the Unified Buffer (UB). In each iteration, the Vector compute unit reads **8** **datablocks** from the UB (the addresses within each datablock are contiguous, and each datablock is 32 bytes in length), performs computation, and writes the results to the corresponding 8 datablocks. The following figure shows the schematic diagram of Exp computation on 8 datablocks within a single iteration.

**Figure 1** Schematic diagram of Exp computation on 8 datablocks within a single iteration<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_fig143499419171"></a>  
![](../../../../figures/schematic_diagram_of_exp_computation_on_8_datablocks_within_a_single_iteration.png "Schematic diagram of Exp computation on 8 datablocks within a single iteration")

-   Vector computation APIs allow developers to configure the number of iterations through **repeatTime**, thereby controlling the execution of multiple iterations of an instruction. Assuming **repeatTime** is set to **2**, the Vector compute unit performs 2 iterations of computation, producing 2 \* 8 (8 datablocks per iteration) \* 32 bytes (32 bytes per datablock) = 512 bytes of results. If the data type is half, 256 elements are computed. The following figure shows the schematic diagram of Exp computation over 2 iterations. Due to hardware limitations, **repeatTime cannot exceed 255.**

    **Figure 2** Exp computation over 2 iterations<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_fig148871676180"></a>  
    ![](../../../../figures/exp_computation_over_2_iterations.png "Exp computation over 2 iterations")

-   For data within the same iteration, you can use the **mask** parameter to perform a mask operation to control the number of elements that actually participate in the computation. The following figure shows the schematic diagram of controlling which elements participate in the computation bit by bit using the mask bit-by-bit mode during Abs computation, where 1 indicates participation in the computation and 0 indicates non-participation.

    **Figure 3** Schematic diagram of mask operation using the **mask** parameter (using the float data type as an example)<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_fig7630131541814"></a>  
    ![](../../../../figures/mask_operation_using_mask_parameter_with_float_data_type_as_example.png "Schematic diagram of mask operation using the mask parameter (using the float data type as an example)")

-   The Vector compute unit also supports vector computation with intervals, which is configured through **dataBlockStride** (the address step between different datablocks within a single iteration) and **repeatStride** (the address step of the same datablock between adjacent iterations).
    -   **dataBlockStride**

        To control the data processing stride within a single iteration, set **dataBlockStride**, which is the address step between different datablocks within the same iteration. The following figure shows a schematic diagram of a non-contiguous scenario within a single iteration. In this example, **dataBlockStride** of the source operand is set to **2**, indicating that the address step (the interval between start addresses) between different datablocks within a single iteration is 2 datablocks.

        **Figure 4** Schematic diagram of a non-contiguous scenario within a single iteration<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_fig744519103222"></a>  
        ![](../../../../figures/non_contiguous_scenario_within_single_iteration.png "Schematic diagram of a non-contiguous scenario within a single iteration")

    -   **repeatStride**

        When **repeatTime** is greater than 1 and multiple iterations are required to complete the vector computation, you can set **repeatStride**, which is the address step of the same datablock between adjacent iterations, to an appropriate value based on the usage scenario.

        The following figure shows a schematic diagram of a non-contiguous scenario across multiple iterations. In this example, **repeatStride** of both the source operand and the destination operand is set to 9, indicating that the interval between the start addresses of the same datablock in adjacent iterations is 9 datablocks. The same datablock refers to a datablock at the same position within an iteration. For example, src1 and src9 in the following figure are in adjacent iterations and both occupy the first datablock position within their respective iterations, so the interval between them is the value of **repeatStride**.

        **Figure 5** Schematic diagram of a non-contiguous scenario across multiple iterations<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_fig7376324392"></a>  
        ![](../../../../figures/non_contiguous_scenario_across_multiple_iterations.png "Schematic diagram of a non-contiguous scenario across multiple iterations")

The following sections provide detailed configuration instructions and examples for **dataBlockStride**, **repeatStride**, and **mask**.

## dataBlockStride<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_section2815124173416"></a>

**dataBlockStride** is the address step between different datablocks within the same iteration.

-   For continuous computation, set **dataBlockStride** to **1** to process the eight datablocks within the same iteration continuously.
-   For non-continuous computation, set **dataBlockStride** to a value greater than 1 (for example, 2). In this case, an interval of one datablock occurs between different datablocks within the same iteration when data is read, as shown in the following figure.

    **Figure 6**  Example of different dataBlockStride values<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_fig6550124115203"></a>  
    ![](../../../../figures/dataBlockStride_different_value_example.png "Example of different dataBlockStride values")

## repeatStride<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_section139459347420"></a>

**repeatStride** is the address step of the same datablock between adjacent iterations.

-   **Contiguous computation scenario**: Assume that a tensor is defined for both the destination operand and the source operand (that is, the addresses overlap), and **repeatStride** is set to **8**. In this case, the vector compute unit reads eight contiguous datablocks in the first iteration and the next eight contiguous datablocks in the second iteration. After multiple iterations, the computation of all input data is completed.

    ![](../../../../figures/repeat_stride1.png)

-   **Non-continuous computation scenario**: When **repeatStride** is greater than 8 (for example, 10), the data read by the vector compute unit between adjacent iterations is non-contiguous in address, with an interval of two datablocks.

    ![](../../../../figures/repeat_stride2.png)

-   **Repeated computation scenario**: When **repeatStride** is set to **0**, the vector compute unit repeatedly reads and computes the first eight contiguous datablocks.

    ![](../../../../figures/repeat_stride3.png)

-   **Partially repeated computation**: When **repeatStride** is greater than 0 and less than 8, part of the data between adjacent iterations is repeatedly read and computed by the vector compute unit. This scenario is generally not involved.

    ![](../../../../figures/repeat_stride4.png)

## mask Parameter<a name="zh-cn_topic_0000002267504656_zh-cn_topic_0000001764162593_section4252658182"></a>

**mask** is used to control the elements that participate in computation within each iteration. It can be set in two modes: contiguous mode and bit-by-bit mode.

-   Contiguous mode: indicates how many contiguous elements at the front participate in computation. The data type is uint64\_t. The value range depends on the data type of the source operand. Different data types have different maximum numbers of elements that can be processed within each iteration (the maximum number of elements that can be processed in a single iteration for the current data type is 256 / sizeof(data type)). When the operand data type occupies 16 bits (such as half/uint16\_t), mask∈\[1, 128\]; when the operand is 32 bits (such as float/int32\_t), mask∈\[1, 64\].

    The specific examples are as follows:

    ```
    // The maximum number of elements that can be processed in a single iteration for the int16_t data type is 256/sizeof(int16_t) = 128. mask = 64, and mask∈[1, 128], so it is a valid input.
    // repeatTime = 1. There are 128 elements in total, and a single iteration can process 128 elements, so repeatTime = 1.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1. Data is read and written continuously within a single iteration.
    // dstRepStride, src0RepStride, src1RepStride = 8. Data is read and written continuously between iterations.
    uint64_t mask = 64;
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 1, { 1, 1, 1, 8, 8, 8 });
    ```

    The result is as follows:

    ```
    Input data(src0Local): [1 2 3 ... 64 ...128]
    Input data(src1Local): [1 2 3 ... 64 ...128]
    Output data(dstLocal): [2 4 6 ... 128 undefined...undefined]
    ```

    ```
    // The maximum number of elements that can be processed in a single iteration for the int32_t data type is 256/sizeof(int32_t) = 64. mask = 64, and mask∈[1, 64], so it is a valid input.
    // repeatTime = 1. There are 64 elements in total, and a single iteration can process 64 elements, so repeatTime = 1.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration
    // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between iterations
    uint64_t mask = 64;
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 1, { 1, 1, 1, 8, 8, 8 });
    ```

    The result is as follows:

    ```
    Input data(src0Local): [1 2 3 ... 64]
    Input data(src1Local): [1 2 3 ... 64]
    Output data(dstLocal): [2 4 6 ... 128]
    ```

-   Bit-by-bit mode: You can control which elements participate in the computation on a bit-by-bit basis. A bit value of 1 indicates that the element participates in the computation, and 0 indicates that it does not.

    **mask** is in array form. The array length and the value range of array elements depend on the data type of the operand. When the operand is 16-bit, the array length is 2, and mask\[0\] and mask\[1\] ∈ \[0, 2<sup>64</sup>-1\] and are not both 0. When the operand is 32-bit, the array length is 1, and mask\[0\] ∈ \(0, 2<sup>64</sup>-1\]. When the operand is 64-bit, the array length is 1, and mask\[0\] ∈ \(0, 2<sup>32</sup>-1\].

    The following is a specific example:

    ```
    // The data type is int16_t
    uint64_t mask[2] = {6148914691236517205, 6148914691236517205};
    // repeatTime = 1, there are 128 elements in total, and a single iteration can process 128 elements, so repeatTime = 1.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, data is read and written continuously within a single iteration.
    // dstRepStride, src0RepStride, src1RepStride = 8, data is read and written continuously between iterations.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 1, { 1, 1, 1, 8, 8, 8 });
    ```

    The result is as follows:

    ```
    Input data(src0Local): [1 2 3 ... 64 ...127 128]
    Input data(src1Local): [1 2 3 ... 64 ...127 128]
    Output data(dstLocal): [2 undefined 6 ... undefined ...254 undefined]
    ```

    The mask process is as follows:

    mask=\{6148914691236517205, 6148914691236517205\} (Note: 6148914691236517205 represents the 64-bit binary number 0b010101....01, and the mask is arranged from low bit to high bit.)

    ![](../../../../figures/mask.png)

    ```
    // The data type is int32_t.
    uint64_t mask[1] = {6148914691236517205};
    // repeatTime = 1. There are 64 elements in total, and a single iteration can process 64 elements, so repeatTime = 1.
    // dstBlkStride, src0BlkStride, src1BlkStride = 1. Data is read and written continuously within a single iteration.
    // dstRepStride, src0RepStride, src1RepStride = 8. Data is read and written continuously between iterations.
    AscendC::Add(dstLocal, src0Local, src1Local, mask, 1, { 1, 1, 1, 8, 8, 8 });
    ```

    The result is as follows:

    ```
    Input data(src0Local): [1 2 3 ... 63 64]
    Input data(src1Local): [1 2 3 ... 63 64]
    Output data(dstLocal): [2 undefined 6 ... 126 undefined]
    ```

    The mask process is as follows:

    mask=\{6148914691236517205, 0\} (Note: 6148914691236517205 represents the 64-bit binary number 0b010101....01.)

    ![](../../../../figures/mask_32bit.png)

