# LoadDataWithSparse<a name="ZH-CN_TOPIC_0000002568950935"></a>

<!-- md-trans-meta sourceCommit=unknown translatedAt=2026-08-31T11:51:20.875Z -->

## Applicable Products<a name="zh-cn_topic_0000002512171654_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="zh-cn_topic_0000002512171654_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_mm_intf.h"`.

It is used to transfer the dense weight cube stored in units of 512 bytes from the L1 Buffer to the L0B Buffer, and simultaneously transfer the Index Cube stored in units of 128 bytes to the built-in dedicated buffer space (for subsequent reading by the MmadWithSparse API).

The data type of the Index Cube is uint2, which must be assembled into the uint8 data type before being passed to the API. The Index Cube is arranged in reverse order within a uint8 address. For example, the Index Cube is arranged in the address as 1 0 2 1 0 1 2 0, where 1 0 2 1 (corresponding to the first four bits 1 2 0 1 of the Index Cube) forms one uint8, and 0 1 2 0 (corresponding to the last four bits 0 2 1 0 of the Index Cube) forms another uint8.

Only the following data path transfer is supported: L1 Buffer->L0B Buffer.

**Figure 1** Schematic diagram of L1 Buffer->L0B Buffer LoadDataWithSparse. The LoadData2dParams parameter is configured with startIndex = 1 and repeatTimes = 5, indicating that 5 consecutive data fractals are transferred starting from the first data fractal in the source operand src, and 5 consecutive index fractals are transferred starting from the first index fractal in the source operand idx.<a name="zh-cn_topic_0000002512171654_fig71111314164414"></a>  

![](../../../../figures/loaddatawithsparse_l12l0b.png)

## Prototype<a name="zh-cn_topic_0000002512171654_section82039854412"></a>

```cpp
template <typename T = int8_t, typename U = uint8_t, typename Std::enable_if<Std::is_same<PrimT<T>, int8_t>::value, bool>::type = true, typename Std::enable_if<Std::is_same<PrimT<U>, uint8_t>::value, bool>::type = true>
__aicore__ inline void LoadDataWithSparse(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx, const LoadData2dParams& loadDataParam)
```

## Parameters<a name="zh-cn_topic_0000002512171654_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| -------- | ------ |
| T | Data type of **dst** and **src**. |
| U | Data type of **idx**. |
| Std::enable_if\<Std::is_same\<PrimT\<T\>, int8_t\>::value, bool\>::type | Used for data type checking of **T**. Users do not need to pay attention to it. |
| Std::enable_if\<Std::is_same\<PrimT\<U\>, uint8_t\>::value, bool\>::type | Used for data type checking of **U**. Users do not need to pay attention to it. |

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
| ---------- | ----------- | ------ |
| **dst** | Output | Destination operand, of type **LocalTensor**.<br>For fractal constraints, see [Cube Computation Input Transfer Constraints](matrix_computation_input_movement_constraint.md).<br>For start address alignment constraints, see [Alignment Constraint](matrix_computation_input_movement_constraint.md).<br>Supported data type: **int8_t**.<br>Atlas A2 training products/Atlas A2 inference products: supported physical storage location is L0B Buffer (TPosition: B2).<br>Atlas A3 training products/Atlas A3 inference products: supported physical storage location is L0B Buffer (TPosition: B2). |
| **src** | Input | Source operand, of type **LocalTensor**.<br>For fractal constraints, see [Cube Computation Input Transfer Constraints](matrix_computation_input_movement_constraint.md).<br>For start address alignment constraints, see [Alignment Constraint](matrix_computation_input_movement_constraint.md).<br>Supported data type: **int8_t**.<br>Atlas A2 training products/Atlas A2 inference products: supported physical storage location is L1 Buffer (TPosition: B1).<br>Atlas A3 training products/Atlas A3 inference products: supported physical storage location is L1 Buffer (TPosition: B1). |
| **idx** | Input | Source operand, of type **LocalTensor**.<br>The data fractal size is 128 bytes, and each data fractal has a shape of 16 * 32 * 2 bits.<br>For start address alignment constraints, see [Alignment Constraint](matrix_computation_input_movement_constraint.md).<br>Supported data type: **uint8_t**.<br>Atlas A2 training products/Atlas A2 inference products: supported physical storage location is L1 Buffer (TPosition: B1).<br>Atlas A3 training products/Atlas A3 inference products: supported physical storage location is L1 Buffer (TPosition: B1). |
| **loadDataParam** | Input | **LoadData** parameter structure, of type:<br>&nbsp;&nbsp;&bull; **LoadData2dParams**. For details, see [LoadData2dParams structure parameter description](Load2D.md#table_load2d_params).<br>Note that this API supports only continuous data fractal transfer and does not support stride skipping. Therefore, only the **startIndex** and **repeatTimes** parameters in **loadDataParam** can be configured; the remaining parameters are unused and require no configuration. |

## Data Type<a name="zh-cn_topic_0000002512171654_section4219135304818"></a>

Supported data types: **src** and **dst** support **int8_t**, and **idx** supports **uint8_t**.

## Return Value<a name="zh-cn_topic_0000002512171654_section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000002512171654_section2045914466492"></a>

- When **repeatTimes** in **loadDataParam** is set to 0, no transfer is performed and this API is treated as a NOP (no operation). The maximum supported value of **repeatTimes** is 255.
- Transposition is not supported. Only continuous data fractal transfer is supported, and stride skipping is not supported. Only the **startIndex** and **repeatTimes** parameters in **loadDataParam** can be configured; the remaining parameters are unused.
- The dedicated buffer space for storing the Index Cube is one quarter of the L0B Buffer size. Developers do not need to configure the address, and the **MmadWithSparse** API automatically reads data from this buffer.
- Only the L1 Buffer -> L0B Buffer path is supported, and the fractal on the L1 Buffer is Zn.
- The **startIndex** in each iteration cannot be less than zero.

## Example<a name="zh-cn_topic_0000002512171654_section088124295117"></a>

This API is used together with Sparse Mmad. For a complete example, see [MmadWithSparse example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/mmad_with_sparse).
