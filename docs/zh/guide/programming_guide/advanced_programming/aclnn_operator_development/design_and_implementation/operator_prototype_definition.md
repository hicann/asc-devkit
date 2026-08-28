# 算子原型定义

本文属于基础内容，介绍如何通过算子原型定义描述算子对外暴露的接口和支持范围。本文承接[算子功能设计](./operator_function_design.md)的结论，说明如何将功能设计中的输入输出、属性、dtype/format和Tiling函数关联落地为`OpDef`代码，并介绍图模式下的输出shape和dtype推导配置。

## 原型定义放在哪里

在AddCustom样例工程中，算子原型定义位于Host侧实现文件：

```text
op_host/add_custom.cpp
```

该文件中同时包含三类内容：

| 内容 | 作用 |
|---|---|
| `TilingFunc` | Host侧Tiling函数，负责计算核函数（Kernel）启动和分块参数 |
| `InferShape`、`InferDataType` | 图模式下的输出shape和dtype推导函数，仅在需要支持图模式时编写 |
| `class AddCustom : public OpDef` | 算子原型定义，描述输入、输出、属性、Tiling关联 |

本文重点讲`class AddCustom : public OpDef`中的原型定义内容，同时说明它和Tiling函数之间的连接关系，并补充图模式下关联推导函数的方法。

## 定义OpDef类并注册算子

算子原型通过一个继承自`OpDef`的C++类承载。类名通常与算子类型保持一致。AddCustom的算子类型为`AddCustom`，因此定义`AddCustom`类。

```cpp
namespace ops {
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char* name) : OpDef(name)
    {
        // 原型定义内容
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

这里有两个关键点：

- 构造函数通过`OpDef(name)`初始化基类，后续的`Input`、`Output`、`Attr`、`AICore`等声明都写在构造函数中。
- 文件末尾通过`OP_ADD(AddCustom)`注册算子类型，编译工具据此识别算子原型，并生成aclnn接口、图模式原型等相关代码。

> [!NOTE]说明
>
> OpType应采用大驼峰命名，首字符为大写英文字母，后续只能使用英文字母或数字，例如`AddCustom`。不要使用下划线、连字符、空格、中文或其他特殊字符。
>
> 自定义算子的OpType不能与内置算子重名。确定OpType后，可使用[optype_collector工具](../../../appendix/optype_collector_tool.md)查询OpType信息并检测命名冲突。

## 定义输入和输出

输入输出定义来自功能设计阶段的结论。AddCustom有两个输入`x`、`y`，一个输出`z`，三个Tensor共同表达`z = x + y`的计算语义，因此都声明为`REQUIRED`。基础样例没有属性，输入输出声明就是原型定义中最先落地的接口内容。

```cpp
this->Input("x")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT16})
    .Format({ge::FORMAT_ND});
this->Input("y")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT16})
    .Format({ge::FORMAT_ND});
this->Output("z")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT16})
    .Format({ge::FORMAT_ND});
