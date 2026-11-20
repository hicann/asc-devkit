---
okftype: reference.api
description: "Configuration API that sets the Mask value: in Normal mode, it controls the elements participating in computation within a single iteration bit by bit; in Counter mode, the low-order input parameter specifies the number of elements participating in computation for the entire computation process. On 950, it is used to configure the mask register outside VF scenarios, with pipeline type PIPE_S."
tag:
  - roles: [configuration]
dependencies:
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_mask/asc_get_mask_spr.md
    kind: related
    condition: "On 950, the mask register value is obtained through the b16/b32 forms of this API."
  - path: docs/zh/api/SIMD-API/c_api/reg_compute/reg_mask/reg_mask.md
    kind: related
    condition: "Recommended API section for Reg vector compute mask configuration"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_mask_config/asc_set_mask_count.md
    kind: related
    condition: "On A3/A2, call this API to switch the mode before using Counter mode"
  - path: docs/zh/api/SIMD-API/c_api/vector_compute/vector_mask_config/asc_set_mask_norm.md
    kind: related
    condition: "On A3/A2, call this API to switch the mode before using Normal mode"
version: ">=8.5.0"
version_changes:
  - since: "9.2.0"
    kind: arch_support_extended
    scope: "page"
    scope_basis: kind_default
    note: "Added support for Ascend 950PR&950DT (NPU architecture 3510): mask registers can be configured outside Vector Function and used with Reg vector compute APIs, with values read through the b16/b32 forms of asc_get_mask_spr; based on CANN_9_2_0 Release Notes line 32 and the @archs of this declaration in include/c_api/vector_compute/compute/vector_mask_config.h extended from 2201 to 2201 and 3510."
---
# asc_set_vector_mask

<!-- md-trans-meta sourceCommit=54f1d966cfcc22290ddc2c6312bbe4c6ac2a9232 translatedAt=2026-11-16T01:40:03.930104+00:00 pushedAt=2026-11-16T02:10:59.077Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

## Description

The header file path is `"c_api/vector_compute/compute/vector_mask_config.h"`.

This API is used to set the mask value. Some vector computation APIs require the mask to be set in advance to indicate which data participates in the computation. When using this API alone, include `"c_api/vector_compute/compute/vector_mask_config.h"`; when using the C API as a whole, include `"c_api/asc_simd.h"`.

<!-- npu="950" id8 -->
- For Ascend 950PR&950DT products: This API is typically used to set the mask register outside the VF scenario, in conjunction with Reg vector computation APIs.
<!-- end id8 -->

<!-- npu="A3,910b" id9 -->
- For the following product models:

  <!-- npu="A3" id10 -->
  Atlas A3 products
  <!-- end id10 -->
  <!-- npu="910b" id11 -->
  Atlas A2 products
  <!-- end id11 -->

  This API supports Normal mode and Counter mode. Before use, you must first call [asc_set_mask_count](asc_set_mask_count.md) or [asc_set_mask_norm](asc_set_mask_norm.md) to set the mask mode. The meaning of Mask in different modes is as follows:

  - Normal mode: used for high-dimensional slicing computation APIs. In this mode, the Mask parameter controls the elements that participate in the computation within a single iteration. It controls which elements participate in the computation on a bit-by-bit basis: a bit value of 1 indicates participation in the computation, and 0 indicates non-participation. The Mask parameter is divided into mask_high (high-order mask) and mask_low (low-order mask). The value range of the parameter is related to the data type of the operand. Different data types have different maximum numbers of elements that can be processed in each iteration. When the operand is 16-bit, mask_high and mask_low ∈ $[0, 2^{64}-1]$, and they cannot both be 0. When the operand is 32-bit, mask_high is 0 and mask_low ∈ $(0, 2^{64}-1]$. When the operand is 64-bit, mask_high is 0 and mask_low ∈ $(0, 2^{32}-1]$.
  - Counter mode: used for the first-n data computation APIs. In this mode, the Mask parameter controls the number of elements that participate in the computation during the entire computation process. The first-n data computation APIs already implement the conversion between Normal mode and Counter mode, so users do not need to configure it themselves.
<!-- end id9 -->

## Prototype

```cpp
__aicore__ inline void asc_set_vector_mask(uint64_t mask_high, uint64_t mask_low)
```

## Parameters

<!-- npu="950" id12 -->
**Table 1** Parameter description ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mask_high | Input | High-order 64-bit mask value. |
| mask_low | Input | Low-order 64-bit mask value. |
<!-- end id12 -->

<!-- npu="A3,910b" id13 -->
**Table 2** Parameter description ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))

| Parameter | Input/Output | Description |
| --- | --- | --- |
| mask_high | Input | Normal mode: high-order mask value.<br>Counter mode: Set to 0; this input parameter does not take effect. |
| mask_low | Input | Normal mode: low-order mask value.<br>Counter mode: number of elements that participate in the computation during the entire vector computation process. |
<!-- end id13 -->

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

<!-- npu="950" id14 -->
- For Ascend 950PR&950DT products: This API is used to set the mask register outside the VF scenario. When configuring Reg vector computation, use `asc_get_mask_spr_b16` or `asc_get_mask_spr_b32` in the [asc_get_mask_spr](../../reg_compute/reg_mask/asc_get_mask_spr.md) API to obtain the mask register value. Users are recommended to use the APIs in the [Mask Register Operations](../../reg_compute/reg_mask/reg_mask.md) section within the VF scenario to complete Reg vector computation mask configuration.
<!-- end id14 -->

