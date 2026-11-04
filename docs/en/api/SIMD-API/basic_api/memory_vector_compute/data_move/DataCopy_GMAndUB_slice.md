# DataCopy (Sliced Data Movement Between GM and UB)<a name="ZH-CN_TOPIC_0000002568770166"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:35:04.960Z pushedAt=2026-10-28T03:14:39.490Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002565968945_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMAndUB_slice_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002565968945_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API is a software simulation API designed for ease of use. It supports slice data movement and extracts a subset of a multi-dimensional Tensor for movement.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> UB
    - GM -> VECIN
- UB -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id8 -->
    - CO2 -> GM (supported only on the AI Core of Atlas inference products)
    <!-- end id8 -->

## Prototype<a name="zh-cn_topic_0000002565968945_section82039854412"></a>

- Global Memory -> UB

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

- UB -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T> &dst, const LocalTensor<T> &src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

> [!NOTE]Note 
> For the data types supported by each prototype, please refer to [Data Type](#zh-cn_topic_0000002565968945_section4219135304818).

## Parameter Description<a name="zh-cn_topic_0000002565968945_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. For supported data types, please refer to [Data Type](#zh-cn_topic_0000002565968945_section4219135304818). |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand.<br>&bull;When the type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md), the storage location is Unified Buffer (UB), and the destination address requires 32-byte alignment.<br>&bull;When the type is [GlobalTensor](../../data_structures/GlobalTensor/GlobalTensor_intro.md), the storage location is Global Memory, and the destination address requires 1-byte alignment. |
| src | Input | Source operand.<br>&bull;When the type is GlobalTensor, the storage location is Global Memory, and the source address requires 1-byte alignment.<br>&bull;When the type is LocalTensor, the storage location is UB, and the source address requires 32-byte alignment. |
| dstSliceInfo | Input | Slice information of the destination operand, of the SliceInfo type. This parameter configures the start and end element counts, interval, length, and other information of the slice.<br>For details about the SliceInfo parameters, please refer to [Table 3](#table_slice_3). |
| srcSliceInfo | Input | Slice information of the source operand, of the SliceInfo type. This parameter configures the start and end element counts, interval, length, and other information of the slice.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For details about the SliceInfo parameters, please refer to [Table 3](#table_slice_3). |
| dimValue | Input | Dimension information of the operand. The default value is 1. |

**Table 3**  SliceInfo structure parameter definition<a name="table_slice_3"></a>

| Parameter | Description |
| :--- | :--- |
| startIndex | Start element position of the slice. The data type is uint32_t, and the unit is the number of elements. |
| endIndex | End element position of the slice. The data type is uint32_t, and the unit is the number of elements. |
| stride | Number of interval elements of the slice. The data type is uint32_t. |
| burstLen | Horizontal slice, the length of each slice of data. It takes effect only when dimValue = 1. When the dimension exceeds 1, it must be configured as 1, and other values are not supported. The data type is uint32_t, and the unit is datablock (32B).<br>For example, the List of srcSliceInfo is {{16, 70, 7, 3, 87},  {0, 2, 1, 1, 3}}. {16, 70, 7, 3, 87} indicates the slice information of the first dimension. When burstLen is set to 3, it indicates that the size of one slice data segment is 3 datablocks. {0, 2, 1, 1, 3} is the slice information of the second dimension, and burstLen can only be set to 1. |
| shapeValue | Original length of the current dimension. The data type is uint32_t, and the unit is the number of elements. |

As shown in [Figure 1](#fig1196816115710), taking the float data type as an example, the slice transfer API can select four discontinuous horizontal slices from the source operand with a shape of 3\*87 and transfer them to the destination operand. The following analyzes the SliceInfo structure parameters with reference to the figure:

**Figure 1**  Parameter parsing diagram<a name="fig1196816115710"></a>  
![](../../../../figures/slice_datacopy_parameter_analysis_diagram.png "Parameter parsing diagram")

- dimValue is 2, indicating that the operand has 2 dimensions.
- srcSliceInfo is {{16, 70, 7, 3, 87},  {0, 2, 1, 1, 3}}
    - {16, 70, 7, 3, 87} is configured for a single row, that is, from the one-dimensional perspective, and each element represents a number:

        **startIndex** = 16, indicating that the valid data segment starts from the 16th element;

        **endIndex** = 70, indicating that the valid data segment ends at the 70th element;

        **stride** = 7, in number of elements, indicating the number of elements between two adjacent slice data segments, which is a gap of seven 0s;

        **burstLen** = 3, in 32B, indicating that within this valid data segment, the size of one slice data segment is three datablocks;

        **shapeValue** = 87, indicating the length of a single row, in number of elements, that is, 8 \* 10 + 7 = 87 elements.

    - {0, 2, 1, 1, 3} is for multiple rows, that is, configured from a two-dimensional perspective, where each element represents one row:

        **startIndex** = 0, indicating that the valid data segment starts from row 0;

        **endIndex** = 2, indicating that the valid data segment ends at row 2;

        **stride** = 1, indicating that the interval between two adjacent slice data segments is one row;

        **burstLen** = 1, which must be set to 1 when dimValue \> 1;

        **shapeValue** = 3, indicating there are 3 rows in total.

- dstSliceInfo is {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}}
    - {0, 47, 0, 3, 48} is for a single row, that is, configured from a one-dimensional perspective, where each element represents a number:

        **startIndex** = 0, indicating the valid data segment starts from element 0;

        **endIndex** = 47, indicating the valid data segment ends at element 47;

        **stride** = 0, in number of elements, indicating the number of elements between two adjacent slice data segments; 0 means there is no gap between the two slice data segments;

        **burstLen** = 3, in 32B units, indicating that within this valid data segment, one slice data segment is 3 datablocks in size;

        **shapeValue** = 48, indicating the length of a single row, in number of elements, that is, 8 × 6 = 48 elements.

    - {0, 1, 0, 1, 2} is for multiple rows, that is, configured from a two-dimensional perspective, where each element represents one row:

        **startIndex** = 0, indicating the valid data segment starts from row 0;

        **endIndex** = 1, indicating that the valid data segment ends at row 1;

        **stride** = 0, indicating that there is no interval between two adjacent slice data segments;

        **burstLen**  = 1, which must be set to 1 when dimValue \> 1;

        **shapeValue** = 2, indicating that there are 2 rows in total.

## Data Type<a name="zh-cn_topic_0000002565968945_section4219135304818"></a>

The source operand and destination operand support the same data types. The Global Memory -> UB and UB -> Global Memory data paths support the same data types, as follows:

<!-- npu="950" id9 -->

- Ascend 950PR&950DT products: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.

<!-- end id9 -->

<!-- npu="A3" id10 -->

- Atlas A3 products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

<!-- end id10 -->

<!-- npu="910b" id11 -->

- Atlas A2 products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

<!-- end id11 -->

<!-- npu="310p" id12 -->

- Atlas inference products AI Core: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.

<!-- end id12 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002565968945_section2045914466492"></a>

- This API is a software simulation API designed for ease of use.
- Addresses in Global Memory must be 1-byte aligned, and addresses in UB must be 32-byte aligned.
- burstLen takes effect only when dimValue = 1. When the dimension exceeds 1, it must be set to 1 and cannot be configured to any other value.
- The horizontal burstLen size in slice data transfer must be calculated by you: the number of horizontal slice elements \* sizeof\(T\)/32 bytes. The size of the number of horizontal slice elements \* sizeof\(T\) must be a multiple of 32 bytes.
- The values of the startIndex, endIndex, stride, and burstLen member variables of the SliceInfo structure must satisfy:

$$
endIndex - startIndex + 1 = (N - 1) \times stride + N \times burstLen \times \frac{32}{sizeof(T)}, \quad N \in positive integer
$$
- The values of the startIndex, endIndex, and shapeValue member variables of the SliceInfo structure must satisfy:

$$
startIndex < endIndex \le shapeValue
$$
- The size of the SliceInfo structure array in slice data transfer must be consistent with dimValue and must not exceed 8.
- In slice data transfer, the size of the srcSliceInfo structure array must be consistent with the size of the dstSliceInfo structure array, and the burstLen in the two structures must be equal (srcSliceInfo\[i\].burstLen = dstSliceInfo\[i\].burstLen).
- Slice data transfer has certain requirements on parameters. You are advised to refer to the examples, verify the simulation results on the CPU, and then execute the transfer on the NPU side.

## Examples<a name="zh-cn_topic_0000002565968945_section088124295117"></a>

```cpp
// Configure the slice transfer parameters.
AscendC::SliceInfo srcSliceInfoIn[] = {{16, 70, 7, 3, 87}, {0, 2, 1, 1, 3}};
AscendC::SliceInfo dstSliceInfoIn[] = {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}};
uint32_t dimValueIn = 2;
dimValue = dimValueIn;
for (uint32_t i = 0; i < dimValueIn; i++) {
    srcSliceInfo[i].startIndex = srcSliceInfoIn[i].startIndex;
    srcSliceInfo[i].endIndex = srcSliceInfoIn[i].endIndex;
    srcSliceInfo[i].stride = srcSliceInfoIn[i].stride;
    srcSliceInfo[i].burstLen = srcSliceInfoIn[i].burstLen;
    srcSliceInfo[i].shapeValue = srcSliceInfoIn[i].shapeValue;
    dstSliceInfo[i].startIndex = dstSliceInfoIn[i].startIndex;
    dstSliceInfo[i].endIndex = dstSliceInfoIn[i].endIndex;
    dstSliceInfo[i].stride = dstSliceInfoIn[i].stride;
    dstSliceInfo[i].burstLen = dstSliceInfoIn[i].burstLen;
    dstSliceInfo[i].shapeValue = dstSliceInfoIn[i].shapeValue;
}

// Perform slice transfers on two data paths in sequence.
// Global Memory -> UB: MTE2 pipeline transfer.
AscendC::DataCopy(srcLocal, srcGlobal, dstSliceInfo, srcSliceInfo, dimValue);
// Wait for the MTE2 transfer to complete so that MTE3 can start.
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
// UB -> Global Memory: MTE3 pipeline transfer.
AscendC::DataCopy(dstGlobal, srcLocal, dstSliceInfo, dstSliceInfo, dimValue);
```

For the complete sample, see [Slice Data Transfer Sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_slice). For the srcSliceInfo and dstSliceInfo parameter parsing and result examples, see [Figure 1](#fig1196816115710).
