# Select<a name="ZH-CN_TOPIC_0000001550716925"></a>

<!-- md-trans-meta sourceCommit=27a8d829e1498b83498ed8bfc14b3c9e69ae662d translatedAt=2026-09-09T08:21:39.336Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id18 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id18 -->
<!-- npu="A3" id19 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id19 -->
<!-- npu="910b" id20 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id20 -->
<!-- npu="310b" id21 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id21 -->
<!-- npu="310p" id22 -->
- Atlas inference products AI Core: Supported
<!-- end id22 -->
<!-- npu="310p" id23 -->
- Atlas inference products Vector Core: Not supported
<!-- end id23 -->
<!-- npu="910" id24 -->
- Atlas training products: Supported
<!-- end id24 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_cmpsel_intf.h"`

Given two source operands src0 and src1, elements are selected based on the bit values of selMask (the mask used for selection) to obtain the destination operand dst. The selection rule is as follows: when a bit of selMask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.

![](../../../../figures/select_selMask.png)

For the tensor high-dimensional splitting computation API, the selection result can be filtered again based on the mask parameter. Valid bits (bits of mask set to 1) are written to the final dst, while invalid bits (bits of mask set to 0) retain the original dst value.

![](../../../../figures/select_mask.png)

This API supports three modes:

- Mode 0 (VSEL\_CMPMASK\_SPR): Selects elements from two tensors based on selMask. The valid bit count of selMask is limited to 256/sizeof\(T\), and all multi-round iterations use the same valid value of selMask.

    ![](../../../../figures/select_mode0.png)

- Mode 1 (VSEL\_TENSOR\_SCALAR\_MODE): selects elements from one tensor (src0) and one scalar (src1) based on selMask. Each iteration uses 256/sizeof\(T\) bits of selMask, and multi-round iteration continuously consumes different parts of selMask.

    ![](../../../../figures/select_mode1.png)

- Mode 2 (VSEL\_TENSOR\_TENSOR\_MODE): selects elements from two tensors based on selMask. Each iteration uses 256/sizeof\(T\) bits of selMask, and multi-round iteration continuously consumes different parts of selMask.

    ![](../../../../figures/select_mode2.png)

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT supports modes 0, 1, and 2.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products support modes 0, 1, and 2.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products support modes 0, 1, and 2.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 inference products support modes 0, 1, and 2.
<!-- end id4 -->

<!-- npu="310p" id5 -->
Atlas inference products AI Core supports modes 0, 1, and 2.
<!-- end id5 -->

<!-- npu="910" id6 -->
Atlas training products support only mode 0.
<!-- end id6 -->

## Prototype<a name="section620mcpsimp"></a>

- Computation on the first n data elements of the tensor
    - Select mode 1

        ```cpp
        template <typename T, typename U>
        __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode, uint32_t count)
        ```

    - Select mode 0 and Select mode 2

        ```cpp
        template <typename T, typename U>
        __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1, SELMODE selMode, uint32_t count)
        ```

