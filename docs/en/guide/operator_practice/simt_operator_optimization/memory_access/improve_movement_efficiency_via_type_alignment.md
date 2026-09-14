# Improving Movement Efficiency Through Type Alignment

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:46:51.591Z -->

[Priority] Medium

[Description] In SIMT programming mode, struct type alignment affects the memory access efficiency of Global Memory. Ascend 950PR/Ascend 950DT support memory access instructions with four bit widths: 1B, 2B, 4B, and 8B. When the total data size remains unchanged, you can improve data copy efficiency by aligning the type to a larger bit width.

[Example Introduction] Set the total copy size `MEM_BYTES` to 512 MiB, construct multiple groups of structures `TData` with different sizes and alignment modes, and sequentially copy the entire input array `d_idata` of length `num_elements` to the output array `d_odata` for testing. Compare the time difference between unaligned and `alignas` type-aligned cases. For details, see the [type alignment movement efficiency optimization best practice example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/03_simt_api/03_best_practices/00_memory_optimizations/aligned_types), where the kernel function is implemented as follows:

```
constexpr uint32_t THREAD_COUNT = 2048;                  // threads per block
constexpr size_t MEM_BYTES = 512ULL * 1024ULL * 1024ULL; // Total data size: 512 MiB
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

The following uses the two structures `Align4` and `Misalign4` as typical representatives to demonstrate the comparison scenarios.

[Negative Example] `Misalign4` does not set explicit alignment. Its default `alignof` is limited by the field type, so the compiler can only select narrow-bit-width memory access instructions.

```
struct Misalign4 {
    unsigned char r, g, b, a;
}; // sizeof=4, alignof=1
```

The performance data is as follows:

| Task Duration(us) | aiv_total_cycles |
| :---------------: |:--------------: |
|     81223.02      | 134028903.60    |

For the preceding structure, the field type determines `alignof=1`, so the compiler can only copy each element using the 8-bit width. As a result, both `aiv_total_cycles` and `Task Duration` are high.

[Positive Example] Using `alignas` to improve the alignment attribute allows the compiler to select memory access instructions with a wider bit width.

```
struct alignas(4) Align4 {
    unsigned char r, g, b, a;
}; // sizeof=4, alignof=4
```

The performance data is as follows:

| Task Duration(us) | aiv_total_cycles |
| :---------------: | :--------------: |
|     27889.33      |  46005213.32    |

`Align4` raises `alignof` from 1B to 4B, allowing the compiler to use the 32-bit width to copy a single element. Both `aiv_total_cycles` and `Task Duration` are significantly reduced.

Comparing multiple samples with different structure sizes, the duration differences before and after type alignment are as follows:

| Structure Size                | Aligned Task Duration(us) | Unaligned Task Duration(us) | Unaligned Task Duration/Aligned Task Duration |
| :------------------------: | :-----------------------: | :-------------------------: | :---------: |
| 4B (4×u8)                  | 27889.33 (alignof=4)      | 81223.02 (alignof=1)        | **2.91x**   |
| 4B (2×u16)                 | 27833.56 (alignof=4)      | 46656.61 (alignof=2)        | **1.68x**   |
| 4B (2×u8 + u16)            | 27854.56 (alignof=4)      | 46666.11 (alignof=2)        | **1.68x**   |
| 4B (u8 + 1B padding + u16) | 27809.72 (alignof=4)      | 46664.42 (alignof=2)        | **1.68x**   |
| 8B                         | 17417.23 (alignof=8)      | 21287.91 (alignof=4)        | **1.22x**   |
| 16B                        | 32983.77 (alignof=16)     | 38353.60 (alignof=4)        | **1.16x**   |
| 32B                        | 60901.24 (alignof=32)     | 77790.93 (alignof=4)        | **1.28x**   |

The test results show that: **when the total amount of data remains unchanged, type alignment can improve movement efficiency for structures of the same size**. Taking the 4B structure as an example, as the structure type alignment bit width increases from 1B to 4B, `Unaligned Task Duration/Aligned Task Duration` decreases gradually, indicating that the duration gap between unaligned and aligned copies narrows progressively. In addition, for 4B structures of the same size, neither the field combination nor the presence of padding bits affects copy efficiency.

[Summary] When a SIMT operator uses structures to copy data, structure type alignment allows memory access instructions to merge and complete the copy at a larger bit width, reducing the copy duration. Ascend 950PR/Ascend 950DT support memory access instructions with 1B, 2B, 4B, and 8B bit widths. Therefore, it is recommended to select the highest possible bit width without exceeding the structure size.
