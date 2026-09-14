# CompileInfo<a name="ZH-CN_TOPIC_0000002114097965"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T15:49:29.020Z pushedAt=2026-09-11T06:32:51.875Z -->

## Description<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_section36583473819"></a>

Passes the pointer to **CompileInfo** into **TilingContext**.

## Prototype<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &CompileInfo(void *compileInfo)
```

## Parameters<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p8563195616313"></a>compileInfo</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_p2684123934216"></a>Void pointer to CompileInfo.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current **ContextBuilder** object.

## Constraints<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_section19165124931511"></a>

Because the internal data ordering of **TilingContext** differs from that of **KernelContext** and **TilingParseContext**, CompileInfo\(\) is supported only when used on the premise of calling BuildTilingContext\(\). For other scenarios, use the **Outputs** API; otherwise, undefined behavior occurs.

## Example<a name="zh-cn_topic_0000001867289941_zh-cn_topic_0000001389787297_section320753512363"></a>

```
void AddCompileInfo(TilingParseContext *tilingParseContext)
{
    ......
    void *compilerInfo = *tilingParseContext->GetOutputPointer<void **>(0);
    auto kernelContextHolder = context_ascendc::ContextBuilder()
                                    ...... // Add the call to the operator input/output interfaces.
                                    .CompileInfo(compileInfo)
                                    .BuildTilingContext();
    ......
}
```

