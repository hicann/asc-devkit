# Introduction<a name="ZH-CN_TOPIC_0000002114097953"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-13T09:57:19.314Z pushedAt=2026-11-16T07:07:41.667Z nl=2 -->

The ContextBuilder class provides a series of APIs to support manually constructing the TilingContext class for verifying the Tiling function and the KernelContext class for verifying the TilingParse function.

## Examples<a name="zh-cn_topic_0000001819028992_zh-cn_topic_0000001389787297_section320753512363"></a>

```
// Construct the KernelContext.
auto kernelContextHolder = context_ascendc::ContextBuilder()
    .Inputs(...)
    .Outputs(...)
    .BuildKernelRunContext();
gert::KernelContext* tilingParseContext = kernelContextHolder->GetContext<gert::KernelContext>();

// Construct the TilingContext.
auto tilingContextHolder = context_ascendc::ContextBuilder()
    .SetOpNameType(...,...)
    .NodeIoNum(...)
    .IrInstanceNum(...)
    .AddInputTd(...)
    .AddOutputTd(...)
    .AddAttr(...)
    .BuildTilingContext(...);
gert::TilingContext* tilingContext = tilingContextHolder->GetContext<gert::TilingContext>();
```

