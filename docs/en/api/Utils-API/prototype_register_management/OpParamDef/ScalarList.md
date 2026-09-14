# ScalarList<a name="ZH-CN_TOPIC_0000002078492708"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:38:25.642Z pushedAt=2026-09-11T03:37:51.725Z -->

## Description<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

After this parameter is configured, the input type of the automatically generated single-operator API (**aclnnxxx**) is **aclScalarList**.

## Prototype<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &ScalarList()
```

## Parameters<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   This parameter can be configured only for operator inputs. If it is configured for operator outputs, the configuration does not take effect.
-   This API is supported only in the following scenarios:
    -   Developing a single-operator invocation application by executing a single-operator API.
    -   Indirectly invoking a single operator API (aclnnxxx): the scenario of direct invocation of a single operator in the PyTorch framework.

## Example<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_section320753512363"></a>

```
this->Input("x")
    .ScalarList()
```

