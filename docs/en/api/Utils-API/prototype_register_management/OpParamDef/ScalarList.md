# ScalarList<a name="ZH-CN_TOPIC_0000002078492708"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T11:16:00.239Z pushedAt=2026-11-13T08:46:49.468Z nl=2 -->

## Description<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

After this parameter is configured, the input type of the automatically generated single-operator API (aclnnxxx) is aclScalarList.

## Prototype<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &ScalarList()
```

## Parameters<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

-   This parameter can be configured only for operator inputs. If it is configured for operator outputs, the configuration does not take effect.
-   This API is supported only in the following scenarios:
    -   Developing a single operator invocation application by using the single operator API execution method.
    -   Indirectly calling the single operator API (aclnnxxx): the scenario of directly invoking a single operator in the PyTorch framework.

## Examples<a name="zh-cn_topic_0000001711659948_zh-cn_topic_0000001526594958_zh-cn_topic_0000001575944081_section320753512363"></a>

```
this->Input("x")
    .ScalarList()
```

