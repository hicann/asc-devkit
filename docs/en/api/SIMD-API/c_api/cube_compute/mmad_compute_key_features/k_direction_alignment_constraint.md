---
okftype: reference.concept
description: "Introduces the K direction alignment constraint feature of asc_mmad: for the float data type, k_direction_align controls the alignment of the L0A and L0B cubes in the K direction, aligning to 16 when true and to 8 when false. For the A cube, it does not need to be enabled in the non-transposed scenario but must be enabled in the transposed scenario. This parameter is only provided as an overload in the Atlas A2 and A3 products. For Ascend 950PR&950DT, it does not need to be enabled because L0A uses Nz layout and is aligned to 8 in the K direction."
---
# K Direction Alignment Constraint

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:21:58.508Z pushedAt=2026-11-18T08:15:34.235Z nl=2 -->

**Feature Description:**

<!-- npu="910b,A3" id1 -->
On Atlas A2 products/Atlas A3 products, the asc_mmad interface supports enabling this feature.
<!-- end id1 -->

<!-- npu="950" id2 -->
On Ascend 950PR&950DT products, the L0A Buffer uses Nz layout instead of Zz layout. In the scenario where the A cube is transposed and non-aligned with the float data type, the L0A Buffer is 8-aligned in the K direction, so there is no need to enable this feature to ensure computation correctness.
<!-- end id2 -->

<!-- npu="910b,A3" id3 -->
The core function of K direction alignment is to control, through the k_direction_align parameter, the alignment of the L0A Buffer and L0B Buffer cubes in the K direction when using the float data type (other data types do not need to consider this feature).

When k_direction_align = true is set, the cubes in the L0A Buffer/L0B Buffer align the K direction to `ceil\(K/16\)\*16`.

![K direction alignment schematic diagram](../../../../figures/mmad_kdirectionalign_c_api.png)

For example, when K=44 and K=36, both are aligned to 48. In this case, for K=44, all 12 fractal blocks in the L0A Buffer/L0B Buffer are fully read into the CUBE; whereas for K=36, although it is also aligned to 48, only 10 of the fractal blocks are read into the CUBE.

![Schematic diagram of K direction alignment to 48](../../../../figures/kdirectionalign_k_is36.png)

When `k_direction_align = false` is set, the cubes in the L0A Buffer/L0B Buffer align the K direction to `ceil\(K/8\)\*8`.

For example, when K=44, it is aligned to 48, and when K=36, it is aligned to 40. In this case, for K=44, all 12 fractal blocks in the L0A Buffer/L0B Buffer are fully read into the CUBE; for K=36, all 10 fractal blocks in the L0A Buffer/L0B Buffer are read into the CUBE.

![Schematic diagram of K direction alignment to 40](../../../../figures/kdirectionalign_k_is36_false.png)

1. In the non-transposed A cube scenario, there is no need to enable k_direction_align.

2. In the transposed A cube scenario, k_direction_align needs to be enabled.

When the input requires a transpose operation, for example, the original cube A has dimensions K×M. After being moved into the L1 Buffer, its storage form becomes Nz layout (essentially still K×M). In this case, the cube move-in interface with transpose capability should be called. The internal implementation of this interface is shown in the following figure: 16-element alignment is performed in the K direction (as shown in the left figure). After the transpose is completed, the data appears in the Zz fractal form shown in the right figure and is stored in the L0A Buffer. To avoid reading invalid data, you can enable the k_direction_align parameter to read only the valid fractal data.

![Schematic diagram of K direction alignment transpose](../../../../figures/kdirectionalign_demo.png)

<!-- npu="A3,910b" id4 -->
This parameter is provided in the corresponding overloads of [asc_mmad](../mmad_compute/asc_mmad.md) for the Atlas A3 products and the Atlas A2 products. For the fractal layout of cubes A and B, see [Key Fractal Format Details](../cube_compute_fractal_intro/key_fractal_format_details.md). For transposed move-in, see [asc_copy_l12l0a_trans](../../cube_datamove/cube_compute_load/asc_copy_l12l0a_trans.md).
<!-- end id4 -->

**Usage example:**

```cpp
asc_mmad(c, a, b, m, k, n, asc_unit_flag_mode::DISABLE,
         true /* k_direction_align */, false, true);
```
<!-- end id3 -->
