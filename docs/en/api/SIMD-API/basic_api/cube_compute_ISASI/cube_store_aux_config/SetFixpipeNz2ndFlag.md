# SetFixpipeNz2ndFlag<a id="ZH-CN_TOPIC_0000002569070993"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:05:53.676Z pushedAt=2026-10-26T07:41:44.158Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002516209232_section18204144912492"></a>

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
- Atlas 200I/500 A2 inference products: supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixpipeNz2ndFlag_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002516209232_section618mcpsimp"></a>

During DataCopy data transfer ([L0C Buffer->GM](../cube_compute_store/Fixpipe_L0CToGM.md)), when inline format conversion (converting NZ format to ND format) is performed, call this API to set the related configuration for format conversion.

## Prototype<a id="zh-cn_topic_0000002516209232_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint16_t dstNdStride)
```

<!-- npu="950" id10 -->
```cpp
// The following prototype is supported only by the Ascend 950PR&950DT products.
__aicore__ inline void SetFixpipeNz2ndFlag(uint16_t ndNum, uint16_t srcNdStride, uint32_t dstNdStride)
```
<!-- end id10 -->

## Parameters<a id="zh-cn_topic_0000002516209232_section622mcpsimp"></a>

<!-- npu="A3,910b,310b" id11 -->
For the following product models, see the following table for parameter descriptions:
<!-- npu="A3" id12 -->
Atlas A3 products
<!-- end id12 -->
<!-- npu="910b" id13 -->
Atlas A2 products
<!-- end id13 -->
<!-- npu="310b" id14 -->
Atlas 200I/500 A2 inference products
<!-- end id14 -->
**Table 1** Parameter Description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| **ndNum** | Input | Number of nd matrices, with a value range of [1, 65535]. (See the [NZ2ND feature section](../cube_store_key_features/NZ2ND.md) for the specific meaning illustration.) |
| **srcNdStride** | Input | Source stride in units of fractal size, the offset between adjacent source nz matrices (head-to-head). When **ndNum** is greater than 1, the value range is [1, 512]; when **ndNum** is configured as 1, **srcNdStride** can be configured as 0, in which case it does not take effect. The unit is 1024B. |
| **dstNdStride** | Input | Offset between adjacent destination nd matrices (head-to-head). The value range is [1, 65535], and the unit is elements. |

<!-- end id11 -->

<!-- npu="950" id15 -->
For Ascend 950PR&950DT products:

**Table 2** Parameter Description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| **ndNum** | Input | Number of nd matrices, with a value range of [1, 65535]. (See the [NZ2ND feature section](../cube_store_key_features/NZ2ND.md) for the specific meaning illustration.) |
| **srcNdStride** | Input | Source stride in units of fractal size, the offset between adjacent source nz matrices (head-to-head). When **ndNum** is configured as 1, **srcNdStride** can be configured as 0, in which case it does not take effect. The value range is [0, 65535], and the unit is C0_SIZE. |
| **dstNdStride** | Input | Offset between adjacent destination nd matrices (head-to-head). The value range is **dstNdStride** ∈ [1, 2^32-1], and the unit is element. |

<!-- end id15 -->

## Data Types

For the data types of the parameters, see [Prototype](#zh-cn_topic_0000002516209232_section620mcpsimp).

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="zh-cn_topic_0000002516209232_section633mcpsimp"></a>

- Before calling the **DataCopy** API and performing inline NZ-to-ND format conversion, you must call **SetFixpipeNz2ndFlag** to set the format conversion configuration.

## Example<a id="section6461234123118"></a>

For the complete example, see [data_copy_l0c2gm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

```cpp
uint16_t ndNum = 2;
uint16_t srcNdStride = 2;
uint16_t dstNdStride = 1;
AscendC::SetFixpipeNz2ndFlag(ndNum, srcNdStride, dstNdStride); // Set the parameters for converting NZ format to ND format during FIX data movement.
```