```

输入输出参数说明如下：

| 原型项 | 参数 | 作用 |
|---|---|---|
| `Input`/`Output` | 输入输出名称 | 声明输入输出名称，名称需要和功能设计中的参数名称一致。声明顺序会影响aclnn接口参数顺序和核函数（Kernel）入口参数顺序。 |
| `Input`/`Output` | `ParamType` | 声明参数提供方式。`REQUIRED`表示每次调用都必须提供，`OPTIONAL`表示部分场景可以省略，`DYNAMIC`表示数量随调用变化。 |
| `Input`/`Output` | `DataType` | 声明输入输出支持的dtype。多个dtype按列表顺序和format列表一一对应。 |
| `Input`/`Output` | `Format` | 声明输入输出支持的format。多个format按列表顺序和dtype列表一一对应。 |

同一个输入或输出支持多组dtype/format时，`DataType`和`Format`列表需要按位置对应。例如下面的写法表示支持`DT_FLOAT16 + FORMAT_ND`、`DT_FLOAT + FORMAT_ND`和`DT_INT32 + FORMAT_ND`三组组合。

```cpp
this->Input("x")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
```

- 如果某个输入或输出的dtype或format能够和其他输入输出的组合自由搭配，可以使用`DataTypeList`或`FormatList`减少重复列举。以下两种写法表达相同组合关系。

    ```cpp
    // 方式一：逐项列出所有一一对应的组合。
    this->Input("x")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT16, ge::DT_FLOAT16, ge::DT_FLOAT16})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    this->Input("y")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    this->Output("z")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});

    // 方式二：`x`固定支持`DT_FLOAT16 + FORMAT_ND`时，使用`DataTypeList`和`FormatList`表达`x`的支持集合。
    this->Input("x")
        .ParamType(REQUIRED)
        .DataTypeList({ge::DT_FLOAT16})
        .FormatList({ge::FORMAT_ND});
    this->Input("y")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    this->Output("z")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
    ```

- 通过[Follow接口](../../../../../api/Utils-API/prototype_register_management/OpParamDef/Follow.md)可以指定当前输入或输出的datatype、format和shape跟随前面已经定义过的输入。输出完全跟随某个输入时，Follow可以直接表达“输出与输入一致”的关系，通常无需再编写shape推导函数。Follow shape和`SetInferShape`不能混用；如果注册了`SetInferShape`，需要由推导函数完成所有输出的shape推导。

    例如输出`y1`跟随输入`x1`时，`y1`的datatype、format和shape都会与`x1`保持一致：

    ```cpp
    this->Input("x1")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND});
    this->Input("x2")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT, ge::DT_FLOAT})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND});
    this->Output("y1")
        .ParamType(REQUIRED)
        .Follow("x1");
    ```

## 定义属性

属性用于描述算子的配置项，例如维度、阈值、缩放系数、开关等。AddCustom基础样例只实现`z = x + y`，功能设计中已经确认没有额外属性，因此原型定义中没有`Attr`声明。

带属性的算子可以在构造函数中使用`Attr`定义。例如将AddCustom扩展为`z = x + alpha * y`时，可以增加`alpha`属性：

```cpp
this->Attr("alpha")
    .AttrType(OPTIONAL)
    .Float(1.0);
```

`AttrType`描述属性是否必须提供，常用取值为`REQUIRED`和`OPTIONAL`。`Bool`、`Float`、`Int`等接口用于声明属性数据类型和默认值，具体说明请参考[OpAttrDef](../../../../../api/Utils-API/prototype_register_management/OpAttrDef/OpAttrDef_functions.md)。

属性在原型中声明后，Host侧Tiling函数可以通过`context->GetAttrs()`获取属性值。核函数（Kernel）侧需要使用属性参与计算时，可以通过TilingData将属性值传递到核函数（Kernel）。属性传递方法见[通过TilingData传递属性信息](./tiling_data_attributes.md)。

## 图模式下注册输出shape和dtype推导函数

`DataType`和`Format`用于声明输入输出支持的dtype和format，不是图模式下的输出推导函数。算子仅通过aclnn单算子接口调用时，不需要注册输出shape和dtype推导函数。如果算子还需要支持图模式，可以通过`SetInferShape`和`SetInferDataType`注册输出shape和dtype推导函数。AddCustom的输出`z`与输入`x`具有相同shape和dtype，注册方式如下：

```cpp
this->SetInferShape(ge::InferShape)
    .SetInferDataType(ge::InferDataType);
```

[SetInferShape](../../../../../api/Utils-API/prototype_register_management/OpDef/SetInferShape.md)用于注册输出shape推导函数，[SetInferDataType](../../../../../api/Utils-API/prototype_register_management/OpDef/SetInferDataType.md)用于注册输出dtype推导函数。注册后，框架在图模式下需要推导输出shape或dtype时会调用对应函数。

如果输出shape可以通过`Follow`表达，则优先使用`Follow`，无需再注册`SetInferShape`。例如，AddCustom的输出`z`与输入`x`具有相同shape时，可以在输出定义中通过`Follow("x", FollowType::SHAPE)`指定shape跟随关系。

本文只说明原型定义中如何关联推导函数。推导函数内部如何读取输入shape、设置输出shape、读取输入dtype和设置输出dtype，属于图模式开发和Host侧实现细节，见[算子入图开发](../../operator_graph_development/basic_development_flow.md)。

## 关联Host侧Tiling实现

算子原型还需要关联Host侧Tiling函数。AddCustom样例中，Tiling函数名为`optiling::TilingFunc`，通过`AICore().SetTiling(...)`注册。

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc);
```

