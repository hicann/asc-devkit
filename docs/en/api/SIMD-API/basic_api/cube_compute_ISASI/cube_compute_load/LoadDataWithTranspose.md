# LoadDataWithTranspose<a id="ZH-CN_TOPIC_0000002538231154"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:11:27.696Z pushedAt=2026-10-26T07:41:44.106Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002543851571_section796754519912"></a>

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
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadDataWithTranspose_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002543851571_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

**LoadDataWithTranspose** transfers 2D-format data required for ordinary matrix computation, and a transpose operation is performed during the transfer. For details, see [fractal transpose](#zh-cn_topic_0000002543851571_section10110103802915). Data is transferred in units of 512-byte data fractals, and the following data paths are supported:

L1 Buffer->L0A Buffer and L1 Buffer->L0B Buffer.

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products, this API does not support the L1 Buffer->L0A Buffer path and supports only the L1 Buffer->L0B Buffer path.
<!-- end id10 -->

For the implementation principle, see the pseudocode: [LoadDataWithTranspose pseudocode](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0/scripts/load_data_with_transpose.py).

## Prototype<a id="zh-cn_topic_0000002543851571_section82039854412"></a>

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams)
```

<!-- npu="950" id11 -->
For Ascend 950PR&950DT products:

```cpp
template <typename T>
__aicore__ inline void LoadDataWithTranspose(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams)
```
<!-- end id11 -->

## Parameters<a id="zh-cn_topic_0000002543851571_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| -------- | ------ |
| T | Data type of the source operand and destination operand. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| dst | Output | Destination operand, the result matrix, of type **LocalTensor**.<br>For fractal constraints, see [Matrix Compute Input Transfer Constraints](cube_compute_input_move_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](cube_compute_input_move_constraint.md).<br>The start address of **LocalTensor** must be 512-byte aligned.<br>The data type must be consistent with that of **src**. |
| src | Input | Source operand, of type **LocalTensor**.<br>For fractal constraints, see [Matrix Compute Input Transfer Constraints](cube_compute_input_move_constraint.md).<br>For start address alignment constraints, see [Alignment Constraints](cube_compute_input_move_constraint.md).<br>The start address of **LocalTensor** must be 32-byte aligned.<br>The data type must be consistent with that of **dst**. |
| loadDataParams | Input | Parameters related to **LoadDataWithTranspose**. The type of this parameter varies by model. See [loadDataParams Parameter Type Description](#loaddataparams-parameter-type-description).<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`, where `${INSTALL_DIR}` is the file storage path after the CANN software is installed. |

### loadDataParams Parameter Type Description

The type of the parameter **loadDataParams** varies by model. Developers can refer to the following:

