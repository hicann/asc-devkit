# IgnoreContiguous<a name="ZH-CN_TOPIC_0000002114097061"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T11:30:09.084Z pushedAt=2026-09-11T03:26:59.958Z -->

## Description<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section36583473819"></a>

Some operators support non-contiguous tensors and perform conversion on them in their implementations. After this parameter is configured, the framework skips the non-contiguity check.

## Prototype<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpParamDef &IgnoreContiguous(void)
```

## Parameters<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section25791320141317"></a>

**OpParamDef** operator definition. For details about **OpParamDef**, see [OpParamDef](OpParamDef.md).

## Constraints<a name="zh-cn_topic_0000001691759320_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_section19165124931511"></a>

None

