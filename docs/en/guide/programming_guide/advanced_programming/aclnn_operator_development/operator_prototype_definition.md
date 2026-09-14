# Operator Prototype Definition<a name="ZH-CN_TOPIC_0000001565730914"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T11:54:56.100Z pushedAt=2026-09-03T07:00:51.206Z -->

An operator prototype mainly describes the input/output, attributes, and other information of an operator, as well as the related implementation information of the operator on the AI processor, and associates functions such as [tiling implementation](host_tiling_implementation/basic_process.md). The operator prototype is carried by a custom operator class, which inherits from the **OpDef** class. After the operator prototype definition and other operations are completed, you need to call the **OP\_ADD** API and pass in the operator type (the class name of the custom operator class) to register the operator prototype. The following is a simple example of defining and registering the prototype of an Add operator.

```
namespace ops {
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Input("y")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
        // The following shape/datatype inference functions are used only when operators are integrated into a graph.
        this->SetInferShape(ge::InferShape);
        this->SetInferDataType(ge::InferDataType);  
        this->AICore()
            .SetTiling(optiling::TilingFunc);
        // Replace it with the actual Ascend AI Processor model.
        this->AICore().AddConfig("ascendxxx");
    }
};
OP_ADD(AddCustom);
} // namespace ops
```

>[!NOTE] Note
>- Based on the operator prototype definition, a custom operator project can implement the following automation capabilities:
>    - Automatically generate the implementation and API of [single-operator API call](single_operator_api_call.md). Developers can directly use the generated API to implement single-operator calls.
>    - Automatically generate the operator prototype definition **REG\_OP** used in graph mode scenarios. Developers can use the generated operator prototype to perform graph construction, graph compilation, graph execution, and other operations.
>- After an operator type is registered, the framework obtains the operator registration information based on the operator type, and matches the operator implementation file name and the kernel-side kernel function name according to certain rules during compilation and runtime. To ensure correct matching, the operator type, operator implementation file name, and kernel function name must follow the following definition rules. Generally, developers only need to ensure that the value of the operator type **op** parameter in the prototype definition JSON file uses the **UpperCamelCase** naming convention when creating an operator project. The code automatically generated after project creation satisfies this rule. When manually writing the operator prototype definition and operator implementation file, define them according to the following rules.
>    - The operator type must use the **UpperCamelCase** naming convention, that is, uppercase characters are used to distinguish different semantics.
>    - The operator implementation file name and the kernel function name must be the same, both being the value obtained after converting the operator type to the **snake_case** naming convention. The following describes the process of converting an operator type into the operator implementation file name and kernel function name:
>        - Convert the first uppercase character to a lowercase character. For example: Abc -\> abc.
>        - If the character before an uppercase character is a lowercase character or digit, insert an underscore (\_) before the uppercase character and convert the character to lowercase. For example: AbcDef -\> abc\_def.
>        - If the character before an uppercase character is an uppercase character and the character after it is a lowercase character, insert an underscore (\_) before the uppercase character and convert the character to lowercase. For example: AbcAAc -\> abc\_a\_ac.
>        - Convert other uppercase characters to lowercase characters, and keep lowercase characters unchanged.

## Operator Input/Output/Attribute Definition<a name="section18511125582"></a>

The operator prototype definition describes the operator input/output, attributes, and other information. The number of datatype and format entries supported by the input/output must be consistent and maintain a one-to-one correspondence.

The following code snippet presents the description information of input x of the Add operator.

```
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16, ge::DT_FLOAT, ge::DT_INT32})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND, ge::FORMAT_ND});
```

**Table 1** Input/output parameter description

<a name="table147931939192620"></a>
<table><thead align="left"><tr id="row879393962613"><th class="cellrowborder" valign="top" width="12.93%" id="mcps1.2.4.1.1"><p id="p15793239132612"><a name="p15793239132612"></a><a name="p15793239132612"></a>Prototype Definition</p></th>
<th class="cellrowborder" valign="top" width="14.580000000000002%" id="mcps1.2.4.1.2"><p id="p1279313913269"><a name="p1279313913269"></a><a name="p1279313913269"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="72.49%" id="mcps1.2.4.1.3"><p id="p16793183917266"><a name="p16793183917266"></a><a name="p16793183917266"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row279311397266"><td class="cellrowborder" rowspan="3" valign="top" width="12.93%" headers="mcps1.2.4.1.1 "><p id="p1679303952616"><a name="p1679303952616"></a><a name="p1679303952616"></a>Input/Output</p></td>
<td class="cellrowborder" valign="top" width="14.580000000000002%" headers="mcps1.2.4.1.2 "><p id="p979333919266"><a name="p979333919266"></a><a name="p979333919266"></a>ParamType</p></td>
<td class="cellrowborder" valign="top" width="72.49%" headers="mcps1.2.4.1.3 "><p id="p1079373911266"><a name="p1079373911266"></a><a name="p1079373911266"></a>Parameter type. The value options are OPTIONAL, REQUIRED, and DYNAMIC (dynamic input).</p>
<a name="ul17931339162615"></a><a name="ul17931339162615"></a><ul id="ul17931339162615"><li>Similar to the Add sample above, its input and output are required.</li><li>Some operators have a dynamic number of inputs or outputs. For example, AddN accumulates N input tensors into one output tensor; SplitV splits one tensor into N output tensors along an axis.</li><li>Some operators have optional input/output. For example, the BatchNorm operator has no mean and variance inputs during training, but has mean and variance inputs during inference.</li></ul></td>
</tr>
<tr id="row5793173910268"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p9794133992614"><a name="p9794133992614"></a><a name="p9794133992614"></a>DataType</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p177941839162615"><a name="p177941839162615"></a><a name="p177941839162615"></a>Data types supported by the operator input/output.</p></td>
</tr>
<tr id="row17794439112616"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11794339162616"><a name="p11794339162616"></a><a name="p11794339162616"></a>Format</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p5794939172616"><a name="p5794939172616"></a><a name="p5794939172616"></a>Formats supported by the operator input/output.</p></td>
</tr>
</tbody>
</table>

