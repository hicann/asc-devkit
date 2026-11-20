# SetSparseIndex

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-02T06:49:54.894Z pushedAt=2026-11-18T04:54:01.547Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Not supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetSparseIndex_res.md#id1 -->

## Description

Sets the index cube generated during the [densification process](../../../basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md#description) of the sparse cube.

For the role of the index cube in densification, see [MmadWithSparse](../../../basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md#section618mcpsimp).

## Prototype

```
__aicore__ inline void SetSparseIndex(const GlobalTensor<uint8_t>& indexGlobal)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| indexGlobal | Input | First address of the index cube in Global Memory, of type GlobalTensor.<br><br>The data type of the index cube is uint2, which must be assembled by the user into the uint8 data type before being passed to this API. The Format of the index cube supports only the NZ format. |

## Return Value

None

## Constraints

-   The Format of the index cube must be the NZ format.
-   This API can be used only in pure Cube mode (cube computation only) with the [MDL template](MatmulConfig.md#mdl-template).

## Examples

```
#define ASCENDC_CUBE_ONLY // Enable pure Cube mode (cube computation only).
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetSparseIndex(gm_index); // Set the index cube.
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
