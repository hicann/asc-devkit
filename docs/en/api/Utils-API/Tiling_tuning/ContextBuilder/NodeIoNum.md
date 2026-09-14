# NodeIoNum<a name="ZH-CN_TOPIC_0000002114052961"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T15:56:14.266Z pushedAt=2026-09-11T06:36:46.274Z -->

## Description<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_section36583473819"></a>

Declares the input and output counts of the operator definition.

## Prototype<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
ContextBuilder &NodeIoNum(size_t inputNum, size_t outputNum)
```

## Parameters<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650060_p759124015322"><a name="zh-cn_topic_0000001820650060_p759124015322"></a><a name="zh-cn_topic_0000001820650060_p759124015322"></a>inputNum</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_p2684123934216"></a>Number of inputs in the operator IR prototype definition.</p></td>
</tr>
<tr id="zh-cn_topic_0000001820650060_row10125205610321"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001820650060_p10125145633215"><a name="zh-cn_topic_0000001820650060_p10125145633215"></a><a name="zh-cn_topic_0000001820650060_p10125145633215"></a>outputNum</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001820650060_p111251456123217"><a name="zh-cn_topic_0000001820650060_p111251456123217"></a><a name="zh-cn_topic_0000001820650060_p111251456123217"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001820650060_p201253567324"><a name="zh-cn_topic_0000001820650060_p201253567324"></a><a name="zh-cn_topic_0000001820650060_p201253567324"></a>Number of outputs in the operator IR prototype definition.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_section25791320141317"></a>

The current **ContextBuilder** object.

## Constraints<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_section19165124931511"></a>

Must be used together with [IrInstanceNum](IrInstanceNum.md).

## Example<a name="zh-cn_topic_0000001820650060_zh-cn_topic_0000001389787297_section320753512363"></a>

```
auto builder = ContextBuilder().NodeIoNum(5, 3); // This operator has 5 inputs and 3 outputs.
```

