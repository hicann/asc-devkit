# set_mmad_direction

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

设置矩阵乘加计算时结果分形的生成方向。该配置只改变L0C Buffer上结果分形的生成顺序，不改变计算结果、乘加次数或输出格式。

## 函数原型

```cpp
__aicore__ inline void set_mmad_direction(mmad_direction direction)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| direction | 输入 | 矩阵乘加计算方向，类型为`mmad_direction`。 |

`direction`支持如下取值：

| 取值 | 说明 |
| --- | --- |
| m | 逐列生成矩阵计算结果分形。 |
| n | 逐行生成矩阵计算结果分形。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口需在矩阵乘加计算前调用，以确保方向配置在矩阵乘加计算过程中生效。
- 方向配置一旦写入会持续生效，后续矩阵乘加计算若不显式重新配置，将沿用当前方向配置。

## 调用示例

以下示例在16×16 float矩阵乘法前将结果分形生成方向设置为逐列生成。矩阵A和矩阵B的所有元素均为1，结果矩阵的每个元素均为16。

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
