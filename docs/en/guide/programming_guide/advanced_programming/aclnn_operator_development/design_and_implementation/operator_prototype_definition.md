# Operator Prototype Definition

<!-- md-trans-meta sourceCommit=693fbd4c936677f1c45b391eccc6b9b290f0c189 translatedAt=2026-10-23T13:55:50.096Z pushedAt=2026-11-03T07:05:13.408Z nl=2 -->

This document covers basic content and describes how to use the operator prototype definition to describe the interfaces exposed by an operator and the supported scope. Building on the conclusions of [Operator Function Design](./operator_function_design.md), this document explains how to implement the input/output, attributes, dtype/format, and Tiling function associations from the function design as `OpDef` code, and introduces the output shape and dtype inference configuration in graph mode.

## Where to Place the Prototype Definition

In the AddCustom sample project, the operator prototype definition is located in the Host-side implementation file:

```text
op_host/add_custom.cpp
```

This file contains the following three types of content:

| Content | Description |
|---|---|
| `TilingFunc` | Host-side Tiling function, which calculates the kernel function launch and tiling parameters |
| `InferShape`, `InferDataType` | Output shape and dtype inference functions in graph mode, written only when graph mode needs to be supported |
| `class AddCustom : public OpDef` | Operator prototype definition, which describes the input/output, attributes, and Tiling association |

This document focuses on the prototype definition content in `class AddCustom : public OpDef`, explains its connection with the Tiling function, and supplements the method of associating inference functions in graph mode.

## Defining the OpDef Class and Registering the Operator

The operator prototype is carried by a C++ class that inherits from `OpDef`. The class name is usually consistent with the operator type. The operator type of AddCustom is `AddCustom`, so the `AddCustom` class is defined.

```cpp
namespace ops {
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char* name) : OpDef(name)
    {
        // Prototype definition content
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

There are two key points here:

- The constructor initializes the base class through `OpDef(name)`. The subsequent declarations of `Input`, `Output`, `Attr`, `AICore`, and so on are all written in the constructor.
- At the end of the file, `OP_ADD(AddCustom)` registers the operator type. Based on this, the compilation tool identifies the operator prototype and generates related code such as the aclnn API and the graph mode prototype.

> [!NOTE] Note
>
> OpType must use upper camel case naming. The first character must be an uppercase English letter, and subsequent characters can only be English letters or digits, for example, `AddCustom`. Do not use underscores, hyphens, spaces, Chinese characters, or other special characters.
>
> The OpType of a custom operator must not duplicate the name of a built-in operator. After determining the OpType, you can use the [optype_collector tool](../../../appendix/optype_collector_tool.md) to query OpType information and detect naming conflicts.

## Defining Inputs and Outputs

The input/output definitions come from the conclusions of the functional design phase. AddCustom has two inputs `x` and `y` and one output `z`. The three tensors together express the computation semantics of `z = x + y`, so they are all declared as `REQUIRED`. The basic sample has no attributes, so the input/output declarations are the first interface content to be implemented in the prototype definition.

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

The input/output parameters are described as follows:

| Prototype Item | Parameter | Description |
|---|---|---|
| `Input`/`Output` | Input/output name | Declares the input/output name, which must be consistent with the parameter name in the functional design. The declaration order affects the parameter order of the aclnn API and the entry parameter order of the kernel function. |
| `Input`/`Output` | `ParamType` | Declares how the parameter is provided. `REQUIRED` means it must be provided for every call, `OPTIONAL` means it can be omitted in some scenarios, and `DYNAMIC` means the quantity varies with the call. |
| `Input`/`Output` | `DataType` | Declares the dtypes supported by the input/output. Multiple dtypes correspond one-to-one with the format list in list order. |
| `Input`/`Output` | `Format` | Declares the formats supported by the input/output. Multiple formats correspond one-to-one with the dtype list in list order. |

When the same input or output supports multiple dtype/format groups, the `DataType` and `Format` lists must correspond by position. For example, the following notation indicates support for three groups: `DT_FLOAT16 + FORMAT_ND`, `DT_FLOAT + FORMAT_ND`, and `DT_INT32 + FORMAT_ND`.

```cpp
this->Input("x")
    .ParamType(REQUIRED)
    .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
    .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
