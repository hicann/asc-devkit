# Unaligned Scenario<a name="ZH-CN_TOPIC_0000002532388127"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T13:52:16.092Z -->

This section describes more ways to handle non-32-byte-aligned data, including data move-in, calculation, and move-out. During actual operator development, you can flexibly handle unaligned scenarios by referring to the following solution descriptions and operator samples.

## Alignment Requirements for Data Movement and Vector Calculation<a name="zh-cn_topic_0000001945665465_section2052121844116"></a>

>[!NOTE]
>When performing data movement and vector calculation, the following alignment requirements apply to the data length to be moved and the start address of the operand:
>- When using the DataCopy API for data movement, the data length to be moved and the start address of the operand (on UB) must be 32-byte aligned.
>- Generally, when performing vector calculation, the start address of the operand must be 32-byte aligned. For specific alignment requirements, refer to the corresponding API reference for confirmation.

In the following description, Global refers to a tensor on Global Memory, and Local refers to a tensor on Local Memory.

The following are some examples of unaligned movement and calculation.

-   **Unaligned Move In**

    When 11 half values need to be copied from Global to Local, to ensure that the data length to be moved is 32-byte aligned, use DataCopy to copy 16 half (32B) values to Local. Local\[11\] to Local\[15\] are written as invalid data -1.

    **Figure 1**  Unaligned move in<a name="zh-cn_topic_0000001945665465_fig077582618576"></a>  
    ![](../../../figures/unaligned_move_in.png "Unaligned move in")

-   **Unaligned Move Out**

    When 11 half values need to be copied from Local to Global, to ensure that the data length to be moved is 32-byte aligned, use DataCopy to copy 16 half (32B) values to Global. Global\[11\] to Global\[15\] are written as invalid data -1.

    **Figure 2**  Unaligned move out<a name="zh-cn_topic_0000001945665465_fig926205085819"></a>  
    ![](../../../figures/unaligned_move_out.png "Unaligned move out")

-   **Incorrect example of a vector calculation start address that is not 32-byte aligned**

    During vector calculation, ensure that the start address is 32-byte aligned. In the following example, the calculation starts from Local1\[7\], that is, the 8th element of the LocalTensor. The start address is not 32-byte aligned, so this is an incorrect example.

    **Figure 3**  Incorrect example of a vector calculation start address that is not 32-byte aligned<a name="zh-cn_topic_0000001945665465_fig2226615174"></a>  
    ![](../../../figures/vector_calculation_start_address_not_32_byte_aligned_incorrect_example.png "Incorrect example of a vector calculation start address that is not 32-byte aligned")

## Unaligned Data Handling Solutions<a name="zh-cn_topic_0000001945665465_section423121219432"></a>

The DataCopyPad API provides the unaligned move capability. If you develop operators based on products that support this API (see product support), you can directly use this API to resolve the move issue in unaligned scenarios. For a complete example of using DataCopyPad, see [DataCopyPad Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).

Some models do not support the DataCopyPad API. In this case, refer to the following move solutions.


First, use the DataCopy aligned data move API to move data. Because the data length moved in must be 32-byte aligned, when the data length is not aligned, Tensor data is moved from Global to Local row by row, and each row in Local contains redundant random data.

After the data is moved in, the redundant data can be handled in the following ways during vector calculation:

-   **Redundant data participates in calculation**. This is generally used in elewise calculation scenarios. Even if the redundant data participates in calculation, it does not affect the final result, as shown in the following figure:

    **Figure 5**  Redundant data participates in calculation<a name="zh-cn_topic_0000001945665465_fig1622416415109"></a>  
    ![](../../../figures/redundant_data_participates_in_calculation.png "Redundant data participates in calculation")



-   **Mask redundant data through the mask parameter**. This is generally used in scenarios such as axis reduction calculation. As shown in the following figure, ReduceMin is performed only on the first four half data elements. The redundant data can be masked by setting the mask parameter:

    **Figure 6**  Using mask to mask dirty data<a name="zh-cn_topic_0000001945665465_fig1465824716196"></a>  
    ![](../../../figures/using_mask_to_mask_dirty_data.png "Using mask to mask dirty data")

    The example snippet is as follows:
    ```
    uint64_t Mask[2] = {((uint64_t)1 << 4) - 1, 0}; // Only the first four elements participate in the operation
    AscendC::ReduceMin<half>(output, input, workLocal, Mask, 1, 8);
    ```


