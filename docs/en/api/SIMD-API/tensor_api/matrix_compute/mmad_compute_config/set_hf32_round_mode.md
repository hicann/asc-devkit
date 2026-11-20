# set_hf32_round_mode

<!-- md-trans-meta sourceCommit=7b36bb07889b61179ceefa6904303c10d9dd5ef4 translatedAt=2026-11-19T02:39:31.230Z pushedAt=2026-11-19T05:54:06.647Z nl=2 -->

## Supported Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not support
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not support
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

Sets the rounding mode of the HF32 mode. The HF32 mode is used for matrix computation. For details, see [HF32](../mmad_compute_key_features/hf32.md).

## Prototype

```cpp
__aicore__ inline void set_hf32_round_mode(hf32_round_mode mode)
```

## Parameters

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mode | Input | HF32 rounding mode, of the `hf32_round_mode` type. The following values are supported:<br>• `nearest_away`: FP32 rounds to the nearest value, with ties rounding away from zero.<br>• `nearest_even`: FP32 rounds to the nearest value, with ties rounding to even. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- After the HF32 mode is enabled, if this API is not called, the rounding mode represented by `nearest_even` is used by default.
- This API must be called before matrix multiplication and accumulation computation to ensure that the mode configuration takes effect during the matrix multiplication and accumulation computation.
- The rounding mode configured by this API takes effect only while the HF32 mode is enabled. If the HF32 mode is not enabled, the rounding mode configuration has no actual effect.
- The rounding mode configuration remains in effect after being configured. If the HF32 mode is disabled and then enabled again, the previous rounding mode configuration is still used. To switch the rounding mode, call this API again.

## Example

The following example enables the HF32 mode before 16×16 float matrix multiplication and sets the rounding mode to round to even. All elements of matrix A and matrix B are 1, and each element of the result matrix is 16.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

