# AllocMessage<a name="ZH-CN_TOPIC_0000001960224984"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T13:40:46.059Z pushedAt=2026-10-26T07:41:44.214Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_group_mgmt_ISASI/CubeResGroupHandle/AllocMessage_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

**AIV** requests message space from the message queue to store the message structure, and returns the address of the currently requested message space. The depth of the message queue is fixed at 4, and message space is requested in a top-down and then cyclic order. When the message space pointed to by the message queue pointer is in the FREE state, **AllocMessage** returns the address of the space; otherwise, it waits in a loop until the current space is in the FREE state.

**Figure 1**  AllocMessage diagram<a name="fig153760010192"></a>  
![](../../../../figures/allocmessage_diagram.png "AllocMessage diagram")

## Prototype<a name="section765814724715"></a>

```
template <PipeMode pipeMode = PipeMode::SCALAR_MODE>             
__aicore__ inline __gm__ CubeMsgType *AllocMessage()
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**Table 1** Template parameter description

<a name="table1436511617117"></a>
<table><thead align="left"><tr id="row4365369115"><th class="cellrowborder" valign="top" width="12.82%" id="mcps1.2.3.1.1"><p id="p236526181117"><a name="p236526181117"></a><a name="p236526181117"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="87.18%" id="mcps1.2.3.1.2"><p id="p236514615117"><a name="p236514615117"></a><a name="p236514615117"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row14365116121111"><td class="cellrowborder" valign="top" width="12.82%" headers="mcps1.2.3.1.1 "><p id="p23653641116"><a name="p23653641116"></a><a name="p23653641116"></a>**pipeMode**</p></td>
<td class="cellrowborder" valign="top" width="87.18%" headers="mcps1.2.3.1.2 "><p id="p1213214123127"><a name="p1213214123127"></a><a name="p1213214123127"></a>Configures the execution unit for sending messages. It is of the **PipeMode** type, defined as follows:</p>
<a name="screen11132212171216"></a><a name="screen11132212171216"></a><pre class="screen" codetype="Cpp" id="screen11132212171216">enum class PipeMode : uint8_t { 
  SCALAR_MODE = 0, // The Scalar execution unit writes messages to GM.
  MTE3_MODE = 1, // The MTE3 unit writes messages to GM.
  MAX 
}</pre>
<p id="p168827373292"><a name="p168827373292"></a><a name="p168827373292"></a></p>
<p id="p2132171218121"><a name="p2132171218121"></a><a name="p2132171218121"></a>Note that when **pipeMode** is **MTE3_MODE**, messages can only be sent subsequently using the **PostMessage** API. In addition, the template parameter **pipeMode** of the **AllocMessage** and **PostMessage** APIs must be the same.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

Returns the address of the currently allocated message space.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

None

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
auto queIdx = AscendC::GetBlockIdx();
handle.AssignQueue(queIdx);
auto msgPtr = handle.AllocMessage();        // After the queue is bound, apply for message space from the queue. The message space address is msgPtr.
```