`SetTiling`只负责建立原型和Tiling函数之间的调用关系。`TilingFunc`内部会读取输入shape等信息，计算核函数（Kernel）需要的运行参数，并写入TilingData。

TilingData字段设计、Tiling函数写法、workspace设置和`numBlocks`配置见[Host侧Tiling实现](./host_tiling_implementation.md)。

## 注册GenSimplifiedKey回调

当算子需要依据运行时输入、输出或属性，从多份预编译核函数（Kernel）binary中快速选择一份时，可以注册`GenSimplifiedKey`回调。回调由算子开发者编写，只负责生成自定义simplified key片段；运行时框架负责在该片段前补齐算子类型和运行时公共维度，使用完整Key查找并选择对应的binary。

在已有Tiling注册的`AICore()`链式调用中增加`SetGenSimplifiedKey`即可：

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc)
    .SetGenSimplifiedKey(optiling::GenSimplifiedKeyFunc);
```

该接口适用于同一算子存在多份二进制，且选择维度可由Host侧`TilingContext`读取的场景，例如输入dtype、format、shape分档或属性组合。回调生成的片段必须与编译配置JSON中每个`op_list`项的`simplified_key`严格对应；没有注册回调或不提供这类JSON时，保持原有binary匹配流程。

回调的具体写法、约束及其与运行时选择的边界见[GenSimplifiedKey回调实现](./gen_simplified_key.md)。

## 配置指定AI处理器的差异化配置

`AddConfig`为指定AI处理器设置差异化编译配置。

### 使用OpAICoreConfig注册差异化配置

当某个AI处理器的输入输出原型或其他编译配置与通用配置不同时，在算子原型构造函数中创建`OpAICoreConfig`，并通过`AddConfig("ascendxxx", config)`注册。"ascendxxx"填写规则请参考算子工程目录下编译配置项文件`CMakePresets.json`中的`ASCEND_COMPUTE_UNIT`字段，该字段取值在使用msOpGen创建工程时自动生成。

`OpAICoreConfig`中未配置的输入输出继承`OpDef`中的定义；重新配置的同名输入输出覆盖通用定义。以下示例为指定AI处理器增加`DT_BF16`支持：

```cpp
namespace ops {
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char* name) : OpDef(name)
    {
        // 通用输入输出原型配置省略。
        OpAICoreConfig config;
        config.Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        config.Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        config.Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_BF16})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        this->AICore().AddConfig("ascendxxx", config);
    }
};
} // namespace ops
```

### 在独立文件中注册差异化配置

也可以通过`REGISTER_OP_AICORE_CONFIG`在独立文件中注册差异化配置，便于按芯片或模块拆分维护。例如，在`op_host/add_custom_ascendxxx.cpp`中注册：

```cpp
#include "register/op_config_registry.h"

namespace ops {
REGISTER_OP_AICORE_CONFIG(AddCustom, ascendxxx, []() {
    ops::OpAICoreConfig config("ascendxxx");
    config.Input("x")
        .ParamType(REQUIRED)
        .DataType({ge::DT_FLOAT16, ge::DT_BF16})
        .Format({ge::FORMAT_ND, ge::FORMAT_ND});
    return config;
});
} // namespace ops
```

> [!NOTE]说明
>
> 当两种方式为同一算子、同一AI处理器配置相同项时，在算子原型构造函数中通过`AICore().AddConfig(...)`注册的配置优先级高于在独立文件中通过`REGISTER_OP_AICORE_CONFIG`注册的配置。

## 完整示例

以下代码展示AddCustom的原型定义主体。

```cpp
namespace ops {
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND});

        this->AICore()
            .SetTiling(optiling::TilingFunc);
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

## 相关文档

- [算子功能设计](./operator_function_design.md)：了解如何确定算子的输入输出、属性和Host侧Tiling设计。
- [Host侧Tiling实现](./host_tiling_implementation.md)：了解TilingData定义、Tiling函数编写和`SetTiling`关联关系。
- [GenSimplifiedKey回调实现](./gen_simplified_key.md)：了解如何为预编译核函数（Kernel）binary生成自定义simplified key片段。
- [核函数（Kernel）侧算子实现](./kernel_operator_implementation.md)：了解核函数（Kernel）入口格式、参数顺序和TilingData读取方式。
- [命名转换规则对照表](../appendix/naming_conversion_table.md)：了解OpType、核函数（Kernel）入口和aclnn接口之间的命名转换关系。
