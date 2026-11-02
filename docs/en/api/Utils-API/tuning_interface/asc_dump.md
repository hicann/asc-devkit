# asc\_dump

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-23T07:45:38.425Z pushedAt=2026-10-26T11:12:09.545Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference series products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training series products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/tuning_interface/asc_dump_res.md#id1 -->

## Description

Prints the data in the corresponding memory and also supports printing custom additional information (only information of the uint32\_t type is supported), such as the current line number.

To use this API, include the `"utils/debug/asc_dump.h"` header file.

> [!CAUTION] Note
> This feature is mainly used for **debugging and performance analysis**, typically during the debugging phase, and it may affect operator performance to a certain extent. **In the production environment, you can disable it by setting ASCENDC_DUMP=0.**
> By default, this feature is enabled.

## Prototype

```cpp
// Print the data in GM memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// Print the data in Unified Buffer (UB) memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print the data in L1 memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print the data in L0C memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size)

// Print the data in GM memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// Print the data in UB memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data in L1 memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data in L0C memory.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__cc__ T* input, uint32_t desc, uint32_t dump_size)
```

<!-- npu="950" id13 -->
The following APIs are supported only on Ascend 950PR&950DT series products.

```cpp
// Print data in the BiasTable Buffer.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__biasbuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data in the Fixpipe Buffer.
template<typename T>
__aicore__ static __attribute__((noinline)) void asc_dump(__fbuf__ T* input, uint32_t desc, uint32_t dump_size)
```
<!-- end id13 -->

<!-- npu="950" id7 -->
The following APIs are the asc\_dump APIs used in simd\_vf, and are supported only on Ascend 950PR&950DT series products.

```cpp
// Print data in registers.
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)

// Print data in registers.
template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)

// Print data in UB memory.
template <typename T>
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data in UB memory.
template <typename T>
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
```
<!-- end id7 -->

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| T | Input | Data type to dump. |
| U | Input | Register data type to dump. |
| input | Input | Start address of the memory block to print. |
| desc | Input | Custom information of the uint32_t type. The supported data range is [0, 2^32 - 1]. |
| dump_size | Input | Number of elements to print. |

## Data Type
<!-- npu="950" id10 -->
- For Ascend 950PR&950DT series products: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 series products: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 series products: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id12 -->

## Return Value

None

## Constraints

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT series products, when using this API to print L1 Buffer, BiasTable Buffer, or Fixpipe Buffer data, the HDK version must be upgraded to at least 25.7.RC1.6 or later.
<!-- end id8 -->
<!-- npu="950" id15 -->
- For Ascend 950PR&950DT series products, in the scenario of printing Tensor information in the Fixpipe Buffer:
    - The Fixpipe Buffer stores hardware parameter bit fields, so the printed result may not be bitwise identical to the original data in the L1 Buffer. For the preceding Quant parameters, every 8 bytes retain bit[7:0], bit[31:13], and bit[46:37], with the expected value `input & 0x00007fe0ffffe0ffULL`; for the preceding ReLU parameters, every 4 bytes retain bit[31:13], with the expected value `word & 0xffffe000U`.
<!-- end id15 -->
- When using this API, the total amount of data dumped on each core cannot exceed 30KB. You must control the amount of content to print; otherwise, the data will not be printed.
- When the value of `dump_size` exceeds the actual number of elements in `input`, undefined behavior occurs.
- When calculating the data amount, if the total dump length is not aligned, consider the impact of padding data. During a non-aligned dump, if the actual dumped element length does not meet 32-byte alignment, the system automatically appends a certain amount of padding data at the end (this data is not printed) to meet the alignment requirement.
-   In the SIMD scenario, the total amount of data printed by a single call to this API must not exceed the print size limit, which is 30KB by default. Note that if this limit is exceeded, the data will not be printed. You can configure this limit through the `"simd_printf_fifo_size_per_core"` field in acl.json, with a configurable range from a minimum of 1KB to a maximum of 64MB (adjustable through the [aclInit](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/02_initialization_and_deinitialization.md#aclinit) API). When the amount of data to be printed is large, it is recommended to increase the buffer space. This configuration is not supported in the PyTorch invocation and operator graph compilation scenarios.

<!-- npu="950" id16 -->
-   In the SIMD VF scenario, when using `asc_dump_reg` or `asc_dump` with register parameters, if `dump_size` exceeds the number of elements that a single vector register can hold, the system automatically truncates it to the number of elements that the register can hold.
-   In the SIMD VF scenario, each AIV core uses 2KB of reserved UB space to temporarily store debugging data during a single `asc_vf_call` execution. The `assert`, `ascendc_assert`, `printf`, and `asc_dump` operations within the same `asc_vf_call` share this space. After the data in this space is transferred, the space is reused, so the cumulative debugging data generated by the preceding APIs can exceed 2KB. A single piece of debugging data must fit entirely in this space; otherwise, that piece of data will not be printed.
-   Each time `asc_dump` of `simd_vf` is called, in addition to the actual dumped data, 72 bytes of management information are always occupied; the actual dumped data must be aligned upward to 32 bytes.
-   In the SIMD VF scenario, it is recommended to configure `simd_printf_fifo_size_per_core` to 3KB or more. If the configuration is too small and the amount of data to be printed is large, some debugging data will not be printed.
<!-- end id16 -->

## Examples

```cpp
__gm__ half* src;
uint32_t desc = 1;
uint32_t dump_size = 32;
asc_dump_gm<half>(src, desc, dump_size);
```

SIMD VF example:

```cpp
__simd_vf__ inline void SimdVfDumpReg()
{
    AscendC::Reg::RegTensor<float> src_reg;
    uint32_t desc = 1;
    uint32_t dump_size = 32;
    asc_dump<float>(src_reg, desc, dump_size);
}
```
