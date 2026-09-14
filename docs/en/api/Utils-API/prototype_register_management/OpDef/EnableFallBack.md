# EnableFallBack<a name="ZH-CN_TOPIC_0000002190399885"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T10:45:21.998Z pushedAt=2026-09-11T02:57:39.117Z -->

## Description<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section36583473819"></a>

This API enables the fallback configuration. Once enabled, a fallback function is automatically generated and registered with GE. The core logic of the fallback function is to convert the inputs, outputs, and attributes of GE into the parameter format required by the aclnn single-operator API, and then call the aclnn API. In dynamic graph scenarios, GE can directly call the fallback function (which calls the aclnn API), thereby simplifying the scheduling process. For details about how fallback dispatches operators, see [Graph Development](https://www.hiascend.com/document/redirect/CannCommunityGraphguide).

## Prototype<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpDef &EnableFallBack(void)
```

## Parameters<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpDef** operator definition. For details about **OpDef**, see [OpDef](OpDef.md).

## Constraints<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   The operator must register and implement the **InferShape** function.
-   The operator must register and implement the **InferDataType** function.

## Example<a name="zh-cn_topic_0000001600267337_zh-cn_topic_0000001526112350_zh-cn_topic_0000001575944081_section320753512363"></a>

```
class AddCustom : public OpDef {
public:
    AddCustom(const char* name) : OpDef(name)
    {
        this->Input("x").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND});
        this->Input("y").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND});
        this->Output("z").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND})
        this->AICore().AddConfig("ascendxxx");
        this->SetInferShape(ge::InferShapeFunc);
        this->SetInferDataType(ge::InferDataTypeFunc);
        this->EnableFallBack();
    }
};
OP_ADD(AddCustom);
```

