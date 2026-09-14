# asc\_dump

<!-- md-trans-meta sourceCommit=1a78f19c20282bdf308f922f8abfbf826fdea485 translatedAt=2026-09-06T08:58:56.132Z pushedAt=2026-09-11T01:45:50.817Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
- Atlas inference products Vector Core: not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: not supported
<!-- end id6 -->

## Description

Prints the data from the corresponding memory, and also supports printing custom additional information (only uint32\_t type information is supported), such as the current line number.

To use this API, include the `"utils/debug/asc_dump.h"` header file.

> [!CAUTION]
>This feature is mainly used for **debugging and performance analysis** and is typically used in the debugging phase. It may affect operator performance to a certain extent. **In the production environment, you can disable it by setting ASCENDC_DUMP=0**.
>By default, this feature is enabled.

## Prototype

```cpp
// Print data on GM memory.
template<typename T>
__aicore__ inline void asc_dump_gm(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on UB memory.
template<typename T>
__aicore__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on L1 memory.
template<typename T>
__aicore__ inline void asc_dump_l1buf(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on L0C memory.
template<typename T>
__aicore__ inline void asc_dump_cbuf(__cc__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on GM memory.
template<typename T>
__aicore__ inline void asc_dump(__gm__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on UB memory.
template<typename T>
__aicore__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on L1 memory.
template<typename T>
__aicore__ inline void asc_dump(__cbuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on L0C memory.
template<typename T>
__aicore__ inline void asc_dump(__cc__ T* input, uint32_t desc, uint32_t dump_size)
```

<!-- npu="950" id7 -->
The following APIs are the asc\_dump APIs used in simd\_vf, and are supported only on Ascend 950PR/Ascend 950DT.

```cpp
// Print data on the register.
template <typename T, typename U>
__simd_callee__ inline void asc_dump_reg(U& input, uint32_t desc, uint32_t dump_size)

// Print data on the register.
template <typename T, typename U>
__simd_callee__ inline void asc_dump(U& input, uint32_t desc, uint32_t dump_size)

// Print data on the UB memory.
template <typename T>
__simd_callee__ inline void asc_dump_ubuf(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)

// Print data on the UB memory.
template <typename T>
__simd_callee__ inline void asc_dump(__ubuf__ T* input, uint32_t desc, uint32_t dump_size)
```
<!-- end id7 -->

## Parameters

**Table 1** Parameters
| Name | Input/Output | Description |
| --- | --- | --- |
| T | Input | Data type to be dumped. |
| U | Input | Register data type to be dumped. |
| input | Input | Start address of the memory block to be printed. |
| desc | Input | Custom information. The type is uint32_t, and the supported data range is [0, 2^32 - 1]. |
| dump_size | Input | Number of elements to be printed. |

## Data Types
<!-- npu="950" id10 -->
- For Ascend 950PR/Ascend 950DT: bool, int8_t, uint8_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For Atlas A3 training products/Atlas A3 inference products: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For Atlas A2 training products/Atlas A2 inference products: bool, int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id12 -->

## Return Value

None

## Constraints

<!-- npu="950" id8 -->
-   For Ascend 950PR/Ascend 950DT, when using this API to print L1 Tensor data, the HDK version must be upgraded to at least 25.7.0 or later.
<!-- end id8 -->
-   When using this API, the total amount of data dumped on each core cannot exceed 30 KB. Developers must control the amount of content data to be printed; otherwise, the data will not be printed.
-   When calculating the data amount, if the total dump length is not aligned, the impact of padding data must be considered. During non-aligned dump, if the actual length of the dumped elements does not satisfy 32-byte alignment, the system automatically appends a certain amount of padding data at the end (this data is not printed) to satisfy the alignment requirement.
-   In SIMD scenarios, the total amount of data printed by a single call to this API cannot exceed the print size limit, which defaults to 30KB. Note that if this limit is exceeded, the data will not be printed. You can configure this through the `"simd_printf_fifo_size_per_core"` field in acl.json, with a configurable range from a minimum of 1KB to a maximum of 64MB (adjustable through the [aclInit](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/latest/API/runtimeapi/aclcppdevg_03_0022.html) API). When the amount of printed data is large, it is recommended to increase the buffer space. This configuration is not supported in pytorch invocation and operator graph compilation scenarios.

-   In the `simd_vf` scenario, each AIV core can use at most 2 KB of UB space during a single `asc_vf_call` execution; all `asc_dump` and `printf` calls of `simd_vf` within the same `asc_vf_call` share this reserved space.
-   Each call to `asc_dump` of `simd_vf` occupies a fixed 72 bytes of management information in addition to the actual dumped data; the actual dumped data must be aligned upward to 32 bytes.
-   The `simd_vf` debugging API does not check whether the reserved space described above is exceeded. Exceeding the limit may write out of bounds into the reserved space, causing abnormal print results and potentially affecting operator execution.

## Example

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
