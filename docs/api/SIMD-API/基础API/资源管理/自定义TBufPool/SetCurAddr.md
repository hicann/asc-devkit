# SetCurAddr<a name="ZH-CN_TOPIC_0000002087922325"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->


## 功能说明<a name="section618mcpsimp"></a>

设置自定义TBufPool已经被分配完的地址，比如初始化时一共申请了32k的内存大小，给某一个TQue分配了8K，则需要调用该接口以保证后续的内存块从8K开始分配。

## 函数原型<a name="section620mcpsimp"></a>

```
__aicore__ inline void SetCurAddr(uint32_t curAddr)
```

**表1**  参数说明

<a name="table1960411494236"></a>
<table><thead align="left"><tr id="row17605204922320"><th class="cellrowborder" valign="top" width="12.36%" id="mcps1.2.4.1.1"><p id="p6605649122315"><a name="p6605649122315"></a><a name="p6605649122315"></a>参数名称</p>
</th>
<th class="cellrowborder" valign="top" width="12.379999999999999%" id="mcps1.2.4.1.2"><p id="p17605949182313"><a name="p17605949182313"></a><a name="p17605949182313"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="75.26%" id="mcps1.2.4.1.3"><p id="p17605184915230"><a name="p17605184915230"></a><a name="p17605184915230"></a>含义</p>
</th>
</tr>
</thead>
<tbody><tr id="row260544916231"><td class="cellrowborder" valign="top" width="12.36%" headers="mcps1.2.4.1.1 "><p id="p104214518241"><a name="p104214518241"></a><a name="p104214518241"></a>curAddr</p>
</td>
<td class="cellrowborder" valign="top" width="12.379999999999999%" headers="mcps1.2.4.1.2 "><p id="p1842145172415"><a name="p1842145172415"></a><a name="p1842145172415"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="75.26%" headers="mcps1.2.4.1.3 "><p id="p171754287124"><a name="p171754287124"></a><a name="p171754287124"></a>已分配的内存地址。</p>
</td>
</tr>
</tbody>
</table>

## 约束说明<a name="section633mcpsimp"></a>

无

## 返回值说明<a name="section640mcpsimp"></a>

无

## 调用示例<a name="section1234017553610"></a>

请参考[调用示例](EXTERN_IMPL_BUFPOOL宏.md#section1234017553610)。