```

- If the dtype or format of an input or output can be freely combined with those of other inputs and outputs, you can use `DataTypeList` or `FormatList` to reduce repeated enumeration. The following two notations express the same combination relationship.

    ```cpp
    // Method 1: List all one-to-one combinations item by item.
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

    // Method 2: When `x` is fixed to support `DT_FLOAT16 + FORMAT_ND`, use `DataTypeList` and `FormatList` to express the supported set of `x`.
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

- Through the [Follow API](../../../../../api/Utils-API/prototype_register_management/OpParamDef/Follow.md), you can specify that the datatype, format, and shape of the current input or output follow a previously defined input. When an output completely follows an input, Follow can directly express the relationship that "the output is consistent with the input", and usually no shape inference function needs to be written. Follow shape and `SetInferShape` cannot be mixed; if `SetInferShape` is registered, the inference function must complete the shape inference of all outputs.

    For example, when output `y1` follows input `x1`, the datatype, format, and shape of `y1` are all consistent with `x1`:

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

## Defining Attributes

Attributes are used to describe the configuration items of an operator, such as dimensions, thresholds, scaling coefficients, and switches. The basic AddCustom sample only implements `z = x + y`, and the functional design has confirmed that no additional attributes are required. Therefore, no `Attr` declaration is included in the prototype definition.

For operators with attributes, you can use `Attr` in the constructor to define them. For example, when extending AddCustom to `z = x + alpha * y`, you can add the `alpha` attribute:

```cpp
this->Attr("alpha")
    .AttrType(OPTIONAL)
    .Float(1.0);
```

`AttrType` describes whether an attribute must be provided. The common values are `REQUIRED` and `OPTIONAL`. The `Bool`, `Float`, and `Int` APIs are used to declare the attribute data type and default value. For details, see [OpAttrDef](../../../../../api/Utils-API/prototype_register_management/OpAttrDef/OpAttrDef_functions.md).

After an attribute is declared in the prototype, the Host-side Tiling function can obtain the attribute value through `context->GetAttrs()`. When the kernel function needs to use the attribute in computation, the attribute value can be passed to the kernel function through TilingData. For details about how to pass attributes, see [Passing Attribute Information Through TilingData](./tiling_data_attributes.md).

## Registering Output Shape and dtype Inference Functions in Graph Mode

`DataType` and `Format` are used to declare the dtype and format supported by the input/output, and are not output inference functions in graph mode. When an operator is invoked only through the aclnn single-operator API, there is no need to register output shape and dtype inference functions. If the operator also needs to support graph mode, you can register the output shape and dtype inference functions through `SetInferShape` and `SetInferDataType`. The output `z` of AddCustom has the same shape and dtype as the input `x`, and the registration is as follows:

```cpp
this->SetInferShape(ge::InferShape)
    .SetInferDataType(ge::InferDataType);
```

[SetInferShape](../../../../../api/Utils-API/prototype_register_management/OpDef/SetInferShape.md) is used to register the output shape inference function, and [SetInferDataType](../../../../../api/Utils-API/prototype_register_management/OpDef/SetInferDataType.md) is used to register the output dtype inference function. After registration, the framework calls the corresponding function when it needs to infer the output shape or dtype in graph mode.

If the output shape can be expressed through `Follow`, use `Follow` first, and there is no need to register `SetInferShape`. For example, when the output `z` of AddCustom has the same shape as the input `x`, you can specify the shape following relationship through `Follow("x", FollowType::SHAPE)` in the output definition.

This document only describes how to associate inference functions in the prototype definition. How the inference function internally reads the input shape, sets the output shape, reads the input dtype, and sets the output dtype belongs to the graph mode development and Host-side implementation details. For details, see [Operator Graph Development](../../operator_graph_development/basic_development_flow.md).

## Tiling Implementation on the Host Side

