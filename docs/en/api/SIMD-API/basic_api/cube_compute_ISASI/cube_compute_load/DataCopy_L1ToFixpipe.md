# DataCopy (L1ToFixpipe-Buffer Data Movement)<a id="ZH-CN_TOPIC_0000002569070951"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:10:51.022Z -->

## Applicable Products<a id="zh-cn_topic_0000002538001510_section18204144912492"></a>

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a id="zh-cn_topic_0000002538001510_section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

DataCopy data movement supports moving the accompanying quantization parameters from the L1 Buffer to the Fixpipe Buffer. The Fixpipe Buffer contains two types of parameters: the quantization parameters of pre_stage, which are stored independently and have independent address spaces. The quantization parameters and ReLU parameters are distinguished by the upper 16 bits of the dst address (dst[31:16]), as follows:

**Figure 1** Fixpipe Buffer diagram

![](../../../../figures/fixpipe_buffer_02.png)

## Prototype<a id="section_function_prototype"></a>

```cpp
// Continuous data movement scenario.
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)

// Continuous or non-continuous data movement scenario (high-dimensional splitting).
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
```

## Parameters<a id="section_param_desc"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the operand. For moving the quantization parameters in the pre_stage, set it to **uint64_t**. For supported data types, see [data type](#section_data_type). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type. It is stored in the Fixpipe Buffer, and the destination address must be 128-byte aligned. |
| src | Input | Source operand, of the **LocalTensor** type. It is stored in the L1 Buffer, and the source address must be 32-byte aligned. |
| repeatParams | Input | Movement parameters, of the **DataCopyParams** type. This parameter configures the size, number, and interval of the data blocks to be moved, and supports both non-contiguous and contiguous movement.<br>For the definition of the preceding structure parameters, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_data\_copy.h. Replace \$\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. |
| count | Input | Number of elements involved in the movement. count * sizeof(T) must be 32-byte aligned. If it is not aligned, the movement amount is rounded down to 32-byte alignment. |

**Table 3** DataCopyParams structure parameter definition

| Parameter | Description |
| ---------- | ---------- |
| blockCount | Number of contiguous data blocks to be moved. Type: **uint16_t**. Value range: blockCount∈[1, 4095].<br>**Note: blockCount = 0 means no movement is performed, and this API is treated as a NOP (no operation).** |
| blockLen | Length of each contiguous data block to be moved. When dst is in the Fixpipe Buffer, the unit is 128 bytes. Type: **uint16_t**. Value range: blockLen∈[1, 65535].<br>**Note: blockLen = 0 means no movement is performed, and this API is treated as a NOP (no operation).** |
| srcGap | In the L1 Buffer -> Fixpipe Buffer scenario, srcGap refers to the interval between adjacent contiguous data blocks of the source operand (the interval between the head of the preceding data block and the head of the following data block), in the unit of DataBlock (32 bytes). Type: **uint16_t**. srcGap∈[0, 2^16-1].<br>&nbsp;&nbsp;&bull; When blockCount = 1, srcGap is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When blockCount ≠ 1 and srcGap = 0, the first data block of the source operand is moved repeatedly. |
| dstGap | In the L1 Buffer -> Fixpipe Buffer scenario, dstGap refers to the interval between adjacent contiguous data blocks of the destination operand (the interval between the head of the preceding data block and the head of the following data block), in the unit of DataBlock (128 bytes). Type: **uint16_t**. dstGap must not exceed the value range of this data type. dstGap∈[1, 2^16-1].<br>&nbsp;&nbsp;&bull; When blockCount = 1, dstGap is meaningless and can be set to 0.<br>&nbsp;&nbsp;&bull; When blockCount ≠ 1 and dstGap = 0, each data block moved into the destination cube overwrites the first data block. |

## Data Type<a id="section_data_type"></a>

The **src** data type supports **uint64_t**, and the **dst** data type supports **uint64_t**.

## Return Value

None

## Constraints<a id="section_constraint"></a>

<!-- npu="950" id11 -->
- For Ascend 950PR/Ascend 950DT, the unit of the **blockLen** parameter is 64B.
<!-- end id11 -->
- The **dst** physical memory is the Fixpipe Buffer, whose start address must be 128-byte aligned. The **src** physical memory is the L1 Buffer, whose start address must be 32-byte aligned.
- Value range of **blockCount**: **blockCount** ∈ [1, 4095]. Setting it to 0 means no execution, in which case this instruction is treated as a NOP and a warning is reported.
- **srcGap** ∈ [0, 2^16-1], and **dstGap** ∈ [1, 2^16-1].
- If the read/write address exceeds the corresponding mem_block, the address is wrapped around and a warning is raised.
- If the destination **dst** address in the Fixpipe Buffer overflows, an exception is raised.

## Example<a id="section_call_example"></a>

**L1 Buffer** (C1) -> **Fixpipe Buffer** (C2PIPE2GM), moving Vector quantization parameters.

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

For the complete example, see scenarios 2, 4, and 6 in [L1->Fixpipe Buffer Data Movement (DataCopy)](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).