- High-dimensional splitting computation of tensor
    - Select mode 1
        - Bitwise mask mode

            ```cpp
            template <typename T, typename U, bool isSetMask = true>
            __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode, uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, typename U, bool isSetMask = true>
            __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - Without passing the mask parameter (used together with [SetVectorMask](../mask_operations/SetVectorMask.md) and [SetCmpMask \(ISASI\)](SetCmpMask_ISASI.md))

            ```cpp
            template <typename T, typename U>
            __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

    - Select mode 0 and Select mode 2
        - Bitwise mask mode

            ```cpp
            template <typename T, typename U, bool isSetMask = true>
            __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1, SELMODE selMode, uint64_t mask[],  uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - Continuous mask mode

            ```cpp
            template <typename T, typename U, bool isSetMask = true>
            __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1, SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

        - Without passing the mask parameter (used together with [SetVectorMask](../mask_operations/SetVectorMask.md) and [SetCmpMask \(ISASI\)](SetCmpMask_ISASI.md))

            ```cpp
            template <typename T, SELMODE selMode>
            __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
            ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand and destination operand. |
| U | Data type of selMask. |
| isSetMask | Reserved parameter. Keep the default value. To use the function of setting the mask outside the API, call the API that does not pass the mask parameter. |
| selMode | Instruction mode, of the SELMODE type. For details, see the selMode parameter in [Table 2 Parameters](#table-2-select-parameters). |

**Table 2**  Parameters<a id="table-2-select-parameters"></a>

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| selMask | Input | Selection mask.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned.<br>Each bit indicates the selection of one element. When a bit of selMask is 1, the element is selected from src0; when the bit is 0, the element is selected from src1.<br>For details about how selMask is used in each mode, see the description of the selMode parameter.<br> |
| src0 | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| src1 | Input | Source operand.<br>&bull; When selMode is mode 0 or mode 2:<br>The type is LocalTensor, and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned.<br>&bull; When selMode is mode 1, the type is T, a scalar data type. |
| selMode | Input | Instruction mode, of the SELMODE type. The values are as follows:<br>enum class SELMODE : uint8_t {<br>    VSEL_CMPMASK_SPR = 0, <br>    VSEL_TENSOR_SCALAR_MODE,<br>    VSEL_TENSOR_TENSOR_MODE,<br>};<br>For details about the three modes, see [Description](#description).|
| mask/mask[] | Input | mask controls the elements that participate in the computation in each iteration.<br>For details, see [Mask](../SIMD_compute/mask.md).|
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 bytes of consecutive data for computation each time. To complete the processing of the input data, multiple iterations (repeat) are required to read and compute all the data. repeatTime indicates the number of iterations.<br>For details, see [High-Dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |
| count | Input | Number of elements involved in the computation. |

## Data Type

- The data types supported by T are as follows:
    <!-- npu="950" id7 -->
    - Ascend 950PR/Ascend 950DT: int8_t/uint8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float/complex32/int64_t/uint64_t/complex64.
    <!-- end id7 -->

    <!-- npu="A3" id8 -->
    - Atlas A3 training products/Atlas A3 inference products: half/float.
    <!-- end id8 -->

    <!-- npu="910b" id9 -->
    - Atlas A2 training products/Atlas A2 inference products: half/float.
    <!-- end id9 -->

    <!-- npu="310b" id10 -->
    - Atlas 200I/500 A2 inference products: half/float.
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    - Atlas inference products AI Core: half/float.
    <!-- end id11 -->

    <!-- npu="910" id12 -->
    - Atlas training products: half/float.
    <!-- end id12 -->



- The data types supported by U are as follows: uint8_t, uint16_t, uint32_t, uint64_t.

## Return Value<a name="section198548421851"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).

- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

<!-- npu="950" id13 -->
- For Ascend 950PR/Ascend 950DT, data types int8\_t/uint8\_t/complex32/int64\_t/uint64\_t/complex64 support only the API for computing the first n data of a tensor.
<!-- end id13 -->

<!-- npu="910b, A3, 310p" id17 -->
- For the following models, when mode 1 and mode 2 are used, 8 KB of Unified Buffer space must be reserved as the temporary data storage area of the API:

    <!-- npu="A3" id14 -->
    - Atlas A3 training products/Atlas A3 inference products
    <!-- end id14 -->
    <!-- npu="910b" id15 -->
    - Atlas A2 training products/Atlas A2 inference products
    <!-- end id15 -->
    <!-- npu="310p" id16 -->
    - Atlas inference products AI Core
    <!-- end id16 -->
<!-- end id17 -->
<!-- npu="310b" id33 -->
- Atlas 200I/500 A2 inference products do not support the following function prototypes:
  - The function prototype that does not pass the mask parameter in Select mode 1 of tensor high-dimensional slicing computation [SetVectorMask](../mask_operations/SetVectorMask.md) and [SetCmpMask_ISASI](SetCmpMask_ISASI.md).
  - The function prototype that does not take the mask parameter in tensor high-dimensional splitting computation Select mode 0 and mode 2 must be used together with [SetVectorMask](../mask_operations/SetVectorMask.md) and [SetCmpMask_ISASI](SetCmpMask_ISASI.md).
<!-- end id33 -->


## Example<a name="section642mcpsimp"></a>

This example shows only part of the key code. If you need to run the sample code, see the [Select example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/select) for the complete example.

- Select-tensor high-dimensional splitting computation (mode 2)

    ```cpp
    uint64_t mask = 256/sizeof(float);
    int repeat = 4;
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats
    AscendC::Select(dstLocal, maskLocal, src0Local, src1Local, AscendC::SELMODE::VSEL_TENSOR_TENSOR_MODE, mask, repeat, repeatParams);
    ```

- Computation of the first n data of Select-tensor (mode 1)

    ```cpp
    AscendC::Select(dstLocal, maskLocal, src0Local, static_cast<float>(0), AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE, dataSize);
    ```

- Computation of the first n data of Select-tensor (mode 0, in each round of repeat iteration, only the first 64 bits of maskLocal take effect)

    ```cpp
    // float type
    AscendC::Select(dstLocal, maskLocal, src0Local, src1Local, AscendC::SELMODE::VSEL_CMPMASK_SPR, dataSize);
    ```

- Computation of Select-tensor high-dimensional slicing - mask continuous mode (mode 0, in each round of repeat iteration, only the first 64 bits of maskLocal take effect)

    ```cpp
    uint64_t mask = 256/sizeof(float);
    int repeat = 4;
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats
    AscendC::Select(dstLocal, maskLocal, src0Local, src1Local, AscendC::SELMODE::VSEL_CMPMASK_SPR, mask, repeat, repeatParams);
    ```

- Computation of Select-tensor high-dimensional slicing - mask bitwise mode (in each round of repeat iteration, only the first 64 bits of maskLocal take effect)

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0};
    int repeat = 4;
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // repeat = 4, 64 elements one repeat, 256 elements total
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::Select(dstLocal, maskLocal, src0Local, src1Local, AscendC::SELMODE::VSEL_CMPMASK_SPR, mask, repeat, repeatParams);
    ```

