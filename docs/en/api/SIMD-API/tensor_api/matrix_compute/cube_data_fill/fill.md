# fill

<!-- md-trans-meta sourceCommit=53cacf821c4d3ab06652650874e352140527b080 translatedAt=2026-11-19T02:33:15.233Z pushedAt=2026-11-19T05:54:06.643Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Function Description

The header file path is `tensor_api/tensor.h`.

`fill` writes a scalar fill value to the L1 Buffer and broadcasts the fill value to multiple 32-byte fractal blocks. It can be used to initialize the L1 Buffer before cube computation.

The Tensor API provides two fill forms: explicitly passing `fill_params<T>` to describe the fill configuration; and passing only the fill value, in which case the API derives the fill parameters based on the Layout of the destination Tensor and completely fills the Tensor. Both forms require that the fill value type `T` be exactly consistent with the `data_type` of the destination Tensor.

## Prototype

```cpp
template <typename T>
struct fill_params {
    uint16_t repeat_times = 0;
    uint16_t block_num = 0;
    uint16_t dst_gap = 0;
    T init_value = {};
};

template <typename Tensor, typename T>
__aicore__ inline void fill(const Tensor& dst, const fill_params<T>& params)

template <typename Tensor, typename T>
__aicore__ inline void fill(const Tensor& dst, const T& init_value)
```

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| :--- | :--- |
| Tensor | Destination Tensor type. |
| T | Fill value type, which must be consistent with `Tensor::data_type`. |

**Table 2** `fill` API parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Output | L1 destination Tensor. |
| params | Input | Physical block fill configuration. |
| init_value | Input | Scalar fill value. The API derives the complete fill configuration based on the Layout of the destination Tensor. |

**Table 3** `fill_params` members

| Member | Default Value | Description |
| :--- | :---: | :--- |
| repeat_times | `0` | Number of repetitions, that is, the total number of times the fill value is repeatedly written at intervals of `dst_gap` after being written to the L1 Buffer in `block_num` 32-byte blocks. The value range is `[0, 32767]`. |
| block_num | `0` | Number of 32-byte physical blocks written in a single repetition. The value range is `[0, 32767]`. |
| dst_gap | `0` | Interval between two adjacent repetitions, in units of 32-byte blocks. The value range is `[0, 32767]`. |
| init_value | `T{}` | Scalar fill value. |

## Data Type

`T` supports `half`, `bfloat16_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, and `float`.

## Return Value

None

## Constraints

- This API takes effect only on the AIC.
- The destination Tensor must be located in the L1 Buffer.
- `T` must be consistent with `Tensor::data_type`; otherwise, a compilation error is reported.
- The start address of the destination Tensor must be 32-byte aligned.
- The total capacity of the L1 Buffer is 512KB. The sum of the destination Tensor offset and the actual occupied space must not exceed the L1 Buffer capacity.
- If this instruction has data dependencies with other instructions, a synchronization instruction must be inserted to serialize multiple instructions and prevent abnormal data.

## Example

The following example first uses fill to initialize matrix A in L1 to 2, then moves the matrix from L1 to L0A, moves the same initialized data to L0B as matrix B, performs cube multiplication, and writes the L0C result back to Global Memory. Each output element equals columns * 2 * 2.

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

constexpr uint32_t rows = 128;
constexpr uint32_t columns = 128;
constexpr uint32_t element_count = rows * columns;

__global__ __cube__ void fill_l1_kernel(__gm__ half* output)
{
    using namespace asc::te;
    __cbuf__ half l1_buffer[element_count];
    __ca__ half l0_a_buffer[element_count];
    __cb__ half l0_b_buffer[element_count];
    __cc__ float l0_c_buffer[element_count];

    auto l1_layout = make_frame_layout<nz_layout_ptn, half>(rows, columns);
    auto l0_a_layout = make_frame_layout<nz_layout_ptn, half>(rows, columns);
    auto l0_b_layout = make_frame_layout<zn_layout_ptn, half>(rows, columns);
    auto l0_c_layout = make_frame_layout<nz_layout_ptn, float>(rows, columns);
    auto output_layout = make_frame_layout<nd_ext_layout_ptn, half>(rows, columns);
    auto l1_tensor = make_tensor(make_mem_ptr(l1_buffer), l1_layout);
    auto l0_a_tensor = make_tensor(make_mem_ptr(l0_a_buffer), l0_a_layout);
    auto l0_b_tensor = make_tensor(make_mem_ptr(l0_b_buffer), l0_b_layout);
    auto l0_c_tensor = make_tensor(make_mem_ptr(l0_c_buffer), l0_c_layout);
    auto output_tensor = make_tensor(make_mem_ptr<location::gm>(output), output_layout);

    fill_params<half> params{};
    params.repeat_times = rows;
    params.block_num = columns * sizeof(half) / 32;
    params.dst_gap = 0;
    params.init_value = static_cast<half>(2);

    mutex<PIPE_MTE2> fill_mutex(0);
    fill_mutex.lock();
    fill(l1_tensor, params);
    fill_mutex.unlock();

    mutex<PIPE_MTE1> l0_mutex(0);
    l0_mutex.lock();
    copy(make_copy(copy_l1_to_l0a{}, l1_to_l0a_trait_default{}), l0_a_tensor, l1_tensor);
    copy(make_copy(copy_l1_to_l0b{}, l1_to_l0b_trait_default{}), l0_b_tensor, l1_tensor);
    l0_mutex.unlock();

    mutex<PIPE_M> mmad_mutex(0);
    mmad_mutex.lock();
    mmad(make_mmad(mmad_operation{}, mmad_trait_default{}).with(mmad_params{rows, columns, columns,
                  unit_flag_mode::disable, true}),
        l0_c_tensor, l0_a_tensor, l0_b_tensor);
    mmad_mutex.unlock();

    mutex<PIPE_FIX> output_mutex(0);
    output_mutex.lock();
    copy(make_copy(copy_l0c_to_gm{}, l0c_to_gm_trait_default{}), output_tensor, l0_c_tensor);
    output_mutex.unlock();
    sync_pipe(PIPE_ALL);
}
```
