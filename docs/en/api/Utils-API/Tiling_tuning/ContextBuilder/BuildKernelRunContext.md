# BuildKernelRunContext<a name="ZH-CN_TOPIC_0000002078653622"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-13T09:54:26.058Z pushedAt=2026-11-16T07:07:41.654Z nl=2 -->

## Description<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section36583473819"></a>

Constructs a KernelRunContext and returns a smart pointer to [KernelRunContextHolder](KernelRunContextHolder_struct_definition.md). The object can be used to obtain a KernelContext object through the GetContext API.

## Prototype<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
std::shared_ptr<KernelRunContextHolder> BuildKernelRunContext()
```

## Parameters<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section25791320141317"></a>

A shared pointer to KernelRunContextHolder. You can obtain the KernelContext object by calling `GetContext<gert::KernelContext>()`.

## Constraints<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section320753512363"></a>

```
auto kernelContextHolder = context_ascendc::ContextBuilder().Inputs().Outputs().BuildKernelRunContext();
gert::KernelContext* tilingParseContext = kernelContextHolder->GetContext<gert::KernelContext>();
```

