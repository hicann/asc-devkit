# aclrtcDestroyProg<a name="ZH-CN_TOPIC_0000002433199240"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T15:23:57.485Z pushedAt=2026-09-09T11:43:18.728Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT: supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 training products/Atlas A3 inference products: supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 training products/Atlas A2 inference products: supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->

## Function Description<a name="section618mcpsimp"></a>

Destroys the compilation program instance.

## Prototype<a name="section620mcpsimp"></a>

```
aclError aclrtcDestroyProg(aclrtcProg *prog)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameter description

<a name="table549971610414"></a>
<table><thead align="left"><tr id="row15008166418"><th class="cellrowborder" valign="top" width="17.580000000000002%" id="mcps1.2.4.1.1"><p id="p19500141674120"><a name="p19500141674120"></a><a name="p19500141674120"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.559999999999999%" id="mcps1.2.4.1.2"><p id="p092775021313"><a name="p092775021313"></a><a name="p092775021313"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="66.86%" id="mcps1.2.4.1.3"><p id="p650071616415"><a name="p650071616415"></a><a name="p650071616415"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row05001016134116"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p12769163425119"><a name="p12769163425119"></a><a name="p12769163425119"></a>prog</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p0927135021312"><a name="p0927135021312"></a><a name="p0927135021312"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1717692211109"><a name="p1717692211109"></a><a name="p1717692211109"></a>Handle of the runtime compilation program.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

**aclError** is an int variable. For details, see [RTC Error Codes](rtc_error_codes.md).

## Constraints<a name="section633mcpsimp"></a>

None

## Example<a name="section837496171220"></a>

```
aclrtcProg prog;
aclError result = aclrtcDestroyProg(&prog);
```

