# SetTensorScaleA

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T07:05:22.565Z pushedAt=2026-09-12T09:55:18.152Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Not supported

- Atlas A2 training products/Atlas A2 inference products: Not supported

- Atlas 200I/500 A2 inference products: Not supported

- AI Core of Atlas inference products: Not supported

- Vector Core of Atlas inference products: Not supported

- Atlas training products: Not supported

## Description

In the MxMatmul scenario, sets the quantization coefficient matrix scaleA of the left matrix in matrix multiplication.

## Prototype

```
__aicore__ inline void SetTensorScaleA(const GlobalTensor<ScaleT>& gm, bool isTransposeScaleA = false);
```

```
__aicore__ inline void SetTensorScaleA(const LocalTensor<ScaleT>& leftMatrix, bool isTransposeScaleA = false);
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| gm | Input | The quantization coefficient scaleA matrix. Its type is [GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)

. Supported data type: fp8_e8m0_t. |

| leftMatrix | Input | The quantization coefficient scaleA matrix. Its type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)

, with supported TPosition of TSCM/VECOUT. Supported data type: fp8_e8m0_t. |

| isTransposeScaleA | Input | Whether the scaleA matrix needs to be transposed.<br><br>Supported values are as follows:<br>false: default value, the scaleA matrix is not transposed. true: the scaleA matrix is transposed.<br><br>Note:<br>When the scaleA matrix is in NZ format, this parameter only supports the value false.<br>If SCALE_ISTRANS in the MatmulTypeWithScale parameter of the scaleA matrix is set to true, except for the NZ-format scenario, this parameter supports both true and false, meaning the scaleA matrix can be used interchangeably in transposed and non-transposed forms at runtime.<br>If SCALE_ISTRANS in the MatmulTypeWithScale parameter of the scaleA matrix is set to false, this parameter only supports false; forcibly setting it to true will cause precision anomalies.<br><br>For scenarios with Bias input, to ensure the L1 Buffer space calculation on the Tiling side is consistent with that on the Kernel side and the result precision is correct, this parameter value must be consistent with the SCALE_ISTRANS parameter of MatmulTypeWithScale that defines matrix A on the Kernel side and the [isScaleTrans](../Matmul-Tiling/SetScaleAType.md) parameter of the SetScaleAType() interface on the Tiling side. That is, in scenarios with Bias input, the above three parameters must all be set to true or all set to false. |

## Return Value

None

## Constraints