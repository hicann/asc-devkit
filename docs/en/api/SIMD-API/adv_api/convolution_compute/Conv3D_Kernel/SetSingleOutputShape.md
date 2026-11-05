# SetSingleOutputShape

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:16:29.714Z pushedAt=2026-10-29T02:14:34.172Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/convolution_compute/Conv3D_Kernel/SetSingleOutputShape_res.md#id1 -->

## Description

Sets the shape of the result matrix Output on a single core.

The Conv3D high-level API currently supports the M-axis merge mode for output. In M-axis merge mode, the Conv3D API internally treats Wout and Hout as the same axis. During output, it first outputs along the Wout direction, and after completing an entire row of Wout, it proceeds to output the next row of Wout.

**Figure 1**  M axis-merge mode diagram  
![](../../../../figures/m_axis_merge_mode.png "M axis-merge mode diagram")

## Prototype

```
__aicore__ inline void SetSingleOutputShape(uint64_t singleCo, uint64_t singleDo, uint64_t singleM)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| singleCo | Input | C dimension size of Output on a single core. |
| singleDo | Input | D dimension size of Output on a single core. |
| singleM | Input | M dimension size of Output on a single core, that is, the product of the H dimension size and the W dimension size. |

## Return Value

None

## Constraints

This API currently supports setting only the C dimension, D dimension, and M dimension (that is, the dimension obtained by merging the H axis and W axis) of Output. It does not support setting the size of the original Output.

## Examples

```
conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);
```
