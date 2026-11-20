# SetMMColumnMajor/SetMMRowMajor

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T04:41:20.135Z pushedAt=2026-11-18T01:25:06.811Z nl=2 -->

## Applicable Products

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/mmad_compute_aux_config/SetMMColumnMajor-SetMMRowMajor_res.md#id1 -->

## Description

Controls which direction of M/N is prioritized by Mmad/MmadWithSparse.

## Function Prototype

```cpp
__aicore__ inline void SetMMColumnMajor() // Indicates that CUBE generates results by traversing the M direction first and then the N direction.
__aicore__ inline void SetMMRowMajor()   // Indicates that CUBE generates results by traversing the N direction first and then the M direction.
```

## Parameters

None

## Return Value

None

## Constraints

None

## Example

```cpp
AscendC::SetMMRowMajor();
```
