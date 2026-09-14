# AutoContiguous<a name="ZH-CN_TOPIC_0000002114052037"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:15:17.905Z pushedAt=2026-09-11T03:18:33.421Z -->

## Description<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section36583473819"></a>

When this parameter is configured and the input (aclTensor type) of a single-operator API (aclnnxxx) is a non-contiguous tensor, the framework automatically converts it into a contiguous tensor.

**This API is supported only in the following scenarios:**

-   Developing a single-operator invocation application by executing a single-operator API.
-   Indirectly invoking a single-operator API\(aclnnxxx\): the scenario where the PyTorch framework directly invokes a single operator.

## Prototype<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &AutoContiguous()
```

## Parameters<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001711819448_zh-cn_topic_0000001526594958_zh-cn_topic_0000001525424352_section19165124931511"></a>

When an Input calls AutoContigous\(\), other Inputs that do not call IgnoreContiguous\(\) are also processed by the framework to convert non-contiguous tensors into contiguous tensors by default.