- When the type is **LoadData2dTransposeParams**, see [Table 3](#zh-cn_topic_0000002543851571_table13526111319538) for the parameter description.
<!-- npu="950" id12 -->
- Specifically for Ascend 950PR&950DT products, the type is **LoadData2dTransposeParamsV2**. See [Table 4](#zh-cn_topic_0000002543851571_table64891930194618) for the parameter description.
<!-- end id12 -->

**Table 3** Parameter description of the **LoadData2dTransposeParams** structure<a id="zh-cn_topic_0000002543851571_table13526111319538"></a>

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| startIndex | Input | Block matrix ID. The transfer start position is the index of the block matrix in the source operand (0 indicates the first block matrix in the source operand). Value range: startIndex∈[0, 65535]. Default value: 0.<br>For example, if the source operand contains 20 fractals of 16\*8\*4 bytes (data type **float**), startIndex=1 indicates that the transfer start position is the second block matrix, that is, the third and fourth fractals are transposed from the source operand to the destination operand (the first and second fractals form the first block matrix, and the third and fourth fractals form the second block matrix).<br>For feature details, see [Setting the Transfer Start Position](#zh-cn_topic_0000002543851571_section520575413118). |
| repeatTimes | Input | Number of iterations.<br>For the uint8_t/int8_t data type, each iteration processes 32\*32\*1 bytes of data.<br>For the half/bfloat16_t data type, each iteration processes 16\*16\*2 bytes of data.<br>For the float/int32_t/uint32_t data type, each iteration processes 16\*16\*4 bytes of data.<br>For the int4b_t data type, each iteration processes 64\*64\*0.5 bytes of data.<br>Value range: repeatTimes∈[0, 255]. Default value: 0.<br>**Note: repeatTimes = 0 indicates that no transfer is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Input | Interval between the start addresses of the previous fractal and the next fractal of the source operand between adjacent iterations. The unit here is actually the size of the concatenated block matrix.<br>For the uint8_t/int8_t data type, the unit is 32\*32\*1 bytes.<br>For the half/bfloat16_t data type, the unit is 16\*16\*2 bytes.<br>For the float/int32_t/uint32_t data type, the unit is 16\*16\*4 bytes.<br>For the int4b_t data type, each iteration processes 16\*64\*0.5 bytes of data.<br>Value range: srcStride∈[0, 65535]. Default value: 0.<br>**Note: srcStride = 0 indicates that the same fractal matrix is fetched repeatedly across consecutive repeated execution cycles.**<br>For feature details, see [Non-contiguous Transfer](#zh-cn_topic_0000002543851571_section1750533101219). |
| dstGap | Input | Interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand, in units of 512 bytes. Value range: dstGap∈[0, 65535]. Default value: 0.<br>**Note: dstGap = 0 indicates that there is no interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand, and the fractals are arranged contiguously.**<br>For feature details, see [Non-contiguous Transfer](#zh-cn_topic_0000002543851571_section1750533101219). |
| dstFracGap | Input | Interval between the end address of the previous fractal and the start address of the next fractal after transposition in the destination operand within each iteration, in units of 512 bytes. This parameter is valid only when the data type is float/int32_t/uint32_t/uint8_t/int8_t/int4b_t. Value range: dstFracGap∈[0, 65535]. Default value: 0.<br>**Note: dstFracGap = 0 indicates that there is no interval between the end address of the previous fractal and the start address of the next fractal in the destination operand within each iteration, and the fractals are arranged contiguously.**<br>For feature details, see [Non-contiguous Transfer](#zh-cn_topic_0000002543851571_section1750533101219). |
| addrMode | Input | Controls the address update method. Default value: false.<br>&nbsp;&nbsp;&bull; true: Decrement. In each iteration, srcStride is subtracted from the previous address.<br>&nbsp;&nbsp;&bull; false: Increment. In each iteration, srcStride is added to the previous address.<br>For feature details, see [Controlling the Address Update Method](#zh-cn_topic_0000002543851571_section26691915141315).<br>**Note: Keep the default value. This parameter does not affect performance.** |

<!-- npu="950" id13 -->
For Ascend 950PR&950DT products, the type is **LoadData2dTransposeParamsV2**. Refer to the following table:

**Table 4** Parameter description of the **LoadData2dTransposeParamsV2** structure<a id="zh-cn_topic_0000002543851571_table64891930194618"></a>

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| startIndex | Input | Block matrix ID. The transfer start position is the index of the fractal in the source operand. Value range: startIndex∈[0, 65535]. Default value: 0. |
| repeatTimes | Input | Number of iterations.<br>For the int4b_t data type, each iteration processes 4 fractals, and each fractal is 16\*64\*0.5 bytes of data.<br>For the uint8_t/int8_t data type, each iteration processes 2 fractals, and each fractal is 16\*32\*1 bytes of data.<br>For the half/bfloat16_t data type, each iteration processes 1 fractal, and each fractal is 16\*16\*2 bytes of data.<br>For the int32_t/uint32_t/float data type, each iteration processes 4 fractals, and each fractal is 16\*8\*4 bytes of data.<br>Value range: repeatTimes∈[1, 255]. |
| srcStride | Input | Interval between the start addresses of the previous fractal and the next fractal of the source operand between adjacent iterations. The unit is a single fractal of 512 bytes.<br>Value range: srcStride∈[0, 65535]. Default value: 0. |
| dstGap | Input | Interval between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand, in units of 512 bytes. Value range: dstGap∈[0, 65535]. Default value: 0. |
| dstFracGap | Input | Interval between the end address of the previous fractal and the start address of the next fractal after transposition in the destination operand within each iteration, in units of 512 bytes. This parameter is valid only when the data type is float/int32_t/uint32_t/uint8_t/int8_t/int4b_t. |
| srcFracGap | Input | Interval between the end address of the previous fractal and the start address of the next fractal in the source operand within each iteration, in units of 512 bytes. This parameter is valid only when the data type is float/int32_t/uint32_t/uint8_t/int8_t/int4b_t. |
| addrMode | Input | Controls the address update method. Default value: false.<br>&nbsp;&nbsp;&bull; true: Decrement. In each iteration, srcStride is subtracted from the previous address.<br>&nbsp;&nbsp;&bull; false: Increment. In each iteration, srcStride is added to the previous address. |

<!-- end id13 -->

## Data Types<a id="zh-cn_topic_0000002543851571_section4219135304818"></a>

<!-- npu="950" id14 -->
For Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id14 -->

<!-- npu="A3" id15 -->
Atlas A3 products:

- For loading into L0A Buffer, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, and float are supported.
- For loading into L0B Buffer, int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, and float are supported.
<!-- end id15 -->

<!-- npu="910b" id16 -->
Atlas A2 products:

- For loading into L0A Buffer, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, and float are supported.
- For loading into L0B Buffer, int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, and float are supported.
<!-- end id16 -->

<!-- npu="310b" id17 -->
For Atlas 200I/500 A2 inference products, the supported data types are: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id17 -->

## Return Value<a id="zh-cn_topic_0000002543851571_section640mcpsimp"></a>

None

## Constraints<a id="zh-cn_topic_0000002543851571_section2045914466492"></a>

- When **repeatTimes** is 0, no transfer is performed, and the API is treated as a NOP (no operation).
- When the destination address is in the L0A Buffer/L0B Buffer, the address must be 512-byte aligned. The source address is in the L1 Buffer and must be 32-byte aligned. The b4 data type is supported only when the destination operand is the L0B Buffer. The instruction occupies the PIPE_MTE1 pipeline.
- When **srcStride** is 0, the same data fractal in the source operand is read between consecutive repeats.
- In the fp32 scenario, two consecutive 16\*8 fractals of the source operand are merged into one 16\*16 block matrix, and transposition is then performed based on that block matrix. Therefore, the two consecutive fractals must be merged into a block matrix, and the A matrix on the L1 Buffer must satisfy the ZZ or ZN layout.
- The developer must ensure that the transposed fractals of the destination operand do not overlap.
<!-- npu="950" id19 -->
- For the Ascend 950PR&950DT products, it is recommended to use **LoadData2dTransposeParamsV2** as the parameter, which provides a finer transfer granularity.
<!-- end id19 -->
## Key Feature Description<a id="zh-cn_topic_0000002543851571_section1891111310132"></a>

### Function and Parameter Description

The following examples illustrate the API functions and key parameters: in the figures below, an N shape or a Z shape represents a fractal.

- For the uint8\_t/int8\_t data type, each iteration processes 32*32*1 bytes of data and can handle 2 fractals (512 bytes per fractal). In each iteration, 2 consecutive 16*32 fractals in the source operand are merged into 1 32*32 block matrix, transposed based on the block matrix, and then split into 2 16*32 fractals after transposition. Different layouts are possible depending on parameters such as the destination operand fractal gap.

    As shown in the following figure:

    - A total of 3072 bytes of data need to be processed. Each iteration processes 32\*32\*1 bytes of data, so 3 iterations are required to complete, with repeatTimes = 3;
    - srcStride = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 32\*32\*1 bytes). The unit here is actually the size of the concatenated block matrix;
    - dstGap = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512 bytes);
    - dstFracGap = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512 bytes).

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_1.png)

    As shown in the following figure:

    - The explanations of **repeatTimes** and **srcStride** are consistent with the example in the preceding figure.
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand.
    - **dstFracGap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 2 (unit: 512 bytes).

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_2.png)

- For the half/bfloat16\_t data type, each iteration processes 16\*16\*2 byte data and can process one fractal (one fractal is 512 bytes). In each iteration, one 16\*16 fractal in the source operand is transposed.

    - A total of 1536 bytes of data need to be processed. Each iteration processes 16\*16\*2 bytes of data, so three iterations are required, with **repeatTimes** = 3.
    - **srcStride** = 1, indicating that between adjacent iterations, the gap between the start addresses of the previous block matrix and the next block matrix in the source operand is 1 (unit: 16\*16\*2 bytes).
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand.
    - In this scenario, because the fractal is the block matrix itself and each iteration processes one fractal, there is no gap between fractals within an iteration, so this parameter setting is invalid.

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_3.png)

- For the float/int32\_t/uint32\_t data type, each iteration processes 16\*16\*4 bytes of data and can handle 2 fractals (one fractal is 512 bytes). In each iteration, 2 consecutive 16\*8 fractals of the source operand are merged into 1 16\*16 block matrix. Transposition is performed based on the block matrix, and after transposition it is split into 2 16\*8 fractals, which can have different layouts depending on parameters such as the fractal gap of the destination operand.

    The following figure shows an example:

    - A total of 3072 bytes of data need to be processed. Each iteration processes 16\*16\*4 bytes of data, so 3 iterations are required to complete, with repeatTimes = 3;
    - srcStride = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix of the source operand is 1 (unit: 16\*16\*4 bytes). The unit here is actually the size of the block matrix after concatenation;
    - dstGap = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration of the destination operand is 1 (unit: 512 bytes);
    - dstFracGap = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512 bytes).

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_4.png)

    The following figure shows an example:

    - The explanations of repeatTimes and srcStride are the same as in the figure above;
    - dstGap = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand;
    - **dstFracGap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512 bytes).

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_5.png)

