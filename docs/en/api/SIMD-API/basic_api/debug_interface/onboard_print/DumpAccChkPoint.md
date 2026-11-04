# DumpAccChkPoint<a name="ZH-CN_TOPIC_0000001877958569"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:13:31.735Z pushedAt=2026-10-26T07:41:44.383Z nl=2 -->

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
- Atlas 200I/500 A2 inference product: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpAccChkPoint_res.md#id1 -->

## Description<a name="section259105813316"></a>

The header file path is `"basic_api/kernel_operator_dump_tensor_intf.h"`.

This API dumps the content of a specified Tensor. It also supports printing a custom label (only information of the uint32_t data type is supported), such as the current line number. Unlike [DumpTensor](DumpTensor.md), this API supports printing a Tensor at a specified offset position.

In the operator kernel-side implementation code, call the DumpAccChkPoint API to print the relevant content at the location where the offset Tensor data needs to be printed. The sample is as follows:

```cpp
AscendC::DumpAccChkPoint(srcLocal, 5, 32, dataLen);
```
> [!CAUTION]Note
> This API is mainly used for debugging and analysis. Enabling it affects operator performance. It is usually used in the debugging phase, and it is recommended to disable it in the production environment.<br>
> By default, calling this API prints the relevant content. Developers can refer to [Disabling ASCENDC_DUMP](../disable_ascendc_dump.md) to disable this API as needed.

## Prototype<a name="section2067518173415"></a>

```cpp
template <typename T>
__aicore__ inline void DumpAccChkPoint(const LocalTensor<T> &tensor, uint32_t index, uint32_t countOff, uint32_t dumpSize)
template <typename T>
__aicore__ inline void DumpAccChkPoint(const GlobalTensor<T> &tensor, uint32_t index, uint32_t countOff, uint32_t dumpSize)
```

## Parameters<a name="section158061867342"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the Tensor to be dumped. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ------ | ------ | ------ |
| tensor | Input | Tensor to be dumped.<br>•When the tensor to be dumped resides in UB/L1 Buffer/L0C Buffer, use the **LocalTensor** type tensor parameter as input.<br>•When the tensor to be dumped resides in Global Memory, use the **GlobalTensor** type tensor parameter as input.|
| index | Input | User-defined additional information (line number or other custom number).|
| countOff | Input | Number of offset elements. The Tensor address after the offset must satisfy the alignment constraints of its physical location. For details, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).|
| dumpSize | Input | Number of elements to be dumped.|

## Data Types

<!-- npu="950" id101 -->
- Ascend 950PR&950DT products, the supported data types for T are: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id101 -->
<!-- npu="A3" id102 -->
- Atlas A3 products, T supports the following data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id102 -->
<!-- npu="910b" id103 -->
- Atlas A2 products, T supports the following data types: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id103 -->
<!-- npu="310p" id105 -->
- Atlas inference products AI Core, the supported data types of T are: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.<!-- end id105 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section794123819592"></a>

- Currently, only printing of Tensor information stored in Unified Buffer (UB)/L1 Buffer/L0C Buffer/Global Memory is supported.
<!-- npu="950" id100 -->
- For the Ascend 950PR&950DT products, when using this API to print L1 Tensor data, the HDK version is required to be upgraded to at least 25.7.RC1.6 or later.
<!-- end id100 -->
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The total amount of data printed by a single DumpAccChkPoint call must not exceed 30KB (including a small amount of header and trailer information required by the framework, which can usually be ignored). Note that if this limit is exceeded, the data will not be printed.

## Example<a name="section82241477610"></a>

```cpp
constexpr uint32_t totalLength = 256;    // Number of elements involved in the data transfer.
AscendC::LocalTensor<half> srcLocal;
AscendC::GlobalTensor<half> srcGlobal;
AscendC::DataCopy(srcLocal, srcGlobal, totalLength * sizeof(half));
uint32_t index = 56;    // User-defined additional information. The line number of the DumpAccChkPoint instruction is passed here.
uint32_t countOff = 32;    // Number of offset elements. Printing starts from srcLocal[32].
uint32_t dumpSize = 128;    // Number of elements to dump. 128 elements are printed starting from srcLocal[32].
AscendC::DumpAccChkPoint(srcLocal, index, countOff, dumpSize);
```

The print result is as follows:
```plain
DumpTensor: desc=56, addr=0x40, data_type=float16, position=UB, dump_size=15
[4.710938, 4.707031, 4.773438, 2.271484, 4.347656, 2.359375, 1.284180, 1.073242, 1.242188, 2.298828, 0.521973, 1.099609, 1.880859, 1.226562, 3.916016]
```