As shown in the prototype definition above, all combinations of datatype and format for the input/output are listed in a one-to-one correspondence. The following APIs can be used to simplify this code logic.

-   When specifying the datatype information of an input/output, if the datatype of a certain input/output supports being combined with the datatype/format of all other inputs/outputs, its datatype can be expressed through **DataTypeList**. When specifying the format information of an input/output, if the format of a certain input/output supports being combined with the datatype/format of all other inputs/outputs, its format can be expressed through **FormatList**. The following two code snippets have the same meaning.

    ```
    // List all one-to-one combinations
    class XxxCustom : public OpDef {
    public:
        XxxCustom(const char* name) : OpDef(name)
        {
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
            ...
        }
    };
    // Express through DataTypeList and FormatList without repeated listing.
    class XxxCustom : public OpDef {
    public:
        XxxCustom(const char* name) : OpDef(name)
        {
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
            ...
        }
    };
    
    ```

-   Use the **Follow** API to specify that the datatype/format/shape information of the current input/output is consistent with a previously defined input. The following example shows the scenario where output "y1" follows input "x1". In this case, the datatype, format, and shape of "y1" are consistent with those of "x1". Using the **Follow** API to specify shape consistency is usually simpler than the [shape inference](../operator_graph_development/basic_development_process.md#section358205411221) function logic. If the logic can be expressed with **Follow**, it is recommended to use the **Follow** API, in which case there is no need to register the InferShape function.

    ```
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
        .Follow("x1")
        .OutputShapeDependOnCompute();
    ```

The prototype definition also includes operator attribute information. The following code snippet presents the description information of the **reduceDim** and **isKeepDim** attributes of the ReduceMax operator.

```
        this->Attr("reduceDim")
            .AttrType(REQUIRED)
            .Int();
        this->Attr("isKeepDim")
            .AttrType(OPTIONAL)
            .Int(1);
```

The specific parameters are described as follows:

**Table 2** Attribute parameter description

<a name="table128323615112"></a>
<table><thead align="left"><tr id="row132831936115120"><th class="cellrowborder" valign="top" width="12.93%" id="mcps1.2.4.1.1"><p id="p1738292912418"><a name="p1738292912418"></a><a name="p1738292912418"></a>Prototype Definition</p></th>
<th class="cellrowborder" valign="top" width="14.580000000000002%" id="mcps1.2.4.1.2"><p id="p1028383635110"><a name="p1028383635110"></a><a name="p1028383635110"></a>Registration Method</p></th>
<th class="cellrowborder" valign="top" width="72.49%" id="mcps1.2.4.1.3"><p id="p16283103655111"><a name="p16283103655111"></a><a name="p16283103655111"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row8733105514414"><td class="cellrowborder" rowspan="2" valign="top" width="12.93%" headers="mcps1.2.4.1.1 "><p id="p573315551546"><a name="p573315551546"></a><a name="p573315551546"></a>Attr</p></td>
<td class="cellrowborder" valign="top" width="14.580000000000002%" headers="mcps1.2.4.1.2 "><p id="p373395512413"><a name="p373395512413"></a><a name="p373395512413"></a>AttrType</p></td>
<td class="cellrowborder" valign="top" width="72.49%" headers="mcps1.2.4.1.3 "><p id="p673345514420"><a name="p673345514420"></a><a name="p673345514420"></a>Sets the operator attribute type. The value can be OPTIONAL or REQUIRED.</p></td>
</tr>
<tr id="row11428375616"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1342437165"><a name="p1342437165"></a><a name="p1342437165"></a>Bool/Float/Int...</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p942143715612"><a name="p942143715612"></a><a name="p942143715612"></a>Sets the operator attribute data type to Bool/Float/Int... For details, see OpAttrDef.</p></td>
</tr>
</tbody>
</table>

## Related Implementation Information on AI Processors<a name="section7126203419413"></a>

Use **AddConfig** to register the AI processor models supported by the operator and the related configuration information. The **AddConfig** API prototype is as follows: the **soc** parameter indicates the AI processor model, and **aicore\_config** indicates other configuration information.

```
void AddConfig(const char *soc);
void AddConfig(const char *soc, OpAICoreConfig &aicore_config);
```

The following is an example of registering an AI processor model through this API. For the rules for filling in **ascendxxx**, see the **ASCEND\_COMPUTE\_UNIT** field in the **CMakePresets.json** compilation configuration file in the operator project directory. This field is automatically generated when you create a project using [msOpGen](operator_project_creation.md).

```
        this->AICore().AddConfig("ascendxxx");
```

For details about how to configure other AI Core configuration information, see **OpAICoreConfig**.

## Registering Tiling Implementation and Shape Inference Functions<a name="section16570761141"></a>

Use the **SetInferShape**, **SetInferDataType**, and **SetTiling** APIs to register the corresponding tiling implementation and shape inference functions. The following is an example. The registered tiling implementation and other functions are called by the framework, which passes the corresponding context to developers for use. For details about how to implement the tiling function, see [Host-side Tiling Implementation](host_tiling_implementation/basic_process.md). For details about how to implement the shape inference and other functions related to integrating operators into a graph, see [Integrating Operators into a Graph Development](../operator_graph_development/overview.md).

```
        // The following shape/datatype inference functions are used only in the scenario of integrating operators into a graph.
        this->SetInferShape(ge::InferShape);
        this->SetInferDataType(ge::InferDataType);
        this->AICore()
            .SetTiling(optiling::TilingFunc);
```

## Registering Differentiated Operator Prototypes for Multiple Hardware Platforms<a name="section25861074132"></a>

An operator class inherits from the base class **OpDef** and registers operator prototype information using Input, Output, Attr, and the like. When multiple hardware platforms support the same operator prototype, you can directly add the supported AI processor models through **AICore\(\).AddConfig**. When different hardware forms define different operator prototypes, you can register differentiated operator prototypes for different AI processor models by adding **OpAICoreConfig**.

The rules for differentiated operator prototypes to take effect are as follows:

-   For the input/output prototype information of an operator class, the parts not configured in **OpAICoreConfig** inherit the prototype defined in **OpDef**. For example, if output y is defined in the operator class but not in **OpAICoreConfig**, **OpAICoreConfig** inherits the prototype definition of y.
-   When the operator prototype defined in the operator class is the same as that defined in the newly added **OpAICoreConfig**, the operator prototype information defined in the newly added **OpAICoreConfig** overrides the prototype information defined in **OpDef**. For example, if input x is defined in the operator class to support the **DT\_FLOAT16** data type, and input x is also defined in the newly added **OpAICoreConfig** but supports the **DT\_FLOAT16** and **DT\_BF16** data types, the definition in the newly added **OpAICoreConfig** prevails.

In the following example, ascendxxx1 and ascendxxx2 (AI processor models) use the same operator prototype. The operator class inherits from the base class **OpDef**, registers operator prototype information using Input, Output, Attr, and the like, and then adds the supported AI processor models through **AICore\(\).AddConfig**. The operator prototype supported by ascendxxx3 requires customization, with the **DT\_BF16** type newly added. It is registered by adding **OpAICoreConfig**, and the definitions of x, y, and z override the corresponding prototype information defined in the operator class.

```
namespace ops {
class MyAdd : public OpDef {
public:
    MyAdd(const char* name) : OpDef(name)
    {
        // Prototype definition for the ascendxxx1 and ascendxxx2 AI processor models
        this->Input("x")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND});
        this->Input("y")
            .ParamType(OPTIONAL)
            .DataType({ge::DT_INT64})
            .Format({ge::FORMAT_ND});
        this->Output("z")
            .ParamType(REQUIRED)
            .DataType({ge::DT_FLOAT16})
            .Format({ge::FORMAT_ND});
        this->AICore()
            .SetTiling(optiling::TilingFunc);
        this->AICore().AddConfig("ascendxxx1");
        this->AICore().AddConfig("ascendxxx2");
        // Define the OpAICoreConfig variable for the ascendxxx3 AI processor to customize the prototype
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
        this->AICore().AddConfig("ascendxxx3", config);
    }
};
OP_ADD(MyAdd);
}
```

In the following example, only a few parameter prototype information items differ across hardware platforms. Developers can also customize part of the operator prototype information through **OpAICoreConfig** and reuse the other operator prototype information defined in **OpDef**, thereby customizing part of the prototype information for specific hardware platforms.

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x").DataType({ ge::DT_FLOAT16 }).ParamType(OPTIONAL);
        this->Output("y").DataType({ ge::DT_FLOAT16 });
        OpAICoreConfig aicConfig1;
        OpAICoreConfig aicConfig2;
        aicConfig1.Input("x")
            .ParamType(OPTIONAL)
            .DataType({ ge::DT_FLOAT })
            .Format({ ge::FORMAT_ND });
        aicConfig2.Input("x")
            .ParamType(REQUIRED)
            .DataType({ ge::DT_INT32 })
            .Format({ ge::FORMAT_ND });
        this->AICore().AddConfig("ascendxxx1", aicConfig1);
        this->AICore().AddConfig("ascendxxx2", aicConfig2);
    }
};
```
