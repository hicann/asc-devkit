# BuildTilingContext<a name="ZH-CN_TOPIC_0000002114097969"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T15:48:29.859Z pushedAt=2026-09-11T06:32:14.670Z -->

## Description<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section36583473819"></a>

Constructs an object of **KernelRunContextHolder**, from which an object of the **TilingContext** type can be obtained through the **GetContext** API.

## Prototype<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
std::shared_ptr<KernelRunContextHolder> BuildTilingContext()
```

## Parameters<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section25791320141317"></a>

Shared pointer to **KernelRunContextHolder**.

## Constraints<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section320753512363"></a>

```
auto tilingContextHolder = context_ascendc::ContextBuilder().
  .SetOpNameType(...,...)
  .NodeIoNum(...)
  .IrInstanceNum(...)
  .AddInputTd(...)
  .AddOutputTd(...)
  .AddAttr(...)
  .BuildTilingContext(...);
gert::TilingContext *tilingContext = tilingContextHolder->GetContext<gert::TilingContext>();
```

