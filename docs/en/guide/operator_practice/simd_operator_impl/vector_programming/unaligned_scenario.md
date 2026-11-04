# Unaligned Scenarios<a name="ZH-CN_TOPIC_0000002532388127"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:26:47.418Z pushedAt=2026-11-03T07:05:13.317Z nl=2 -->

This section describes more ways to handle data that is not 32-byte aligned, including data move-in, calculation, and move-out. When developing operators, you can flexibly handle unaligned scenarios by referring to the following solution descriptions and operator samples.

## Alignment Requirements for Data Movement and Vector Computation<a name="zh-cn_topic_0000001945665465_section2052121844116"></a>

>[!NOTE] Note
>When performing data movement and vector calculation, the following alignment requirements apply to the data length to be moved and the start address of the operands:
>- When using the DataCopy API for data movement, the data length to be moved and the start address of the operands in the Unified Buffer (UB) must be 32-byte aligned.
>- Generally, when performing vector calculation, the start address of the operands must be 32-byte aligned. For specific alignment requirements, refer to the corresponding API reference.

In the following description, Global refers to a tensor in Global Memory, and Local refers to a tensor in Local Memory.

The following are some examples of unaligned movement and calculation.

-   **Unaligned copy-in**

    When 11 half values need to be copied from Global to Local, to ensure that the data length to be moved is 32-byte aligned, use DataCopy to copy 16 half (32B) values to Local, Local\[11\]\~Local\[15\] which are written as invalid data -1.

    **Figure 1**  Unaligned copy-in<a name="zh-cn_topic_0000001945665465_fig077582618576"></a>  
    ![](../../../figures/unaligned_in.png "Unaligned move in")

-   **Unaligned copy-out**

    When 11 half values need to be copied from Local to Global, to ensure that the data length to be moved is 32-byte aligned, use DataCopy to copy 16 half (32B) values to Global, Global\[11\]\~Global\[15\] which are written as invalid data -1.

    **Figure 2**  Unaligned move out<a name="zh-cn_topic_0000001945665465_fig926205085819"></a>  
    ![](../../../figures/unaligned_out.png "Unaligned move out")

-   **Error example of a vector calculation start address that is not 32-byte aligned**

    During vector calculation, ensure that the start address is 32-byte aligned. In the example as follows, the calculation starts from Local1\[7\], that is, the 8th number of the LocalTensor. The start address is not 32-byte aligned, so this is an error example.

    **Figure 3**  Error example of a vector calculation start address that is not 32-byte aligned<a name="zh-cn_topic_0000001945665465_fig2226615174"></a>  
    ![](../../../figures/vec_addr_err.png "Error example of a vector calculation start address that is not 32-byte aligned")

## Unaligned Data Handling Solutions<a name="zh-cn_topic_0000001945665465_section423121219432"></a>

The DataCopyPad API provides the unaligned move capability. If the operator is developed based on a product that supports this API (see product support), you can directly use this API to solve the data movement problem in unaligned scenarios. For a complete example of using DataCopyPad, see [DataCopyPad sample](../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_pad_gm2ub_ub2gm).

Some models do not support the DataCopyPad API. In this case, refer to the following data movement solutions.


First, use the DataCopy aligned data movement API to move data. Because the data length moved in must be 32-byte aligned, when the data length is not aligned, the Tensor data is moved from Global to Local row by row, and each row in Local contains redundant random data.

After the data is moved in, the redundant data can be handled in the following ways during vector calculation:

-   **Redundant data participates in calculation**. This is generally used in elewise calculation scenarios. Even if the redundant data participates in calculation, it does not affect the final result, as shown in the following figure:

    **Figure 4**  Redundant data participates in calculation<a name="zh-cn_topic_0000001945665465_fig1622416415109"></a>  
    ![](../../../figures/redundant_calc.png "Redundant data participates in calculation")



-   **Mask out the redundant data using the mask parameter**. This is generally used in scenarios such as axis reduction calculation. As shown in the following figure, ReduceMin is performed only on the first four half data elements. The redundant data can be masked out by setting the mask parameter:

    **Figure 5**  Mask out dirty data using mask<a name="zh-cn_topic_0000001945665465_fig1465824716196"></a>  
    ![](../../../figures/mask_dirty.png "Mask out dirty data using mask")

    The example snippet is as follows:
    ```
    uint64_t Mask[2] = {((uint64_t)1 << 4) - 1, 0}; // Only the first 4 elements participate in the operation.
    AscendC::ReduceMin<half>(output, input, workLocal, Mask, 1, 8);
    ```


