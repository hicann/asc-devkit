# DataCopy (L1ToFixpipe-Buffer Data Transfer)<a id="ZH-CN_TOPIC_0000002569070951"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:36:12.357Z pushedAt=2026-10-26T07:41:44.073Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002538001510_section18204144912492"></a>

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
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/DataCopy_L1ToFixpipe_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002538001510_section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

DataCopy data transfer supports moving the in-band quantization parameters from the L1 Buffer to the Fixpipe Buffer. The Fixpipe Buffer contains two types of parameters: the quantization parameters of **pre_stage**, These parameters are stored independently and have independent address spaces. The quantization parameters and ReLU parameters are distinguished by the upper 16 bits of the dst address (**dst[31:16]**), as follows:

**Figure 1** Fixpipe Buffer diagram

![](../../../../figures/fixpipe_buffer_02.png)

## Prototype<a id="section_function_prototype"></a>

```cpp
// Continuous transfer scenario.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// Continuous or non-continuous transfer scenario (high-dimensional splitting).
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameters<a id="section_param_desc"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operand. For the data transfer of pre_stage quantization parameters, set it to uint64_t. For supported data types, see [data type](#section_data_type). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the LocalTensor type. It is stored in the Fixpipe Buffer, and the destination address must be 128-byte aligned. |
| src | Input | Source operand, of the LocalTensor type. It is stored in the L1 Buffer, and the source address must be 32-byte aligned. |
| repeatParams | Input | Data transfer parameters, of the DataCopyParams type. This parameter configures the size, number, and gap of the data blocks to be transferred, and supports both non-contiguous and contiguous transfer.<br>For the definition of the preceding structure parameters, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation. |
| count | Input | Number of elements involved in the transfer.<br>**Note: count * sizeof(T) must be 32-byte aligned. If it is not aligned, the transfer amount is rounded down to 32-byte alignment.** |

**Table 3** DataCopyParams structure parameter definition

| Parameter | Description |
| ---------- | ---------- |
| blockCount | Number of contiguous data blocks to be transferred. Type: uint16_t. Value range: blockCount∈[1, 4095].<br>**Note: blockCount = 0 means no transfer is performed, and this API is treated as a NOP (no operation).** |
| blockLen | Length of each contiguous data block to be transferred. When dst is in the Fixpipe Buffer, the unit is 128 bytes. Type: uint16_t. Value range: blockLen∈[1, 65535].<br>**Note: blockLen = 0 means no transfer is performed, and this API is treated as a NOP (no operation).** |
| srcGap | In the L1 Buffer -> Fixpipe Buffer scenario, srcGap refers to the gap between adjacent contiguous data blocks of the source operand (the gap between the head of the preceding data block and the head of the following data block), in the unit of DataBlock (32 bytes). Type: uint16_t. srcGap∈[0, 2^16-1].<br>&nbsp;&nbsp;&bull; When blockCount = 1, srcGap is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When blockCount ≠ 1 and srcGap = 0, the first data block of the source operand is repeatedly transferred. |
| dstGap | In the L1 Buffer -> Fixpipe Buffer scenario, dstGap refers to the gap between adjacent contiguous data blocks of the destination operand (the gap between the head of the preceding data block and the head of the following data block), in the unit of DataBlock (128 bytes). Type: uint16_t. dstGap must not exceed the value range of this data type. dstGap∈[1, 2^16-1].<br>&nbsp;&nbsp;&bull; When blockCount = 1, dstGap is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When blockCount ≠ 1 and dstGap = 0, each data block transferred into the destination matrix overwrites the first data block. |

## Data Type<a id="section_data_type"></a>

The **src** data type supports **uint64_t**, and the **dst** data type supports **uint64_t**.

## Return Value

None

## Constraints<a id="section_constraint"></a>

<!-- npu="950" id11 -->
- For the Ascend 950PR&950DT products, the unit of the **blockLen** parameter is 64B.
<!-- end id11 -->
- The dst physical memory is the Fixpipe Buffer start address, which requires 128-byte alignment. The src physical memory is the L1 Buffer, whose start address requires 32-byte alignment.
- Value range of **blockCount**: **blockCount** ∈ [1, 4095]. Setting it to 0 means no execution, and this instruction is treated as a NOP with a warning reported.
- **srcGap** ∈ [0, 2^16-1], **dstGap** ∈ [1, 2^16-1].
- If the read or write address exceeds the corresponding **mem_block**, the address is wrapped around and a warning is raised.
- If the destination **dst** address in the Fixpipe Buffer overflows, an exception is raised.

## Example<a id="section_call_example"></a>

Transfer Vector quantization parameters from L1 Buffer (C1) to Fixpipe Buffer (C2PIPE2GM).

```cpp
// c1Addr = 0, fbAddr = 0, N = 256, n = 256; burstLen = CeilAlign(256 * sizeof(uint64_t), 128) / 128 = 16, dataCopyParams = {1, 16, 0, 0}.
AscendC::LocalTensor<uint64_t> quantAlphaTensor(AscendC::TPosition::C1, c1Addr, N);

if constexpr (scenarioNum == 2 || scenarioNum == 4 || scenarioNum == 6) {
    CopyQuantAlphaGmToL1(quantAlphaTensor);
}

// vector quant mode
if constexpr (scenarioNum == 2) {
    intriParams.quantPre = QuantMode_t::VDEQF16;
} else if constexpr (scenarioNum == 4) {
    intriParams.quantPre = QuantMode_t::VQF322B8_PRE;
} else {
    intriParams.quantPre = QuantMode_t::VREQ8;
}
AscendC::LocalTensor<uint64_t> fbTensor(AscendC::TPosition::C2PIPE2GM, fbAddr, N);
uint16_t burstLen = CeilAlign(n * sizeof(uint64_t), 128) / 128;
AscendC::DataCopyParams dataCopyParams(1, burstLen, 0, 0);
AscendC::DataCopy(fbTensor, quantAlphaTensor, dataCopyParams);
AscendC::SetFixPipeConfig(fbTensor);
```

For the complete example, see scenarios 2, 4, and 6 in [DataCopy (L1->Fixpipe Buffer data transfer)](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).