-   **Clear row by row through Duplicate**. Before calculation, for each row of data, call the basic API [Duplicate](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_padding/Duplicate.md) to fill 0 into the redundant data positions. By setting the mask value, control only the last five element positions to be valid, and fill the redundant data with 0, as shown in the following figure:

    **Figure 7**  Clearing row by row through Duplicate<a name="zh-cn_topic_0000001945665465_fig698012297178"></a>  
    ![](../../../figures/clear_row_by_row_through_duplicate.png "Clearing row by row through Duplicate")

    The example snippet is as follows:
    ```
    uint64_t mask[2] = {0b1111100000000000, 0}; // Control the last five of the 16 elements to be valid
    for (int32_t i = 0; i < 16; i++) {
        AscendC::Duplicate<half>(input[i*16], static_cast<half>(0), mask, 1, 1, 1);
    }
    ```

-   **Clear at once through Pad**. Before calculation, for multiple rows of data, you can use the high-order API [Pad](../../../../api/SIMD-API/advanced_api/tensor_transform/Pad.md) to clear the redundant data at once.

    **Figure 8**  Clearing at once through Pad<a name="zh-cn_topic_0000001945665465_fig181561339121716"></a>  
    ![](../../../figures/clear_at_once_through_pad.png "Clearing at once through Pad")

    The example snippet is as follows:
    ```
    AscendC::PadParams padParams = {0, 10, 0}; 
    AscendC::Pad(outputLocal, inputLocal, padParams, tiling);
    ```

Finally, use the DataCopy aligned data move API to move data out. Because the data length to be moved and the start address of the operand (on the UB) must be 32-byte aligned when moving out, you can choose to remove the redundant data or move out with the redundant data. The following methods are available:

-   **Use the UnPad API to remove redundant data and then move out**. When the total length of the valid data to be moved out meets the 32-byte alignment requirement, you can use the high-order API [UnPad](../../../../api/SIMD-API/advanced_api/tensor_transform/UnPad.md) to remove the redundant data and move it out completely. As shown in the following figure:

    **Figure 9**  Move out after removing redundant data using the UnPad API<a name="zh-cn_topic_0000001945665465_fig20692195012173"></a>  
    ![](../../../figures/move_out_after_removing_redundant_data_using_unpad.png "Move out after removing redundant data using the UnPad API")

    The example snippet is as follows:
    ```
    AscendC::UnPadParams unPadParams = {0, 10}; 
    AscendC::UnPad<half>(outputLocal, inputLocal, unPadParams, tiling);
    ...
    AscendC::DataCopy(dstGlobal, outputLocal, 96);
    ```

-   **Collect valid data using GatherMask and then move it out**. When the total length of the valid data to be moved out is greater than or equal to 32 bytes, you can use [GatherMask](../../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/GatherMask.md) to collect the valid data again, ensuring that the start address and data length of the valid data to be moved out are 32-byte aligned. The following figure shows this:

    **Figure 10**  Collect valid data using GatherMask and then move it out.<a name="zh-cn_topic_0000001945665465_fig1988535971719"></a>  
    ![](../../../figures/collect_valid_data_using_gathermask_and_move_it_out.png "Collect valid data using GatherMask and then move it out")

    The example snippet is as follows:
    ```
    AscendC::LocalTensor<uint16_t> bufPattern = tmpPattern.Get<uint16_t>(); 
    AscendC::Duplicate<uint16_t>(bufPattern, 0, 16); 
    // Use a user-defined pattern to configure the mask for data collection and select the data at index positions 3 to 18.
    bufPattern.SetValue(0, 0b1111111111111000); // uint16_t type
    bufPattern.SetValue(1, 0b0000000000000111);
    uint32_t mask = 32; 
    uint64_t rsvdCnt = 0; 
    AscendC::GatherMask(tailLocal, outputLocal, bufPattern, true, mask, {1, 1, 8, 8}, rsvdCnt);
    ...
    AscendC::DataCopy(dstGlobal, outputLocal, 16);
    AscendC::DataCopy(dstGlobal[3], tailLocal, 16); // Overwrite the data at index positions 3 to 18 on Global.
    ```
