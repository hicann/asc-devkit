# Introduction<a name="ZH-CN_TOPIC_0000002114097953"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T15:40:49.694Z pushedAt=2026-09-11T07:41:24.184Z -->

The **ContextBuilder** class provides a series of APIs to manually construct the **TilingContext** class for verifying Tiling functions and the **KernelContext** class for verifying **TilingParse** functions.

## Example<a name="zh-cn_topic_0000001819028992_zh-cn_topic_0000001389787297_section320753512363"></a>

```
// Construct KernelContext.
auto kernelContextHolder = context_ascendc::ContextBuilder()
    .Inputs(...)
    .Outputs(...)
    .BuildKernelRunContext();
gert::KernelContext* tilingParseContext = kernelContextHolder->GetContext<gert::KernelContext>();

// Construct TilingContext.
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