<!-- npu="A3,910b" id15 -->
- For the following product models:

  <!-- npu="A3" id16 -->
  Atlas A3 products
  <!-- end id16 -->
  <!-- npu="910b" id17 -->
  Atlas A2 products
  <!-- end id17 -->

  You must first call [asc_set_mask_count](asc_set_mask_count.md) or [asc_set_mask_norm](asc_set_mask_norm.md) to set the mask mode.
<!-- end id15 -->

## Examples

Save the code as `example.asc` and compile and run it with the `bisheng` command, where the `--npu-arch` parameter must specify the corresponding NPU architecture based on the actual product model. For the mapping between specific products and NPU architectures, see [\_\_NPU\_ARCH\_\_](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch).

<!-- npu="950" id18 -->
- Taking the Ascend 950PR&950DT products (corresponding to the NPU architecture `dav-3510`) as an example, the compilation and execution command is as follows:

  ```bash
  bisheng example.asc -o main --npu-arch=dav-3510 && ./main
  ```

  The following example code is supported only by Ascend 950PR&950DT products. The example calls this API outside the VF scenario to set the mask register (all 1s in the low 64 bits and all 0s in the high 64 bits, meaning the first 64 elements participate in the computation in b16 mode), reads the mask value through [asc_get_mask_spr](../../reg_compute/reg_mask/asc_get_mask_spr.md) within the VF scenario, and then completes Reg vector computation. Ultimately, only the first 64 elements are written as the broadcast value.

  ```cpp
  #include <cstdint>
  #include <iostream>
  #include <vector>
  #include "c_api/asc_simd.h"
  #include "acl/acl.h"

  namespace {

  constexpr uint32_t ELEMENTS = 128; // In b16 mode, a single vector computation processes 128 int16_t elements.
  constexpr uint32_t BYTES = ELEMENTS * sizeof(int16_t);

  __simd_vf__ inline void exercise(__ubuf__ int16_t* output)
  {
      // Read the mask register value set by asc_set_vector_mask outside the VF scenario.
      vector_bool mask16 = asc_get_mask_spr_b16();
      vector_int16_t value16;
      // Broadcast 2 to the elements selected by the mask, and set the unselected elements to 0.
      asc_duplicate_scalar(value16, static_cast<int16_t>(2), mask16);
      // Write the elements selected by the mask to the UB, and keep the original UB values for the unselected elements.
      asc_storealign(output, value16, mask16);
  }

  __global__ __vector__ void asc_set_vector_mask_kernel(__gm__ int16_t* output, __gm__ int16_t* input)
  {
      asc_init();
      __ubuf__ int16_t output_local[ELEMENTS];
      asc_copy_gm2ub_align(output_local, input, BYTES);
      asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
      asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);
      // Set the mask register outside the VF scenario: all 1s in the low 64 bits and all 0s in the high 64 bits, meaning the first 64 elements participate in the computation.
      asc_set_vector_mask(0, 0xffffffffffffffff);
      exercise(output_local);
      asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
      asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
      asc_copy_ub2gm_align(output, output_local, BYTES);
      asc_sync();
  }
  } // namespace

  int main()
  {
      std::vector<int16_t> input(ELEMENTS), output(ELEMENTS), golden(ELEMENTS);
      for (uint32_t i = 0; i < ELEMENTS; ++i) {
          input[i] = static_cast<int16_t>(i);
          // The first 64 elements are written with 2, and the last 64 elements keep the input values.
          golden[i] = (i < 64) ? static_cast<int16_t>(2) : static_cast<int16_t>(i);
      }
      aclInit(nullptr);
      aclrtSetDevice(0);
      int16_t* input_device = nullptr;
      aclrtMalloc(reinterpret_cast<void**>(&input_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
      int16_t* output_device = nullptr;
      aclrtMalloc(reinterpret_cast<void**>(&output_device), BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
      aclrtMemcpy(input_device, BYTES, input.data(), BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
      asc_set_vector_mask_kernel<<<1, 0>>>(output_device, input_device);
      aclrtSynchronizeDevice();
      aclrtMemcpy(output.data(), BYTES, output_device, BYTES, ACL_MEMCPY_DEVICE_TO_HOST);
      const bool passed = output == golden;
      std::cout << (passed ? "[Success] asc_set_vector_mask passed." : "[Failed] asc_set_vector_mask failed.")
                << std::endl;
      aclrtFree(input_device);
      aclrtFree(output_device);
      aclrtResetDevice(0);
      aclFinalize();
      return passed ? 0 : 1;
  }
  ```
<!-- end id18 -->

<!-- npu="A3,910b" id19 -->
- For the following product models:

  <!-- npu="A3" id20 -->
  Atlas A3 products
  <!-- end id20 -->
  <!-- npu="910b" id21 -->
  Atlas A2 products
  <!-- end id21 -->

  See [asc_set_mask_count](asc_set_mask_count.md#examples) or [asc_set_mask_norm](asc_set_mask_norm.md#examples).
<!-- end id19 -->
