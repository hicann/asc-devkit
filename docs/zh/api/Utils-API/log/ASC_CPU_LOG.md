# ASC\_CPU\_LOG<a name="ZH-CN_TOPIC_0000002466921669"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas 训练系列产品：支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

提供Host侧打印Log的功能。开发者可以在算子的TilingFunc代码中使用ASC\_CPU\_LOG\_XXX接口来输出相关内容。一般情况下，开发者也可以选择使用printf等Host侧通用的打印方式进行调试。然而，在Tiling下沉场景中，由于Tiling函数运行在AI CPU上，必须使用本接口进行打印。

-   非Tiling下沉场景，日志输出到plog中。比如，debug级别的日志输出到/root/ascend/log/debug/plog中，日志级别通过环境变量ASCEND_GLOBAL_LOG_LEVEL控制。会打印日志级别、时间戳、日志所在代码行和日志所在函数名。
-   Tiling下沉场景，日志不会输出到plog中，而是需要落盘并进行解析。算子运行之前需要开启Dump功能，使得日志Dump功能生效。如何开启Dump功能依赖于具体的网络运行方式。以TorchAir图模式为例，需要配置enable\_dump、dump\_path、dump\_mode等Dump参数。详细说明可参考[算子data dump功能](https://www.hiascend.com/document/detail/zh/Pytorch/latest/devguide/TorchAir/docs/zh/ascend_ir/features/advanced/data_dump.md)。示例如下：

    ```
    import torch_npu, torchair
    config = torchair.CompilerConfig()
    # data dump开关：[必选]
    config.dump_config.enable_dump = True
    # dump类型：[可选]，all代表dump所有数据
    config.dump_config.dump_mode = "all"
    # dump路径：[可选]，缺省为当前目录
    config.dump_config.dump_path = '/home/dump'
    ...
    ```

    算子运行完成后，在Dump数据存放路径下会有日志Dump文件生成，文件名命名规则格式为_\{op\_type\}.\{op\_name\}.\{taskid\}.\{stream\_id\}.\{timestamp\}_，其中_\{op\_type\}_表示算子类型，_\{op\_name\}_表示算子名称，_\{taskid\}_表示调用算子计算接口的taskId，_\{stream\_id\}_表示算子具体执行的流Id，_\{timestamp\}_表示时间戳。

## 需要包含的头文件<a name="section12341115212912"></a>

```
#include "utils/log/asc_cpu_log.h"
```

## 函数原型<a name="section620mcpsimp"></a>

```
#define ASC_CPU_LOG_ERROR(format, ...)
#define ASC_CPU_LOG_INFO(format, ...)
#define ASC_CPU_LOG_WARNING(format, ...)
#define ASC_CPU_LOG_DEBUG(format, ...)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
|--------|----------|------|
| format | 输入 | 格式控制字符串，包含两种类型：普通字符和转换说明。<br>- 普通字符将直接输出。<br>- 转换说明用于控制参数的格式化输出。每个转换说明以百分号（%）开始，后跟类型说明符，用于指定输出数据的类型。支持的数据类型和C/C++规范保持一致。 |
| ... | 输入 | 附加参数，数量和类型可变的参数列表。其数量和类型需与格式控制字符串中的%标签数量和类型匹配。每个参数将替换格式字符串中的相应%标签，以实现预期的输出效果。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

Tiling下沉场景下，若使用旧版本CANN包（不支持ASC\_CPU\_LOG接口）生成的自定义算子工程，需特别注意兼容性问题，此时调用该接口无法输出日志。您可以通过查看自定义算子工程下cmake/device\_task.cmake中有无DEVICE\_OP\_LOG\_BY\_DUMP字段来确认当前工程是否支持日志Dump功能，如果未找到该字段，则需要重新生成自定义算子工程。

## 调用示例<a name="section837496171220"></a>

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

非Tiling下沉场景，打印会输出至xxxxxxx\_2025xxxxxxxxxxxxx.log中，结果示例如下：

```
[ERROR] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am ERROR log: 291
[INFO] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am INFO log: 291
[WARNING] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am WARNING log: 291
[DEBUG] ASCENDCKERNEL(xxx,execute_add_op):2025-xx-xx-xx:xx:xx.xxx.xxx [/xxx/xxx.cpp:xx][TilingFunc] I am DEBUG log: 291
```
