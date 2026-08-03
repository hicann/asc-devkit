# AllocMessage<a name="ZH-CN_TOPIC_0000001960224984"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->


## 功能说明<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

AIV从消息队列里申请消息空间，用于存放消息结构体，返回当前申请的消息空间的地址。消息队列的深度固定为4，申请消息空间的顺序为自上而下，然后循环。当消息队列指针指向的消息空间为FREE状态时，AllocMessage返回空间的地址，否则循环等待，直到当前空间的状态为FREE。

**图1**  AllocMessage示意图<a name="fig153760010192"></a>  
![](../../../../figures/AllocMessage示意图.png "AllocMessage示意图")

## 函数原型<a name="section765814724715"></a>

```
template <PipeMode pipeMode = PipeMode::SCALAR_MODE>             
__aicore__ inline __gm__ CubeMsgType *AllocMessage()
```

## 参数说明<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

**表1**  模板参数说明

<a name="table1436511617117"></a>
<table><thead align="left"><tr id="row4365369115"><th class="cellrowborder" valign="top" width="12.82%" id="mcps1.2.3.1.1"><p id="p236526181117"><a name="p236526181117"></a><a name="p236526181117"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="87.18%" id="mcps1.2.3.1.2"><p id="p236514615117"><a name="p236514615117"></a><a name="p236514615117"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row14365116121111"><td class="cellrowborder" valign="top" width="12.82%" headers="mcps1.2.3.1.1 "><p id="p23653641116"><a name="p23653641116"></a><a name="p23653641116"></a>pipeMode</p>
</td>
<td class="cellrowborder" valign="top" width="87.18%" headers="mcps1.2.3.1.2 "><p id="p1213214123127"><a name="p1213214123127"></a><a name="p1213214123127"></a>用于配置发送消息的执行单元。PipeMode类型，其定义如下：</p>
<a name="screen11132212171216"></a><a name="screen11132212171216"></a><pre class="screen" codetype="Cpp" id="screen11132212171216">enum class PipeMode : uint8_t { 
  SCALAR_MODE = 0, // Scalar执行单元往GM上写消息。
  MTE3_MODE = 1, // 使用MTE3单元往GM上写消息。
  MAX 
}</pre>
<p id="p168827373292"><a name="p168827373292"></a><a name="p168827373292"></a></p>
<p id="p2132171218121"><a name="p2132171218121"></a><a name="p2132171218121"></a>注意，pipeMode为MTE3_MODE时，后续只能使用PostMessage接口发送消息。同时两个接口AllocMessage与PostMessage的模板参数pipeMode需要相同。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

当前申请的消息空间的地址。

## 约束说明<a name="zh-cn_topic_0000001526206862_section65498832"></a>

无

## 调用示例<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
auto queIdx = AscendC::GetBlockIdx();
handle.AssignQueue(queIdx);
auto msgPtr = handle.AllocMessage();        // 绑定队列后，从该队列中申请消息空间，消息空间地址为msgPtr。
```
