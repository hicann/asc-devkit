# AutoContiguous<a name="ZH-CN_TOPIC_0000002114052037"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T11:05:58.831Z pushedAt=2026-11-13T08:46:49.423Z nl=2 -->

## Description<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

When this parameter is configured and the input (of the aclTensor type) in a single-operator API (aclnnxxx) is a non-contiguous tensor, the framework automatically converts it into a contiguous tensor.

**This API is supported only in the following scenarios:**

-   Developing a single operator invocation application by using the single operator API execution method.
-   Indirectly calling the single operator API (aclnnxxx): the scenario of directly invoking a single operator in the PyTorch framework.

## Prototype<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &AutoContiguous()
```

## Parameters<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

When an Input calls AutoContiguous\(\), other Inputs that do not call IgnoreContiguous\(\) are also processed by the framework to convert non-contiguous tensors to contiguous tensors by default.