The result is as follows:

- Mode 2 example:

    ```
    Input data (src0Local):
    [-80.4933, 52.2499, -50.6124, -72.3737, -33.7107, -83.4001, 34.3954, 61.3188, 96.5484, 27.1321, -56.8153, 9.80549, 9.11199, -53.1848, -77.2548, -0.0681466, -69.5783, 6.53722, -22.5986, 37.6655, -25.4146, 89.232, 55.4716, 21.6069, 23.0464, 28.5975, -46.3033, 50.0312, -42.6339, 41.8752, -87.0426, 37.9717, 10.4336, 10.7653, -30.6943, -65.4774, 8.38653, -89.6462, 65.1115, 42.2134, -91.1666, -84.6927, -97.9312, 98.861, 19.8888, -64.0522, -27.1243, 72.7673, -9.9489, 94.274, -72.868, 43.1349, 84.1897, 87.0729, 87.2606, 34.5548, 87.7985, 4.84555, -10.2156, -93.7445, 71.8209, -63.4942, 45.6619, 93.4737, 79.6631, 66.8743, 18.1016, -27.7082, -67.0339, -11.9576, 52.0373, -11.5452, -73.8953, -63.5915, 99.4875, -46.2296, 75.453, -67.2079, 89.8868, -19.9666, 30.5359, 42.1465, -19.8105, 82.3653, -89.2164, -0.959167, -50.0723, -30.3058, 48.1772, -27.7686, 26.1484, 94.8462, -15.5663, -87.346, -84.2826, -58.8268, -42.4957, -23.7061, 67.0375, -83.5848, 64.167, 63.3315, -33.3809, 35.1264, 96.6872, 91.8399, 33.9888, 78.5923, -30.4885, 26.3331, -62.3014, -30.3431, 96.9554, 50.3262, 66.612, -71.5939, -97.0042, 71.4549, 42.3891, 71.308, 72.3209, 59.1608, -4.57918, -81.3639, -37.2619, 28.2445, 16.7995, -46.5868, -88.6893, 82.0504, -39.3391, -33.7141, -88.6628, -94.1271, -74.7738, -80.0798, -67.5639, -69.8237, -37.5219, 11.9601, -30.3912, -30.1169, 22.7411, -85.9541, 19.5141, -37.6203, -49.5693, 5.09318, 11.4884, 18.9713, 21.1073, -84.9266, 11.9436, -22.4703, -58.5243, -24.0218, -63.2767, -2.72752, -87.8947, -91.5162, 33.4207, -85.9841, 18.743, 48.9581, 69.3992, 2.42074, -75.0209, -53.2579, -45.4509, 66.6121, 51.6616, -48.073, 74.2754, -51.1623, -89.9345, 4.15238, -4.47531, 79.6587, -31.1646, 69.4103, -83.5936, -78.7341, 56.8626, 72.8834, -27.0248, -80.3328, 71.7272, -77.979, -76.6814, -14.9994, -94.5054, -75.2802, -96.4931, -17.6781, -5.50804, -83.4637, -56.8385, 51.5406, -60.527, -11.0762, -33.3166, -54.9609, -36.9426, -70.3942, 28.3439, 5.28754, -61.4775, 96.0657, -69.1967, 70.5489, 32.817, -53.5746, 49.2601, -88.5728, -1.94822, -3.16238, 19.083, -81.5139, 87.8383, 90.714, 4.75546, 31.9277, -4.1301, -0.160932, -31.2602, 56.4225, -72.1826, 91.5082, 68.2155, -81.7476, -14.0418, -79.4093, -30.7375, 38.8967, -16.5589, -69.4351, 48.6597, -8.44998, -74.4274, -20.3394, -59.2265, 19.407, 88.1542, -15.4888, 60.9066, 59.5144, -42.6935, 20.3518, 11.7192, -31.3635, 26.0055, -26.9334, 79.3798, 46.4724, 22.984, 67.5759]
    Input data (src1Local):
    [35.8789, 44.0334, 54.9997, 44.8567, -30.8579, -53.714, -59.8013, 71.1663, 46.3484, 8.56818, -59.4716, 6.07412, -39.0137, 64.5595, 17.0849, 45.2641, -63.2115, -98.7838, -52.3835, -65.9849, 50.4909, 69.9812, -22.3447, -32.3809, -97.8394, -45.4997, 63.5391, -69.3535, 43.3368, 98.8541, -77.2888, 1.02385, 20.4965, -26.7797, 98.2463, -78.9606, -62.4907, -13.5348, -49.5058, -4.06369, 77.2982, -32.8221, 84.6766, -62.3829, 58.8673, -75.8509, -95.3497, -79.7642, 67.1185, 34.4278, 34.5305, -76.1646, 53.1497, -12.3158, -42.9392, 59.2962, -4.12072, 47.1292, -17.0687, -78.0087, -59.4565, -98.9565, -54.0959, 56.5437, -74.3328, 77.2781, 52.7964, -0.932984, 70.8957, -68.1249, 85.895, 25.4119, 71.9202, -73.1287, 63.6916, 21.4303, 66.0614, 66.1438, -22.2332, 84.0665, -7.86752, -2.38648, 1.37756, -98.691, -35.847, -15.2647, -85.2363, -54.3978, -46.6612, 99.3826, -75.7728, -31.2539, 97.9558, 92.4507, 80.2871, -60.8802, -82.0434, -80.625, 19.6418, 51.0559, 35.3667, -56.306, -41.2088, 0.955906, -85.7743, 8.18112, 36.4615, -0.572343, -16.0821, 36.0277, -4.61647, 26.5385, 88.6082, 9.17454, 44.8951, -42.173, 51.5339, 5.93139, 93.7096, -68.8219, 68.2573, -67.325, -88.4579, -56.8873, -75.8117, -40.5576, -98.378, 32.3699, 64.6693, -73.1523, -57.8738, 63.1893, -40.4731, -1.00914, -80.6115, 44.2928, 76.6212, -29.3298, -58.1212, 83.3083, -20.6412, 26.8912, -82.1719, 26.8713, -56.5484, 35.4743, -8.59957, -12.4709, 27.8249, 76.6877, -27.5806, 63.2649, 66.1106, 15.8328, 9.19251, -79.6418, -9.31359, 63.7053, -8.37093, 55.6421, -99.0591, -64.1341, 91.4046, 26.7268, -92.1002, -34.7002, -6.41819, -18.15, 12.207, 48.6667, -39.4883, -21.0939, -50.3433, 58.2913, 7.64983, -82.6098, -89.6739, -25.9494, 82.4803, 20.8037, 21.483, -29.0788, 31.7695, 50.462, -83.7715, 63.4177, 52.7679, -90.2271, 16.1258, -61.4531, -61.7242, 25.0575, -97.8702, 26.9708, -23.039, -52.7595, -97.0177, -13.1399, -47.6936, -29.7551, 88.9603, -82.1242, -56.6307, 91.7884, 0.0381027, -49.0936, -43.5545, 47.3574, 97.1801, 43.4392, 22.7347, 12.6125, 63.7829, 22.3428, 53.4543, -91.4307, 45.6971, -92.1851, -81.4774, 35.8835, -33.043, -79.7464, 69.0971, -82.6252, -63.0042, -61.0205, -8.00347, -60.0369, 56.2894, -38.1932, 17.976, 5.82004, 4.41524, -52.2192, 93.1915, 21.1114, -29.3558, -18.5685, 20.7356, -4.71108, -0.947533, 73.0143, 62.5668, 96.1632, 41.4265, -89.503, -83.7747, -97.6047, -60.7304, 28.9736, -42.6681, 55.2584, 59.1584, -14.6596, -41.1826, 48.8083]
    Input data (maskLocal):
    [60, 32, 10, 7, 42, 52, 38, 26, 19, 15, 18, 83, 41, 43, 91, 30, 45, 77, 80, 58, 34, 76, 44, 4, 64, 45, 48, 31, 30, 56, 43, 88]
    Output data (dstLocal):
    [35.8789,44.0334,-50.6124,-72.3737,-33.7107,-83.4001,-59.8013,71.1663,46.3484,8.56818,-59.4716,6.07412,-39.0137,-53.1848,17.0849,45.2641,-63.2115,6.53722,-52.3835,37.6655,50.4909,69.9812,-22.3447,-32.3809,23.0464,28.5975,-46.3033,-69.3535,43.3368,98.8541,-77.2888,1.02385,20.4965,10.7653,98.2463,-65.4774,-62.4907,-89.6462,-49.5058,-4.06369,77.2982,-32.8221,-97.9312,-62.3829,19.8888,-64.0522,-95.3497,-79.7642,67.1185,94.274,-72.868,-76.1646,53.1497,87.0729,-42.9392,59.2962,-4.12072,4.84555,-17.0687,-93.7445,71.8209,-98.9565,-54.0959,56.5437,79.6631,66.8743,52.7964,-0.932984,-67.0339,-68.1249,85.895,25.4119,-73.8953,-63.5915,99.4875,-46.2296,66.0614,66.1438,-22.2332,84.0665,-7.86752,42.1465,1.37756,-98.691,-89.2164,-15.2647,-85.2363,-54.3978,48.1772,-27.7686,-75.7728,-31.2539,-15.5663,92.4507,-84.2826,-60.8802,-42.4957,-80.625,19.6418,-83.5848,35.3667,63.3315,-41.2088,0.955906,96.6872,91.8399,36.4615,78.5923,-16.0821,26.3331,-4.61647,26.5385,96.9554,50.3262,44.8951,-71.5939,-97.0042,5.93139,42.3891,-68.8219,68.2573,59.1608,-4.57918,-81.3639,-37.2619,-40.5576,-98.378,32.3699,-88.6893,-73.1523,-39.3391,-33.7141,-40.4731,-94.1271,-80.6115,44.2928,-67.5639,-29.3298,-37.5219,11.9601,-20.6412,26.8912,22.7411,26.8713,-56.5484,35.4743,-8.59957,-12.4709,11.4884,76.6877,21.1073,63.2649,66.1106,-22.4703,9.19251,-24.0218,-63.2767,-2.72752,-8.37093,55.6421,-99.0591,-85.9841,91.4046,26.7268,-92.1002,2.42074,-6.41819,-18.15,12.207,48.6667,51.6616,-48.073,-50.3433,58.2913,-89.9345,-82.6098,-89.6739,-25.9494,-31.1646,69.4103,21.483,-78.7341,31.7695,50.462,-83.7715,63.4177,71.7272,-90.2271,16.1258,-61.4531,-61.7242,25.0575,-97.8702,26.9708,-23.039,-52.7595,-97.0177,-13.1399,-60.527,-29.7551,-33.3166,-82.1242,-36.9426,-70.3942,0.0381027,5.28754,-43.5545,47.3574,97.1801,43.4392,22.7347,12.6125,49.2601,-88.5728,53.4543,-91.4307,19.083,-81.5139,87.8383,90.714,4.75546,-79.7464,69.0971,-82.6252,-63.0042,56.4225,-72.1826,91.5082,68.2155,-38.1932,17.976,5.82004,4.41524,-52.2192,93.1915,-69.4351,48.6597,-8.44998,20.7356,-4.71108,-59.2265,19.407,62.5668,-15.4888,41.4265,59.5144,-83.7747,-97.6047,-60.7304,28.9736,-42.6681,-26.9334,79.3798,-14.6596,22.984,48.8083]
    ```

