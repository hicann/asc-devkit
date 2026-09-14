# asc_dcci

<!-- md-trans-meta sourceCommit=0ece502ce5e30f12c91b14fc87b28b1b051cb796 translatedAt=2026-08-26T10:26:43.816Z -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->

## Description

Inside the AI Core, both the Scalar unit and the DMA unit can access Global Memory.

![](../figures/dcci.png "Data Cache memory hierarchy diagram") \\

As shown in the preceding figure:

The DMA unit reads from and writes to Global Memory. Data is exchanged between Local Memory such as UB and Global Memory through APIs such as [asc_copy_ub2gm](../vector_datamove/asc_copy_ub2gm/asc_copy_ub2gm.md) and [asc_copy_gm2ub](../vector_datamove/asc_copy_gm2ub/asc_copy_gm2ub.md), with no cache coherence issue. When the Scalar unit accesses Global Memory, it first accesses the Data Cache in each core, so a cache coherence issue exists between the Data Cache and Global Memory. This API is used to flush the cache and ensure cache coherence. Its usage scenarios are as follows:

When reading data from Global Memory that may have been modified externally by other cores, use the **asc_dcci** API to directly access Global Memory and obtain the latest data. When the user writes data to Global Memory through the Scalar unit and wants the data to be written out immediately, the **asc_dcci** API is also required.

When the Scalar unit accesses UB data, this API must be used together with the [asc_set_ctrl()](../sys_var/asc_set_ctrl.md) API to set CTRL[49] to 1'b1 and enable the datacache mode.

<!-- npu="950" id4 -->
For Ascend 950PR/Ascend 950DT, the **asc_dcci_entire_ub** API is not supported.
<!-- end id4 -->

## Prototype

   ```cpp
   // Flush the single cache line where the passed address resides.
   __aicore__ inline void asc_dcci_single(__gm__ void* dst)

   // Flush the single cache line where the passed UB address space address resides.
   __aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst)

   // Flush all cache lines.
   __aicore__ inline void asc_dcci_entire_all()
   __aicore__ inline void asc_dcci_entire_ub()
   __aicore__ inline void asc_dcci_entire_out()
   __aicore__ inline void asc_dcci_entire_atomic()
   ```

## Parameters

| Parameter | Input/Output | Description |
| :--- | :--- | :--- |
| dst | Input | Start address of the vector whose cache needs to be refreshed. |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

None

## Example

```cpp
// Example: SINGLE_CACHE_LINE mode (x_gm 64B-aligned scenario).
// Assume that x is an externally passed address, each core computes 16 data elements, block_idx is a built-in variable, and the offset is calculated through block_idx to achieve data isolation across multiple cores.
__gm__ float* x_gm = x + block_idx * 16;
for (int i = 0; i < 16; i++) {
    x_gm[i] = static_cast<float>(i + block_idx);
}
// Because the start address is 64B-aligned, after calling the asc_dcci_single instruction, the data is immediately flushed to Global Memory.
asc_dcci_single(reinterpret_cast<__gm__ uint64_t*>(x_gm));
```
