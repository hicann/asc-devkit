# simplified key二进制配置JSON

本文介绍在自定义算子工程中如何提供核函数（Kernel）二进制配置JSON，以及该JSON如何驱动按自定义simplified key编译多份binary。该机制适用于一个OpType需要按dtype、format、shape分档或属性组合预编译多份binary，并由运行时根据`GenSimplifiedKey`回调选择的场景。

回调的编写和注册方式见[GenSimplifiedKey回调实现](../design_and_implementation/gen_simplified_key.md)，运行时选择过程见[基于simplified key的运行时选择流程](../invocation/simplified_key_runtime_flow.md)。

## JSON如何提供

将JSON放在算子工程现有的Host侧配置目录中，并按AI处理器型号划分。例如`AddCustomTemplate`样例使用：

```text
op_host/config/ascendxxx/add_custom_template_binary.json
```

在`op_kernel/CMakeLists.txt`中通过`npu_op_kernel_options()`传递`--kernel-json-file`。该参数属于编译选项，不写入`npu_op_kernel_sources()`：

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustomTemplate
    KERNEL_DIR add_custom_template
    KERNEL_FILE add_custom_template.cpp
)

npu_op_kernel_options(ascendc_kernels AddCustomTemplate
    OPTIONS
        --kernel-json-file=${CMAKE_CURRENT_SOURCE_DIR}/../op_host/config/ascendxxx/add_custom_template_binary.json
)
```

构建配置阶段会将该选项记录到自动生成的`custom_opc_options.ini`，后续二进制编译从该文件读取。例如：

```text
AddCustomTemplate@@--kernel-json-file=<工程路径>/op_host/config/ascendxxx/add_custom_template_binary.json
```

## JSON格式

JSON文件最外层包含`op_type`和`op_list`两个字段：`op_type`指定算子类型，`op_list`中的每个元素描述一份待编译的binary。以下是`AddCustomTemplate`的完整示例，按FP16和FP32生成两份binary：

```json
{
  "op_type": "AddCustomTemplate",
  "op_list": [
    {
      "bin_filename": "AddCustomTemplate_FP16",
      "simplified_key": "custom_fp16",
      "inputs": [
        {
          "name": "x",
          "index": 0,
          "dtype": "float16",
          "format": "ND",
          "shape": [-2]
        },
        {
          "name": "y",
          "index": 1,
          "dtype": "float16",
          "format": "ND",
          "shape": [-2]
        }
      ],
      "outputs": [
        {
          "name": "z",
          "index": 0,
          "dtype": "float16",
          "format": "ND",
          "shape": [-2]
        }
      ]
    },
    {
      "bin_filename": "AddCustomTemplate_FP32",
      "simplified_key": "custom_fp32",
      "inputs": [
        {
          "name": "x",
          "index": 0,
          "dtype": "float32",
          "format": "ND",
          "shape": [-2]
        },
        {
          "name": "y",
          "index": 1,
          "dtype": "float32",
          "format": "ND",
          "shape": [-2]
        }
      ],
      "outputs": [
        {
          "name": "z",
          "index": 0,
          "dtype": "float32",
          "format": "ND",
          "shape": [-2]
        }
      ]
    }
  ]
}
```

字段说明如下：

**表1**  simplified key二进制配置JSON字段说明

| 字段 | 说明 |
|---|---|
| `op_type` | 必填，必须与`npu_op_kernel_sources()`中的`OP_TYPE`一致。 |
| `op_list` | 必填非空数组；数组中的每一项对应一份独立编译的binary。 |
| `bin_filename` | 必填，不含目录；用于生成`.o`和配套`.json`文件名。去掉扩展名后必须等于`${op_type}`或以`${op_type}_`开头，其中`${op_type}`为大驼峰风格的算子名，且文件名不能重复。 |
| `simplified_key` | 必填非空字符串；是`GenSimplifiedKey`回调返回的自定义片段，必须与回调逐字节一致；字符串经UTF-8编码后的长度必须小于256字节。 |
| `inputs`、`outputs` | 每个输入输出包含`name`、`index`、`dtype`、`format`和`shape`，描述本项binary支持的组合。 |

`name`和`index`必须与算子原型定义的输入输出顺序一致；`dtype`和`format`必须属于原型声明的支持范围；`shape`应表达该binary覆盖的shape范围。JSON解析会检查最外层结构、`op_type`、非空`op_list`、`bin_filename`和`simplified_key`等基本格式，后续`asc_opc`编译还会依据输入输出描述处理具体组合。因此，JSON中的组合必须与原型定义、核函数（Kernel）实现和回调分支保持一致。

## 对编译过程的影响

配置JSON后，编译链路按以下顺序工作：

1. CMake将`--kernel-json-file`写入`custom_opc_options.ini`。
2. 二进制构建读取JSON，按`op_list`拆分为单组合JSON；每一项使用自身的`bin_filename`作为输出名称。
3. 对每个单组合JSON调用`asc_opc`编译核函数（Kernel）源码，得到对应的`.o`和描述该`.o`的`.json`。
4. JSON中存在非空`simplified_key`时，`asc_opc`使用自定义simplified key模式2，并将算子类型、运行时公共维度和该片段组合为完整simplified key。
5. 汇总阶段生成算子配置JSON和`binary_info_config.json`，记录每个完整Key关联的`.o`与`.json`路径。

以FP16项为例，主要产物如下：

```text
binary/ascendxxx/add_custom_template/AddCustomTemplate_FP16.o
binary/ascendxxx/add_custom_template/AddCustomTemplate_FP16.json
binary/config/ascendxxx/add_custom_template.json
binary/config/ascendxxx/binary_info_config.json
```

其中`AddCustomTemplate_FP16.json`记录单份`.o`的核函数（Kernel）元信息；`add_custom_template.json`汇总每个组合的支持信息；`binary_info_config.json`为运行时提供完整simplified key到binary路径的查找表。示例如下：

```json
{
  "AddCustomTemplate": {
    "simplifiedKeyMode": 2,
    "binaryList": [
      {
        "simplifiedKey": ["AddCustomTemplate/d=0,p=1/custom_fp16"],
        "binPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP16.o",
        "jsonPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP16.json"
      },
      {
        "simplifiedKey": ["AddCustomTemplate/d=0,p=1/custom_fp32"],
        "binPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP32.o",
        "jsonPath": "ascendxxx/add_custom_template/AddCustomTemplate_FP32.json"
      }
    ]
  }
}
```

实际产物可能针对确定性和实现模式写入多个完整Key；它们共享同一个自定义片段和binary路径。算子开发者不应在回调中手动拼接`AddCustomTemplate/d=0,p=1/`等前缀。

## 与普通编译的关系

不配置`--kernel-json-file`且不注册`GenSimplifiedKey`时，算子继续使用原有编译和binary匹配流程。只有同时满足“JSON为每个组合提供非空`simplified_key`”和“Host侧注册相应回调”时，运行时才具备自定义simplified key选择所需的两端信息。

## 相关文档

- [编译与部署基本流程](./basic_process.md)：了解通用的工程构建、打包和部署流程。
- [GenSimplifiedKey回调实现](../design_and_implementation/gen_simplified_key.md)：了解回调返回值如何与JSON中的`simplified_key`对应。
- [基于simplified key的运行时选择流程](../invocation/simplified_key_runtime_flow.md)：了解运行时如何使用`binary_info_config.json`查找binary。
