# Unaligned Scenarios<a name="ZH-CN_TOPIC_0000002532388127"></a>

<!-- md-trans-meta sourceCommit=49eda737d7d7268a9ddb02fcaba4732176bfc076 translatedAt=2026-11-09T10:33:52.484Z pushedAt=2026-11-19T06:32:19.761Z nl=2 -->

This section describes more ways to handle data that is not 32-byte aligned, including data copy-in, compute, and copy-out. When developing operators, you can flexibly handle unaligned scenarios by referring to the following solution descriptions and operator samples.

## Alignment Requirements for Data Movement and Vector Computation<a name="zh-cn_topic_0000001945665465_section2052121844116"></a>

>[!NOTE] Note 
>When performing data movement and vector compute, the following alignment requirements apply to the data length to be moved and the start address of the operands:
>- When using the **DataCopy** API for data movement, the data length to be moved and the start address of the operands in the Unified Buffer (UB) must be 32-byte aligned.
>- Generally, when performing vector compute, the start address of the operands must be 32-byte aligned. For specific alignment requirements, refer to the corresponding API reference.

In the following description, Global refers to a tensor in Global Memory, and Local refers to a tensor in Local Memory.

The following are some examples of unaligned movement and compute.

-   **Unaligned copy-in**

    When 11 half values need to be copied from Global to Local, to ensure that the data length to be moved is 32-byte aligned, use **DataCopy** to copy 16 half (32B) values to Local, Local\[11\]\~Local\[15\] which are written as invalid data **-1**.

    **Figure 1**  Unaligned copy-in<a name="zh-cn_topic_0000001945665465_fig077582618576"></a>  
    <img src="../../../figures/unaligned_in.png" alt="Unaligned copy-in" width="600px" height="139px">

-   **Unaligned copy-out**

    When 11 half values need to be copied from Local to Global, to ensure that the data length to be moved is 32-byte aligned, use **DataCopy** to copy 16 half (32B) values to Global, Global\[11\]\~Global\[15\] which are written as invalid data **-1**.

    **Figure 2**  Unaligned copy-out<a name="zh-cn_topic_0000001945665465_fig926205085819"></a>  
    <img src="../../../figures/unaligned_out.png" alt="Unaligned copy-out" width="600px" height="139px">

-   **Error example of a vector compute start address that is not 32-byte aligned**

    During vector compute, ensure that the start address is 32-byte aligned. In the example as follows, the compute starts from Local1\[7\], that is, the 8th number of the LocalTensor. The start address is not 32-byte aligned, so this is an error example.

    **Figure 3**  Error example of a vector compute start address that is not 32-byte aligned<a name="zh-cn_topic_0000001945665465_fig2226615174"></a>  
    <img src="../../../figures/vec_addr_err.png" alt="Error example of a vector compute start address that is not 32-byte aligned" width="600px" height="232px">

## Unaligned Data Handling Solutions<a name="zh-cn_topic_0000001945665465_section423121219432"></a>

The **DataCopyPad** API provides the unaligned move capability. If the operator is developed based on a product that supports this API (see product support), you can directly use this API to solve the data movement problem in unaligned scenarios. For a complete example of using **DataCopyPad**, see the [DataCopyPad sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).

Some models do not support the **DataCopyPad** API. In this case, refer to the following data movement solutions.


First, use the **DataCopy** aligned data movement API to move data. Because the data length moved in must be 32-byte aligned, when the data length is not aligned, the tensor data is moved from Global to Local row by row, and each row in Local contains redundant random data.

After the data is moved in, the redundant data can be handled in the following ways during vector compute:

-   **Redundant data participates in compute**. This is generally used in elewise compute scenarios. Even if the redundant data participates in compute, it does not affect the final result, as shown in the following figure:

    **Figure 4**  Redundant data participating in compute<a name="zh-cn_topic_0000001945665465_fig1622416415109"></a>  
    <img src="../../../figures/redundant_calc.png" alt="Redundant data participating in compute" width="600px" height="244px">



-   **Mask out the redundant data using the mask parameter**. This is generally used in scenarios such as axis reduction compute. As shown in the following figure, **ReduceMin** is performed only on the first four half data elements. The redundant data can be masked out by setting the mask parameter:

    **Figure 5**  Using a mask to mask out dirty data<a name="zh-cn_topic_0000001945665465_fig1465824716196"></a>  
    <img src="../../../figures/mask_dirty.png" alt="Using a mask to mask out dirty data" width="600px" height="244px">

    The example snippet is as follows:

    ```
    uint64_t Mask[2] = {((uint64_t)1 << 4) - 1, 0}; // Only the first 4 elements participate in the operation.
    AscendC::ReduceMin<half>(output, input, workLocal, Mask, 1, 8);
    ```


