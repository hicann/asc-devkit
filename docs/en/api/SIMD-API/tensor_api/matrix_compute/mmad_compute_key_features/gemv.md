# GEMV

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-27T10:46:24.221Z pushedAt=2026-10-27T13:04:46.145Z nl=2 -->

## Description

The core function of GEMV is as follows: when the dimension M of cube A is 1, the API can manually enable the GEMV function, and this operation degenerates into a multiplication between a 1×K row vector and a K×N cube.

<!-- npu="950" id1 -->
For Ascend 950PR&950DT products, the GEMV mode is disabled by default. To enable the GEMV mode, set the `mmad_trait::disable_gemv` parameter to `false`.
<!-- end id1 -->

## Constraints

- The 1×K cube A must satisfy 512B address alignment, and the K data elements are stored contiguously. Taking the `half` type as an example, when K=256, the software side can view it as 16×16 blocks. After configuring m=1, the hardware parses it as a 1×256 vector, and the 256 `half` data elements can be moved from the L1 Buffer to the L0A Buffer through the `copy` API.

- C cube is a 1×N vector. When the 1×N vector is divided into multiple 1×16 sub-vectors, each sub-vector occupies 1024B in the L0C Buffer. However, the actual valid data occupies only the lowest 32B or 64B. For example, when N=50, a total of 4 1×16 sub-vectors are obtained, occupying 4×512B=2048B, of which the valid data is only 4×32B=128B.

**Figure 1**  Schematic diagram of cube multiplication in GEMV mode

![Schematic diagram of cube multiplication in GEMV mode](../../../../figures/mmad_gemv.png)

## Advantages

When GEMV mode is enabled with M=1, cube multiplication processes the M dimension as a non-aligned scenario. Compared with the non-aligned processing method, GEMV mode moves less data and delivers better performance. The following uses a cube multiplication example with M=1, K=256, and N=32, where the data type of both the left and right cubes is `half`, for illustration.

- GEMV mode

    When moving cube A from the L1 Buffer to the L0A Buffer, the 1×256 vector is processed as a 16×16 cube, and the `copy` API is called once to complete the cube move of a 16×16 fractal size. The move of cube B and the cube multiplication computation are the same as in the basic scenario, as shown in the following figure.

    **Figure 2** Computation schematic diagram of cube multiplication in GEMV mode with M=1

    ![Computation schematic of cube multiplication in GEMV mode with M=1](../../../../figures/mmad_gemv_compare.png)

- Non-GEMV mode

    When moving cube A from the L1 Buffer to the L0A Buffer, the 1×256 vector is processed as non-aligned cube data, and the M dimension is aligned to 32 bytes before the move. The `copy` API is called to move a 16×16 fractal-sized cube each time, for a total of K/16=16 moves, which increases the amount of data moved and results in worse performance than GEMV mode, as shown in the following figure.

    **Figure 3** Computation schematic of cube multiplication in non-GEMV mode with M not equal to 1

    ![Computation schematic of cube multiplication in non-GEMV mode with M not equal to 1](../../../../figures/mmad_nongemv_compare.png)

## Example

The specifications of this GEMV scenario example are as follows:

**Table 1** Example description

| Cube | Dimension | Data Type | Format |
| --- | --- | --- | --- |
| A | 1×4096 | half | ND |
| B | 4096×256 | half | NZ |
| C | 1×256 | float | ND |

Use the [copy](../cube_compute_load/copy_gm_to_l1.md) API to move input cube A from Global Memory to the L1 Buffer while keeping the ND format unchanged. Then convert cube A to the NZ format in the L1 Buffer, and use the [copy](../cube_compute_load/copy_l1_to_l0a.md) API to move it to the L0A Buffer, thereby performing cube multiplication in GEMV mode. Note that GEMV mode is disabled by default and must be enabled by customizing [mmad_trait](../mmad_compute/mmad.md).

```cpp
#include "tensor_api/tensor.h"

using namespace asc::te;

struct mmad_trait_custom {
    using trait_type = mmad_trait;
    // Set mmad_trait::disable_gemv to false to enable GEMV.
    static constexpr const trait_type value = mmad_trait(0, false, false, false, mmad_type::normal);
};

__aicore__ inline void mmad_gemv_example(__gm__ half* a_gm, __gm__ half* b_gm, __gm__ float* c_gm)
{
    using a_type = half;
    constexpr uint32_t m = 1;
    constexpr uint32_t k = 4096;
    constexpr uint32_t n = 256;
    // For the half data type, c0_element = c0_size(32B) / sizeof(half) = 16. The 1×K vector is rearranged into a Tensor of size (K/c0_element)×c0_element in NZ format.
    constexpr uint32_t c0 = c0_element<a_type>;

    __cbuf__ a_type l1_a_buf[m * k];
    __ca__ a_type l0a_buf[m * k];

    // Cube A Global Memory->L1 Buffer: ND->ND. The 1×K row vector is moved into the L1 Buffer contiguously in ND format.
    auto gm_a_tensor = make_tensor(make_mem_ptr(a_gm), make_frame_layout<nd_ext_layout_ptn>(m, k));
    auto l1_a_tensor = make_tensor(make_mem_ptr(l1_a_buf), make_frame_layout<nd_ext_layout_ptn>(m, k));
    auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    copy(copy_gm_to_l1_atom, l1_a_tensor, gm_a_tensor);

    // Reinterpret this contiguous block in the L1 Buffer as an NZ fractal cube of size (K/c0_element)×c0_element (the address remains unchanged, only the layout changes).
    auto l1_a_nz_tensor = make_tensor(l1_a_tensor.data(), make_frame_layout<nz_layout_ptn, a_type>(k / c0, c0));
    // L1 Buffer->L0A Buffer: NZ->NZ non-transposed move, completing the fractal move in one step.
    auto l0a_tensor = make_tensor(make_mem_ptr(l0a_buf), make_frame_layout<nz_layout_ptn, a_type>(k / c0, c0));
    auto copy_l1_to_l0a_atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    copy(copy_l1_to_l0a_atom, l0a_tensor, l1_a_nz_tensor);

    // The move of cube B is the same as in the basic scenario (omitted here; assume l0b has been obtained).
    // ...

    // mmad computation: use the Trait configured with GEMV mode.
    mmad_params params(m, n, k, unit_flag_mode::disable, true);
    auto atom = make_mmad(mmad_operation{}, mmad_trait_custom{}).with(params);
    mmad(atom, l0c, l0a_tensor, l0b);
}
```
