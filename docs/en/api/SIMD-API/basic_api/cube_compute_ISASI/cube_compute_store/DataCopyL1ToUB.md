# DataCopyL1ToUB (Data Copy from L1 to UB)

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:43:07.840Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not Supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not Supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not Supported
<!-- end id7 -->
## Description

Provides the basic capability to transfer data from the **L1 Buffer** to the **Unified Buffer**. The data remains unchanged in its original format and content during transmission, and both contiguous and non-contiguous data transfer are supported.

## Prototype

```cpp
// Continuous transfer.
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// Supports both non-continuous and continuous transfer.
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL1ToUB(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameter Description

**Table 1** Template parameter description

| Parameter Name | Description |
| :--- | :--- |
| T | Data type of the operand. |
| subBlockId | Vector core ID on the AI Core. Based on the input vector core ID, data is copied to the UB address on the corresponding core. |

**Table 2** Parameter description

| Parameter Name | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | Destination operand of type **LocalTensor**. The start address of **LocalTensor** must be 32-byte aligned. |
| src | Input | Source operand of type **LocalTensor**. The start address of **LocalTensor** must be 32-byte aligned. |
| repeatParams | Input | Copy parameters of type [DataCopyParams](#parameter-description). This parameter configures the Data Block size, count, interval, and other information for the copy, supporting both non-contiguous and contiguous copy. For the specific definition, see ${INSTALL_DIR}/include/ascendc/basic_api/API/kernel_struct_data_copy.h, where ${INSTALL_DIR} is the path where the CANN software is installed. |
| count | Input | Number of elements involved in the copy.<br>Note: count * sizeof(T) must be 32-byte aligned. If not aligned, the copy amount is rounded down to a multiple of 32 bytes. |

**Table 3** DataCopyParams structure parameter definition

| Parameter Name | Description |
| :--- | :--- |
| blockCount | Number of contiguous Data Blocks to be copied. Type: uint16_t. Value range: blockCount ∈ [1, 4095]. |
| blockLen | Length of each contiguous Data Block to be copied, in DataBlock (32 bytes). Type: uint16_t. Value range: blockLen ∈ [1, 65535]. |
| srcGap | Interval between adjacent contiguous Data Blocks in the source operand (the interval between the end of the preceding Data Block and the start of the following Data Block), in DataBlock (32 bytes). Type: uint16_t. srcGap must not exceed the value range of this data type. |
| dstGap | Interval between adjacent contiguous Data Blocks in the destination operand (the interval between the end of the preceding Data Block and the start of the following Data Block), in DataBlock (32 bytes). Type: uint16_t. dstGap must not exceed the value range of this data type. |

## Data Types

The supported data types are **b8**, **b16**, **b32**, and **b64**.

## Return Value

None

## Constraints

- This API is applicable only to the Mix operator scenario and supports only the configuration where the Cube core count to Vector core count ratio is **1:2**. In this scenario, **subBlockId** supports only the values **0** or **1**.

- If multiple DataCopyL1ToUB instructions need to be executed and their destination addresses overlap, call [PipeBarrier\(ISASI\)](../../sync_control/intra_core_sync/PipeBarrier_ISASI.md) to insert a synchronization instruction, ensuring that multiple **DataCopyL1ToUB** instructions are serialized to prevent abnormal data.

## Example

```cpp
// srcLocal and dstLocal are LocalTensor of the half type.
// Use the copy API with the count parameter to perform continuous copy.
AscendC::DataCopyL1ToUB(dstLocal, srcLocal, 512);
// Use the copy API with the DataCopyParams parameter, which supports continuous and non-continuous copy.
DataCopyParams intriParams;
intriParams.blockCount = 1; // The number of continuous data blocks is 1.
intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the continuous data block, in DataBlock. The length here is 512 half elements.
intriParams.srcGap = 0; // The source operand is copied continuously.
intriParams.dstGap = 0; // The destination operand is arranged continuously.
AscendC::DataCopyL1ToUB(dstLocal, srcLocal, intriParams);
```
