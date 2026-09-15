# DataCopy (UBToL1 Continuous Data Transfer)

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:13:18.200Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
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
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description

> [!NOTE]Note
> This API is implemented through software simulation. It is built on the **Matmul** high-order API and uses the **workspace** GM space in the **Matmul** high-order API as the data relay space. Data is first moved into GM and then into the **L1 Buffer**. Therefore, before using this API, you must first register the high-order API using **REGISTER_MATMUL**.

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API moves a cube from the **Unified Buffer** (**UB**, with **TPosition** being **VECIN**/**VECCALC**/**VECOUT**) to the **L1 Buffer** in a continuous manner. The format and content of the data remain unchanged during the move.

## Prototype

```cpp
// Continuous copy.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operands. The source operand and the destination operand must have the same data type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, whose type is **LocalTensor** and whose storage location is **L1 Buffer**. The start address must be 32-byte aligned. |
| src | Input | Source operand, whose type is **LocalTensor** and whose storage location is **Unified Buffer** (**TPosition** is **VECIN**/**VECCALC**/**VECOUT**). The start address must be 32-byte aligned. |
| count | Input | Number of elements involved in the copy. count * sizeof(T) must be 32-byte aligned. If not aligned, the copy amount is rounded down to 32 bytes. |

## Data Types

The source cube and the destination cube support the same data types.

<!-- npu="950" id10 -->
For Ascend 950PR/Ascend 950DT, the supported data types are: bool, int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32, int64_t, uint64_t, double, complex64.
<!-- end id10 -->

<!-- npu="A3" id11 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, bfloat16_t, float, double.
<!-- end id11 -->

<!-- npu="910b" id12 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, bfloat16_t, float, double.
<!-- end id12 -->

<!-- npu="310p" id13 -->
For Atlas inference products AI Core, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float, double.
<!-- end id13 -->

## Return Value

None

## Constraints

- If multiple **DataCopy** instructions need to be executed and their destination addresses overlap, call [PipeBarrier(ISASI)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that the multiple **DataCopy** instructions are serialized to prevent abnormal data.

- If the destination addresses in the L1 Buffer overlap, call PipeBarrier\<PIPE_MTE2\>() between the two data movement instructions to add synchronization for the MTE2 move-in pipeline.

<!-- npu="A3,910b" id14 -->
- For the following product models:

    <!-- npu="A3" id15 -->
    Atlas A3 training products/Atlas A3 inference products;
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    Atlas A2 training products/Atlas A2 inference products;
    <!-- end id16 -->

    In cross-card communication operator development scenarios, the **DataCopy** APIs support cross-card data movement only over the HCCS physical link, and do not support other paths. During development, developers need to pay attention to the physical paths involved in inter-card communication, and can query the HCCS physical link using the `npu-smi info -t topo` command.
<!-- end id14 -->

<!-- npu="950" id17 -->
- For Ascend 950PR/Ascend 950DT, when moving data from UB to L1 Buffer, you can select between two movement paths by configuring the compilation option `ENABLE_CV_COMM_VIA_SSBUF`. When `ENABLE_CV_COMM_VIA_SSBUF` is set to true, SSBuffer is used for communication, and data is moved through the hardware channel between UB and L1 Buffer (recommended). See the sample [hardware channel movement](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1). When `ENABLE_CV_COMM_VIA_SSBUF` is false, data is moved to the L1 Buffer via GM, in which case the **Matmul** high-order API is required for registration.
<!-- end id17 -->

## Example

The following is a sample code snippet. For the complete sample, see [DataCopy_UB2L1 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1).

```cpp
// dstLocal and srcLocal are LocalTensors of the half type, located in L1 Buffer and UB, respectively.
// Use the copy API that takes the count parameter to perform continuous copying.
AscendC::DataCopy(dstLocal, srcLocal, 512);
```