- For the int4b\_t data type, each iteration processes 64\*64\*0.5 byte data and can process 4 fractals (one fractal is 512 bytes). In each iteration, 4 consecutive 16\*64 fractals in the source operand are merged into 1 64\*64 block matrix. Transposition is performed based on the block matrix, and after transposition it is split into 4 16\*64 fractals, which can be arranged differently according to parameters such as the destination operand fractal gap.

    The int4b\_t data type requires two numbers to be combined into one int8\_t or uint8\_t number. The combination rules are as follows:

    ![](../../../../figures/zh-cn_image_0000001898040113.png)

    As shown in the following example:

    - A total of 6144 bytes of data needs to be processed. Each iteration processes 64*64*0.5 bytes of data, so 3 iterations are required to complete, with **repeatTimes** = 3;
    - **srcStride** = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix of the source operand is 1 (unit: 64\*64\*0.5 bytes). The unit here is actually the size of the concatenated block matrix;
    - **dstGap** = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration of the destination operand is 1 (unit: 512 bytes);
    - **dstFracGap** = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512 bytes).

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_6.png)

    As shown in the following example:

    - The explanation of **repeatTimes** and **srcStride** is the same as in the preceding example;
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand;
    - **dstFracGap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 2 (unit: 512 bytes).

    ![](../../../../figures/LoadDataWithTranspose_nd2nz_7.png)