-   **Clear row by row through Duplicate**. Before compute, for each row of data, call the basic API [Duplicate](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_padding/Duplicate.md) to fill 0 values at the redundant data positions. By setting the mask value, control that only the last 5 element positions are valid, and fill the redundant data with 0, as shown in the following figure:

    **Figure 6**  Row-by-row clearing through Duplicate<a name="zh-cn_topic_0000001945665465_fig698012297178"></a>  
    <img src="../../../figures/dup_clear.png" alt="Row-by-row clearing through Duplicate" width="600px" height="244px">

    The example snippet is as follows:

    ```
    uint64_t mask[2] = {0b1111100000000000, 0}; // Control the last 5 of the 16 elements to be valid.
    for (int32_t i = 0; i < 16; i++) {
        AscendC::Duplicate<half>(input[i*16], static_cast<half>(0), mask, 1, 1, 1);
    }
    ```

-   **Clear at once through Pad**. Before compute, for multiple rows of data, you can use the advanced API [Pad](../../../../api/SIMD-API/adv_api/tensor_transform/Pad.md) to clear the redundant data at once.

    **Figure 7**  One-time clearing through Pad<a name="zh-cn_topic_0000001945665465_fig181561339121716"></a>  
    <img src="../../../figures/pad_clear.png" alt="One-time clearing through Pad" width="600px" height="525px">

    The example snippet is as follows:

    ```
    AscendC::PadParams padParams = {0, 10, 0}; 
    AscendC::Pad(outputLocal, inputLocal, padParams, tiling);
    ```

Finally, use the **DataCopy** aligned data move API to move data out. Because the data length to be moved and the start address of the operand (on the UB) must be 32-byte aligned when moving out, you can choose to remove the redundant data or move out with the redundant data. The following methods are available:

-   **Use the UnPad API to remove redundant data and then move out**. When the total length of the valid data to be moved out meets the 32-byte alignment requirement, you can use the advanced API [UnPad](../../../../api/SIMD-API/adv_api/tensor_transform/UnPad.md) to remove the redundant data and move it out completely. As shown in the following figure:

    **Figure 8**  Removing redundant data through the UnPad API and then moving out<a name="zh-cn_topic_0000001945665465_fig20692195012173"></a>  
    <img src="../../../figures/unpad_copy.png" alt="Removing redundant data through the UnPad interface and then moving out" width="600px" height="522px">

    The example snippet is as follows:

    ```
    AscendC::UnPadParams unPadParams = {0, 10}; 
    AscendC::UnPad<half>(outputLocal, inputLocal, unPadParams, tiling);
    ...
    AscendC::DataCopy(dstGlobal, outputLocal, 96);
    ```

-   **Use GatherMask to collect valid data and then move it out**. When the total length of the valid data to be moved out is greater than or equal to 32 bytes, you can use [GatherMask](../../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/GatherMask.md) to collect the valid data again, ensuring that the start address and data length of the valid data to be moved out are 32-byte aligned. The following figure shows this:

    **Figure 9** Move out after collecting valid data using GatherMask.<a name="zh-cn_topic_0000001945665465_fig1988535971719"></a>  
    <img src="../../../figures/gather_mask.png" alt="Move out after collecting valid data using GatherMask" width="920px" height="406px">
    
    The example snippet is as follows:

    ```
    AscendC::LocalTensor<uint16_t> bufPattern = tmpPattern.Get<uint16_t>(); 
    AscendC::Duplicate<uint16_t>(bufPattern, 0, 16); 
    // Use the user-defined mode to configure the data collection mask and select the data at index positions 3 to 18.
    bufPattern.SetValue(0, 0b1111111111111000); // uint16_t type
    bufPattern.SetValue(1, 0b0000000000000111);
    uint32_t mask = 32; 
    uint64_t rsvdCnt = 0; 
    AscendC::GatherMask(tailLocal, outputLocal, bufPattern, true, mask, {1, 1, 8, 8}, rsvdCnt);
    ...
    AscendC::DataCopy(dstGlobal, outputLocal, 16);
    AscendC::DataCopy(dstGlobal[3], tailLocal, 16); // Overwrite the data at index positions 3 to 18 on Global.
    ```
