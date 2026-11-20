# IgnoreContiguous<a name="ZH-CN_TOPIC_0000002114097061"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T11:11:53.797Z pushedAt=2026-11-13T08:46:49.439Z nl=2 -->

## Description<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Some operators support non-contiguous tensors and convert them in their implementations. After this parameter is configured, the framework skips the non-contiguity validation.

## Prototype<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &IgnoreContiguous(void)
```

## Parameters<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpParamDef operator definition. For details about OpParamDef, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

