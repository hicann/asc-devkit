# SetFixpipePreQuantFlag<a name="ZH-CN_TOPIC_0000002538071284"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:07:13.624Z pushedAt=2026-10-26T07:41:44.159Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002516049318_section18204144912492"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixpipePreQuantFlag_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002516049318_section618mcpsimp"></a>

When in-path quantization is performed during DataCopy data transfer ([L0C Buffer->GM](../cube_compute_store/Fixpipe_L0CToGM.md), [L0C Buffer->L1 Buffer](../cube_compute_store/Fixpipe_L0CToL1.md)), call this API to set the scalar quantization parameter.

## Function Prototype<a name="zh-cn_topic_0000002516049318_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipePreQuantFlag(uint64_t config)
```

## Parameter Description<a name="zh-cn_topic_0000002516049318_section622mcpsimp"></a>

**Table 1** Parameter description

| parameter name | input/output | meaning |
| ---------- | ---------- | ---------- |
| config | input | Scalar quantization parameter used in the quantization process. |

## Data Type

The data type of the parameter is **uint64_t**.

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000002516049318_section633mcpsimp"></a>

The quantization parameter cannot be inf/nan or a denormalized number.

## Example<a name="section6461234123118"></a>

For the complete example, see [data_copy_l0c2gm](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

```cpp
float tmp = (float)0.5;
// Convert the float tmp to uint64_t deqScalar.
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp)); 
AscendC::SetFixpipePreQuantFlag(deqScalar);  // Set the quantization parameter.
AscendC::PipeBarrier<PIPE_FIX>();
```
