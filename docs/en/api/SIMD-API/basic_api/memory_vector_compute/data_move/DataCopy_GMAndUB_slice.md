# GM and UB Slice Data Transfer \(DataCopy\)<a name="ZH-CN_TOPIC_0000002568770166"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T13:18:46.806Z -->

## Applicable Products<a name="zh-cn_topic_0000002565968945_section796754519912"></a>

<!-- npu="950" id13 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id13 -->
<!-- npu="A3" id14 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id14 -->
<!-- npu="910b" id15 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id15 -->
<!-- npu="310b" id16 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id16 -->
<!-- npu="310p" id17 -->
- Atlas inference products AI Core: Supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products Vector Core: Not supported
<!-- end id18 -->
<!-- npu="910" id19 -->
- Atlas training products: Not supported
<!-- end id19 -->

## Description<a name="zh-cn_topic_0000002565968945_section106841136114319"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

This API is a software simulation API designed from the perspective of ease of use. It supports slice data transfer and extracts a subset of a multi-dimensional Tensor for transfer.

The supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN
- Unified Buffer -> Global Memory
    - VECOUT -> GM
    <!-- npu="310p" id1 -->
    - CO2 -> GM (supported only by the AI Core of Atlas Inference Series products)
    <!-- end id1 -->

## Prototype<a name="zh-cn_topic_0000002565968945_section82039854412"></a>

- Global Memory -> Unified Buffer

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

