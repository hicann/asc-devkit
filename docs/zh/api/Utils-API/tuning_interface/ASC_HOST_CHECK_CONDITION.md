# ASC_HOST_CHECK_CONDITION

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：支持
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

头文件路径为：`"utils/debug/check_condition.h"`。

用于检查Host侧条件是否成立。条件成立时，程序继续执行；条件不成立时，接口向标准错误流输出自定义错误信息、源文件名和行号，并调用C++标准库`<cassert>`中的`assert`宏进行断言检查。未定义`NDEBUG`时，断言失败通常会导致程序异常终止。

## 函数原型

```cpp
ASC_HOST_CHECK_CONDITION(condition, violation_message)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| condition | 输入 | 待检查的Host侧条件。条件为`true`时检查通过；条件为`false`时输出自定义错误信息，并调用C++标准库`<cassert>`中的`assert`宏。 |
| violation_message | 输入 | 条件不成立时输出的自定义错误信息。传入`nullptr`时，输出默认信息`No violation message provided.`。 |

## 返回值说明

无

## 约束说明

- 仅支持在Host侧代码中调用，不支持在Device侧代码中调用。
- `NDEBUG`宏用于控制接口内部`assert`断言的行为，可通过以下方式定义或取消定义：
    - 可通过编译选项`-DNDEBUG`或在包含`utils/debug/check_condition.h`之前增加`#define NDEBUG`定义`NDEBUG`。

        ```bash
        bisheng example.asc -o main --npu-arch=dav-2201 -DNDEBUG && ./main
        ```

    - 不设置`NDEBUG`相关编译选项且未在源代码中定义`NDEBUG`时，默认未定义`NDEBUG`。

        ```bash
        bisheng example.asc -o main --npu-arch=dav-2201 && ./main
        ```

- 当输入条件为`false`时，接口的行为与是否定义`NDEBUG`相关：
  - 定义了`NDEBUG`时，接口输出自定义错误信息后返回，后续代码会继续执行。此时多次调用`ASC_HOST_CHECK_CONDITION`（输入条件均为`false`）时，会依次输出多条自定义错误信息。在CMake工程的Release模式下，默认定义`NDEBUG`。
  - 未定义`NDEBUG`时，断言失败会导致程序异常终止。此时多次调用`ASC_HOST_CHECK_CONDITION`（输入条件均为`false`）时，只会输出首次调用对应的自定义错误信息。
- 接口内部使用已经求值的条件触发断言，条件表达式只求值一次。

<!-- npu="A3,910b" id8 -->
## 调用示例

将代码保存为`example.asc`后，可通过`bisheng`命令编译运行。本示例使用`asc_add`接口仅在[NPU架构版本2201](../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)下支持，因此编译命令中的`--npu-arch`参数固定为`dav-2201`。

编译运行命令如下：

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
    // 在申请设备内存后，使用`ASC_HOST_CHECK_CONDITION`检查设备指针。
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

以`src0_device`为空为例，接口输出的错误诊断格式如下：

```text
[ERROR] ASC_HOST_CHECK_CONDITION failed: src0_device is nullptr., file=example.asc, line=46
```

输出中的信息来源如下：

- `[ERROR] ASC_HOST_CHECK_CONDITION failed:`：接口固定输出的错误前缀，无需调用方提供。
- `src0_device is nullptr.`：由宏参数`violation_message`得到，即调用方传入的自定义错误信息。
- `example.asc`：发生条件检查的源文件。
- `46`：调用`ASC_HOST_CHECK_CONDITION`的源代码行号。假设将以上代码块保存为`example.asc`，并以第一行`#include`作为第1行，则第一次调用位于第46行；代码增删后行号会相应变化。
<!-- end id8 -->
