# SetSingleOutputShape

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:08:18.590Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Not supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Sets the shape of the result matrix **Output** on a single core.

The Conv3D high-level API currently supports the M axis-aligned output mode. In the M axis-aligned mode, the Conv3D API internally treats **Wout** and **Hout** as the same axis. During output, it first outputs along the **Wout** direction, and after completing an entire row of **Wout**, it proceeds to output the next row of **Wout**.

**Figure 1** M axis-aligned mode diagram  
![](../../../../figures/m_axis_aligned_mode_diagram.png "M axis-aligned mode diagram")

## Prototype

```
__aicore__ inline void SetSingleOutputShape(uint64_t singleCo, uint64_t singleDo, uint64_t singleM)
```

## Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **singleCo** | Input | C dimension size of **Output** on a single core. |
| **singleDo** | Input | D dimension size of **Output** on a single core. |
| **singleM** | Input | M dimension size of **Output** on a single core, that is, the product of the H dimension size and the W dimension size. |

## Return Value

None

## Constraints

This API currently supports setting only the C dimension, D dimension, and M dimension (that is, the dimension after merging the H axis and W axis) of **Output**, and does not support setting the size of the original **Output**.

## Examples

```
conv3dApi.SetSingleOutputShape(singleCoreCout, singleCoreDout, singleCoreM);
```