### Fractal Transpose<a id="zh-cn_topic_0000002543851571_section10110103802915"></a>

For matrix transpose operations on int4b_t/int8_t/half/float, the number of 512-byte fractals required differs:

- For the int4b_t data type, four consecutive 16x64 fractals are concatenated into a 64x64 block matrix, which is then transposed and split into four 16x64 fractals.
- For the int8_t data type, two consecutive 16x32 fractals are concatenated into a 32x32 block matrix, which is then transposed and split into two 16x32 fractals.
- For the half data type, only one 16x16 fractal is read and directly transposed.
- For the float data type, two consecutive 16x8 fractals are concatenated into a 16x16 block matrix, which is then transposed and split into two 16x8 fractals.

The specific scenarios are as follows:

- For the int4b_t data type, each iteration processes 64*64*0.5 bytes of data and can process four fractals (one fractal is 512 bytes). In each iteration, four consecutive 16*64 fractals in the source operand are merged into one 64*64 block matrix, which is transposed and then split into four 16*64 fractals. Different layouts are possible depending on parameters such as the fractal gap of the destination operand.

    For the int4b_t data type, two values need to be combined into one int8_t or uint8_t value. The combination rules are as follows:

    ![](../../../../figures/loaddatawithtrans_b4_format.png)

    When concatenating two int4b_t values 6 and 7 into a uint8_t, first take their 4-bit binary representations: 6 is 0110 and 7 is 0111. Placing 7 in the high bits and 6 in the low bits yields 01110110, whose corresponding decimal value is 118.

    As shown in the following example:

    - A total of 12288 bytes of data need to be processed, and each iteration processes 64\*64\*0.5 bytes of data. Three iterations are required to complete, so **repeatTimes** = 3;
    - **srcStride** = 1, which indicates that between adjacent iterations, the gap between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 64\*64\*0.5 bytes);
    - **dstGap** = 7, which indicates that between adjacent iterations, the gap from the end address of the first fractal in the previous iteration to the start address of the first fractal in the next iteration for the destination operand is 7 (unit: 512 bytes);
    - **dstFracGap** = 0, which indicates that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512 bytes);
    - The outer for loop iterates 2 times, the address offset of the source operand is 3 (unit: 64\*64\*0.5 bytes), and the address offset of the destination operand is 1 (unit: 64\*64\*0.5 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b4_demo1.png)

    As shown in the following example:

    - The explanation of **repeatTimes** and **srcStride** is the same as in the example above;
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration for the destination operand;
    - **dstFracGap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512 bytes);
    - The outer for loop iterates 2 times, the address offset of the source operand is 3 (unit: 64\*64\*0.5 bytes), and the address offset of the destination operand is 3 (unit: 64\*64\*0.5 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b4_demo2.png)

