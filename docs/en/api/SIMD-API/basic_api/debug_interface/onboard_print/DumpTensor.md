# DumpTensor<a name="ZH-CN_TOPIC_0000001584608650"></a>

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-09-23T15:14:10.749Z pushedAt=2026-10-26T07:41:44.385Z nl=2 -->

## Applicable Products

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
- Atlas 200I/500 A2 inference product: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor_res.md#id1 -->

## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_dump_tensor_intf.h"`.

This API prints the content of a Tensor and also supports printing custom labels (only information of the uint32_t data type), such as the current line number.
In the operator kernel-side implementation code, call the DumpTensor API at the location where Tensor data needs to be printed. The sample is as follows:

```cpp
AscendC::DumpTensor(srcLocal, 5, dataLen);
```
> [!CAUTION]Note
> This API is mainly used for debugging and analysis. Enabling it affects operator performance to a certain extent. It is usually used in the debugging phase and is recommended to be disabled in the production environment.<br>
> By default, calling this API prints the related content. Developers can refer to [Disabling ASCENDC_DUMP](../disable_ascendc_dump.md) to disable this API as needed.

The print example is as follows:  
```plain
DumpTensor: desc=5, addr=0, data_type=float16, position=UB, dump_size=32
[19.000000, 4.000000, 38.000000, 50.000000, 39.000000, 67.000000, 84.000000, 98.000000, 21.000000, 36.000000, 18.000000, 46.000000, 10.000000, 92.000000, 26.000000, 38.000000, 39.000000, 9.000000, 82.000000, 37.000000, 35.000000, 65.000000, 97.000000, 59.000000, 89.000000, 63.000000, 70.000000, 57.000000, 35.000000, 3.000000, 16.000000,
42.000000]
DumpTensor: desc=5, addr=100, data_type=float16, position=UB, dump_size=32
[6.000000, 34.000000, 52.000000, 38.000000, 73.000000, 38.000000, 35.000000, 14.000000, 67.000000, 62.000000, 30.000000, 49.000000, 86.000000, 37.000000, 84.000000, 18.000000, 38.000000, 18.000000, 44.000000, 21.000000, 86.000000, 99.000000, 13.000000, 79.000000, 84.000000, 9.000000, 48.000000, 74.000000, 52.000000, 99.000000, 80.000000,
53.000000]
...
DumpTensor: desc=5, addr=0, data_type=float16, position=UB, dump_size=32
[35.000000, 41.000000, 41.000000, 22.000000, 84.000000, 49.000000, 60.000000, 0.000000, 90.000000, 14.000000, 67.000000, 80.000000, 16.000000, 46.000000, 16.000000, 83.000000, 6.000000, 70.000000, 97.000000, 28.000000, 97.000000, 62.000000, 80.000000, 22.000000, 53.000000, 37.000000, 23.000000, 58.000000, 65.000000, 28.000000, 4.000000,
29.000000]
```

## Prototype<a name="section2067518173415"></a>

- Printing without **Tensor** shape.

    ```cpp
    template <typename T>
    __aicore__ inline void DumpTensor(const LocalTensor<T> &tensor, uint32_t desc, uint32_t dumpSize)
    template <typename T>
    __aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize)
    ```

- Printing with **Tensor** shape.

    ```cpp
    template <typename T>
    __aicore__ inline void DumpTensor(const LocalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
    template <typename T>
    __aicore__ inline void DumpTensor(const GlobalTensor<T>& tensor, uint32_t desc, uint32_t dumpSize, const ShapeInfo& shapeInfo)
    ```

