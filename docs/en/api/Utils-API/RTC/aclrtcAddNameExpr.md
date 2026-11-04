# aclrtcAddNameExpr<a name="ZH-CN_TOPIC_0000002601551137"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T07:09:12.861Z pushedAt=2026-10-26T11:12:09.461Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference product AI Core: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->

## Description<a name="section618mcpsimp"></a>

Registers a specified name expression, which represents a \_\_global\_\_ function.

It is mainly used in scenarios where the \_\_global\_\_ function is a template function. After calling this API, you can obtain the mangling name of the function through the [aclrtcGetLoweredName](aclrtcGetLoweredName.md) API, and then call the corresponding template \_\_global\_\_ function during the launch phase.

## Prototype<a name="section620mcpsimp"></a>

```
aclError aclrtcAddNameExpr(aclrtcProg prog, const char *const nameExpression)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameters

<a name="table10560024131915"></a>
<table><thead align="left"><tr id="row35607245197"><th class="cellrowborder" valign="top" width="17.580000000000002%" id="mcps1.2.4.1.1"><p id="p1356016243193"><a name="p1356016243193"></a><a name="p1356016243193"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.559999999999999%" id="mcps1.2.4.1.2"><p id="p092775021313"><a name="p092775021313"></a><a name="p092775021313"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="66.86%" id="mcps1.2.4.1.3"><p id="p20560122410195"><a name="p20560122410195"></a><a name="p20560122410195"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row15560192415195"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p12769163425119"><a name="p12769163425119"></a><a name="p12769163425119"></a>prog</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p0927135021312"><a name="p0927135021312"></a><a name="p0927135021312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1717692211109"><a name="p1717692211109"></a><a name="p1717692211109"></a>Handle of the runtime compilation program.</p></td>
</tr>
<tr id="row13879152135218"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p19982811135218"><a name="p19982811135218"></a><a name="p19982811135218"></a>nameExpression</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p1792775011320"><a name="p1792775011320"></a><a name="p1792775011320"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p4268745104714"><a name="p4268745104714"></a><a name="p4268745104714"></a>A constant expression, usually a string literal, used to identify a __global__ function.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

aclError is a variable of the int type. For details, see [RTC error code](RTC_error_code.md).

## Constraints<a name="section633mcpsimp"></a>

This API must be used before the [aclrtcCompileProg](aclrtcCompileProg.md) compilation API.

## Examples<a name="section837496171220"></a>

```
aclrtcProg prog;
const char* kernelNameExpr = "add_custom<float>";
aclError result = aclrtcAddNameExpr(prog, kernelNameExpr);
```
