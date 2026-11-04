# DataCopy (UB-to-L1 High-Dimensional Sliced Data Movement)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:39:08.398Z pushedAt=2026-10-26T07:41:44.076Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_UBToL1_highdim_split_res.md#id1 -->

## Description

> [!NOTE]Note
> This API is implemented through software simulation. It is built on top of the Matmul high-level API and uses the workspace GM space of the Matmul high-level API as the data relay space. Data is first moved into GM and then into the L1 Buffer. Therefore, before using this API, you must first use REGISTER_MATMUL to register the high-level API.

The header file path is: `"basic_api/kernel_operator_data_copy_intf.h"`.

This API moves a matrix from the Unified Buffer (UB, with TPosition being VECIN/VECCALC/VECOUT) to the L1 Buffer. It supports both non-contiguous and contiguous movement, and the format and content remain unchanged during data movement.

## Prototype

```cpp
// Support both non-contiguous and contiguous data movement.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameter Description

**Table 1** Template parameter description

| Parameter Name | Description |
| ---------- | ---------- |
| T | Data type of the operand. The data types of the source operand and destination operand must be the same. |

**Table 2** Parameter description

| Parameter Name | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand. The type is LocalTensor, the storage location is L1 Buffer, and the start address must be 32-byte aligned. |
| src | Input | Source operand. The type is LocalTensor, the storage location is UB (TPosition is VECIN/VECCALC/VECOUT), and the start address must be 32-byte aligned. |
| repeatParams | Input | Movement parameters, of the DataCopyParams type. This parameter configures the size, count, and gap of the data blocks to be moved, and supports both non-contiguous and contiguous movement.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. |

**Table 3** DataCopyParams structure parameter definition

| Parameter Name | Description |
| ---------- | ---------- |
| blockCount | Number of contiguous data blocks to be moved. The type is uint16_t, and the value range is blockCount∈[1, 4095]. |
| blockLen | Length of each contiguous data block to be moved. The unit is DataBlock (32 bytes). The type is uint16_t, and the value range is blockLen∈[1, 65535]. |
| srcGap | Gap between adjacent contiguous data blocks of the source operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). The type is uint16_t. |
| dstGap | Gap between adjacent contiguous data blocks of the destination operand (the gap between the tail of the preceding data block and the head of the following data block). The unit is DataBlock (32 bytes). The type is uint16_t. |

The following figure shows how to use the DataCopyParams structure parameters. In the example, two contiguous data blocks are moved, each containing 8 DataBlocks. There is no gap between adjacent data blocks of the source operand, and the gap between the tail and head of adjacent data blocks of the destination operand is 1 DataBlock.

**Figure 1**  DataCopyParams structure parameter usage diagram<a id="fig_repeat_times_demo"></a>

![](../../../../figures/repeat-times.png "DataCopyParams structure parameter usage diagram")

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

    In the cross-card communication operator development scenario, the **DataCopy** APIs support cross-card data movement, supporting only the HCCS physical link and not other paths. During development, pay attention to the physical paths involved in inter-card communication. You can query the HCCS physical link by running the `npu-smi info -t topo` command.
<!-- end id14 -->

<!-- npu="950" id17 -->
- For the Ascend 950PR&950DT products, when moving data from UB to the L1 Buffer, you can select one of two movement paths by configuring the compilation option [ENABLE_CV_COMM_VIA_SSBUF](../../../../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#built-in-compilation-macro-switches). When **ENABLE_CV_COMM_VIA_SSBUF** is set to **true**, SSBuffer is used for communication, and data is moved through the hardware channel between UB and the L1 Buffer (recommended). See the sample [hardware channel movement](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1). When **ENABLE_CV_COMM_VIA_SSBUF** is **false**, data is moved to the L1 Buffer through GM, in which case the Matmul high-level API is required for registration.
<!-- end id17 -->

## Example

The example scenario is shown in [Figure 1](#fig_repeat_times_demo).

The example code snippet is as follows:

```cpp
constexpr uint32_t dataBlockElemCount = 16;
constexpr uint32_t srcElemCount = 2 * 8 * dataBlockElemCount;
constexpr uint32_t dstElemCount = (2 * 8 + 1) * dataBlockElemCount;

// Source operand: UB. Two data blocks are tightly arranged at the source, each with 8 DataBlocks, totaling 16 DataBlocks.
AscendC::LocalTensor<half> srcLocal(AscendC::TPosition::VECIN, ubAddr, srcElemCount);

// Destination operand: L1 Buffer. A gap of 1 DataBlock is reserved between the two destination data blocks, with a total span of 17 DataBlocks.
AscendC::LocalTensor<half> dstLocal(AscendC::TPosition::A1, a1Addr, dstElemCount);

AscendC::DataCopyParams repeatParams;
// Move 2 consecutive transfer data blocks.
repeatParams.blockCount = 2;
// Each data block is 8 DataBlocks long, that is, 256B, equal to 128 halves.
repeatParams.blockLen = 8;
// There is no gap between the tail and head of adjacent data blocks in the source UB; the two data blocks are read contiguously.
repeatParams.srcGap = 0;
// There is a gap of 1 DataBlock between the tail and head of adjacent data blocks in destination A1, that is, 32B, equal to 16 halves.
repeatParams.dstGap = 1;

AscendC::DataCopy(dstLocal, srcLocal, repeatParams);
```
