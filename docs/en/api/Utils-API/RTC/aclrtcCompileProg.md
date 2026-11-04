# aclrtcCompileProg<a name="ZH-CN_TOPIC_0000002433359052"></a>

<!-- md-trans-meta sourceCommit=f83e857605e3844db8b9de0442007cc115278a47 translatedAt=2026-10-23T07:09:08.630Z pushedAt=2026-10-26T11:12:09.458Z nl=2 -->

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
- Atlas inference products AI Core: Not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference products Vector Core: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->

## Description<a name="section618mcpsimp"></a>

Compilation API that compiles the specified program.

## Prototype<a name="section620mcpsimp"></a>

```
aclError aclrtcCompileProg(aclrtcProg prog, int numOptions, const char **options)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** API parameters

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
<tr id="row13879152135218"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p159984212474"><a name="p159984212474"></a><a name="p159984212474"></a>numOptions</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p1792775011320"><a name="p1792775011320"></a><a name="p1792775011320"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p13205163764718"><a name="p13205163764718"></a><a name="p13205163764718"></a>Number of compilation options.</p></td>
</tr>
<tr id="row57431411164719"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p17431411114711"><a name="p17431411114711"></a><a name="p17431411114711"></a>options</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p107431119474"><a name="p107431119474"></a><a name="p107431119474"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p2743311144712"><a name="p2743311144712"></a><a name="p2743311144712"></a>Array of compilation options, which stores the specific compilation options (-std=c++17 is added by default).</p>
<p id="li196189152610p0"><a name="li196189152610p0"></a><a name="li196189152610p0"></a>For supported compilation options, see [*Bisheng Compiler*](https://www.hiascend.com/document/detail/en/CANNCommunityEdition/920/compiler/BishengCompiler/atlas_bisheng_10_0001.html).</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

aclError is a variable of the int type. For details, see [RTC error codes](RTC_error_code.md).

## Constraints<a name="section633mcpsimp"></a>

None

## Examples<a name="section837496171220"></a>

```
aclrtcProg prog;
const char *options[] = {"--npu-arch=dav-2201"};
int numOptions = sizeof(options) / sizeof(options[0]);
aclError result = aclrtcCompileProg(prog, numOptions, options);
```