## Parameters<a name="section158061867342"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the Tensor to be dumped. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| tensor | Input | Tensor to be dumped.<br>• When the tensor to be dumped resides in UB/L1 Buffer/L0C Buffer, pass the tensor parameter of the **LocalTensor** type.<br>• When the tensor to be dumped resides in Global Memory, pass the tensor parameter of the **GlobalTensor** type. |
| desc | Input | User-defined additional information (line number or other custom number).<br>When using the DumpTensor function, you can attach custom information through the **desc** parameter to distinguish the source of the dumped content in different scenarios. This helps precisely locate the output of a specific DumpTensor and improves debugging and analysis efficiency. |
| dumpSize | Input | Number of elements to be dumped. |
| shapeInfo | Input | Shape information of the input Tensor, which can be used for printing according to the shape information.<br>• When the shape size is greater than the number of elements in **dumpSize**, elements are printed according to **ShapeInfo**, and the missing dump data is displayed with "-".<br>• When the shape size is less than or equal to the number of elements in **dumpSize**, elements are printed according to **ShapeInfo**, and the extra dump data is not displayed. |

## Data Types

<!-- npu="950" id101 -->
- Ascend 950PR&950DT products, the supported data types of T are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id101 -->
<!-- npu="A3" id102 -->
- Atlas A3 products, the supported data types of T are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id102 -->
<!-- npu="910b" id103 -->
- For Atlas A2 products, the data types supported by T are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id103 -->
<!-- npu="310b" id104 -->
- Atlas 200I/500 A2 inference product, the supported data types for T are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id104 -->
<!-- npu="310p" id105 -->
- On the AI Core of Atlas inference products, the supported data types for T are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id105 -->

## Constraints<a name="section794123819592"></a>

- Currently, printing of Tensor information stored in Unified Buffer (UB)/L1 Buffer/L0C Buffer/Global Memory is supported.
<!-- npu="950" id111 -->
- On Ascend 950PR&950DT products, printing of Tensor data in BiasTable Buffer and Fixpipe Buffer is added.
<!-- end id111 -->
<!-- npu="950" id112 -->
- For Ascend 950PR&950DT products, when using this API to print L1 Buffer, BiasTable Buffer, or Fixpipe Buffer data, the HDK version must be upgraded to at least 25.7.RC1.6 or later.
<!-- end id112 -->
<!-- npu="950" id113 -->
- For Ascend 950PR&950DT products, the scenario of printing Tensor information in Fixpipe Buffer:
  - Fixpipe Buffer stores hardware parameter bit fields, so the print result may not be bitwise identical to the original data in L1 Buffer. For the preceding Quant parameters, every 8 bytes retain bit[7:0], bit[31:13], and bit[46:37], with the expected value of `input & 0x00007fe0ffffe0ffULL`; for the preceding ReLU parameters, every 4 bytes retain bit[31:13], with the expected value of `word & 0xffffe000U`.
