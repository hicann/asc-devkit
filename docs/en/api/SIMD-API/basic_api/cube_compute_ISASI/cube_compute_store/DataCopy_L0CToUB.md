# DataCopy (L0C Buffer to UB Data Copy)<a name="ZH-CN_TOPIC_0000002382867725"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:21:35.980Z pushedAt=2026-10-26T07:41:44.111Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

>**Note:**
> The copy function of this API is supported only on the L0C Buffer->UB (CO1 -> CO2) path of the AI Core model of Atlas inference products. For other models and other paths, the API can be called but the function does not take effect, and the function is equivalent to basic data copy.

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: not supported
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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/DataCopy_L0CToUB_res.md#id1 -->

## Description<a name="section474617392321"></a>

Enhances the data copy capability. Compared with the basic data copy API, it adds in-path computation for the L0C Buffer-\>UB path.

## Prototype<a name="section1954364615315"></a>

- Local Memory -\> Local Memory

    ```cpp
    template <typename T>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

- Local Memory -> Local Memory, supporting inconsistent data types between the source operand and the destination operand.

    ```cpp
    template <typename T, typename U>
    __aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
    ```

>**Note:**
> For the specific data paths and data types supported by each prototype, see [data type](#section8664371012).

## Parameters<a name="section1592117358563"></a>

**Table 1** Template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T, U | Data type of the operand. For supported data types, see [Data Types](#section8664371012). |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type. |
| src | Input | Source operand, of the **LocalTensor** type. |
| intriParams | Input | Data copy parameters, of the **DataCopyParams** type. |
| enhancedParams | Input | Enhanced information parameters, of the **DataCopyEnhancedParams** type.<br>For the definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3** DataCopyEnhancedParams structure parameter description

| Parameter | Description |
| ---------- | ---------- |
| blockMode | Basic fractal of data copy, of the **BlockMode** enumeration type. The following configurations are supported:<br>&nbsp;&nbsp;&bull; **BLOCK_MODE_NORMAL**: The transfer unit is 32 bytes. Currently not supported.<br>&nbsp;&nbsp;&bull; **BLOCK_MODE_MATRIX**: The transfer unit is a 16 \* 16 cube fractal.<br>&nbsp;&nbsp;&bull; **BLOCK_MODE_VECTOR**: The transfer unit is a 1 \* 16 cube fractal.<br>&nbsp;&nbsp;&bull; **BLOCK_MODE_SMALL_CHANNEL**: The transfer unit is a 16 \* 4 cube fractal. Currently not supported.<br>&nbsp;&nbsp;&bull; **BLOCK_MODE_DEPTHWISE**: The transfer unit is a 16 \* 16 cube fractal, providing the in-path channel-split function. Currently not supported.<br>For the units of **blockLen** and other parameters in each mode, see [Table 4](#table13396838183618). |
| deqScale | In-path precision conversion auxiliary parameter, that is, the quantization mode. For the supported quantization mode values and corresponding data types, see [Table 5](#table168091348673). For the DEQ, DEQ8, and DEQ16 modes, the **deqValue** quantization coefficient must be passed in, and the corresponding bits of **deqValue** must be set. For the VDEQ, VDEQ8, and VDEQ16 modes, a quantization parameter vector containing 16 elements (**deqValue**) must be passed in, the corresponding bits of **deqTensorAddr** must be set, and each element (**deqValue**) of the dequantization parameter vector stored in DEQADDR must meet the expected requirements and usage restrictions.<br>In VDEQ mode, the dequantization parameter vector is 32 bytes long (16 half elements). In other modes, the dequantization parameter vector is 128 bytes long (16 64-bit dequantization elements). |
| deqValue | Quantization coefficient. For the configuration method of **deqValue**, see [deqValue Configuration](#table54451538192912). |
| deqTensorAddr | Start address of the dequantization parameter vector stored in the Unified Buffer (UB). When **deqScale** is VDEQ/VDEQ8/VDEQ16, the address of the parameter vector used in the dequantization operation must be passed in. This address must be 32-byte aligned.<br>For VDEQ mode, this address points to a 32-byte dequantization parameter vector, where each element is 16 bits (half).<br>For VDEQ8 and VDEQ16 modes, each element in the dequantization parameter vector is 64 bits. During the copy, **blockCount** contiguous data blocks are transferred, and the length of each data block is **blockLen**. Each data block corresponds to a 128-byte dequantization vector. For the same data block, the 16 elements in the dequantization parameter vector are reused consecutively. Different data blocks correspond to different dequantization parameter vectors, and the address is offset by 12 accordingly. For example, assume that the start address is A. The start address of the dequantization parameter vector of the first data block is A, and that of the second data block is A + 12.<br>The MCB flag bit of each element in the same dequantization parameter vector must be consistent. |
| sidStoreMode | Configures the storage mode when **deqScale** is DEQ8/VDEQ8, controlling how the dequantization result is stored in the **dst** address. For the configuration effect, see [sidStoreMode Configuration Example](#fig5416115192414).<br>&nbsp;&nbsp;&bull; 0: The **dst** data is stored in the first half of each DataBlock, that is, the high 16 bytes of every 32 bytes.<br>&nbsp;&nbsp;&bull; 1: The **dst** data is stored in the second half of each DataBlock, that is, the low 16 bytes of every 32 bytes.<br>&nbsp;&nbsp;&bull; 2: The **dst** data is stored in the complete DataBlock, that is, the entire 32 bytes. |
| isRelu | Configures whether the in-path linear rectification operation can be performed. When **deqValue** is configured, if this parameter is set to **true**, the ReLU flag bit of **deqValue** is refreshed to 1; if it is set to **false**, no modification is made. When **deqTensorAddr** is configured, the ReLU flag bit in the dequantization parameter vector element does not take effect, and **isRelu** prevails.<br>When only **isRelu** is configured without a quantization parameter, that is, when **deqValue** is configured as DEQ_NONE, the supported data type combinations of **src** and **dst** are as follows: {half, half}, {float, float}, {int32_t, int32_t}, {float, half}. When both **isRelu** and a quantization parameter are configured, for the supported data type combinations, see [Table 5](#table168091348673). |
| padMode | Reserved parameter. Currently not supported. |

**Table 4** Parameter units corresponding to different blockMode values<a id="table13396838183618"></a>

| blockMode | src | dst | Data Type | blockLen Unit | srcStride Unit | dstStride Unit |
| ---------- | ---------- | ---------- | ---------- | ---------- | ---------- | ---------- |
| BLOCK_MODE_MATRIX | L0C Buffer (CO1) | UB (CO2) | half, int16_t, uint16_t | 512B | 512B | 32B |
| BLOCK_MODE_MATRIX | L0C Buffer (CO1) | UB (CO2) | float, int32_t, uint32_t | 1024B | 1024B | 32B |
| BLOCK_MODE_VECTOR | L0C Buffer (CO1) | UB (CO2) | half, int16_t, uint16_t | 32B | 512B | 32B |
| BLOCK_MODE_VECTOR | L0C Buffer (CO1) | UB (CO2) | float, int32_t, uint32_t | 64B | 1024B | 32B |

**Table 5** deqScale parameter list<a id="table168091348673"></a>

| Quantization mode | src.dtype | dst.dtype | Parameters used together |
| ---------- | ---------- | ---------- | ---------- |
| DEQ | int32_t | half | Variable M in deqValue |
| DEQ | half | half | Variable M in deqValue |
| DEQ8 | int32_t | int8_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;Variable M<br>&nbsp;&nbsp;&bull;Variable N<br>&nbsp;&nbsp;&bull; MCB flag bit<br>&nbsp;&nbsp;&bull; Offset<br>&nbsp;&nbsp;&bull; Sign flag bit<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ8 | int32_t | uint8_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;Variable M<br>&nbsp;&nbsp;&bull;Variable N<br>&nbsp;&nbsp;&bull; MCB flag bit<br>&nbsp;&nbsp;&bull; Offset<br>&nbsp;&nbsp;&bull; Sign flag bit<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ16 | int32_t | half | &bull; deqValue<br>&nbsp;&nbsp;&bull;Variable M<br>&nbsp;&nbsp;&bull;Variable N<br>&nbsp;&nbsp;&bull; MCB flag bit<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| DEQ16 | int32_t | int16_t | &bull; deqValue<br>&nbsp;&nbsp;&bull;Variable N<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ | int32_t | half | The parameters supported by the element deqValue in the dequantization parameter vector stored at the deqTensorAddr address correspond to the descriptions of DEQ/DEQ8/DEQ16 respectively.<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ8 | int32_t | int8_t | The parameters supported by the element deqValue in the dequantization parameter vector stored at the deqTensorAddr address correspond to the descriptions of DEQ/DEQ8/DEQ16 respectively.<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ8 | int32_t | uint8_t | The parameters supported by the element deqValue in the dequantization parameter vector stored at the deqTensorAddr address correspond to the descriptions of DEQ/DEQ8/DEQ16 respectively.<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ16 | int32_t | half | The parameters supported by the element deqValue in the dequantization parameter vector stored at the deqTensorAddr address correspond to the descriptions of DEQ/DEQ8/DEQ16 respectively.<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |
| VDEQ16 | int32_t | int16_t | The parameters supported by the element deqValue in the dequantization parameter vector stored at the deqTensorAddr address correspond to the descriptions of DEQ/DEQ8/DEQ16 respectively.<br>&nbsp;&nbsp;&bull; deqTensorAddr<br>&nbsp;&nbsp;&bull; DEQADDR<br>&nbsp;&nbsp;&bull; ReLU flag bit<br>&nbsp;&nbsp;&bull; isRelu |

**Table 6** deqValue configuration method<a id="table54451538192912"></a>

| Mode | Bit width | Variable name | Description |
| ---------- | ---------- | ---------- | ---------- |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 0~31 | M | The 32-bit value is treated as float and used as the value to be multiplied in the dequantization calculation. In the scenario where src is int32_t and dst is int16_t, variable M does not take effect. |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 32~35 | N | 4-bit field, representing a range of [1, 16] (b'0000 corresponds to 1, and b'1111 corresponds to 16).<br>When the mode is DEQ8 or VDEQ8 and the MCB flag bit is set to 1, the input value is right-shifted by N bits. When the mode is DEQ16 or VDEQ16 and the dst data type is int16_t, an N-bit right shift is performed directly, without being controlled by the MCB flag bit. |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 36 | MCB flag bit | Mode Control Bit. If set to 0, the input int32_t is directly converted to float. If set to 1, the input int32_t is first right-shifted by N bits, converted to int16_t, and then converted to float. |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 37~45 | Offset | 9-bit integer data. After the dequantization result of src * M is calculated, it is added to Offset. It is used only in DEQ8 and VDEQ8 modes. If offset is not used, set it to 0. |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 46 | Sign flag bit | If set to 1, the dequantization result is signed (int8); if set to 0, the dequantization result is unsigned (uint8). It is used only in DEQ8 and VDEQ8 modes. |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 47 | ReLU flag bit | If set to 1, ReLU is applied to the final result; if set to 0, no additional computation is performed.<br>&nbsp;&nbsp;&bull;For int32_t->int8_t, when ReLU is configured, offset must be configured as -128;<br>&nbsp;&nbsp;&bull;For int32_t->uint8_t, when ReLU is configured, offset must be configured as 0. |
| DEQ8, VDEQ8, DEQ16, VDEQ16 | 48~63 | - | Reserved |
| DEQ, VDEQ | 0 ~ 15 | M | These 16 bits are treated as half and used as the value to be multiplied in the dequantization calculation. |

**Figure 1** sidStoreMode configuration example<a id="fig5416115192414"></a>  

![](../../../../figures/sidstoremode_config.png "sidStoreMode configuration example")

## Data Type<a id="section8664371012"></a>

When the data types of the source operand and the destination operand are the same, the supported data types are **half**, **float**, **int32_t**, and **uint32_t**.

**Table 7** Support for different data types of the source operand and the destination operand

| data type of the source operand | data type of the destination operand |
| ---------- | ---------- |
| **float** | **half** |
| **int32_t** | **int8_t**, **uint8_t**, **int16_t**, **half**. |

## Return Value<a name="section16867144513189"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The quantization parameter cannot be inf/nan or a denormalized number.
- Developers must ensure that the **isRelu** parameter configuration in **DataCopyEnhancedParams** is consistent with the ReLU flag bit configuration of the quantization coefficient **deqValue**/quantization parameter vector **deqTensorAddr**: both enabled or both disabled.
- If in-path precision conversion is involved, the **blockLen** unit of the operand must be halved.

## Example<a name="section1491211256313"></a>

- Local Memory -\> Local Memory

    ```cpp
    // srcLocal and dstLocal are LocalTensor of the half type.
    // Use the data copy API that takes DataCopyParams parameters, which supports both contiguous and non-contiguous copies.
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // The number of contiguous data blocks is 1.
    intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the contiguous data block, in DataBlock units. Here the length is 512 half elements.
    intriParams.srcGap = 0; // Perform a contiguous copy on the source operand.
    intriParams.dstGap = 0; // The destination operand is contiguously arranged.
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX; // The transfer unit is a 16*16 cube fractal.
    enhancedParams.deqScale = DeqScale::DEQ_NONE; // No quantization is performed.
    enhancedParams.deqValue = 0;
    enhancedParams.deqTensorAddr = 0;
    enhancedParams.sidStoreMode = 0; // Takes effect only when deqScale is DEQ8/VDEQ8.
    enhancedParams.isRelu = false; // ReLU is not supported.
    enhancedParams.padMode = pad_t::PAD_NONE; // Reserved parameter.
    // Functionally equivalent to basic data movement.
    AscendC::DataCopy(dstLocal, srcLocal, intriParams, enhancedParams);
    ```

- Local Memory -\> Local Memory, where the source operand and destination operand have different types.

    ```cpp
    // The following example demonstrates data copy over the L0C Buffer -> UB path, which is supported only in the Atlas inference products AI Core scenario. srcLocal is a LocalTensor of type int32_t on the L0C Buffer, and dstLocal is a LocalTensor of type int8_t on the UB.
    // Use the data copy API that takes the DataCopyParams parameter, which supports both contiguous and non-contiguous copy.
    DataCopyParams intriParams;
    intriParams.blockCount = 1; // Number of contiguous data blocks is 1.
    intriParams.blockLen = 512 * sizeof(half) / 32; // Length of the contiguous data block, in DataBlock units. The length here is 512 half elements.
    intriParams.srcGap = 0; // Perform contiguous copy on the source operand.
    intriParams.dstGap = 0; // The destination operand is arranged contiguously.
    DataCopyEnhancedParams enhancedParams;
    enhancedParams.blockMode = BlockMode::BLOCK_MODE_MATRIX; // The transfer unit is a 16*16 cube fractal.
    enhancedParams.deqScale = DeqScale::DEQ8; // int32_t -> int8_t quantization mode, used together with deqValue.
    enhancedParams.deqValue = 70369809530880; // float 1 = uint64_t 1065353216, when s322s8 use 1065353216 | (1 << 46)
    enhancedParams.deqTensorAddr = 0; // Used only in VDEQ mode.
    enhancedParams.sidStoreMode = 2; // The data of dstLocal is stored in a complete DataBlock.
    enhancedParams.isRelu = false; // ReLU is not supported.
    enhancedParams.padMode = pad_t::PAD_NONE; // Reserved parameter.
    // At this point, dstLocal = srcLocal.
    AscendC::DataCopy(dstLocal, srcLocal, intriParams, enhancedParams);
    ```
