# SetSingleOutputShape

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-10T06:22:25.551Z pushedAt=2026-11-18T11:07:26.523Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/SetSingleOutputShape_res.md#id1 -->

## Description

Sets the shape of the result matrix Output on a single core.

The Conv3D high-level API currently supports output in M coaxial mode. In M coaxial mode, the Conv3D API internally treats Wout and Hout as the same axis. During output, it outputs along the Wout direction first, and after completing an entire row of Wout output, it proceeds to output the next row of Wout.

**Figure 1** M coaxial mode diagram  
![](../../../../figures/m_axis_merge_mode.png "M coaxial mode diagram")

## Prototype

```
__aicore__ inline void SetSingleOutputShape(uint64_t singleCo, uint64_t singleDo, uint64_t singleM)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleCo | Input | Size of the Output C dimension on a single core. |
| singleDo | Input | Size of the Output D dimension on a single core. |
| singleM | Input | Size of the Output M dimension on a single core, which is the product of the H dimension size and the W dimension size. |

## Return Value

None

## Constraints

This API currently supports setting only the C dimension, D dimension, and M dimension (that is, the dimension obtained by merging the H axis and W axis) of the Output, and does not support setting the original Output size.

## Examples

```
conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);
```