<!-- end id113 --> 
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- In SIMD scenarios, the total amount of data printed by a single call to this API must not exceed the print size limit, which is 30KB by default. Note that if this limit is exceeded, the data will not be printed. You can configure this through the "simd\_printf\_fifo\_size\_per\_core" field in the [aclInit API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/02_initialization_and_deinitialization.md#aclinit), with a configurable range from a minimum of 1KB to a maximum of 64MB. Note that if this limit is exceeded, the data will not be printed.

## Example<a name="section82241477610"></a>

- Printing without Tensor shape.

    ```cpp
    AscendC::DumpTensor(srcLocal, 5, dataLen);
    ```

- Printing with Tensor shape.

    - Shape equals the number of elements in dumpSize.
        ```cpp
        uint32_t array[] = {static_cast<uint32_t>(8), static_cast<uint32_t>(8)};
        AscendC::ShapeInfo shapeInfo(2, array);       // dim is 2, shape is (8,8).
        AscendC::DumpTensor(x, 2, 64, shapeInfo);     // Dump 64 elements of x, parsed and arranged according to shapeInfo (8,8).
        ```
        The print result is as follows:
        ```plain
        DumpTensor: desc=2, addr=0x0, data_type=float16, position=UB, dump_size=64
        [[2.048828,0.113037,4.042969,3.505859,4.554688,4.019531,0.598633,2.160156],
        [2.707031,0.117981,1.134766,4.835938,1.190430,3.085938,1.334961,0.406250],
        [2.658203,1.674805,3.791016,0.747070,3.541016,4.546875,0.394043,2.455078],
        [1.161133,2.775391,0.453857,2.857422,2.837891,1.052734,2.654297,1.828125],
        [0.358643,4.765625,3.681641,0.850098,2.250000,2.001953,0.446777,0.830078],
        [2.154297,4.781250,1.773438,0.201294,0.028412,3.285156,0.772949,3.261719],
        [0.532227,2.789062,0.588867,4.316406,0.146606,2.201172,3.775391,2.023438],
        [2.820312,2.835938,2.957031,2.398438,4.449219,0.516113,4.796875,0.786133]]
        ```

    - Shape is smaller than the number of elements in dumpSize.
        ```cpp
        uint32_t array1[] = {static_cast<uint32_t>(7), static_cast<uint32_t>(8)};
        AscendC::ShapeInfo shapeInfo1(2, array1); // dim is 2, shape is (7,8).
        AscendC::DumpTensor(x1, 3, 64, shapeInfo1); // When the Shape size is smaller than or equal to the number of elements in dumpSize, print elements according to ShapeInfo, and the extra Dump data is not displayed.
        ```
        The print result is as follows:
        ```plain
        DumpTensor: desc=3, addr=0x0, data_type=float16, position=UB, dump_size=64
        shape is [7, 8], dumpSize is 64, dumpSize is greater than shapeSize.
        [[2.048828,0.113037,4.042969,3.505859,4.554688,4.019531,0.598633,2.160156],
        [2.707031,0.117981,1.134766,4.835938,1.190430,3.085938,1.334961,0.406250],
        [2.658203,1.674805,3.791016,0.747070,3.541016,4.546875,0.394043,2.455078],
        [1.161133,2.775391,0.453857,2.857422,2.837891,1.052734,2.654297,1.828125],
        [0.358643,4.765625,3.681641,0.850098,2.250000,2.001953,0.446777,0.830078],
        [2.154297,4.781250,1.773438,0.201294,0.028412,3.285156,0.772949,3.261719],
        [0.532227,2.789062,0.588867,4.316406,0.146606,2.201172,3.775391,2.023438]]
        ```
    - Shape larger than the number of dumpSize elements
        ```cpp
        uint32_t array2[] = {static_cast<uint32_t>(9), static_cast<uint32_t>(8)};
        AscendC::ShapeInfo shapeInfo2(2, array2); // dim is 2, shape is (9,8)
        AscendC::DumpTensor(x2, 4, 64, shapeInfo2); // When the Shape size is larger than the number of dumpSize elements, print elements according to ShapeInfo, and display insufficient Dump data with "-"
        ```
        The print result is as follows:
        ```plain
        DumpTensor: desc=4, addr=0x0, data_type=float16, position=UB, dump_size=64
        shape is [9, 8], dumpSize is 64, data is not enough.
        [[2.048828,0.113037,4.042969,3.505859,4.554688,4.019531,0.598633,2.160156],
        [2.707031,0.117981,1.134766,4.835938,1.190430,3.085938,1.334961,0.406250],
        [2.658203,1.674805,3.791016,0.747070,3.541016,4.546875,0.394043,2.455078],
        [1.161133,2.775391,0.453857,2.857422,2.837891,1.052734,2.654297,1.828125],
        [0.358643,4.765625,3.681641,0.850098,2.250000,2.001953,0.446777,0.830078],
        [2.154297,4.781250,1.773438,0.201294,0.028412,3.285156,0.772949,3.261719],
        [0.532227,2.789062,0.588867,4.316406,0.146606,2.201172,3.775391,2.023438],
        [2.820312,2.835938,2.957031,2.398438,4.449219,0.516113,4.796875,0.786133],
        [-,-,-,-,-,-,-,-]]
        ```