- For the int8_t data type, each iteration processes 32\*32\*1 byte data and can process 2 fractals (one fractal is 512 bytes). In each iteration, 2 consecutive 16\*32 fractals in the source operand are merged into 1 32\*32 block matrix, transposed based on the block matrix, and then split into 2 16\*32 fractals after transposition. Different layouts can be achieved based on parameters such as the destination operand fractal gap.

    The following figure shows an example:

    - A total of 6144 bytes of data need to be processed. Each iteration processes 32\*32\*1 bytes of data, so 3 iterations are required to complete, with **repeatTimes** = 3;
    - **srcStride** = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix of the source operand is 1 (unit: 32\*32\*1 bytes);
    - **dstGap** = 3, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration of the destination operand is 3 (unit: 512 bytes);
    - **dstFracGap** = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512 bytes).
    - The outer for loop iterates twice, the address offset of the source operand is 3 (unit: 32\*32\*1 bytes), and the address offset of the destination operand is 1 (unit: 32\*32\*1 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b8_demo1.png)

    The following figure shows an example:

    - The explanations of **repeatTimes** and **srcStride** are the same as those in the preceding figure.
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration for the destination operand.
    - **dstFracGap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512 bytes).
    - The outer for loop iterates twice, the address offset of the source operand is 3 (unit: 32\*32\*1 bytes), and the address offset of the destination operand is 3 (unit: 32\*32\*1 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b8_demo2.png)

- For the half data type, each iteration processes 16*16*2 bytes of data and can process 1 fractal (one fractal is 512 bytes). In each iteration, 1 16\*16 fractal in the source operand is transposed.

    - A total of 3072 bytes of data need to be processed. Each iteration processes 16\*16\*2 bytes of data, so 3 iterations are required to complete the processing, with **repeatTimes** = 3.
    - **srcStride** = 1, indicating that between adjacent iterations, the gap between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16\*16\*2 bytes);
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand;
    - In this scenario, because the fractal is the block matrix itself, each iteration processes one fractal, and there is no gap between fractals within an iteration, so the **dstFracGap** parameter setting is ineffective.
    - The outer for loop iterates 2 times, the address offset of the source operand is 3 (unit: 16\*16\*2 bytes), and the address offset of the destination operand is 3 (unit: 16\*16\*2 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b16.png)

- For the float data type, each iteration processes 16\*16\*4 bytes of data and can process 2 fractals (one fractal is 512 bytes). In each iteration, 2 consecutive 16\*8 fractals of the source operand are merged into 1 16\*16 block matrix, transposition is performed based on the block matrix, and after transposition it is split into 2 16\*8 fractals, which can have different layouts depending on parameters such as the destination operand fractal gap.

    The following figure shows an example:

    - A total of 6144 bytes of data need to be processed. Each iteration processes 16\*16\*4 bytes of data, requiring 3 iterations to complete, with **repeatTimes** = 3;
    - **srcStride** = 1, indicating that between adjacent iterations, the gap between the start addresses of the previous block matrix and the next block matrix of the source operand is 1 (unit: 16\*16\*4 bytes);
    - **dstGap** = 3, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration of the destination operand is 3 (unit: 512 bytes);
    - **dstFracGap** = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 0 (unit: 512 bytes).
    - The number of iterations of the outer for loop is 2, the address offset of the source operand is 3 (unit: 16\*16\*4 bytes), and the address offset of the destination operand is 1 (unit: 16\*16\*4 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b32_demo1.png)

    As shown in the following example:

    - The explanations of **repeatTimes** and **srcStride** are the same as in the example above;
    - **dstGap** = 0, indicating that between adjacent iterations, there is no gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration of the destination operand;
    - **dstFracGap** = 2, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal of the destination operand is 2 (unit: 512 bytes);
    - The number of iterations of the outer for loop is 2, the address offset of the source operand is 3 (unit: 16\*16\*4 bytes), and the address offset of the destination operand is 3 (unit: 16\*16\*4 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_b32_demo2.png)

### Setting the Transfer Start Position<a id="zh-cn_topic_0000002543851571_section520575413118"></a>

Transfer two block matrices from the L1 Buffer to the L0A Buffer, and skip the first block matrix by setting **startIndex** to 1.

