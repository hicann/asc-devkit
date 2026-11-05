# DataCopy (UBToL1 Contiguous Data Copy)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:36:45.542Z pushedAt=2026-10-26T07:41:44.074Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_continuous_res.md#id1 -->

## Description

> [!NOTE]
> This API is implemented through software simulation. It is built on top of the Matmul high-order API and uses the workspace GM space in the Matmul high-order API as the data relay space. Data is first moved into GM and then into the L1 Buffer. Therefore, when using this API, you must first use REGISTER_MATMUL to register the high-order API.

The header file path is: `"basic_api/kernel_operator_data_copy_intf.h"`.

This API moves a matrix from the Unified Buffer (UB, with TPosition being VECIN/VECCALC/VECOUT) to the L1 Buffer. The movement is continuous, and the format and content of the data remain unchanged during the transfer.

## Prototype

```cpp
// Copy continuously.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operand. The source operand and destination operand have the same data type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, type is **LocalTensor**, stored in L1 Buffer, with a 32-byte aligned start address. |
| src | Input | Source operand, type is **LocalTensor**, stored in UB (**TPosition** is VECIN/VECCALC/VECOUT), with a 32-byte aligned start address. |
| count | Input | Number of elements involved in the movement.<br>**Note: count * sizeof(T) must be 32-byte aligned. If not aligned, the moved amount is rounded down to 32-byte alignment.** |

## Data Types

The source matrix and the destination matrix support the same data types.

<!-- npu="950" id10 -->
For the Ascend 950PR&950DT products, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For the Atlas A3 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, bfloat16_t, float, double.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For the Atlas A2 products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, bfloat16_t, float, double.
<!-- end id12 -->

<!-- npu="310p" id13 -->
For the Atlas inference products AI Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id13 -->

## Return Value Description

None

## Constraints

- If multiple **DataCopy** instructions need to be executed and their destination addresses overlap, call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple **DataCopy** instructions are serialized to prevent abnormal data.

- If the destination addresses in the L1 Buffer overlap, call PipeBarrier\<PIPE_MTE2\>() between the two data movement instructions to add synchronization for the MTE2 move-in pipeline.

<!-- npu="A3,910b" id14 -->
- For the following product models:

    <!-- npu="A3" id15 -->
    Atlas A3 products;
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    Atlas A2 products;
    <!-- end id16 -->

    In cross-card communication operator development scenarios, the **DataCopy** APIs support cross-card data movement only over the HCCS physical link, and do not support other paths. During development, pay attention to the physical paths involved in inter-card communication. You can query the HCCS physical link by running the `npu-smi info -t topo` command.
<!-- end id14 -->

<!-- npu="950" id17 -->
- For the Ascend 950PR&950DT products, when moving data from UB to L1 Buffer, you can configure the compilation option [ENABLE_CV_COMM_VIA_SSBUF](../../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches) to select between two movement paths. When **ENABLE_CV_COMM_VIA_SSBUF** is set to **true**, SSBuffer is used for communication, and data is moved through the hardware channel between UB and L1 Buffer (recommended). See the sample [hardware channel movement](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1). When **ENABLE_CV_COMM_VIA_SSBUF** is set to **false**, data is moved to the L1 Buffer via GM, in which case the Matmul high-order API is required for registration.
<!-- end id17 -->

## Example

The sample code snippet is as follows. For the complete sample, see [DataCopy_UB2L1 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1).

```cpp
// dstLocal and srcLocal are LocalTensors of the half type, located in the L1 Buffer and UB respectively.
// Use the copy API that takes the count parameter to complete continuous copy.
AscendC::DataCopy(dstLocal, srcLocal, 512);
```