- Mode 1 example:

    ```
    Input data (src0Local):
    [-80.4933, 52.2499, -50.6124, -72.3737, -33.7107, -83.4001, 34.3954, 61.3188, 96.5484, 27.1321, -56.8153, 9.80549, 9.11199, -53.1848, -77.2548, -0.0681466, -69.5783, 6.53722, -22.5986, 37.6655, -25.4146, 89.232, 55.4716, 21.6069, 23.0464, 28.5975, -46.3033, 50.0312, -42.6339, 41.8752, -87.0426, 37.9717, 10.4336, 10.7653, -30.6943, -65.4774, 8.38653, -89.6462, 65.1115, 42.2134, -91.1666, -84.6927, -97.9312, 98.861, 19.8888, -64.0522, -27.1243, 72.7673, -9.9489, 94.274, -72.868, 43.1349, 84.1897, 87.0729, 87.2606, 34.5548, 87.7985, 4.84555, -10.2156, -93.7445, 71.8209, -63.4942, 45.6619, 93.4737, 79.6631, 66.8743, 18.1016, -27.7082, -67.0339, -11.9576, 52.0373, -11.5452, -73.8953, -63.5915, 99.4875, -46.2296, 75.453, -67.2079, 89.8868, -19.9666, 30.5359, 42.1465, -19.8105, 82.3653, -89.2164, -0.959167, -50.0723, -30.3058, 48.1772, -27.7686, 26.1484, 94.8462, -15.5663, -87.346, -84.2826, -58.8268, -42.4957, -23.7061, 67.0375, -83.5848, 64.167, 63.3315, -33.3809, 35.1264, 96.6872, 91.8399, 33.9888, 78.5923, -30.4885, 26.3331, -62.3014, -30.3431, 96.9554, 50.3262, 66.612, -71.5939, -97.0042, 71.4549, 42.3891, 71.308, 72.3209, 59.1608, -4.57918, -81.3639, -37.2619, 28.2445, 16.7995, -46.5868, -88.6893, 82.0504, -39.3391, -33.7141, -88.6628, -94.1271, -74.7738, -80.0798, -67.5639, -69.8237, -37.5219, 11.9601, -30.3912, -30.1169, 22.7411, -85.9541, 19.5141, -37.6203, -49.5693, 5.09318, 11.4884, 18.9713, 21.1073, -84.9266, 11.9436, -22.4703, -58.5243, -24.0218, -63.2767, -2.72752, -87.8947, -91.5162, 33.4207, -85.9841, 18.743, 48.9581, 69.3992, 2.42074, -75.0209, -53.2579, -45.4509, 66.6121, 51.6616, -48.073, 74.2754, -51.1623, -89.9345, 4.15238, -4.47531, 79.6587, -31.1646, 69.4103, -83.5936, -78.7341, 56.8626, 72.8834, -27.0248, -80.3328, 71.7272, -77.979, -76.6814, -14.9994, -94.5054, -75.2802, -96.4931, -17.6781, -5.50804, -83.4637, -56.8385, 51.5406, -60.527, -11.0762, -33.3166, -54.9609, -36.9426, -70.3942, 28.3439, 5.28754, -61.4775, 96.0657, -69.1967, 70.5489, 32.817, -53.5746, 49.2601, -88.5728, -1.94822, -3.16238, 19.083, -81.5139, 87.8383, 90.714, 4.75546, 31.9277, -4.1301, -0.160932, -31.2602, 56.4225, -72.1826, 91.5082, 68.2155, -81.7476, -14.0418, -79.4093, -30.7375, 38.8967, -16.5589, -69.4351, 48.6597, -8.44998, -74.4274, -20.3394, -59.2265, 19.407, 88.1542, -15.4888, 60.9066, 59.5144, -42.6935, 20.3518, 11.7192, -31.3635, 26.0055, -26.9334, 79.3798, 46.4724, 22.984, 67.5759]
    Input data (maskLocal):
    [60, 32, 10, 7, 42, 52, 38, 26, 19, 15, 18, 83, 41, 43, 91, 30, 45, 77, 80, 58, 34, 76, 44, 4, 64, 45, 48, 31, 30, 56, 43, 88 ]
    Output data (dstLocal):
    [0,0,-50.6124,-72.3737,-33.7107,-83.4001,0,0,0,0,0,0,0,-53.1848,0,0,0,6.53722,0,37.6655,0,0,0,0,23.0464,28.5975,-46.3033,0,0,0,0,0,0,10.7653,0,-65.4774,0,-89.6462,0,0,0,0,-97.9312,0,19.8888,-64.0522,0,0,0,94.274,-72.868,0,0,87.0729,0,0,0,4.84555,0,-93.7445,71.8209,0,0,0,79.6631,66.8743,0,0,-67.0339,0,0,0,-73.8953,-63.5915,99.4875,-46.2296,0,0,0,0,0,42.1465,0,0,-89.2164,0,0,0,48.1772,-27.7686,0,0,-15.5663,0,-84.2826,0,-42.4957,0,0,-83.5848,0,63.3315,0,0,96.6872,91.8399,0,78.5923,0,26.3331,0,0,96.9554,50.3262,0,-71.5939,-97.0042,0,42.3891,0,0,59.1608,-4.57918,-81.3639,-37.2619,0,0,0,-88.6893,0,-39.3391,-33.7141,0,-94.1271,0,0,-67.5639,0,-37.5219,11.9601,0,0,22.7411,0,0,0,0,0,11.4884,0,21.1073,0,0,-22.4703,0,-24.0218,-63.2767,-2.72752,0,0,0,-85.9841,0,0,0,2.42074,0,0,0,0,51.6616,-48.073,0,0,-89.9345,0,0,0,-31.1646,69.4103,0,-78.7341,0,0,0,0,71.7272,0,0,0,0,0,0,0,0,0,0,0,-60.527,0,-33.3166,0,-36.9426,-70.3942,0,5.28754,0,0,0,0,0,0,49.2601,-88.5728,0,0,19.083,-81.5139,87.8383,90.714,4.75546,0,0,0,0,56.4225,-72.1826,91.5082,68.2155,0,0,0,0,0,0,-69.4351,48.6597,-8.44998,0,0,-59.2265,19.407,0,-15.4888,0,59.5144,0,0,0,0,0,-26.9334,79.3798,0,22.984,0]
    ```

