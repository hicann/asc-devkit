# DataCopy (UBToL1 High-Dimensional Split Data Copy)

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:15:06.061Z -->

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

> [!NOTE]
> This API is implemented through software simulation. It is built on the Matmul high-level API and uses the workspace GM space in the Matmul high-level API as the data relay space. Data is first copied into GM and then into the L1 Buffer. Therefore, before using this API, you must first use **REGISTER_MATMUL** to register the high-level API.

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

This API copies a cube from the Unified Buffer (UB, with **TPosition** set to **VECIN**/**VECCALC**/**VECOUT**) to the L1 Buffer. It supports both non-contiguous and contiguous copy, and the format and content remain unchanged during the data copy.

## Prototype

```cpp
// Supports both non-contiguous and contiguous copy.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operand. The source operand and destination operand must have the same data type. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, whose type is **LocalTensor** and whose storage location is **L1 Buffer**. The start address must be 32-byte aligned. |
| src | Input | Source operand, whose type is **LocalTensor** and whose storage location is **Unified Buffer** (**TPosition** is **VECIN**/**VECCALC**/**VECOUT**). The start address must be 32-byte aligned. |
| repeatParams | Input | Copy parameter, of the **DataCopyParams** type. This parameter configures the size, count, and gap of the data blocks to be copied, and supports both non-contiguous and contiguous copy.<br>For the specific definition, see `${INSTALL_DIR}/include/ascendc/basic_api/API/kernel_struct_data_copy.h`. |

**Table 3** DataCopyParams structure parameter definition

| Parameter | Description |
| ---------- | ---------- |
| blockCount | Number of contiguous data blocks to be copied. Type: **uint16_t**. Value range: blockCount ∈ [1, 4095]. |
| blockLen | Length of each contiguous data block to be copied, in **DataBlock** (32 bytes). Type: **uint16_t**. Value range: blockLen ∈ [1, 65535]. |
| srcGap | Gap between adjacent contiguous data blocks in the source operand (the gap between the tail of the preceding data block and the head of the following data block), in **DataBlock** (32 bytes). Type: **uint16_t**. |
| dstGap | Gap between adjacent contiguous data blocks in the destination operand (the gap between the tail of the preceding data block and the head of the following data block), in **DataBlock** (32 bytes). Type: **uint16_t**. |

The following figure shows how the **DataCopyParams** structure parameters are used. In the example, two contiguous data blocks are copied, each containing eight **DataBlock** units. There is no gap between adjacent data blocks in the source operand, while the gap between the tail and head of adjacent data blocks in the destination operand is one **DataBlock**.

**Figure 1** DataCopyParams structure parameter usage diagram<a id="fig_repeat_times_demo"></a>

![](../../../../figures/repeat-times.png "DataCopyParams structure parameter usage diagram")

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

- If the destination addresses in the L1 Buffer overlap, call `PipeBarrier\<PIPE_MTE2\>()` between the two copy instructions to add synchronization for the MTE2 inbound pipeline.

<!-- npu="A3,910b" id14 -->
- For the following product models:

    <!-- npu="A3" id15 -->
    Atlas A3 training products/Atlas A3 inference products;
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    Atlas A2 training products/Atlas A2 inference products;
    <!-- end id16 -->

    In cross-card communication operator development scenarios, the **DataCopy**-class APIs support cross-card data copy only over the HCCS physical link, and do not support other paths. During development, developers need to pay attention to the physical paths involved in inter-card communication. The HCCS physical link can be queried using the `npu-smi info -t topo` command.
<!-- end id14 -->

<!-- npu="950" id17 -->
- For Ascend 950PR/Ascend 950DT, when copying data from UB to L1 Buffer, you can select between two copy paths by configuring the compilation option **ENABLE_CV_COMM_VIA_SSBUF**. When **ENABLE_CV_COMM_VIA_SSBUF** is set to **true**, SSBuffer is used for communication, and data is copied through the hardware channel between UB and L1 Buffer (recommended). For an example, see [Hardware Channel Copy](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1). When **ENABLE_CV_COMM_VIA_SSBUF** is set to **false**, data is copied to L1 Buffer via GM, and in this scenario, registration must be performed with the help of the Matmul high-level API.
<!-- end id17 -->

## Example

The example scenario is shown in [Figure 1](#fig_repeat_times_demo).

The sample code snippet is as follows:

```cpp
constexpr uint32_t dataBlockElemCount = 16;
constexpr uint32_t srcElemCount = 2 * 8 * dataBlockElemCount;
constexpr uint32_t dstElemCount = (2 * 8 + 1) * dataBlockElemCount;

// Source operand: UB. Two data blocks are tightly arranged at the source, with 8 DataBlocks per block, totaling 16 DataBlocks.
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::VECIN, ubAddr, srcElemCount);

// Destination operand: L1 Buffer. A gap of 1 DataBlock is reserved between the two destination data blocks, with a total span of 17 DataBlocks.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, a1Addr, dstElemCount);

AscendC::DataCopyParams repeatParams;
// Copy 2 consecutive data blocks.
repeatParams.blockCount = 2;
// Each data block is 8 DataBlocks long, that is, 256B, equal to 128 half elements.
repeatParams.blockLen = 8;
// There is no gap between the tail and head of adjacent data blocks in the source UB; the two data blocks are read contiguously.
repeatParams.srcGap = 0;
// There is a gap of 1 DataBlock, that is, 32B, equal to 16 half elements, between the tail and head of adjacent data blocks in the destination A1.
repeatParams.dstGap = 1;

AscendC::DataCopy(dstLocal, srcLocal, repeatParams);
```
