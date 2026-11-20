# ASC\_CPU\_LOG<a name="ZH-CN_TOPIC_0000002466921669"></a>

<!-- md-trans-meta sourceCommit=13f1af5a09ac01160909f73c31bb7f8d23f4ec30 translatedAt=2026-11-10T10:30:35.890Z pushedAt=2026-11-13T08:46:49.260Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Supported
<!-- end id9 -->


<!-- @ref: asc-devkit/res/docs/zh/api/Utils-API/log/ASC_CPU_LOG_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Provides the function of printing logs on the host side. You can use the ASC\_CPU\_LOG\_XXX API in the TilingFunc code of an operator to output relevant content. In general, you can also use printf and other common host-side printing methods for debugging. However, in the Tiling offloading scenario, because the Tiling function runs on the AI CPU, you must use this API for printing.

-   In the non-Tiling offloading scenario, logs are output to plog. For example, debug-level logs are output to /root/ascend/log/debug/plog, and the log level is controlled by the environment variable ASCEND_GLOBAL_LOG_LEVEL. The log level, timestamp, code line and function name where the log is located are printed.
-   In the Tiling offloading scenario, logs are not output to plog. Instead, they must be written to disk and then parsed. Before the operator runs, enable the Dump function so that the log Dump function takes effect. How to enable the Dump function depends on the specific network running mode. Taking the TorchAir graph mode as an example, configure Dump parameters such as enable\_dump, dump\_path, and dump\_mode. For details, see [operator data dump](https://gitcode.com/Ascend/torchair/blob/master/docs/zh/ascend_ir/features/advanced/data_dump.md). The following is an example:

    ```
    import torch_npu, torchair
    config = torchair.CompilerConfig()
    # data dump switch: [Mandatory]
    config.dump_config.enable_dump = True
    # dump type: [Optional]. all means dumping all data.
    config.dump_config.dump_mode = "all"
    # dump path: [Optional]. Defaults to the current directory.
    config.dump_config.dump_path = '/home/dump'
    ...
    ```

    After the operator finishes running, a log Dump file is generated in the Dump data storage path. The file naming rule is _\{op\_type\}.\{op\_name\}.\{taskid\}.\{stream\_id\}.\{timestamp\}_, where _\{op\_type\}_ indicates the operator type, _\{op\_name\}_ indicates the operator name, _\{taskid\}_ indicates the taskId for calling the operator compute API, _\{stream\_id\}_ indicates the stream ID on which the operator is actually executed, and _\{timestamp\}_ indicates the timestamp.

## Header Files to Include<a name="section12341115212912"></a>

```
#include "utils/log/asc_cpu_log.h"
```

## Prototype<a name="section620mcpsimp"></a>

```
#define ASC_CPU_LOG_ERROR(format, ...)
#define ASC_CPU_LOG_INFO(format, ...)
#define ASC_CPU_LOG_WARNING(format, ...)
#define ASC_CPU_LOG_DEBUG(format, ...)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1**  Parameters

| Parameter | Input/Output | Description |
|--------|----------|------|
| format | Input | Format control string, which contains two types of content: ordinary characters and conversion specifications.<br>- Ordinary characters are output directly.<br>- Conversion specifications control the formatted output of parameters. Each conversion specification starts with a percent sign (%) followed by a type specifier that specifies the type of the output data. The supported data types are consistent with the C/C++ specification. |
| ... | Input | Additional parameters, a variable-length parameter list. The number and types must match the number and types of the % tags in the format control string. Each parameter replaces the corresponding % tag in the format string to achieve the intended output. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

In the Tiling offloading scenario, if you use a custom operator project generated with an earlier CANN package (which does not support the ASC\_CPU\_LOG API), pay special attention to compatibility issues. In this case, calling the API does not output logs. You can check whether the DEVICE\_OP\_LOG\_BY\_DUMP field exists in cmake/device\_task.cmake of the custom operator project to determine whether the current project supports the log dump feature. If the field is not found, regenerate the custom operator project.

## Examples<a name="section837496171220"></a>

```
#include "utils/log/asc_cpu_log.h"

namespace optiling {
static ge::graphStatus TilingFunc(gert::TilingContext *context)
{
    TilingData tiling;
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    ...
    ASC_CPU_LOG_ERROR("I am ERROR log: %d\n", 0x123);
    ASC_CPU_LOG_INFO("I am INFO log: %d\n", 0x123);
    ASC_CPU_LOG_WARNING("I am WARNING log: %d\n", 0x123);
    ASC_CPU_LOG_DEBUG("I am DEBUG log: %d\n", 0x123);
    ...
}
} // namespace optiling
```

In a non-Tiling offloading scenario, the output is written to xxxxxxx\_2025xxxxxxxxxxxxx.log. The result is as follows:

```
[ERROR] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am ERROR log: 291
[INFO] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am INFO log: 291
[WARNING] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am WARNING log: 291
[DEBUG] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am DEBUG log: 291
```
