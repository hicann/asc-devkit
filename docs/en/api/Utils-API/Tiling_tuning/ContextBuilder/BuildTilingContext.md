# BuildTilingContext<a name="ZH-CN_TOPIC_0000002114097969"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-13T09:54:48.847Z pushedAt=2026-11-16T07:07:41.656Z nl=2 -->

## Description<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section36583473819"></a>

Constructs an object of KernelRunContextHolder, from which an object of the TilingContext type can be obtained through the GetContext API.

## Prototype<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
std::shared_ptr<KernelRunContextHolder> BuildTilingContext()
```

## Parameters<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section25791320141317"></a>

A shared pointer to KernelRunContextHolder.

## Constraints<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section320753512363"></a>

```
auto tilingContextHolder = context_ascendc::ContextBuilder()
  .SetOpNameType(...,...)
  .NodeIoNum(...)
  .IrInstanceNum(...)
  .AddInputTd(...)
  .AddOutputTd(...)
  .AddAttr(...)
  .BuildTilingContext(...);
gert::TilingContext *tilingContext = tilingContextHolder->GetContext<gert::TilingContext>();
```

