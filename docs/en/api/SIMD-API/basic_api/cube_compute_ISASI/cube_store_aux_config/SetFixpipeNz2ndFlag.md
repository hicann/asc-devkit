# SetFixpipeNz2ndFlag<a id="ZH-CN_TOPIC_0000002569070993"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:29:38.765Z -->

## Applicable Products<a id="zh-cn_topic_0000002516209232_section18204144912492"></a>

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
- Atlas 200I/500 A2 inference products: Supported
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
## Description<a id="zh-cn_topic_0000002516209232_section618mcpsimp"></a>

When performing in-transit format conversion (converting the NZ format to the ND format) during DataCopy data transfer ([L0C Buffer->GM](../cube_compute_store/DataCopy_L0CToGM.md)), call this API to configure the settings related to the format conversion.

## Prototype<a id="zh-cn_topic_0000002516209232_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint16_t dstNdStride)
```

<!-- npu="950" id10 -->
```cpp
// The following prototype is supported only on Ascend 950PR/Ascend 950DT.
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
```
<!-- end id10 -->

## Parameters<a id="zh-cn_topic_0000002516209232_section622mcpsimp"></a>

<!-- npu="A3,910b,310b" id11 -->
For the following product models, see the table below for parameter descriptions:
<!-- npu="A3" id12 -->
Atlas A3 training products/Atlas A3 inference products
<!-- end id12 -->
<!-- npu="910b" id13 -->
Atlas A2 training products/Atlas A2 inference products
<!-- end id13 -->
<!-- npu="310b" id14 -->
Atlas 200I/500 A2 inference products
<!-- end id14 -->
**Table 1** Parameter Description

| Parameter Name | Input/Output | Description |
| ---------- | ---------- | ---------- |
| ndNum | Input | Number of nd matrices, with a value range of ∈[1, 65535]. (For the specific meaning, see the [NZ2ND introduction feature section](../cube_store_key_features/NZ2ND.md).) |
| srcNdStride | Input | Source stride in fractal size units, that is, the offset between adjacent nz matrices in the source (head-to-head). When ndNum is greater than 1, the value range is ∈[1, 512]; when ndNum is configured as 1, srcNdStride can be configured as 0, in which case it does not take effect. The unit is 1024B. |
| dstNdStride | Input | Offset between adjacent nd matrices in the destination (head-to-head). The value range is dstNdStride∈[1, 65535], and the unit is element. |
<!-- end id11 -->

<!-- npu="950" id15 -->
For Ascend 950PR/Ascend 950DT:

**Table 2** Parameter Description

| Parameter Name | Input/Output | Description |
| ---------- | ---------- | ---------- |
| ndNum | Input | Number of nd matrices, with a value range of ∈[1, 65535]. (See [NZ2ND Introduction Feature Section](../cube_store_key_features/NZ2ND.md) for the specific meaning diagram.) |
| srcNdStride | Input | Source stride in units of fractal size, which is the offset between adjacent source nz matrices (head-to-head). When **ndNum** is configured as 1, **srcNdStride** can be configured as 0 and does not take effect. Value range: ∈[0, 65535], unit is C0_SIZE. |
| dstNdStride | Input | Offset between adjacent destination nd matrices (head-to-head). Value range: dstNdStride∈[1, 2^32-1], unit is element. |
<!-- end id15 -->

## Data Types

For the data types of the parameters, see [Prototype](#zh-cn_topic_0000002516209232_section620mcpsimp).

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="zh-cn_topic_0000002516209232_section633mcpsimp"></a>

- Before calling the **DataCopy** API and performing in-band NZ-to-ND format conversion, you must call **SetFixpipeNz2ndFlag** to configure the format conversion settings.

## Example<a id="section6461234123118"></a>

For the complete example, see [data_copy_l0c2gm](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

```cpp
uint16_t ndNum = 2;
uint16_t srcNdStride = 2;
uint16_t dstNdStride = 1;
AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride); // Set the parameters for converting NZ format to ND format during FIX data movement.
```
