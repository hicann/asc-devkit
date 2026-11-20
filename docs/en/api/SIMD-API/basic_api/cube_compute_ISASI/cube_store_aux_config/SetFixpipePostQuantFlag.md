# SetFixpipePostQuantFlag<a name="ZH-CN_TOPIC_0000002738071285"></a>

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-19T06:05:43.579Z pushedAt=2026-11-19T06:14:22.738Z nl=2 -->

## Supported Products<a name="section18204144912492"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
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
<!-- npu="x90" id8 -->
- KirinX90 processor products: not supported
<!-- end id8 -->
<!-- npu="9020" id9 -->
- Kirin9020 processor products: not supported
<!-- end id9 -->
<!-- npu="9030" id10 -->
- Kirin9030 processor products: not supported
<!-- end id10 -->
<!-- npu="9050" id11 -->
- Kirin9050 processor products: supported
<!-- end id11 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixpipePostQuantFlag_res.md#id1 -->

## Function Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_macro_intf.h"`.

During MacroCube computation (such as [MmadFix](../mmad_compute/MmadFix.md) and ConvFix), when the post-quantization (PostQuant) post-processing function is enabled, call this API to set the scalar parameters for post-quantization.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFixpipePostQuantFlag(uint64_t config)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameter description

| Parameter | Input/Output | Meaning |
| ---------- | ---------- | ---------- |
| config | Input | Scalar quantization parameters used in the post-quantization process. |

## Data Type

The data type of the parameter is uint64\_t.

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- The quantization parameters cannot be inf/nan or denormalized numbers.
- This API must be used together with MacroCube series computation APIs (such as [MmadFix](../mmad_compute/MmadFix.md) and ConvFix), and must be called before the MacroCube computation API.
- The registers involved in this section maintain their state within the same kernel function (Kernel) until they are manually changed, and do not inherit the previous state across different kernel functions (Kernel).

## Examples<a name="section6461234123118"></a>

```cpp
float tmp = (float)0.5;
// Convert the float tmp to uint64_t postQuantScalar.
uint64_t postQuantScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t*>(&tmp));
AscendC::SetFixpipePostQuantFlag(postQuantScalar);  // Set the post-quantization parameters.
AscendC::PipeBarrier<PIPE_FIX>();
```
