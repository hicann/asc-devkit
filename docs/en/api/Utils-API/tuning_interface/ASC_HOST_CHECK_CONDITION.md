# ASC_HOST_CHECK_CONDITION

<!-- md-trans-meta sourceCommit=b31f8bdaed16db5f3a14393c0665e0940dcd6f52 translatedAt=2026-11-13T08:59:03.700Z pushedAt=2026-11-16T07:07:41.590Z nl=2 -->

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

The header file path is `"utils/debug/check_condition.h"`.

Used to check whether the Host-side condition is met. When the condition is met, the program continues execution; when the condition is not met, the API outputs a custom error message, the source file name, and the line number to the standard error stream, and calls the `assert` macro in the C++ standard library `<cassert>` for assertion checking. When `NDEBUG` is undefined, an assertion failure usually causes the program to terminate abnormally.

## Prototype

```cpp
ASC_HOST_CHECK_CONDITION(condition, violation_message)
```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| condition | Input | Host-side condition to be checked. When the condition is `true`, the check passes; when the condition is `false`, a custom error message is output and the `assert` macro in the C++ standard library `<cassert>` is called. |
| violation_message | Input | Custom error message output when the condition is not met. When `nullptr` is passed in, the default message `No violation message provided.` is output. |

## Return Value

None

## Constraints

- Only supported for calls in Host-side code, not supported for calls in Device-side code.
- The `NDEBUG` macro is used to control the behavior of the `assert` assertion inside the API. It can be defined or undefined in the following ways:
    - `NDEBUG` can be defined through the compilation option `-DNDEBUG` or by adding `#define NDEBUG` before including `utils/debug/check_condition.h`.

        ```bash
        bisheng example.asc -o main --npu-arch=dav-2201 -DNDEBUG && ./main
        ```

    - When no `NDEBUG`-related compilation option is set and `NDEBUG` is not defined in the source code, `NDEBUG` is undefined by default.

        ```bash
        bisheng example.asc -o main --npu-arch=dav-2201 && ./main
        ```

- When the input condition is `false`, the behavior of the API is related to whether `NDEBUG` is defined:
  - When `NDEBUG` is defined, the API outputs the custom error message and then returns, and subsequent code continues execution. At this time, multiple calls to `ASC_HOST_CHECK_CONDITION` (all input conditions are `false`) output multiple custom error messages in sequence. In the Release mode of a CMake project, `NDEBUG` is defined by default.
  - When `NDEBUG` is undefined, an assertion failure causes the program to terminate abnormally. At this time, multiple calls to `ASC_HOST_CHECK_CONDITION` (all input conditions are `false`) output only the custom error message corresponding to the first call.
- Inside the API, the already-evaluated condition is used to trigger the assertion, and the condition expression is evaluated only once.

<!-- npu="A3,910b" id8 -->
## Examples

After saving the code as `example.asc`, it can be compiled and run through the `bisheng` command. The `asc_add` API used in this example is supported only under [NPU architecture version 2201](../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch), so the `--npu-arch` parameter in the compilation command is fixed to `dav-2201`.

The compile and run command is as follows:

```bash
bisheng example.asc -o main --npu-arch=dav-2201 && ./main
```

```cpp
#include <iostream>
#include <vector>

#include "acl/acl.h"
#include "c_api/asc_simd.h"
#include "utils/debug/check_condition.h"

constexpr uint64_t TOTAL_LENGTH = 64;
constexpr uint64_t DATA_BYTES = TOTAL_LENGTH * sizeof(half);

__global__ __vector__ void add_kernel(__gm__ half* dst, __gm__ half* src0, __gm__ half* src1)
{
    asc_init();
    __ubuf__ half src0_ub[TOTAL_LENGTH];
    __ubuf__ half src1_ub[TOTAL_LENGTH];
    __ubuf__ half dst_ub[TOTAL_LENGTH];

    asc_copy_gm2ub_align(src0_ub, src0, DATA_BYTES);
    asc_copy_gm2ub_align(src1_ub, src1, DATA_BYTES);
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    asc_add(dst_ub, src0_ub, src1_ub, TOTAL_LENGTH);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_ub, DATA_BYTES);
    asc_sync();
}

int main()
{
    std::vector<half> src0(TOTAL_LENGTH, half(1.0f));
    std::vector<half> src1(TOTAL_LENGTH, half(2.0f));
    std::vector<half> dst(TOTAL_LENGTH, half(0.0f));

    aclInit(nullptr);
    aclrtSetDevice(0);

    half* src0_device = nullptr;
    half* src1_device = nullptr;
    half* dst_device = nullptr;
    aclrtMalloc(reinterpret_cast<void**>(&src0_device), DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    // After applying for device memory, use `ASC_HOST_CHECK_CONDITION` to check the device pointer.
    ASC_HOST_CHECK_CONDITION(src0_device != nullptr, "src0_device is nullptr.");
    aclrtMalloc(reinterpret_cast<void**>(&src1_device), DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    ASC_HOST_CHECK_CONDITION(src1_device != nullptr, "src1_device is nullptr.");
    aclrtMalloc(reinterpret_cast<void**>(&dst_device), DATA_BYTES, ACL_MEM_MALLOC_HUGE_FIRST);
    ASC_HOST_CHECK_CONDITION(dst_device != nullptr, "dst_device is nullptr.");

    aclrtMemcpy(src0_device, DATA_BYTES, src0.data(), DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1_device, DATA_BYTES, src1.data(), DATA_BYTES, ACL_MEMCPY_HOST_TO_DEVICE);
    add_kernel<<<1, 0>>>(dst_device, src0_device, src1_device);
    aclrtSynchronizeDevice();
    aclrtMemcpy(dst.data(), DATA_BYTES, dst_device, DATA_BYTES, ACL_MEMCPY_DEVICE_TO_HOST);

    aclrtFree(dst_device);
    aclrtFree(src1_device);
    aclrtFree(src0_device);
    aclrtResetDevice(0);
    aclFinalize();
    return 0;
}
```

Taking `src0_device` being null as an example, the error diagnostic format output by the API is as follows:

```text
[ERROR] ASC_HOST_CHECK_CONDITION failed: src0_device is nullptr., file=example.asc, line=46
```

The sources of the information in the output are as follows:

- `[ERROR] ASC_HOST_CHECK_CONDITION failed:`: the fixed error prefix output by the API, which does not need to be provided by the caller.
- `src0_device is nullptr.`: obtained from the macro parameter `violation_message`, that is, the custom error message passed in by the caller.
- `example.asc`: the source file where the condition check occurs.
- `46`: the source code line number where `ASC_HOST_CHECK_CONDITION` is called. Assuming the above code block is saved as `example.asc` and the first line `#include` is counted as line 1, the first call is located at line 46; the line number changes accordingly after code is added or deleted.
<!-- end id8 -->
