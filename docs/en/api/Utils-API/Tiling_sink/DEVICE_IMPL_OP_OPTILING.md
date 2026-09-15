# DEVICE\_IMPL\_OP\_OPTILING<a name="ZH-CN_TOPIC_0000002269484441"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T16:37:42.525Z pushedAt=2026-09-11T07:02:25.728Z -->

## Description<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section36583473819"></a>

In the [Tiling Offload](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/operator_graph_development/enable_tiling_offload.md) scenario, this macro definition is used to generate a registration class for Tiling Offload, and then register the **Tiling** function to be offloaded by calling the member function of the registration class.

## Prototype<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
namespace optiling {
using SinkTilingFunc = std::function<ge::graphStatus(gert::TilingContext *context)>;

class DeviceOpImplRegisterImpl;
// Developers only need to focus on the Tiling member function.
class DeviceOpImplRegister {
public:
  DeviceOpImplRegister(const char *opType);
  ~DeviceOpImplRegister();
  DeviceOpImplRegister(DeviceOpImplRegister &&other) noexcept;
  DeviceOpImplRegister(const DeviceOpImplRegister &other);
  DeviceOpImplRegister &operator=(const DeviceOpImplRegister &) = delete;
  DeviceOpImplRegister &operator=(DeviceOpImplRegister &&) = delete;
  DeviceOpImplRegister &Tiling(SinkTilingFunc func);

// ...
};
}  // namespace optiling

#define DEVICE_IMPL_OP_OPTILING(optype)                                                                      \
  static optiling::DeviceOpImplRegister VAR_UNUSED g_deviceOpImplRegister##optype =                                    \
      optiling::DeviceOpImplRegister(#optype)
#endif
```

## Parameters<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section75395119104"></a>

**Table 1**  DEVICE\_IMPL\_OP\_OPTILING parameter description

<a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p1693913355118"><a name="p1693913355118"></a><a name="p1693913355118"></a>optype</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001820490272_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p3511614184115"><a name="p3511614184115"></a><a name="p3511614184115"></a>OpType (operator type) for which the Tiling function needs to be registered.</p></td>
</tr>
</tbody>
</table>

**Table 2**  Tiling member function parameter description

<a name="table2060634264310"></a>
<table><thead align="left"><tr id="row860654216436"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="p19606154212434"><a name="p19606154212434"></a><a name="p19606154212434"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="p17606174294315"><a name="p17606174294315"></a><a name="p17606174294315"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="p1460614224313"><a name="p1460614224313"></a><a name="p1460614224313"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row160654218436"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p14606104284320"><a name="p14606104284320"></a><a name="p14606104284320"></a>func</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p16606124294310"><a name="p16606124294310"></a><a name="p16606124294310"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p25503505020"><a name="p25503505020"></a><a name="p25503505020"></a>Tiling function to be registered. This function takes a gert::TilingContext* as input and returns ge::graphStatus.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001867289945_zh-cn_topic_0000001389787297_section320753512363"></a>

```
DEVICE_IMPL_OP_OPTILING(TestOptype).Tiling(TestTilingFunc); // Register the Tiling function and its OpType for Tiling Offload.
```
