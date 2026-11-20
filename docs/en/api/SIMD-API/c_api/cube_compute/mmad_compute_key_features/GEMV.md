---
okftype: reference.concept
description: "Introduces the GEMV feature automatically enabled for cube multiplication and accumulation when M equals 1: it degrades the multiplication of a 1-by-K row vector and a K-by-N cube into a vectorized operation, reducing data movement overhead compared with non-aligned processing. It also explains the constraints that the 1-by-K cube A must be 512-byte address-aligned with K contiguous data elements, and that the C cube occupies 1024 bytes per 1-by-16 sub-vector while only 64 bytes are valid data. It then shows how to disable this feature using the disable_gemv parameter of asc_mmad on Ascend 950PR&950DT products."
---
# GEMV

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T10:19:52.420Z pushedAt=2026-11-18T08:15:34.234Z nl=2 -->

**Feature Description:**

The core function of GEMV is as follows: when the dimension M of cube A is 1, the interface automatically enables the GEMV feature, and the operation degenerates into a multiplication between a 1×K row vector and a K×N cube.

<!-- npu="950" id1 -->
For the Ascend 950PR&950DT products, you can disable this feature by setting the disable_gemv parameter of asc_mmad to true.
<!-- end id1 -->

For computation parameters, see [asc_mmad](../mmad_compute/asc_mmad.md); for the input vector load interface, see [asc_copy_l12l0a](../../cube_datamove/cube_compute_load/asc_copy_l12l0a.md); for the organization of the result storage space, see [L0C Buffer Memory Structure Introduction](../../cube_datamove/cube_compute_store/L0C_memory_structure_intro.md).

**Feature Constraints:**

1. The 1×K cube A must satisfy 512B address alignment, and the K data elements must be stored contiguously. Taking the half type as an example, when K=256, the software side can regard it as a 16\*16 block; after configuring m=1, the hardware parses it as a 1\*256 vector, and the 256 half data elements can be moved from the L1 Buffer to the L0A Buffer through the asc_copy_l12l0a interface.

2. Cube C is a (1\*N) vector. When the 1×N vector is divided into multiple 1×16 sub-vectors, each sub-vector occupies 1024B in the L0C Buffer, but the actual valid data occupies only the lowest 64B. For example, when N=50, it is divided into 4 1\*16 sub-vectors, occupying 4\*1024B = 4096B, where each sub-vector can hold 64B and the four blocks can hold 256B; when N=50, the logical valid data is 50×4B=200B.

**Figure 1** GEMV mode, cube multiplication illustration

![GEMV mode - cube multiplication illustration](../../../../figures/mmad_gemv.png "GEMV mode - cube multiplication illustration")

**Advantages:**

When GEMV mode is enabled, the M dimension of cube multiplication is processed as a non-aligned scenario. Compared with the non-aligned processing method, GEMV mode moves less data, reducing the data movement overhead. The following uses a cube multiplication example with M=1, K=256, N=32, and half data type for both left and right cubes for illustration.

- GEMV mode

    When moving cube A from L1 Buffer to L0A Buffer, the 1\*256 vector is processed as a 16\*16 cube, and the asc_copy_l12l0a interface is called once to complete the cube movement of 16\*16 fractal size. The movement of cube B and the cube multiplication computation are the same as in the basic scenario, as shown in the following figure.

    **Figure 2** Computation illustration of cube multiplication with M=1 in GEMV mode

    ![Computation illustration of cube multiplication with M=1 in GEMV mode](../../../../figures/mmad_gemv_compare.png "Computation illustration of cube multiplication with M=1 in GEMV mode")

- Non-GEMV mode

    When moving cube A from L1 Buffer to L0A Buffer, the 1\*256 vector is processed as non-aligned cube data, and the M dimension is aligned to 32 bytes before movement. In the figure, K/16=16 input fractals need to be covered, and the amount of data moved increases accordingly. The number of fractals is not equal to the number of C API calls; the number of calls must be determined based on the stride and repeat parameters of the selected movement interface, as shown in the following figure.

    **Figure 3** Computation illustration of cube multiplication with M=1 in non-GEMV mode

    ![Computation illustration of cube multiplication with M=1 in non-GEMV mode](../../../../figures/mmad_nongemv_compare.png "Computation illustration of cube multiplication with M=1 in non-GEMV mode")

**Usage example:**

**Table 1** Example configuration description

| Cube | Dimension size | Data type |
| --- | --- | --- |
| A | 1 * 4096 | half |
| B | 4096 * 256 | half |
| C | 1 * 256 | float |

Cube A is a vector of 1\*4096, which is moved to the L0A Buffer through asc_copy_l12l0a. Cube C is a vector of 1\* 256, which can be divided into 16 sub-vectors of 1 \* 16, occupying 16 \* 1024B = 16384B\(f162f32\), of which only 16\*64B = 1024B is valid data.

**Figure 4** GEMV mode cube multiplication illustration

![GEMV mode - cube multiplication illustration - demo](../../../../figures/mmad_gemv_demo.png "GEMV mode - cube multiplication illustration - demo")

```cpp
// m = 1, enable the GEMV capability.
asc_mmad(c, a, b, 1, k, n, asc_unit_flag_mode::DISABLE,
         false, false, true);
```

<!-- npu="950" id3 -->
For the Ascend 950PR&950DT products, you can disable this feature by setting the disable_gemv parameter of asc_mmad to true. The example is as follows:

```cpp
asc_mmad(c, a, b, 1, k, n, asc_unit_flag_mode::DISABLE,
         true /* disable_gemv */, false, true);
```
<!-- end id3 -->