The operator prototype also needs to be associated with the Tiling function on the host side. In the AddCustom sample, the Tiling function is named `optiling::TilingFunc` and is registered through `AICore().SetTiling(...)`.

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc);
```

`SetTiling` only establishes the call relationship between the prototype and the Tiling function. Inside `TilingFunc`, information such as the input shape is read, the runtime parameters required by the kernel function are calculated, and the results are written into TilingData.

For details about TilingData field design, Tiling function implementation, workspace settings, and `numBlocks` configuration, see [Tiling implementation on the host side](./host_tiling_implementation.md).

## Registering the GenSimplifiedKey Callback

When an operator needs to quickly select one from multiple precompiled kernel function binaries based on runtime inputs, outputs, or attributes, you can register the `GenSimplifiedKey` callback. The callback is written by the operator developer and is only responsible for generating a custom simplified key fragment. The runtime framework is responsible for prepending the operator type and runtime common dimensions to this fragment, and then uses the complete key to look up and select the corresponding binary.

Add `SetGenSimplifiedKey` to the existing `AICore()` chained call where Tiling is registered:

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc)
    .SetGenSimplifiedKey(optiling::GenSimplifiedKeyFunc);
```

This API applies to scenarios where multiple binaries exist for the same operator and the selection dimension can be read from the Host-side `TilingContext`, such as input dtype, format, shape binning, or attribute combinations. The fragment generated by the callback must strictly correspond to the `simplified_key` of each `op_list` item in the compilation configuration JSON. If no callback is registered or no such JSON is provided, the original binary matching process is retained.

For details about how to write the callback, its constraints, and its boundary with runtime selection, see [GenSimplifiedKey Callback Implementation](./gen_simplified_key.md).

## Configuring Differentiated Configuration for a Specified AI Processor

`AddConfig` sets differentiated compilation configuration for a specified AI processor.

### Registering Differentiated Configuration Using OpAICoreConfig

When the input/output prototype or other compilation configuration of a specific AI processor differs from the general configuration, create `OpAICoreConfig` in the operator prototype constructor and register it through `AddConfig("ascendxxx", config)`. For the rules of filling in "ascendxxx", refer to the `ASCEND_COMPUTE_UNIT` field in the compilation configuration file `CMakePresets.json` under the operator project directory. The value of this field is automatically generated when the project is created using msOpGen.

Input/output not configured in `OpAICoreConfig` inherits the definitions in `OpDef`; reconfigured input/output with the same name overrides the general definition. The following example adds `DT_BF16` support for a specified AI processor:

```cpp
namespace ops {
class AddCustom : public OpDef {
public:
    explicit AddCustom(const char* name) : OpDef(name)
    {
        // The general input/output prototype configuration is omitted.
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

### Registering Differentiated Configuration in a Separate File

You can also register the differentiated configuration in a separate file through `REGISTER_OP_AICORE_CONFIG`, which facilitates maintenance by chip or module. For example, register it in `op_host/add_custom_ascendxxx.cpp`:

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

> [!NOTE] Note
>
> When the same item is configured for the same operator and the same AI processor in both ways, the configuration registered through `AICore().AddConfig(...)` in the operator prototype constructor takes priority over the configuration registered through `REGISTER_OP_AICORE_CONFIG` in a separate file.

## Complete Example

The following code shows the main body of the AddCustom prototype definition.

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

## Related Documents

- [Operator Function Design](./operator_function_design.md): Learn how to determine the input/output, attributes, and Host-side Tiling design of an operator.
- [Host-side Tiling Implementation](./host_tiling_implementation.md): Learn about TilingData definition, Tiling function writing, and the `SetTiling` association.
- [GenSimplifiedKey Callback Implementation](./gen_simplified_key.md): Learn how to generate a custom simplified key fragment for a precompiled kernel function binary.
- [Kernel-side Operator Implementation](./kernel_operator_implementation.md): Learn about the kernel function entry format, parameter order, and TilingData reading method.
- [Naming Conversion Rule Reference Table](../appendix/naming_conversion_table.md): Learn about the naming conversion relationships among OpType, kernel function entry, and aclnn APIs.