template <uint32_t m, uint32_t k, uint32_t n, uint32_t single_core_m, uint32_t base_m, uint32_t base_k, uint32_t base_n>
__global__ __cube__ void matmul_custom(__gm__ float* a, __gm__ float* b, __gm__ float* c)
{
    using namespace asc::te;
    init_soc_state();

    // Split the output matrix by m and n tiles across Cube cores.
    // Map block_idx to this core's output tile: m_iter_idx iterates fastest over the m-axis splits
    // (one core computes one base_m x base_n output tile).
    constexpr uint32_t m_iter = m / single_core_m;
    uint32_t m_iter_idx = block_idx % m_iter;
    uint32_t n_iter_idx = block_idx / m_iter;

    // Create GM tensors and slice the tile handled by the current core.
    auto gm_a = make_tensor(make_mem_ptr(a), make_frame_layout<nd_ext_layout_ptn>(m, k));
    auto gm_b = make_tensor(make_mem_ptr(b), make_frame_layout<nd_ext_layout_ptn>(k, n));
    auto gm_c = make_tensor(make_mem_ptr(c), make_frame_layout<nd_ext_layout_ptn>(m, n));

    auto global_a = gm_a.slice(make_coord(m_iter_idx * single_core_m, 0), make_shape(base_m, k));
    auto global_b = gm_b.slice(make_coord(0, n_iter_idx * base_n), make_shape(k, base_n));
    auto global_c = gm_c.slice(make_coord(m_iter_idx * single_core_m, n_iter_idx * base_n), make_shape(base_m, base_n));

    // Layout per level: GM uses nd_ext (linear row-major); L1 A/B, L0A and L0C use the NZ fractal,
    // L0B uses the ZN fractal, matching each buffer's data format for the L1->L0 copies.
    auto l1_a_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(base_m, k);
    auto l1_b_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(k, base_n);
    auto l0_a_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float>>(base_m, base_k);
    auto l0_b_layout = make_frame_layout<zn_layout_ptn, layout_trait_default<float>>(base_k, base_n);
    auto l0_c_layout = make_frame_layout<nz_layout_ptn, layout_trait_default<float, _16>>(base_m, base_n);

    __cbuf__ float l1_a_buf[base_m * k];
    __cbuf__ float l1_b_buf[k * base_n];
    __ca__ float l0_a_buf[base_m * base_k];
    __cb__ float l0_b_buf[base_k * base_n];
    __cc__ float l0_c_buf[base_m * base_n];

    auto l1_a_tensor = make_tensor(make_mem_ptr(l1_a_buf), l1_a_layout);
    auto l1_b_tensor = make_tensor(make_mem_ptr(l1_b_buf), l1_b_layout);
    auto l0_a_tensor = make_tensor(make_mem_ptr(l0_a_buf), l0_a_layout);
    auto l0_b_tensor = make_tensor(make_mem_ptr(l0_b_buf), l0_b_layout);
    auto l0_c_tensor = make_tensor(make_mem_ptr(l0_c_buf), l0_c_layout);

    auto copy_gm2l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
    auto copy_l12l0a_atom = make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{});
    auto copy_l12l0b_atom = make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{});
    auto copy_l0c2gm_atom = make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{});
    auto mmad_atom = make_mmad(mmad_operation{}, mmad_trait_default{});

    // K-axis loop count (whole base_k tiles; this sample requires k divisible by base_k)
    // and the event ids serializing buffer accesses between pipelines.
    constexpr uint32_t k_loop = k / base_k;
    constexpr uint32_t l1_index = 0;
    mutex<PIPE_MTE2> pipe_mte2_l1_index_mutex(l1_index);
    mutex<PIPE_MTE1> pipe_mte1_l1_index_mutex(l1_index);
    constexpr uint32_t l0_index = 1;
    constexpr uint32_t l0c_index = 2;
    mutex<PIPE_M> pipe_m_l0c_index_mutex(l0c_index);
    mutex<PIPE_FIX> pipe_fix_l0c_index_mutex(l0c_index);

    // Fill the unused L1 tail once to make padding deterministic before MTE1 loads.
    fill(l1_a_tensor, static_cast<float>(0));
    fill(l1_b_tensor, static_cast<float>(0));

    // Lock L0C for the whole K-accumulation of this output tile.
    pipe_m_l0c_index_mutex.lock();
    // MTE2 stage: move the current core's full A/B tiles from GM to L1 once.
    pipe_mte2_l1_index_mutex.lock();
    copy(copy_gm2l1_atom, l1_a_tensor, global_a);
    copy(copy_gm2l1_atom, l1_b_tensor, global_b);
    pipe_mte2_l1_index_mutex.unlock();
    pipe_mte1_l1_index_mutex.lock();

    for (uint32_t k_iter = 0; k_iter < k_loop; ++k_iter) {
        mutex<PIPE_MTE1> pipe_mte1_l0_index_mutex(l0_index);
        mutex<PIPE_M> pipe_m_l0_index_mutex(l0_index);
        // MTE1 stage: load one base_k slice from L1 to L0.
        pipe_mte1_l0_index_mutex.lock();
        copy(
            copy_l12l0a_atom, l0_a_tensor,
            l1_a_tensor.slice(make_coord(0, k_iter * base_k), make_shape(base_m, base_k)));
        copy(
            copy_l12l0b_atom, l0_b_tensor,
            l1_b_tensor.slice(make_coord(k_iter * base_k, 0), make_shape(base_k, base_n)));
        pipe_mte1_l0_index_mutex.unlock();
        // M stage: first k iteration initializes L0C (C = A*B), later ones accumulate (C += A*B).
        pipe_m_l0_index_mutex.lock();

        enable_hf32();
        set_hf32_round_mode(hf32_round_mode::nearest_even);
        set_mmad_direction(mmad_direction::m);
        mmad_params para{base_m, base_n, base_k, unit_flag_mode::disable, k_iter == 0};

        mmad(mmad_atom.with(para), l0_c_tensor, l0_a_tensor, l0_b_tensor);

        pipe_m_l0_index_mutex.unlock();
    }
    pipe_mte1_l1_index_mutex.unlock();
    pipe_m_l0c_index_mutex.unlock();
    disable_hf32();
    // Store the completed output tile from L0C back to GM.
    pipe_fix_l0c_index_mutex.lock();
    copy(copy_l0c2gm_atom, global_c, l0_c_tensor);
    pipe_fix_l0c_index_mutex.unlock();
    sync_pipe(PIPE_ALL);
}
```