- For the int8_t data type, each iteration processes 32\*32\*1 byte data and can process 2 fractals (one fractal is 512 bytes). In each iteration, 2 consecutive 16\*32 fractals in the source operand are merged into 1 32\*32 block matrix. Transposition is performed based on the block matrix, and after transposition it is split into 2 32\*16 fractals. Different layouts are possible depending on parameters such as the destination operand fractal gap.

    The following figure shows an example:

    - **startIndex** = 1: Indicates that the transfer start position is the second block matrix, that is, the 3rd and 4th fractals are transposed from the source operand to the destination operand.
    - **repeatTimes** = 2: Indicates that each iteration processes 32\*32\*1 byte data, and 2 iterations complete the transfer.
    - **srcStride** = 1: Indicates that between adjacent iterations, the gap between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 1 (unit: 32\*32\*1 bytes).
    - **dstGap** = 1: Indicates that between adjacent iterations, the gap between the end address of the first fractal of the previous iteration and the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512 bytes).
    - **dstFracGap** = 0: Indicates that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_set_strat_pos.png)

### Non-contiguous Transfer<a id="zh-cn_topic_0000002543851571_section1750533101219"></a>

Transfers 2 block matrices from the L1 Buffer to the L0A Buffer, using **srcStride**, **dstGap**, and **dstFracGap** for skip-read and skip-write.

- For the int8_t data type, each iteration processes 32\*32\*1 byte data and can process 2 fractals (one fractal is 512 bytes). In each iteration, 2 consecutive 16\*32 fractals in the source operand are merged into 1 32\*32 block matrix. Transposition is performed based on the block matrix, and after transposition it is split into 2 32\*16 fractals, which can have different layouts depending on parameters such as the destination operand fractal gap.

    The following figure shows an example:

    - **startIndex** = 0: Indicates that the transfer start position is the first block matrix, that is, the first and second fractals are transposed from the source operand to the destination operand.
    - **repeatTimes** = 2: Indicates that each iteration processes 32\*32\*1 byte data, and 2 iterations complete the transfer.
    - **srcStride** = 2: Indicates that between adjacent iterations, the interval between the start address of the previous block matrix and the start address of the next block matrix in the source operand is 2 (unit: 32\*32\*1 bytes), that is, an interval of 2 block matrices.
    - **dstGap** = 2: Indicates that between adjacent iterations, the interval from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 2 (unit: 512 bytes), that is, an interval of 2 data fractals.
    - **dstFracGap** = 1: Indicates that within each iteration, the interval between the end address of the previous fractal and the start address of the next fractal in the destination operand is 1 (unit: 512 bytes), that is, an interval of 1 data fractal.

    ![](../../../../figures/loaddatawithtrans_l12l0a_noncontinuous_copy.png)

### Control Address Update Method<a id="zh-cn_topic_0000002543851571_section26691915141315"></a>

Transfer 3 block matrices from L1 Buffer to L0A Buffer, and set **addrMode** to 1 to change the update method of the source operand address.

- For the int8_t data type, each iteration processes 32\*32\*1 byte data and can process 2 fractals (one fractal is 512 bytes). In each iteration, 2 consecutive 16\*32 fractals in the source operand are merged into 1 32\*32 block matrix. Transposition is performed based on the block matrix, and after transposition it is split into 2 32\*16 fractals, which can be arranged differently according to parameters such as the destination operand fractal gap.

    The following figure shows an example:

    - **repeatTimes** = 3, indicating that each iteration processes 32\*32\*1 byte data and 3 iterations complete the transfer;
    - **startIndex** = 2: indicates that the transfer start position is the 3rd block matrix, that is, the 5th and 6th fractals are transposed from the source operand to the destination operand;
    - **srcStride** = 1, indicating that between adjacent iterations, the gap between the start address of the previous block matrix and the next block matrix in the source operand is 1 (unit: 32\*32\*1 bytes);
    - **addrMode** = 1, the block matrix index ID of each iteration in the source operand decreases;
    - **dstGap** = 1, indicating that between adjacent iterations, the gap from the end address of the first fractal of the previous iteration to the start address of the first fractal of the next iteration in the destination operand is 1 (unit: 512 bytes);
    - **dstFracGap** = 0, indicating that within each iteration, the gap between the end address of the previous fractal and the start address of the next fractal in the destination operand is 0 (unit: 512 bytes).

    ![](../../../../figures/loaddatawithtrans_l12l0a_ctrl_addr_update.png)

