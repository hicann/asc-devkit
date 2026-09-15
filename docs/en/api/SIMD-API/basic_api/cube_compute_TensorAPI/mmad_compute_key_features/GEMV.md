# GEMV

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T17:28:36.226Z -->

## Description

The core function of GEMV is as follows: when the M dimension of cube A is 1, the API can manually enable the GEMV function, and this operation degenerates into a multiplication between a 1×K row vector and a K×N cube.

<!-- npu="950" id1 -->
For Ascend 950PR/Ascend 950DT products, GEMV mode is disabled by default. To enable GEMV mode, set the `MmadTrait::disableGemv` parameter to `false`.
<!-- end id1 -->

## Constraints

- The 1×K cube A must be 512B-aligned, and the K data elements are stored contiguously. Taking the `half` data type as an example, when K=256, the software side can treat it as a 16×16 block. After configuring m=1, the hardware parses it as a 1×256 vector, and the 256 `half` data elements can be moved from the L1 Buffer to the L0A Buffer through the `Copy` API.

- The C cube is a 1×N vector. When the 1×N vector is divided into multiple 1×16 sub-vectors, each sub-vector occupies 1024B in the L0C Buffer. However, the actual valid data occupies only the lowest 32B or 64B. For example, when N=50, the vector is divided into four 1×16 sub-vectors, occupying 4×512B=2048B, of which the valid data is only 4×32B=128B.

**Figure 1**  GEMV mode cube multiplication diagram

![GEMV mode cube multiplication diagram](../../../../figures/mmad_gemv.png)

## Advantages

When M=1, the GEMV mode is enabled, and the cube multiplication processes the M dimension as an unaligned scenario. Compared with the unaligned processing method, the GEMV mode moves less data and delivers better performance. The following uses a cube multiplication example with M=1, K=256, N=32, and the `half` data type for both the left and right matrices for illustration.

- GEMV mode

    When cube A is moved from the L1 Buffer to the L0A Buffer, the 1×256 vector is processed as a 16×16 cube, and the `Copy` API is called once to complete the cube movement of a 16×16 fractal size. The movement of cube B and the cube multiplication computation are the same as those in the basic scenario, as shown in the following figure.

    **Figure 2**  Computation diagram of cube multiplication with M=1 in GEMV mode

    ![Computation diagram of cube multiplication with M=1 in GEMV mode](../../../../figures/mmad_gemv_compare.png)

- Non-GEMV mode

    When cube A is moved from the L1 Buffer to the L0A Buffer, the 1×256 vector is processed as unaligned cube data, and the M dimension is aligned to 32 bytes before movement. The `Copy` API is called to move a cube of a 16×16 fractal size each time, for a total of K/16=16 times, which increases the amount of data moved and results in poorer performance than the GEMV mode, as shown in the following figure.

    **Figure 3**  Computation diagram of cube multiplication with M not equal to 1 in non-GEMV mode

    ![Computation diagram of cube multiplication with M not equal to 1 in non-GEMV mode](../../../../figures/mmad_nongemv_compare.png)

## Example

The specifications of this GEMV scenario example are as follows:

| Cube | Dimension Size | Data Type | Format |
| --- | --- | --- | --- |
| A | 1×4096 | half | ND |
| B | 4096×256 | half | NZ |
| C | 1×256 | float | ND |

Use the [Copy](../cube_compute_load/Copy_GMToL1.md) API to move input cube A from Global Memory to the L1 Buffer while keeping the ND format unchanged. Then convert cube A to the NZ format in the L1 Buffer, and use the [Copy](../cube_compute_load/Copy_L1ToL0A.md) API to move it to the L0A Buffer, so that cube multiplication is executed in GEMV mode. Note that GEMV mode is disabled by default and must be enabled by customizing [MmadTrait](../mmad_compute/Mmad.md).

```cpp
#include "tensor_api/tensor.h"

using namespace AscendC::Te;

struct MmadTraitCustom {
    using TraitType = MmadTrait;
    // Set the MmadTrait::disableGemv parameter to false to enable GEMV.
    static constexpr const TraitType value = MmadTrait(0, false, false, false, MmadType::NORMAL);
};

__aicore__ inline void MmadGemvExample(__gm__ half* aGm, __gm__ half* bGm, __gm__ float* cGm)
{
    using AType = half;
    constexpr uint32_t m = 1;
    constexpr uint32_t k = 4096;
    constexpr uint32_t n = 256;
    // For the half data type, C0_ELEMENT = C0_SIZE(32B) / sizeof(half) = 16. Rearrange the 1×K vector into a Tensor of size (K/C0_ELEMENT)×C0_ELEMENT in the NZ format.
    constexpr uint32_t c0 = C0_ELEMENT<AType>;

    __cbuf__ AType l1ABuf[m * k];
    __ca__ AType l0ABuf[m * k];

    // Cube A Global Memory -> L1 Buffer: ND -> ND. Move the 1×K row vector into the L1 Buffer continuously in the ND format.
    auto gmATensor = MakeTensor(MakeMemPtr(aGm), MakeFrameLayout<NDExtLayoutPtn>(m, k));
    auto l1ATensor = MakeTensor(MakeMemPtr(l1ABuf), MakeFrameLayout<NDExtLayoutPtn>(m, k));
    auto copyGm2L1 = MakeCopy(CopyGM2L1{}, CopyGM2L1TraitDefault{});
    Copy(copyGm2L1, l1ATensor, gmATensor);

    // Reinterpret this contiguous data block in the L1 Buffer as an NZ fractal cube of size (K/C0_ELEMENT)×C0_ELEMENT (the address remains unchanged; only the Layout is changed).
    auto l1ANzTensor = MakeTensor(l1ATensor.Data(), MakeFrameLayout<NZLayoutPtn, AType>(k / c0, c0));
    // L1 Buffer -> L0A Buffer: NZ -> NZ non-transposed move, completing the fractal move in one step.
    auto l0ATensor = MakeTensor(MakeMemPtr(l0ABuf), MakeFrameLayout<NZLayoutPtn, AType>(k / c0, c0));
    auto copyL12L0A = MakeCopy(CopyL12L0A{}, CopyL12L0ATraitDefault{});
    Copy(copyL12L0A, l0ATensor, l1ANzTensor);

    // Cube B is moved in the same way as in the basic scenario (omitted here; assume l0B has been obtained).
    // ...

    // Mmad computation: use the trait configured with GEMV mode.
    MmadParams params(m, n, k, 0, true);
    auto atom = MakeMmad(MmadOperation{}, MmadTraitCustom{}).with(params);
    Mmad(atom, l0C, l0ATensor, l0B);
}
```
