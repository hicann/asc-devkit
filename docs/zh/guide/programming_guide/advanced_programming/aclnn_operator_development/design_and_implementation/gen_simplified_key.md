# GenSimplifiedKey回调实现

本文介绍如何编写并注册`GenSimplifiedKey`回调。该回调用于预编译多份核函数（Kernel）binary的场景：算子开发者根据本次调用的输入、输出和属性生成一个自定义simplified key片段，运行时框架使用该片段查找对应的binary。

本文只说明回调如何生成Key。如何在编译时提供每个Key对应的binary，请参见[simplified key二进制配置JSON](../compilation_and_deployment/simplified_key_json_configuration.md)；运行时如何调用回调并选择binary，请参见[基于simplified key的运行时选择流程](../invocation/simplified_key_runtime_flow.md)。

## 使用场景和职责边界

普通算子由运行时根据常规支持信息匹配binary。对于一个算子需要按自定义维度选择多份预编译binary的场景，例如按输入dtype、format、shape分档或属性组合选择，可以使用`GenSimplifiedKey`。

`GenSimplifiedKey`的职责边界如下：

**表1**  simplified key各参与方职责

| 参与方 | 职责 |
|---|---|
| 算子开发者 | 枚举需要编译的组合，在JSON中为每份binary声明`simplified_key`；编写回调，从本次调用信息生成相同的Key片段。 |
| 编译工具链 | 按JSON拆分并编译每个组合，将完整simplified key和binary路径写入产物配置。 |
| 运行时框架 | 调用回调生成片段，组装完整simplified key，查找并加载匹配的binary。 |

回调不加载binary，也不决定TilingKey或核函数（Kernel）入口；这些工作仍由运行时框架和已注册的`TilingFunc`完成。

## 函数签名

注册接口接受的函数类型为：

```cpp
using GenSimplifiedKeyKernelFunc = UINT32 (*)(gert::TilingContext *, ge::char_t *);
```

实际函数通常声明为：

```cpp
static ge::graphStatus GenSimplifiedKeyFunc(
    gert::TilingContext* context, ge::char_t* simplifiedKey);
```

参数说明如下：

**表2**  GenSimplifiedKey回调参数说明

| 参数 | 说明 |
|---|---|
| `context` | 本次调用的Host侧上下文。可读取输入和输出描述、shape、属性等选择维度。 |
| `simplifiedKey` | 由运行时提供的输出缓冲区。回调将自定义Key片段写入该缓冲区。 |
| 返回值 | 成功返回`ge::GRAPH_SUCCESS`，无法生成有效片段时返回失败码。 |

自定义Key片段经UTF-8编码后的长度必须小于256字节，不能包含算子类型、`d=`或`p=`等完整Key前缀。完整simplified key经UTF-8编码后的长度必须小于1024字节。

## 编写回调

以下示例来自`AddCustomTemplate`。该样例支持FP16和FP32两种组合，两个输入和一个输出使用相同dtype，因此只读取输入`x`的dtype就足以区分binary。

```cpp
static ge::graphStatus GenSimplifiedKeyFunc(
    gert::TilingContext* context, ge::char_t* simplifiedKey)
{
    constexpr size_t CUSTOM_SIMPLIFIED_KEY_MAX_LEN = 256U;
    if (context == nullptr || simplifiedKey == nullptr || context->GetInputDesc(0) == nullptr) {
        return ge::GRAPH_FAILED;
    }

    const char* key = nullptr;
    switch (context->GetInputDesc(0)->GetDataType()) {
        case ge::DT_FLOAT16:
            key = "custom_fp16";
            break;
        case ge::DT_FLOAT:
            key = "custom_fp32";
            break;
        default:
            return ge::GRAPH_FAILED;
    }
    return strcpy_s(simplifiedKey, CUSTOM_SIMPLIFIED_KEY_MAX_LEN, key) == EOK
        ? ge::GRAPH_SUCCESS : ge::GRAPH_FAILED;
}
```

该函数和JSON的对应关系如下：

**表3**  AddCustomTemplate回调与binary对应关系

| 运行时输入`x`的dtype | 回调输出片段 | JSON中的`simplified_key` | 选择的binary |
|---|---|---|---|
| `ge::DT_FLOAT16` | `custom_fp16` | `custom_fp16` | `AddCustomTemplate_FP16.o` |
| `ge::DT_FLOAT` | `custom_fp32` | `custom_fp32` | `AddCustomTemplate_FP32.o` |

对于多输入、多输出或带属性的算子，凡是会改变binary选择结果的维度都必须同时出现在两端：JSON必须枚举组合，回调必须读取相应的`context`信息并生成唯一片段。例如binary同时依赖两个输入dtype时，应分别读取`context->GetInputDesc(0)`和`context->GetInputDesc(1)`；不能只读取第一个输入后假设其余输入相同。

## 注册回调

在算子原型的`OpDef`构造函数中注册回调，通常与Tiling函数一起配置：

```cpp
this->AICore()
    .SetTiling(optiling::TilingFunc)
    .SetGenSimplifiedKey(optiling::GenSimplifiedKeyFunc)
    .AddConfig("ascendxxx");
```

`OP_ADD`注册算子原型时，会将该函数写入算子实现注册信息。运行时加载Host侧算子实现后，才能在调用过程中取得并调用此回调。

## 编写要求

- Key片段应只由会影响binary选择的稳定信息组成。不要使用内存地址、随机数、时间戳或会在同一输入条件下变化的内容。
- 同一调用条件必须始终生成相同片段；不同binary组合必须生成不同片段。
- 回调返回的片段必须与JSON中的`simplified_key`逐字节一致，包括大小写。
- 先检查`context`、输出缓冲区以及会访问的输入、输出描述是否为空；无法覆盖的组合返回失败码，不要写入不确定的默认Key。
- 回调应只完成轻量的描述信息读取和字符串写入，不应执行文件I/O、动态编译、复杂计算或设备侧操作。

## 相关文档

- [算子原型定义](./operator_prototype_definition.md)：了解`SetGenSimplifiedKey`所在的原型注册位置。
- [simplified key二进制配置JSON](../compilation_and_deployment/simplified_key_json_configuration.md)：了解JSON与编译产物的对应关系。
- [基于simplified key的运行时选择流程](../invocation/simplified_key_runtime_flow.md)：了解运行时如何使用回调结果选择binary。
