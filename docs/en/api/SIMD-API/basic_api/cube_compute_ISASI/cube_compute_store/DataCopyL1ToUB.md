# DataCopyL1ToUB (L1 to UB Data Copy)

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:29:13.833Z pushedAt=2026-10-26T07:41:44.118Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopyL1ToUB_res.md#id1 -->

## Description

Provides the basic capability to copy data from the L1 Buffer to the Unified Buffer (UB). The data retains its original format and content during transmission, and both contiguous and non-contiguous data copying are supported.

## Prototype

```cpp
// Continuous copy.
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// Supports both non-continuous and continuous copy.
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameters

**Table 1** Template Parameter Description

| Parameter | Description |
| :--- | :--- |
| T | Data type of the operand. |
| subBlockId | Vector core ID on the AI Core. Based on the input vector core ID, data is copied to the UB address on the corresponding core. |

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination operand, of type **LocalTensor**. The start address of the **LocalTensor** must be 32-byte aligned. |
| src | Input | Source operand, of type **LocalTensor**. The start address of the **LocalTensor** must be 32-byte aligned. |
| repeatParams | Input | Copy parameters, of type [**DataCopyParams**](#parameters). This parameter configures the size, number, and gap of the data blocks to be copied, and supports both non-contiguous and contiguous copying. For the definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |
| count | Input | Number of elements to be copied.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the copy amount is rounded down to 32-byte alignment.** |

**Table 3** DataCopyParams Structure Parameter Definition

| Parameter | Description |
| :--- | :--- |
| blockCount | Number of contiguous data blocks to be copied. Type **uint16_t**. Value range: blockCount∈[1, 4095]. |
| blockLen | Length of each contiguous data block to be copied, in **DataBlock** (32 bytes). Type **uint16_t**. Value range: blockLen∈[1, 65535]. |
| srcGap | Gap between adjacent contiguous data blocks of the source operand (the gap between the end of the preceding data block and the start of the following data block), in **DataBlock** (32 bytes). Type **uint16_t**. **srcGap** must not exceed the value range of this data type. |
| dstGap | Gap between adjacent contiguous data blocks of the destination operand (the gap between the end of the preceding data block and the start of the following data block), in **DataBlock** (32 bytes). Type **uint16_t**. **dstGap** must not exceed the value range of this data type. |

## Data Types

The supported data types are b8, b16, b32, and b64.

## Return Value

None

## Constraints

- This API is applicable only to the Mix operator scenario and supports only the configuration where the Cube core count to Vector core count ratio is **1:2**. In this scenario, **subBlockId** supports only the value **0** or **1**.

- If multiple DataCopyL1ToUB instructions need to be executed and their destination addresses overlap, call [PipeBarrier\(ISASI\)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction to ensure that multiple DataCopyL1ToUB instructions are serialized and to prevent abnormal data.

## Example

```cpp
// srcLocal and dstLocal are LocalTensors of the half type.
// Use the copy API that takes the count parameter to perform contiguous copy.
AscendC::DataCopyL1ToUB(dstLocal, srcLocal, 512);
// Use the copy API that takes the DataCopyParams parameter to support both contiguous and non-contiguous copy.
DataCopyParams intriParams;
intriParams.blockCount = 1; // The number of contiguous data blocks is 1.
intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the contiguous data block, in DataBlock units. Here the length is 512 half elements.
intriParams.srcGap = 0; // The source operand is copied contiguously.
intriParams.dstGap = 0; // The destination operand is laid out contiguously.
AscendC::DataCopyL1ToUB(dstLocal, srcLocal, intriParams);
```
