# Improving Copy Efficiency Through Type Alignment

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T13:17:09.270Z pushedAt=2026-11-03T07:05:13.377Z nl=2 -->

[Priority] Medium

[Description] In SIMT programming mode, struct type alignment affects the memory access efficiency of Global Memory. <!-- npu="950" id1 -->The Ascend 950PR&950DT products supports memory access instructions with five bit widths: 1B, 2B, 4B, 8B, and 16B. <!-- end id1 -->When the total data volume remains unchanged, you can improve data copy efficiency by aligning the type to a larger bit width.

[Sample introduction] Set the total copy volume `MEM_BYTES` to 512 MiB, construct multiple groups of structs `TData` with different sizes and alignment modes, and sequentially copy the entire input array `d_idata` of length `num_elements` to the output array `d_odata` for testing, to compare the time difference between unaligned and `alignas` type-aligned cases. For details, see [Type Alignment Copy Efficiency Optimization Best Practice Example](../../../../../../examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types), where the kernel function is implemented as follows:

```cpp
constexpr uint32_t THREAD_COUNT = 2048;                  // threads per block
constexpr size_t MEM_BYTES = 512ULL * 1024ULL * 1024ULL; // Total data volume: 512 MiB
uint32_t num_elements = static_cast<uint32_t>(MEM_BYTES / sizeof(TData));

template <class TData>
__global__ __launch_bounds__(THREAD_COUNT) void aligned_type_kernel(
    TData* d_odata, TData* d_idata, uint32_t num_elements)
{
    const uint32_t tid = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t num_threads = blockDim.x * gridDim.x;
    for (uint32_t pos = tid; pos < num_elements; pos += num_threads) {
        d_odata[pos] = d_idata[pos];
    }
}
```

The following uses the two structs `Align4` and `Misalign4` as typical representatives to show the comparison scenarios.

[Negative example] `Misalign4` does not set explicit alignment, and its default `alignof` is limited by the field type, so the compiler can only select narrow-bit-width memory access instructions.

```cpp
struct Misalign4 {
    unsigned char r, g, b, a;
}; // sizeof=4, alignof=1
```

The performance data is as follows:

| Task Duration(us) | aiv_total_cycles |
| :---------------: |:--------------: |
|     81223.02      | 134028903.60    |

For the above struct, the field type determines `alignof=1`, so the compiler can only copy each element at the 8-bit level, resulting in high `aiv_total_cycles` and `Task Duration`.

[Positive example] Use `alignas` to improve the alignment attribute so that the compiler can select memory access instructions with a wider bit width.

```cpp
struct alignas(4) Align4 {
    unsigned char r, g, b, a;
}; // sizeof=4, alignof=4
```

The performance data is as follows:

| Task Duration (us) | aiv_total_cycles |
| :---------------: | :--------------: |
|     27889.33      |  46005213.32    |

`Align4` raises `alignof` from 1B to 4B, allowing the compiler to use the 32-bit width to copy a single element. As a result, both `aiv_total_cycles` and `Task Duration` decrease significantly.

Comparing multiple samples with different struct sizes, the duration differences before and after type alignment are as follows:

| Struct Size                | Aligned Task Duration (us) | Unaligned Task Duration (us) | Unaligned Task Duration/Aligned Task Duration |
| :------------------------: | :-----------------------: | :-------------------------: | :---------: |
| 4B (4×u8)                  | 27889.33 (alignof=4)      | 81223.02 (alignof=1)        | **2.91x**   |
| 4B (2×u16)                 | 27833.56 (alignof=4)      | 46656.61 (alignof=2)        | **1.68x**   |
| 4B (2×u8 + u16)            | 27854.56 (alignof=4)      | 46666.11 (alignof=2)        | **1.68x**   |
| 4B (u8 + 1B padding + u16) | 27809.72 (alignof=4)      | 46664.42 (alignof=2)        | **1.68x**   |
| 8B                         | 17417.23 (alignof=8)      | 21287.91 (alignof=4)        | **1.22x**   |
| 16B                        | 27931.29 (alignof=16)     | 38353.60 (alignof=4)        | **1.37x**   |
| 32B                        | 52357.78 (alignof=32)     | 77790.93 (alignof=4)        | **1.49x**   |

The test results show that **when the total amount of data remains unchanged, type alignment can improve copy efficiency for structs of the same size**. Taking the 4B struct as an example, as the type alignment bit width increases from 1B to 4B, `Unaligned Task Duration/Aligned Task Duration` decreases gradually, indicating that the duration gap between unaligned and aligned copies narrows. In addition, for 4B structs of the same size, neither the field combination nor the presence of padding affects copy efficiency.

[Summary] When a SIMT operator uses structs to copy data, struct type alignment allows memory access instructions to merge into a larger bit width to complete the copy, reducing the copy duration. <!-- npu="950" id2 -->The Ascend 950PR&950DT products supports memory access instructions with bit widths of 1B, 2B, 4B, 8B, and 16B. Therefore, it is recommended to select the highest possible bit width without exceeding the struct size. <!-- end id2 -->