- Mode 0 example:

    ```
    Input data (src0Local):
    [-80.4933, 52.2499, -50.6124, -72.3737, -33.7107, -83.4001, 34.3954, 61.3188, 96.5484, 27.1321, -56.8153, 9.80549, 9.11199, -53.1848, -77.2548, -0.0681466, -69.5783, 6.53722, -22.5986, 37.6655, -25.4146, 89.232, 55.4716, 21.6069, 23.0464, 28.5975, -46.3033, 50.0312, -42.6339, 41.8752, -87.0426, 37.9717, 10.4336, 10.7653, -30.6943, -65.4774, 8.38653, -89.6462, 65.1115, 42.2134, -91.1666, -84.6927, -97.9312, 98.861, 19.8888, -64.0522, -27.1243, 72.7673, -9.9489, 94.274, -72.868, 43.1349, 84.1897, 87.0729, 87.2606, 34.5548, 87.7985, 4.84555, -10.2156, -93.7445, 71.8209, -63.4942, 45.6619, 93.4737, 79.6631, 66.8743, 18.1016, -27.7082, -67.0339, -11.9576, 52.0373, -11.5452, -73.8953, -63.5915, 99.4875, -46.2296, 75.453, -67.2079, 89.8868, -19.9666, 30.5359, 42.1465, -19.8105, 82.3653, -89.2164, -0.959167, -50.0723, -30.3058, 48.1772, -27.7686, 26.1484, 94.8462, -15.5663, -87.346, -84.2826, -58.8268, -42.4957, -23.7061, 67.0375, -83.5848, 64.167, 63.3315, -33.3809, 35.1264, 96.6872, 91.8399, 33.9888, 78.5923, -30.4885, 26.3331, -62.3014, -30.3431, 96.9554, 50.3262, 66.612, -71.5939, -97.0042, 71.4549, 42.3891, 71.308, 72.3209, 59.1608, -4.57918, -81.3639, -37.2619, 28.2445, 16.7995, -46.5868, -88.6893, 82.0504, -39.3391, -33.7141, -88.6628, -94.1271, -74.7738, -80.0798, -67.5639, -69.8237, -37.5219, 11.9601, -30.3912, -30.1169, 22.7411, -85.9541, 19.5141, -37.6203, -49.5693, 5.09318, 11.4884, 18.9713, 21.1073, -84.9266, 11.9436, -22.4703, -58.5243, -24.0218, -63.2767, -2.72752, -87.8947, -91.5162, 33.4207, -85.9841, 18.743, 48.9581, 69.3992, 2.42074, -75.0209, -53.2579, -45.4509, 66.6121, 51.6616, -48.073, 74.2754, -51.1623, -89.9345, 4.15238, -4.47531, 79.6587, -31.1646, 69.4103, -83.5936, -78.7341, 56.8626, 72.8834, -27.0248, -80.3328, 71.7272, -77.979, -76.6814, -14.9994, -94.5054, -75.2802, -96.4931, -17.6781, -5.50804, -83.4637, -56.8385, 51.5406, -60.527, -11.0762, -33.3166, -54.9609, -36.9426, -70.3942, 28.3439, 5.28754, -61.4775, 96.0657, -69.1967, 70.5489, 32.817, -53.5746, 49.2601, -88.5728, -1.94822, -3.16238, 19.083, -81.5139, 87.8383, 90.714, 4.75546, 31.9277, -4.1301, -0.160932, -31.2602, 56.4225, -72.1826, 91.5082, 68.2155, -81.7476, -14.0418, -79.4093, -30.7375, 38.8967, -16.5589, -69.4351, 48.6597, -8.44998, -74.4274, -20.3394, -59.2265, 19.407, 88.1542, -15.4888, 60.9066, 59.5144, -42.6935, 20.3518, 11.7192, -31.3635, 26.0055, -26.9334, 79.3798, 46.4724, 22.984, 67.5759]
    Input data (src1Local):
    [35.8789, 44.0334, 54.9997, 44.8567, -30.8579, -53.714, -59.8013, 71.1663, 46.3484, 8.56818, -59.4716, 6.07412, -39.0137, 64.5595, 17.0849, 45.2641, -63.2115, -98.7838, -52.3835, -65.9849, 50.4909, 69.9812, -22.3447, -32.3809, -97.8394, -45.4997, 63.5391, -69.3535, 43.3368, 98.8541, -77.2888, 1.02385, 20.4965, -26.7797, 98.2463, -78.9606, -62.4907, -13.5348, -49.5058, -4.06369, 77.2982, -32.8221, 84.6766, -62.3829, 58.8673, -75.8509, -95.3497, -79.7642, 67.1185, 34.4278, 34.5305, -76.1646, 53.1497, -12.3158, -42.9392, 59.2962, -4.12072, 47.1292, -17.0687, -78.0087, -59.4565, -98.9565, -54.0959, 56.5437, -74.3328, 77.2781, 52.7964, -0.932984, 70.8957, -68.1249, 85.895, 25.4119, 71.9202, -73.1287, 63.6916, 21.4303, 66.0614, 66.1438, -22.2332, 84.0665, -7.86752, -2.38648, 1.37756, -98.691, -35.847, -15.2647, -85.2363, -54.3978, -46.6612, 99.3826, -75.7728, -31.2539, 97.9558, 92.4507, 80.2871, -60.8802, -82.0434, -80.625, 19.6418, 51.0559, 35.3667, -56.306, -41.2088, 0.955906, -85.7743, 8.18112, 36.4615, -0.572343, -16.0821, 36.0277, -4.61647, 26.5385, 88.6082, 9.17454, 44.8951, -42.173, 51.5339, 5.93139, 93.7096, -68.8219, 68.2573, -67.325, -88.4579, -56.8873, -75.8117, -40.5576, -98.378, 32.3699, 64.6693, -73.1523, -57.8738, 63.1893, -40.4731, -1.00914, -80.6115, 44.2928, 76.6212, -29.3298, -58.1212, 83.3083, -20.6412, 26.8912, -82.1719, 26.8713, -56.5484, 35.4743, -8.59957, -12.4709, 27.8249, 76.6877, -27.5806, 63.2649, 66.1106, 15.8328, 9.19251, -79.6418, -9.31359, 63.7053, -8.37093, 55.6421, -99.0591, -64.1341, 91.4046, 26.7268, -92.1002, -34.7002, -6.41819, -18.15, 12.207, 48.6667, -39.4883, -21.0939, -50.3433, 58.2913, 7.64983, -82.6098, -89.6739, -25.9494, 82.4803, 20.8037, 21.483, -29.0788, 31.7695, 50.462, -83.7715, 63.4177, 52.7679, -90.2271, 16.1258, -61.4531, -61.7242, 25.0575, -97.8702, 26.9708, -23.039, -52.7595, -97.0177, -13.1399, -47.6936, -29.7551, 88.9603, -82.1242, -56.6307, 91.7884, 0.0381027, -49.0936, -43.5545, 47.3574, 97.1801, 43.4392, 22.7347, 12.6125, 63.7829, 22.3428, 53.4543, -91.4307, 45.6971, -92.1851, -81.4774, 35.8835, -33.043, -79.7464, 69.0971, -82.6252, -63.0042, -61.0205, -8.00347, -60.0369, 56.2894, -38.1932, 17.976, 5.82004, 4.41524, -52.2192, 93.1915, 21.1114, -29.3558, -18.5685, 20.7356, -4.71108, -0.947533, 73.0143, 62.5668, 96.1632, 41.4265, -89.503, -83.7747, -97.6047, -60.7304, 28.9736, -42.6681, 55.2584, 59.1584, -14.6596, -41.1826, 48.8083]
    Input data (maskLocal):
    [60, 32, 10, 7, 42, 52, 38, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 15, 18, 83, 41, 43, 91, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 77, 80, 58, 34, 76, 44, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 45, 48, 31, 30, 56, 43, 88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    Output data (dstLocal):
    [35.8789, 44.0334, -50.6124, -72.3737, -33.7107, -83.4001, -59.8013, 71.1663, 46.3484, 8.56818, -59.4716, 6.07412, -39.0137, -53.1848, 17.0849, 45.2641, -63.2115, 6.53722, -52.3835, 37.6655, 50.4909, 69.9812, -22.3447, -32.3809, 23.0464, 28.5975, -46.3033, -69.3535, 43.3368, 98.8541, -77.2888, 1.02385, 20.4965, 10.7653, 98.2463, -65.4774, -62.4907, -89.6462, -49.5058, -4.06369, 77.2982, -32.8221, -97.9312, -62.3829, 19.8888, -64.0522, -95.3497, -79.7642, 67.1185, 94.274, -72.868, -76.1646, 53.1497, 87.0729, -42.9392, 59.2962, -4.12072, 4.84555, -17.0687, -93.7445, 71.8209, -98.9565, -54.0959, 56.5437, -74.3328, 77.2781, 18.1016, -27.7082, -67.0339, -11.9576, 85.895, 25.4119, 71.9202, -73.1287, 63.6916, 21.4303, 66.0614, -67.2079, -22.2332, 84.0665, -7.86752, 42.1465, 1.37756, 82.3653, -35.847, -15.2647, -85.2363, -54.3978, 48.1772, -27.7686, 26.1484, -31.2539, 97.9558, 92.4507, 80.2871, -60.8802, -82.0434, -23.7061, 19.6418, -83.5848, 35.3667, 63.3315, -41.2088, 0.955906, -85.7743, 8.18112, 33.9888, -0.572343, -30.4885, 26.3331, -4.61647, 26.5385, 88.6082, 50.3262, 66.612, -42.173, 51.5339, 71.4549, 93.7096, -68.8219, 68.2573, 59.1608, -88.4579, -81.3639, -37.2619, -40.5576, -98.378, 32.3699, 64.6693, -73.1523, -39.3391, -33.7141, -88.6628, -94.1271, -80.6115, 44.2928, 76.6212, -29.3298, -58.1212, 83.3083, -20.6412, -30.1169, -82.1719, 26.8713, -56.5484, -37.6203, -8.59957, 5.09318, 27.8249, 76.6877, -27.5806, 63.2649, 11.9436, -22.4703, -58.5243, -79.6418, -9.31359, 63.7053, -8.37093, 55.6421, -99.0591, -85.9841, 91.4046, 48.9581, -92.1002, 2.42074, -6.41819, -18.15, 12.207, 48.6667, 51.6616, -21.0939, 74.2754, -51.1623, 7.64983, -82.6098, -89.6739, 79.6587, -31.1646, 20.8037, 21.483, -78.7341, 31.7695, 50.462, -83.7715, -80.3328, 52.7679, -77.979, -76.6814, -61.4531, -61.7242, 25.0575, -97.8702, 26.9708, -5.50804, -83.4637, -56.8385, 51.5406, -47.6936, -29.7551, 88.9603, -82.1242, -56.6307, 91.7884, 0.0381027, 5.28754, -43.5545, 47.3574, 97.1801, 70.5489, 22.7347, -53.5746, 63.7829, 22.3428, 53.4543, -91.4307, 19.083, -81.5139, 87.8383, 35.8835, -33.043, -79.7464, 69.0971, -82.6252, -63.0042, 56.4225, -8.00347, 91.5082, 56.2894, -81.7476, 17.976, 5.82004, 4.41524, -52.2192, -16.5589, 21.1114, 48.6597, -8.44998, 20.7356, -4.71108, -0.947533, 19.407, 88.1542, 96.1632, 41.4265, 59.5144, -83.7747, -97.6047, -60.7304, -31.3635, -42.6681, -26.9334, 79.3798, -14.6596, -41.1826, 48.8083]
    ```
