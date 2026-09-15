# GM-to-UB Data Transfer with ND2NZ Conversion (DataCopy)<a name="ZH-CN_TOPIC_0000002349187356"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-09-09T13:23:38.290Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id14 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id14 -->
<!-- npu="A3" id15 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id15 -->
<!-- npu="910b" id16 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id16 -->
<!-- npu="310b" id17 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id17 -->
<!-- npu="310p" id18 -->
- Atlas inference products AI Core: Supported
<!-- end id18 -->
<!-- npu="310p" id19 -->
- Atlas inference products Vector Core: Not supported
<!-- end id19 -->
<!-- npu="910" id20 -->
- Atlas training products: Not supported
<!-- end id20 -->

## Description<a name="section12840195813362"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`

This API is a software simulation API designed from the perspective of ease of use. It supports ND-to-NZ format conversion during data transfer from Global Memory to Unified Buffer.

The specific supported data paths are as follows (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Global Memory -> Unified Buffer
    - GM -> VECIN

## Prototype<a name="section1954364615315"></a>

- The enableSmallC0 mode is not supported

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```

<!-- npu="950" id1 -->
- The enableSmallC0 mode is supported (only Ascend 950PR/Ascend 950DT support it)

    ```cpp
    // This function prototype is supported only on Ascend 950PR/Ascend 950DT
    template <typename T, bool enableSmallC0 = false>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
    ```
<!-- end id1 -->

## Parameters<a name="section1251613311396"></a>

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| T | Data type of the source operand or destination operand. For supported data types, please refer to [Data Type](#section4219135304818). |
| enableSmallC0 | SmallC0 mode switch: when dValue is less than or equal to 4, C0_SIZE is padded to 4 * sizeof(T) bytes.<br>Disabled by default. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| :--- | :---: | :--- |
| dst | Output | Destination operand, whose type is [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md). |
| src | Input | Source operand, whose type is [GlobalTensor](../../data_structures/GlobalTensor/globaltensor_introduction.md). |
| intriParams | Input | Transfer parameters, whose type is Nd2NzParam. For details about the parameters, please refer to [Table 3](#table_nd2nz_3).<br>For the specific definition, please refer to \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h, where \$\{INSTALL\_DIR\} should be replaced with the file storage path after CANN software installation. |

**Table 3** Nd2NzParams structure parameter definition<a name="table_nd2nz_3"></a>

| Parameter | Description |
| :--- | :--- |
| ndNum | Number of ND matrices to transfer. Value range: ndNum∈[0, 4095]. |
| nValue | Number of rows in the ND matrix. Value range: nValue∈[0, 16384]. |
| dValue | Number of columns in the ND matrix. Value range: dValue∈[0, 65535]. |
| srcNdMatrixStride | Offset between the start addresses of adjacent ND matrices in the source operand. Value range: srcNdMatrixStride∈[0, 65535], in elements. |
| srcDValue | Offset between the start addresses of adjacent rows in the same ND matrix of the source operand. Value range: srcDValue∈[1, 65535], in elements. |
| dstNzC0Stride | After ND-to-NZ conversion, one row in the source operand is converted into multiple rows in the destination operand. dstNzC0Stride indicates the offset between the start addresses of adjacent rows in the destination NZ matrix that originate from the same row of the source operand. Value range: dstNzC0Stride∈[1, 16384], in C0_SIZE (32B). |
| dstNzNStride | Offset between the start addresses of adjacent rows of the Z-shaped matrix in the destination NZ matrix. Value range: dstNzNStride∈[1, 16384], in C0_SIZE (32B). |
| dstNzMatrixStride | Offset between the start addresses of adjacent NZ matrices in the destination NZ matrix. Value range: dstNzMatrixStride∈[1, 65535], in elements. |

The ND2NZ conversion diagram is as follows. The sample parameter values and their explanations are as follows:

- ndNum = 2, indicating that the number of ND matrices to transfer is 2 \(ND matrix 1 is A1\~A2 + B1\~B2, and ND matrix 2 is C1\~C2 + D1\~D2\).
- nValue = 2, the number of rows in the ND matrix, that is, the height of the matrix is 2.
- dValue = 24, the number of columns in the ND matrix, that is, the width of the matrix is 24 elements. When dValue is not aligned to 32 bytes, the insufficient part in the destination operand is padded with 0. For example, the blank part of the DataBlock where A2 is located in the figure is padded with 0.
- srcNdMatrixStride = 144, which indicates the offset between the start addresses of adjacent ND matrices, that is, the distance of A1\~C1, that is, 9 DataBlocks, 9 \* 16 = 144 elements.
- srcDValue = 48, which indicates the number of elements contained in one row, that is, the distance from A1 to B1, that is, 3 DataBlocks, 3 \* 16 = 48 elements.
- dstNzC0Stride = 11. After the ND format is converted to the NZ format, one row in the source operand is converted into multiple rows in the destination operand. For example, A1 and A2 in src are one row, while A1 and A2 in dst are divided into two rows. The offset between the start addresses of multiple rows is the offset of A1 and A2 in dst, which is an offset of 11 DataBlocks.
- dstNzNStride = 2, which indicates the offset in dst after the x-th row and the (x+1)-th row of an ND matrix in src are converted to the NZ format, that is, the offset between A1 and B1 in dst is 2 DataBlocks.
- dstNzMatrixStride = 96, which indicates the offset between the start of the x-th ND matrix and the start of the (x+1)-th ND matrix in dst, that is, the distance between A1 and C1, which is 6 DataBlocks, 6 \* 16 = 96 elements.

**Figure 1**  ND2NZ conversion diagram (half data type)<a name="fig128961542184620"></a>  
![](../../../../figures/nd2nz_conversion_half.png "ND2NZ conversion diagram (half data type)")

The ND2NZ conversion diagram in enableSmallC0 mode is as follows:

**Figure 2**  ND2NZ conversion diagram in enableSmallC0 mode (half data type)<a name="fig07641913195410"></a>  
![](../../../../figures/enable_smallc0_nd2nz_conversion_half.png "ND2NZ conversion diagram in enableSmallC0 enable mode (half data type)")

## Data Type<a name="section4219135304818"></a>

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT, supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32.
<!-- end id2 -->

<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products, supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products, supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id4 -->

<!-- npu="310p" id5 -->
- Atlas inference products AI Core, supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id5 -->

## Return Value<a name="section446456163012"></a>

None

## Constraints<a name="section1140315215118"></a>

- This API is a software simulation API, designed from the perspective of ease of use.
- The values of the Nd2NzParams structure parameters must be within their value ranges:

    **Table 4**  Value ranges of Nd2NzParams structure parameters

    | Parameter | Value range |
    | --- | --- |
    | ndNum | [0, 4095] |
    | nValue | [0, 16384] |
    | dValue | [0, 65535] |
    | srcNdMatrixStride | [0, 65535] |
    | srcDValue | [1, 65535] |
    | dstNzC0Stride | [1, 16384] |
    | dstNzNStride | [1, 16384] |
    | dstNzMatrixStride | [1, 65535] |

<!-- npu="310p" id6 -->
- For the AI Core of Atlas inference products, reserve 8 KB of Unified Buffer space as the temporary data storage area for this API.
<!-- end id6 -->

## Example<a name="section10309141400"></a>

For details about the intriParams parameter, see [Figure 1](#fig128961542184620).

```cpp
// dstLocal is a LocalTensor of the half type on the Unified Buffer, and srcGlobal is a GlobalTensor of the half type.
AscendC::Nd2NzParams intriParams{1, 32, 32, 0, 32, 32, 1, 0};
// Global Memory -> Local Memory
AscendC::DataCopy(dstLocal, srcGlobal, intriParams);
```

Result example:

```text
Input data (srcGlobal): [1 2 3 ... 1024]
Output data (dstLocal):[1 2 ... 15 16 33 34 ... 47 48 65 66 ... 79 80 97 98 ... 111 112 ... 1009 1010... 1023 1024]
```
