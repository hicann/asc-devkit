# SetSparseIndex

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:01:16.447Z pushedAt=2026-09-12T09:55:18.140Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
- Atlas training products: Not supported

## Description

Sets the index matrix generated during the [densification process](../../../basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md#description) of a sparse matrix.

For the role of the index matrix in densification, see [MmadWithSparse](../../../basic_api/cube_compute_ISASI/mmad_compute/MmadWithSparse.md).

## Prototype

```
__aicore__ inline void SetSparseIndex(const GlobalTensor<uint8_t>& indexGlobal)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| indexGlobal | Input | Start address of the index matrix in Global Memory, of the GlobalTensor type.<br><br>The data type of the index matrix is uint2, which must be assembled by you into the uint8 data type before being passed to this API. The Format of the index matrix supports only the NZ format. |

## Return Value

None

## Constraints

-   The Format of the index matrix must be NZ format.
-   This API can be used only in pure Cube mode (matrix computation only) with the [MDL template](MatmulConfig.md#table6981133810309).

## Examples

```
#define ASCENDC_CUBE_ONLY // Enable the pure Cube mode (matrix computation only).ube mode (matrix computation only).
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetSparseIndex(gm_index); //Set the index matrix.
mm.SetBias(gm_bias);
mm.IterateAll(gm_c);
```
