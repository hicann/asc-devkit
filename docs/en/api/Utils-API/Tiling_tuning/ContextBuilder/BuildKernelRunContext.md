# BuildKernelRunContext<a name="ZH-CN_TOPIC_0000002078653622"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-06T15:47:39.038Z pushedAt=2026-09-11T06:29:34.443Z -->

## Description<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section36583473819"></a>

Constructs a **KernelRunContext** and returns a smart pointer to [KernelRunContextHolder](kernelruncontextholder_structure_definition.md), through which a **KernelContext** object can be obtained via the **GetContext** API.

## Prototype<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
std::shared_ptr<KernelRunContextHolder> BuildKernelRunContext()
```

## Parameters<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section75395119104"></a>

None

## Return Value<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section25791320141317"></a>

Shared pointer to **KernelRunContextHolder**, through which the **KernelContext** object can be obtained by calling the **GetContext**`gert::KernelContext>()` function.

## Constraints<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001867409733_zh-cn_topic_0000001389787297_section320753512363"></a>

```
auto kernelContextHolder = context_ascendc::ContextBuilder().Inputs().Outputs().BuildKernelRunContext();
gert::KernelContext* tilingParseContext = kernelContextHolder->GetContext<gert::KernelContext>();
```

