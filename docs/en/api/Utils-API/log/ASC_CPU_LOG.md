# ASC\_CPU\_LOG<a name="ZH-CN_TOPIC_0000002466921669"></a>

<!-- md-trans-meta sourceCommit=dc56b6592be9a41ee18c3bfdf6bd8116a9020128 translatedAt=2026-09-06T12:43:37.941Z pushedAt=2026-09-11T07:38:04.901Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT: supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 training products/Atlas A3 inference products: supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 training products/Atlas A2 inference products: supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: supported
<!-- end id9 -->

## Description<a name="section618mcpsimp"></a>

Provides the function of printing logs on the host side. Developers can use the ASC\_CPU\_LOG\_XXX API in the TilingFunc code of an operator to output relevant content. In general, developers can also choose common host-side printing methods such as printf for debugging. However, in the Tiling offloading scenario, because the Tiling function runs on the AI CPU, this API must be used for printing.

-   In non-Tiling offloading scenarios, logs are output to plog. For example, debug-level logs are written to /root/ascend/log/debug/plog, with the log level controlled by the environment variable ASCEND_GLOBAL_LOG_LEVEL. Each log entry includes the log level, timestamp, source code line number, and function name.
-   In the Tiling offloading scenario, logs are not output to plog. Instead, they need to be dumped to disk and parsed. Before the operator runs, the Dump function must be enabled so that the log Dump function takes effect. How to enable the Dump function depends on the specific network running mode. Taking the TorchAir graph mode as an example, Dump parameters such as enable\_dump, dump\_path, and dump\_mode need to be configured. For details, see [operator data dump](https://www.hiascend.com/document/detail/en/Pytorch/2610/devguide/TorchAir/docs/en/ascend_ir/features/advanced/data_dump.md). The example is as follows:

    ```
    import torch_npu, torchair
    config = torchair.CompilerConfig()
    # Data dump switch: [Mandatory]
    config.dump_config.enable_dump = True
    # Dump type: [Optional], where all means dumping all data.
    config.dump_config.dump_mode = "all"
    # Dump path: [Optional], defaulting to the current directory.
    config.dump_config.dump_path = '/home/dump'
    ...
    ```

    After the operator finishes running, a log Dump file is generated in the Dump data storage path. The file naming rule is _\{op\_type\}.\{op\_name\}.\{taskid\}.\{stream\_id\}.\{timestamp\}_, where _\{op\_type\}_ indicates the operator type, _\{op\_name\}_ indicates the operator name, _\{taskid\}_ indicates the taskId for calling the operator computation API, _\{stream\_id\}_ indicates the stream ID on which the operator is actually executed, and _\{timestamp\}_ indicates the timestamp.

## Required Header File<a name="section12341115212912"></a>

```
#include "utils/log/asc_cpu_log.h"
```

## Function Prototype<a name="section620mcpsimp"></a>

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
| **format** | Input | Format control string, which contains two types of content: ordinary characters and conversion specifications.<br>- Ordinary characters are output directly.<br>- Conversion specifications control the formatted output of parameters. Each conversion specification starts with a percent sign (%) followed by a type specifier, which specifies the type of the output data. The supported data types are consistent with the C/C++ specification. |
| **...** | Input | Additional parameters, a variable-length parameter list with variable number and types. The number and types must match the number and types of the % tags in the format control string. Each parameter replaces the corresponding % tag in the format string to achieve the expected output. |

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

In the Tiling offloading scenario, if a custom operator project generated with an older CANN package (which does not support the ASC\_CPU\_LOG API) is used, pay special attention to compatibility issues, because calling this API in this case cannot output logs. You can check whether cmake/device\_task.cmake in the custom operator project contains the DEVICE\_OP\_LOG\_BY\_DUMP field to confirm whether the current project supports the log dump function. If the field is not found, regenerate the custom operator project.

## Example<a name="section837496171220"></a>

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

In the non-Tiling offloading scenario, the logs are output to xxxxxxx\_2025xxxxxxxxxxxxx.log. The result example is as follows:

```
[ERROR] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am ERROR log: 291
[INFO] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am INFO log: 291
[WARNING] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am WARNING log: 291
[DEBUG] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am DEBUG log: 291
```