## Example<a id="zh-cn_topic_0000002543851571_section088124295117"></a>

### Example of the b8 Data Type with the A Matrix Requiring Transpose

In the scenario where the data type is b8 and the A matrix requires transpose, the LoadData API does not support transpose, so the LoadDataWithTranspose API must be called to complete the data transfer.

The following figure shows the data layout changes during the transfer:

![](../../../../figures/loaddatawithtrans_l12l0a_b8.png)

The sample code snippet is as follows. Only part of the code in the sample is shown. For the complete sample, see: [load\_data\_l12l0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
// Solve dstoffset based on the width-direction alignment of the A matrix on the L0 Buffer.
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
// Solve srcoffset based on the height-direction alignment of the A matrix on the L1 Buffer.
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;

// Nz -> Zz
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
// The interval between the end address of the previous fractal and the start address of the next fractal for the destination operand transpose within each iteration, in units of 512 bytes.
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}
```

### Example of the int8_t Data Type with A and B Matrices Requiring Transposition

In this example, the input matrix **a** is of the **int8_t** type with a **shape** of [40,70], the input matrix **b** is of the **int8_t** type with a **shape** of [70,50], and the output **c** is of the **int32_t** type. Matrix **a** is transferred from the **L1 Buffer** (A1) to the **L0A Buffer** (A2) and transposed, and matrix **b** is transferred from the **L1 Buffer** (B1) to the **L0B Buffer** (B2) and transposed, followed by **Mmad** computation and **Fixpipe** computation. The sample code snippet is as follows, showing only part of the code in the sample. For the complete sample, see [LoadData_L12L0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
uint16_t m = 40, k = 70, n = 50;
uint32_t fractalShape[2] = {16, 32 / sizeof(int8_t)};
uint32_t fractalNum = 2;
uint32_t fractalSize = fractalShape[0] * fractalShape[1];

// LoadDataWithTranspose for matrix A: Nz -> Zz.
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
    AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}

// LoadDataWithTranspose for matrix B: Nz -> Zn.
uint32_t dstOffsetB = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffsetB = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParamsB;
loadDataParamsB.startIndex = 0;
loadDataParamsB.repeatTimes = CeilDivision(n, fractalShape[1]);
loadDataParamsB.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParamsB.dstGap = 1;
loadDataParamsB.dstFracGap = 0;
for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffsetB], b1Local[i * srcOffsetB], loadDataParamsB);
}
```

### Example of the scenario where matrices A and B need to be transposed for the half data type

