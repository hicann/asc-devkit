# aclrtcKernelSpecialization

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas训练系列产品：不支持
<!-- end id9 -->

## 功能说明

使用指定的常量参数进行常量化编译，并将生成的Kernel ELF文件保存至指定路径。

常量化编译将Tiling等参数值作为编译期常量，使编译器可进行常量传播、常量折叠和分支消除。此类优化可减少运行时的参数计算和条件判断，其性能收益可体现为Scalar执行耗时的降低。

## 函数原型

```cpp
aclError aclrtcKernelSpecialization(const aclrtcKernelSpecRequest *request, const char *outPath)
```

## 参数说明

**表1**  接口参数说明

| 参数名 | 输入/输出 | 描述 |
| :--- | :---: | :--- |
| request | 输入 | Kernel常量化编译请求，不能为空。各成员的配置要求请参见[表2](#table-aclrtc-kernel-spec-request)。 |
| outPath | 输入 | 特化Kernel ELF的输出文件路径，不能为空指针或空字符串。 |

### aclrtcKernelSpecRequest

`aclrtcKernelSpecRequest`定义位于`"acl/acl_rt_compile.h"`。

**表2**  aclrtcKernelSpecRequest成员说明<a id="table-aclrtc-kernel-spec-request"></a>

| 参数名 | 描述 |
| :--- | :--- |
| resourceId | 常量化编译资源标识，取资源Manifest中的`resource_id`，须精确匹配，不能为空指针或空字符串。Manifest为构建算子包时生成的常量化编译资源清单，获取方式请参见[生成Kernel Spec常量化编译资源](../../../guide/programming_guide/advanced_programming/aclnn_operator_development/compilation_and_deployment/basic_process.md#kernel-spec)。 |
| kernelEntry | 待常量化Kernel的原始入口名称，取资源Manifest中的`kernel_name`，须精确匹配。不能为空指针或空字符串。 |
| outputKernelEntry | 指出编译产物中的Kernel入口名称。传入`nullptr`或空字符串时，沿用`kernelEntry`。指定名称不得包含逗号或ASCII控制字符，且不能以`mix_aic`或`mix_aiv`结尾。 |
| argsCount | `argsAddr`的元素个数，包含用`nullptr`占位的元素。每个待常量化参数的下标均须小于此值。 |
| argsAddr | 数据地址数组，按`kernelEntry`指定的核函数（Kernel）的参数顺序填写，下标从0开始。对Manifest指定的待常量化参数，填写其实际数据在Host上的地址；其余位置可填`nullptr`。 |
| argsBytes | 数据长度数组，单位为字节，顺序与`argsAddr`一致。待常量化参数在Manifest中的`arg_type`为`struct`（默认值）时，填写实际数据长度，且须大于0；为`pointer`时，长度由Manifest指定。若所有待常量化参数的`arg_type`均为`pointer`，本参数可传入`nullptr`。 |
| options | 为本次常量化编译追加的编译选项。`optionCount`为0时，可传入`nullptr`；大于0时，须提供选项数组，且每个元素都须指向非空字符串。 |
| optionCount | `options`数组的元素个数。 |
| skOptions | SuperKernel编译选项数组，不用于链接。在数组中加入`--enable-super-kernel`可启用SuperKernel编译，其余选项用于编译SuperKernel对象。未设置该选项时，忽略数组中的其他选项。该选项只能设置一次。`skOptionCount`为0时，可传入`nullptr`。大于0时，须提供编译选项数组，且每个元素都须指向非空字符串。 |
| skOptionCount | `skOptions`数组的元素个数。 |
| reserved | 保留字段，当前版本必须全部置为0。 |

## 返回值说明

aclError为int类型变量，详细说明请参考[RTC错误码](RTC_error_code.md)。

## 约束说明

- 当前仅支持对通过aclnn接口调用的算子进行常量化编译，不支持通过`<<<>>>`语法直接调用的Kernel。
- 当前不支持对包含可选参数（Optional）的算子进行常量化编译。
- 调用前须确保`resourceId`对应的常量化编译资源已注册且可被运行时发现，`kernelEntry`在该资源中存在，参数数据类型及布局与该资源的常量参数声明一致。
- `request`及其引用的字符串、数组和参数数据须在本次同步调用期间保持有效且不可修改。调用返回后可释放请求数组和常量参数缓冲区，接口不会替调用方释放这些内存。
- 待常量化参数的Host缓冲区可读长度须不小于接口读取的数据长度。
- `resourceId`、`kernelEntry`、`outPath`及各编译选项均须为空字符结尾的字符串；指定`outputKernelEntry`时，该字段也须为空字符结尾的字符串。每个字符串内容长度不超过1MiB，全部字符串内容的累计长度不超过4MiB，长度均不计结尾空字符。
- 每个编译选项字符串作为一个完整参数传递，不按空格拆分。
- 调用期间，调用方不应读取或替换outPath指定的输出文件。
- 编译资源须在调用前通过[CMake接口](../../../guide/programming_guide/advanced_programming/aclnn_operator_development/appendix/cmake_function_reference.md#npu_op_package)配置并在算子包构建时生成。本接口仅使用该资源进行常量化编译，并输出生成的ELF文件。

## 调用示例

以下片段展示MatmulCustom样例的常量化编译请求配置。MatmulCustom有5个Kernel参数，仅将第5个参数`tiling`作为常量参数。调用前按样例准备编译资源，从其Manifest获取`resource_id`和`kernel_name`，分别传入`resourceId`和`kernelEntry`；使用Matmul Tiling API为`M = N = K = 512`生成Tiling参数，并通过`SaveToBuffer`写入Host侧的`tiling`缓冲区。示例通过`options`分别传入`-g`和`-fno-ident`，用于生成调试信息并禁止生成编译器标识信息。

```cpp
#include "acl/acl.h"
#include "acl/acl_rt_compile.h"
#include "tiling/tiling_api.h"
#include <array>
#include <cstdio>

aclError SpecializeMatmulCustom(const char* resourceId, const char* kernelEntry, const char* outPath,
    const std::array<uint8_t, sizeof(AscendC::tiling::TCubeTiling)>& tiling)
{
    // 仅传入第5个参数的Host数据，其他位置用nullptr占位。
    const void* argsAddr[] = {nullptr, nullptr, nullptr, nullptr, tiling.data()};
    const uint64_t argsBytes[] = {0, 0, 0, 0, sizeof(tiling)};

    const char* options[] = {"-g", "-fno-ident"};
    aclrtcKernelSpecRequest request{};
    request.resourceId = resourceId;
    request.kernelEntry = kernelEntry;
    request.argsCount = 5;
    request.argsAddr = argsAddr;
    request.argsBytes = argsBytes;
    request.options = options;
    request.optionCount = 2;

    aclError ret = aclrtcKernelSpecialization(&request, outPath);
    if (ret != ACL_SUCCESS) {
        std::fprintf(stderr, "aclrtcKernelSpecialization failed: %d\n", ret);
    }
    return ret;
}
```