- Unified Buffer -> Global Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const GlobalTensor<T> &dst, const LocalTensor<T> &src, const SliceInfo dstSliceInfo[], const SliceInfo srcSliceInfo[], const uint32_t dimValue = 1)
    ```

> [!NOTE]
> For the data types supported by each prototype, see [Data Type](#zh-cn_topic_0000002565968945_section4219135304818).

## Parameters<a name="zh-cn_topic_0000002565968945_section16128134420472"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002565968945_section4219135304818). |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand.<br>&bull; When the type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md), the storage location is Unified Buffer, and the destination address must be 32-byte aligned.<br>&bull; When the type is [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md), the storage location is Global Memory, and the destination address must be 1-byte aligned. |
| src | Input | Source operand.<br>&bull; When the type is GlobalTensor, the storage location is Global Memory, and the source address must be 1-byte aligned.<br>&bull; When the type is LocalTensor, the storage location is Unified Buffer, and the source address must be 32-byte aligned. |
| dstSliceInfo | Input | Slice information of the destination operand, of the SliceInfo type. This parameter configures the start and end element counts, interval, length, and other information of the slice.<br>For details about the SliceInfo parameters, see [Table 3](#table_slice_3). |
| srcSliceInfo | Input | Slice information of the source operand, of the SliceInfo type. This parameter configures the start and end element counts, interval, length, and other information of the slice.<br>For the specific definition, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after CANN software installation.<br>For details about the SliceInfo parameters, see [Table 3](#table_slice_3). |
| dimValue | Input | Dimension information of the operand. The default value is 1. |

**Table 3** SliceInfo structure parameter definition<a name="table_slice_3"></a>

| Parameter | Description |
| :--- | :--- |
| startIndex | Start element position of the slice. The data type is uint16_t, and the unit is the number of elements. |
| endIndex | End element position of the slice. The data type is uint16_t, and the unit is the number of elements. |
| stride | Number of interval elements of the slice. The data type is uint16_t. |
| burstLen | Horizontal slice, the length of each data segment. It takes effect only when dimValue = 1. When the dimension exceeds 1, it must be set to 1 and cannot be set to other values. The data type is uint16_t, and the unit is datablock (32B).<br>For example, the List of srcSliceInfo is {{16, 70, 7, 3, 87},  {0, 2, 1, 1, 3}}, where {16, 70, 7, 3, 87} indicates the slice information of the first dimension. When burstLen is set to 3, it indicates that the size of one slice data segment is 3 datablocks. {0, 2, 1, 1, 3} is the slice information of the second dimension, where burstLen can only be set to 1. |
| shapeValue | Original length of the current dimension. The data type is uint16_t, and the unit is the number of elements. |

As shown in [Figure 1](#fig1196816115710), taking the float data type as an example, the slice transfer API can select four discontinuous horizontal slices from the source operand with a shape of 3\*87 and transfer them to the destination operand. The following is an analysis of the SliceInfo structure parameters with reference to the figure:

**Figure 1** Parameter parsing diagram<a name="fig1196816115710"></a>  
![](../../../../figures/slice_datacopy_parameter_analysis_diagram.png "Parameter parsing diagram")

- dimValue is 2, indicating that the operand has 2 dimensions.
- srcSliceInfo is {{16, 70, 7, 3, 87},  {0, 2, 1, 1, 3}}
    - {16, 70, 7, 3, 87} is configured for a single row, that is, from a one-dimensional perspective, where each element represents a number:

        **startIndex**  = 16, indicating that the valid data segment starts from the 16th number;

        **endIndex**  = 70, indicating that the valid data segment ends at the 70th number;

        **stride**  = 7, in units of number of elements, indicating the interval between two adjacent slice data segments in elements, which is a gap of 7 zeros;

        **burstLen**  = 3, in units of 32B, indicating that within this valid data segment, the size of one slice data segment is 3 datablocks;

        **shapeValue**  = 87, indicating the length of a single row, in units of number of elements, that is, 8 \* 10 + 7 = 87 elements.

    - {0, 2, 1, 1, 3} is configured for multiple rows, that is, from a two-dimensional perspective, where each element represents a row:

        **startIndex**  = 0, indicating that the valid data segment starts from row 0;

        **endIndex**  = 2, indicating that the valid data segment ends at row 2;

        **stride**  = 1, indicating that the interval between two adjacent slice data segments is 1 row;

        **burstLen**  = 1, must be set to 1 when dimValue \> 1;

        **shapeValue**  = 3, indicating that there are 3 rows in total.

- dstSliceInfo is {{0, 47, 0, 3, 48}, {0, 1, 0, 1, 2}}
    - {0, 47, 0, 3, 48} is for a single row, that is, configured from a one-dimensional perspective, where each element represents one number:

        **startIndex**  = 0, indicating that the valid data segment starts from the 0th number;

        **endIndex**  = 47, indicating that the valid data segment ends at the 47th number;

        **stride**  = 0, in the unit of number of elements, indicating the number of elements between two adjacent slice data segments. A value of 0 indicates that there is no gap between the two slice data segments;

        **burstLen**  = 3, in the unit of 32B, indicating that within this valid data segment, the size of one slice data segment is 3 datablocks;

        **shapeValue**  = 48, indicating the length of a single row, in the unit of number of elements, that is, 8 \* 6 = 48 elements.

    - {0, 1, 0, 1, 2} is for multiple rows, that is, configured from a two-dimensional perspective, where each element represents one row:

        **startIndex** = 0, indicates that the valid data segment starts from row 0;

        **endIndex** = 1, indicates that the valid data segment ends at row 1;

        **stride** = 0, indicates that two adjacent slice data segments have no interval;

        **burstLen**  = 1, must be set to 1 when dimValue \> 1;

        **shapeValue** = 2, indicates that there are 2 rows in total.

## Data Type<a name="zh-cn_topic_0000002565968945_section4219135304818"></a>

The data types supported by the source operand and destination operand must be consistent. The data types supported by the Global Memory -> Unified Buffer and Unified Buffer -> Global Memory data paths are the same, as follows:

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas Inference Series products AI Core: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id5 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002565968945_section2045914466492"></a>

- This API is a software simulation API designed for ease of use.
- Addresses in Global Memory must be 1-byte aligned, and addresses in Unified Buffer must be 32-byte aligned.
- burstLen takes effect only when dimValue = 1. When the dimension exceeds 1, it must be set to 1 and cannot be set to any other value.
- For the horizontal burstLen size in slice data transfer, you need to calculate it yourself: the number of horizontal slice elements \* sizeof\(T\)/32 bytes. The size of the number of horizontal slice elements \* sizeof\(T\) must be a multiple of 32 bytes.
- The values of the member variables startIndex, endIndex, stride, and burstLen of the SliceInfo structure must satisfy:

$$
endIndex - startIndex + 1 = (N - 1) \times stride + N \times burstLen \times \frac{32}{sizeof(T)}, \quad N \in positive integer
$$

- The values of the member variables startIndex, endIndex, and shapeValue of the SliceInfo structure must satisfy:

$$
startIndex < endIndex \le shapeValue
$$

- In slice data transfer, the size of the SliceInfo structure array and dimValue must be consistent and must not exceed 8.
- In slice data transfer, the size of the srcSliceInfo struct array and the size of the dstSliceInfo struct array must be consistent, and the burstLen in the two structs must be equal (srcSliceInfo\[i\].burstLen = dstSliceInfo\[i\].burstLen).
- Slice data transfer has certain requirements on parameters. It is recommended that you refer to the call example, verify the simulation result on the CPU, and then execute it on the NPU side.

## Calling Example<a name="zh-cn_topic_0000002565968945_section088124295117"></a>

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

// Perform slice transfer on two data paths in sequence.
// Global Memory -> Unified Buffer: MTE2 pipeline transfer.
AscendC::DataCopy(srcLocal, srcGlobal, dstSliceInfo, srcSliceInfo, dimValue);
// Wait for the MTE2 transfer to complete so that MTE3 can start.
AscendC::SetFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE3>(EVENT_ID0);
// Unified Buffer -> Global Memory: MTE3 pipeline transfer.
AscendC::DataCopy(dstGlobal, srcLocal, dstSliceInfo, dstSliceInfo, dimValue);
```

For the complete sample, see [Slice Data Transfer Sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_gm2ub_slice). For the parameter parsing and result examples of srcSliceInfo and dstSliceInfo, see [Figure 1](#fig1196816115710).
