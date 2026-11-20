# DataCopy (UBToL1 Continuous Data Movement)

<!-- md-trans-meta sourceCommit=0f9a59471afc8678a19cd607420c13ec5e8e0290 translatedAt=2026-11-02T03:07:34.821Z pushedAt=2026-11-18T01:25:06.694Z nl=2 -->

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
<!-- npu="x90" id8 -->
- Kirin X90 processor products: Not Supported
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030 processor products: Not Supported
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous_res.md#id1 -->

## Description

> [!NOTE]Description
> This API is implemented by software simulation. It is built on top of the Matmul high-level API and uses the workspace GM space of the Matmul high-level API as the data relay space. Data is first moved into GM and then into the L1 Buffer. Therefore, before using this API, you must first register the high-level API using **REGISTER_MATMUL**.

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API moves a matrix from the Unified Buffer (UB, with TPosition being VECIN/VECCALC/VECOUT) to the L1 Buffer. The movement is continuous, and the format and content of the data remain unchanged during the transfer.

## Function Prototype

```cpp
// Copy continuously.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operand. The data types of the source operand and destination operand must be the same. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand. The type is LocalTensor, the storage location is L1 Buffer, and the start address must be 32-byte aligned. |
| src | Input | Source operand. The type is LocalTensor, the storage location is UB (TPosition is VECIN/VECCALC/VECOUT), and the start address must be 32-byte aligned. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

## Data Types

The source matrix and destination matrix support the same data types.

<!-- npu="950" id10 -->
For Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, bfloat16_t, float, double.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, bfloat16_t, float, double.
<!-- end id12 -->

<!-- npu="310p" id13 -->
For the AI Core of Atlas inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id13 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous_res.md#id2 -->

## Return Value

None

## Constraints

- If multiple **DataCopy** instructions need to be executed and their destination addresses overlap, call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that multiple **DataCopy** instructions are serialized to prevent abnormal data.

- If the destination addresses in the L1 Buffer overlap, call PipeBarrier\<PIPE_MTE2\>() between the two movement instructions to add synchronization for the **MTE2** move-in pipeline.

<!-- npu="A3,910b" id14 -->
- For the following product models:

    <!-- npu="A3" id15 -->
    Atlas A3 products;
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    Atlas A2 products;
    <!-- end id16 -->

    In the cross-card communication operator development scenario, the **DataCopy** class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical path involved in inter-card communication. You can run the npu-smi info -t topo command to query the HCCS physical link.
<!-- end id14 -->

<!-- npu="950" id17 -->
- For the Ascend 950PR&950DT products, when moving data from UB to the L1 Buffer, you can select one of two movement paths by configuring the compilation option [ENABLE_CV_COMM_VIA_SSBUF](../../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches). When **ENABLE_CV_COMM_VIA_SSBUF** is set to **true**, SSBuffer is used for communication, and data is moved through the hardware channel between UB and the L1 Buffer (recommended). See the sample [hardware channel movement](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1). When **ENABLE_CV_COMM_VIA_SSBUF** is **false**, data is moved to the L1 Buffer through GM, in which case the Matmul high-level API is required for registration.
<!-- end id17 -->

## Example

The sample code snippet is as follows. For the complete sample, see [DataCopy_UB2L1 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1).

```cpp
// dstLocal and srcLocal are LocalTensors of the half type, located in the L1 Buffer and UB, respectively.
// Use the copy API with the count parameter to perform continuous copying.
AscendC::DataCopy(dstLocal, srcLocal, 512);
```