In this example, the input matrix a is of the half type with shape [40,70], the input matrix b is of the half type with shape [70,50], and the output c is of the float type. Matrix a is transferred from the L1 Buffer (A1) to the L0A Buffer (A2) and transposed, and matrix b is transferred from the L1 Buffer (B1) to the L0B Buffer (B2) and transposed. The sample code snippet is as follows, showing only part of the code in the sample. For the complete sample, see [LoadData_L12L0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
uint16_t m = 40, k = 70, n = 50;
uint32_t fractalShape[2] = {16, 32 / sizeof(half)};
uint32_t fractalNum = 1;
uint32_t fractalSize = fractalShape[0] * fractalShape[1];

// Transpose matrix A with LoadDataWithTranspose: Nz -> Zz.
uint32_t dstOffset = CeilDivision(k, fractalShape[1]) * fractalSize * fractalNum;
uint32_t srcOffset = CeilDivision(k, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParams.srcStride = 1;
loadDataParams.dstGap = 0;
loadDataParams.dstFracGap = CeilDivision(k, fractalShape[1]) - 1;
for (int i = 0; i < CeilDivision(m, fractalShape[1]); ++i) {
  AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}

// Transpose matrix B with LoadDataWithTranspose: Nz -> Zn.
uint32_t dstOffsetB = CeilDivision(n, fractalShape[0] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffsetB = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParamsB;
loadDataParamsB.startIndex = 0;
loadDataParamsB.repeatTimes = CeilDivision(n, fractalShape[1]);
loadDataParamsB.srcStride = CeilDivision(k, fractalShape[0] * fractalNum);
loadDataParamsB.dstGap = 0;
loadDataParamsB.dstFracGap = 0;
for (int i = 0; i < CeilDivision(k, fractalShape[0] * fractalNum); ++i) {
  AscendC::LoadDataWithTranspose(b2Local[i * dstOffsetB], b1Local[i * srcOffsetB], loadDataParamsB);
}
```

### Example of the scenario where matrices A and B need to be transposed for the float data type

In this example, the input matrix a is of the float type with shape [40,70], the input matrix b is of the float type with shape [70,50], and the output c is of the float type. Matrix a is transferred from the L1 Buffer (A1) to the L0A Buffer (A2) and transposed, and matrix b is transferred from the L1 Buffer (B1) to the L0B Buffer (B2) and transposed. The sample code snippet is as follows, showing only part of the code in the sample. For the complete sample, see [LoadData_L12L0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
uint32_t m = 40, k = 70, n = 50;
uint32_t fractalShape[2] = {16, 32 / sizeof(float)};
uint32_t fractalNum = 2;
uint32_t fractalSize = fractalShape[0] * fractalShape[1];

// LoadDataWithTranspose for matrix A: Zz -> Zz
uint32_t dstOffset = CeilDivision(k, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
uint32_t srcOffset = fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParams;
loadDataParams.startIndex = 0;
loadDataParams.repeatTimes = CeilDivision(k, fractalShape[1] * fractalNum);
loadDataParams.srcStride = CeilDivision(m, fractalShape[1] * fractalNum);
loadDataParams.dstGap = 1;
loadDataParams.dstFracGap = 0;
for (int i = 0; i < CeilDivision(m, fractalShape[1] * fractalNum); ++i) {
  AscendC::LoadDataWithTranspose(a2Local[i * dstOffset], a1Local[i * srcOffset], loadDataParams);
}

// LoadDataWithTranspose for matrix B: Nz -> Zn
uint32_t dstOffsetB = CeilDivision(n, fractalShape[0]) * fractalSize * fractalNum;
uint32_t srcOffsetB = CeilDivision(n, fractalShape[1] * fractalNum) * fractalSize * fractalNum;
AscendC::LoadData2dTransposeParams loadDataParamsB;
loadDataParamsB.startIndex = 0;
loadDataParamsB.repeatTimes = CeilDivision(n, fractalShape[1] * fractalNum);
loadDataParamsB.srcStride = 1;
loadDataParamsB.dstGap = 0;
loadDataParamsB.dstFracGap = CeilDivision(n, fractalShape[0]) - 1;
for (int i = 0; i < CeilDivision(k, fractalShape[0]); ++i) {
    AscendC::LoadDataWithTranspose(b2Local[i * dstOffsetB], b1Local[i * srcOffsetB], loadDataParamsB);
}
```

<!-- npu="950" id20 -->
### Example of Using the LoadData2dTransposeParamsV2 Structure as a Parameter

This example uses the **LoadData2dTransposeParamsV2** structure as a parameter. The input matrix a is of type **int8_t** with a shape of [128,128] and an input data format of **NZ**; the input matrix b is of type **int8_t** with a shape of [128,256] and an input data format of **NZ**; and the output c is of type **float**. Matrix a is transferred from the L1 Buffer (A1) to the L0A Buffer (A2) without transposition, and matrix b is transferred from the L1 Buffer (B1) to the L0B Buffer (B2) with transposition. This example only demonstrates the API invocation process; the remaining computation and transfer are not for reference. For the complete example, see [load_data_l12l0 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/load_data_l12l0).

```cpp
uint32_t m = 256;
uint32_t n = 256;
uint32_t k = 128;
pipe = tpipe;
TQue<TPosition::B1, 1> qidB1_;
TQue<TPosition::B2, 1> qidB2_;
pipe->InitBuffer(qidB1_, 1, n * k * sizeof(int8_t));
pipe->InitBuffer(qidB2_, 1, n * k * sizeof(int8_t));
auto rightMatrix = qidB1_.template DeQue<int8_t>();
LocalTensor<int8_t> b2 = qidB2_.AllocTensor<int8_t>();
uint16_t fracNum = 2;
uint16_t kStep = CeilDiv(kLength, 16);
uint16_t nStep = CeilDiv(nLength, 32);
for (uint16_t i = 0; i < nStep; i ++) {
    LoadData2dTransposeParamsV2 loadDataParams;
    loadDataParams.startIndex = i * kStep;
    loadDataParams.repeatTimes = kStep / 2;
    loadDataParams.srcStride = 2;
    loadDataParams.dstGap = nStep*2 - 1;
    LoadDataWithTranspose(b2[1024*i], rightMatrix, loadDataParams);
}
qidB2_.EnQue(b2);
qidB1_.FreeTensor(rightMatrix);
```
<!-- end id20 -->
