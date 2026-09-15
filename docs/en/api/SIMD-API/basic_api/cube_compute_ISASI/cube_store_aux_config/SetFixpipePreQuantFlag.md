# SetFixpipePreQuantFlag<a name="ZH-CN_TOPIC_0000002538071284"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T15:10:45.511Z -->

## Applicable Products<a name="zh-cn_topic_0000002516049318_section18204144912492"></a>

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="zh-cn_topic_0000002516049318_section618mcpsimp"></a>

During DataCopy data transfer ([L0C Buffer->GM](../cube_compute_store/DataCopy_L0CToGM.md), [L0C Buffer->L1 Buffer](../cube_compute_store/DataCopy_L0CToL1.md)) with in-line quantization, call this API to set the scalar quantization parameters.

## Prototype<a name="zh-cn_topic_0000002516049318_section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipePreQuantFlag(uint64_t config)
```

## Parameters<a name="zh-cn_topic_0000002516049318_section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| **config** | Input | Scalar quantization parameters used in the quantization process. |

## Data Type

The data type of the parameter is **uint64_t**.

## Return Value Description<a name="section640mcpsimp"></a>

None

## Constraints<a name="zh-cn_topic_0000002516049318_section633mcpsimp"></a>

None

## Example<a name="section6461234123118"></a>

For a complete example, see [data_copy_l0c2gm](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_l0c2gm).

```cpp
float tmp = (float)0.5;
// Convert the float tmp to uint64_t deqScalar.
uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp)); 
AscendC::SetFixpipePreQuantFlag(deqScalar);  // Set the quantization parameter.
AscendC::PipeBarrier<PIPE_FIX>();
```