-   **Clear row by row through Duplicate**. Before calculation, for each row of data, call the basic API [Duplicate](../../../../api/SIMD-API/basic_api/memory_vector_compute/data_padding/Duplicate.md) to fill 0 values at the redundant data positions. By setting the mask value, control that only the last 5 element positions are valid, and fill the redundant data with 0, as shown in the following figure:

    **Figure 6**  Clear row by row through Duplicate<a name="zh-cn_topic_0000001945665465_fig698012297178"></a>  
    ![](../../../figures/dup_clear.png "Clear row by row through Duplicate")

    The example snippet is as follows:
    ```
    uint64_t mask[2] = {0b1111100000000000, 0}; // Control that the last 5 of the 16 elements are valid.
    for (int32_t i = 0; i < 16; i++) {
        AscendC::Duplicate<half>(input[i*16], static_cast<half>(0), mask, 1, 1, 1);
    }
    ```

-   **Clear at once through Pad**. Before calculation, for multiple rows of data, you can use the advanced API [Pad](../../../../api/SIMD-API/adv_api/tensor_transform/Pad.md) to clear the redundant data at once.

    **Figure 7**  Clear at once through Pad<a name="zh-cn_topic_0000001945665465_fig181561339121716"></a>  
    ![](../../../figures/pad_clear.png "Clear at once through Pad")

    The example snippet is as follows:
    ```
    AscendC::PadParams padParams = {0, 10, 0}; 
    AscendC::Pad(outputLocal, inputLocal, padParams, tiling);
    ```

Finally, use the DataCopy aligned data move interface to move data out. Because the data length to be moved and the start address of the operand (on the UB) must be 32-byte aligned when moving out, you can choose to remove the redundant data or move out with the redundant data. The following methods are available:

-   **Use the UnPad interface to remove redundant data and then move out**. When the total length of the valid data to be moved out meets the 32-byte alignment requirement, you can use the advanced API [UnPad](../../../../api/SIMD-API/adv_api/tensor_transform/UnPad.md) to remove the redundant data and move it out completely. As shown in the following figure:

    **Figure 8** Move data out after removing redundant data using the UnPad API<a name="zh-cn_topic_0000001945665465_fig20692195012173"></a>  
    ![](../../../figures/unpad_copy.png "Move data out after removing redundant data using the UnPad API")

    The example snippet is as follows:
    ```
    AscendC::UnPadParams unPadParams = {0, 10}; 
    AscendC::UnPad<half>(outputLocal, inputLocal, unPadParams, tiling);
    ...
    AscendC::DataCopy(dstGlobal, outputLocal, 96);
    ```

-   **Use GatherMask to collect valid data and then move it out**. When the total length of the valid data to be moved out is greater than or equal to 32 bytes, you can use [GatherMask](../../../../api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/GatherMask.md) to collect the valid data again, ensuring that the start address and data length of the valid data to be moved out are 32-byte aligned. The following figure shows this:

    **Figure 9** Move data out after collecting valid data using GatherMask.<a name="zh-cn_topic_0000001945665465_fig1988535971719"></a>  
    ![](../../../figures/gather_mask.png "Move data out after collecting valid data using GatherMask")
    
    The example snippet is as follows:
    ```
    AscendC::LocalTensor<uint16_t> bufPattern = tmpPattern.Get<uint16_t>(); 
    AscendC::Duplicate<uint16_t>(bufPattern, 0, 16); 
    // Use the user-defined mode to configure the mask for data collection and select the data at index positions 3 to 18.
    bufPattern.SetValue(0, 0b1111111111111000); // uint16_t type
    bufPattern.SetValue(1, 0b0000000000000111);
    uint32_t mask = 32; 
    uint64_t rsvdCnt = 0; 
    AscendC::GatherMask(tailLocal, outputLocal, bufPattern, true, mask, {1, 1, 8, 8}, rsvdCnt);
    ...
    AscendC::DataCopy(dstGlobal, outputLocal, 16);
    AscendC::DataCopy(dstGlobal[3], tailLocal, 16); // Overwrite the data at index positions 3 to 18 on Global.
    ```
