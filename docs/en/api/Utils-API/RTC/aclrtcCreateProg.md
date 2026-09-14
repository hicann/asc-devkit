# aclrtcCreateProg<a name="ZH-CN_TOPIC_0000002466837741"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T15:21:42.507Z pushedAt=2026-09-09T11:41:07.189Z -->

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
- Atlas inference product AI Core: not supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas inference product Vector Core: not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training product: not supported
<!-- end id9 -->

## Description<a name="section618mcpsimp"></a>

Creates a compilation program instance based on the given parameters.

## Prototype<a name="section620mcpsimp"></a>

```
aclError aclrtcCreateProg(aclrtcProg *prog, const char *src, const char *name, int numHeaders, const char **headers, const char **includeNames)
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
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p0927135021312"><a name="p0927135021312"></a><a name="p0927135021312"></a>Output</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1717692211109"><a name="p1717692211109"></a><a name="p1717692211109"></a>Handle of the runtime compilation program.</p></td>
</tr>
<tr id="row13879152135218"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p13367185611013"><a name="p13367185611013"></a><a name="p13367185611013"></a>src</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p1445910412109"><a name="p1445910412109"></a><a name="p1445910412109"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p17693308285"><a name="p17693308285"></a><a name="p17693308285"></a><span>Ascend C Device-side source code content provided as a string.</span></p></td>
</tr>
<tr id="row57431411164719"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p83671756151016"><a name="p83671756151016"></a><a name="p83671756151016"></a>name</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p34596412102"><a name="p34596412102"></a><a name="p34596412102"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p210014091116"><a name="p210014091116"></a><a name="p210014091116"></a>User-defined program name used to identify and distinguish different compiled programs, with a default value of "default_program".</p></td>
</tr>
<tr id="row145835011107"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p193681656171016"><a name="p193681656171016"></a><a name="p193681656171016"></a>numHeaders</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p659205016108"><a name="p659205016108"></a><a name="p659205016108"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p740481202916"><a name="p740481202916"></a><a name="p740481202916"></a>Specifies the number of header files to include, which must be a non-negative integer.</p>
<p id="p63811057145412"><a name="p63811057145412"></a><a name="p63811057145412"></a>Set this parameter to 0 when no header files need to be included or when the required header files are already included in the <span>Ascend C Device-side source code</span>.</p></td>
</tr>
<tr id="row6593502107"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p236845641010"><a name="p236845641010"></a><a name="p236845641010"></a>headers</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p135925020105"><a name="p135925020105"></a><a name="p135925020105"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1431743912546"><a name="p1431743912546"></a><a name="p1431743912546"></a>A pointer to an array, where each element is a null-terminated string representing the source code content of a header file. When numHeaders is 0, this parameter can be set to nullptr.</p></td>
</tr>
<tr id="row205911502104"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p173681656131016"><a name="p173681656131016"></a><a name="p173681656131016"></a>includeNames</p></td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p185914505104"><a name="p185914505104"></a><a name="p185914505104"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p410040131118"><a name="p410040131118"></a><a name="p410040131118"></a>A pointer to an array, where each element is a null-terminated string representing the name of a header file.</p>
<p id="p81471034133119"><a name="p81471034133119"></a><a name="p81471034133119"></a>These names must exactly match the header file names included by the `#include` directives in the source code.</p></td>
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
const char *src = R""""(
#include "kernel_operator.h"
#include "my_const_a.h"
#include "my_const_b.h"

extern "C" __global__ __aicore__ void hello_world(GM_ADDR x)
{
	KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIC_ONLY);
	*x = *x + MY_CONST_A + MY_CONST_B;
}
)"""";
const char* headerSrcA = R"(
#ifndef CONST_A_H
#define CONST_A_H
const int MY_CONST_A = 100;
#endif // CONST_A_H
)";
const char* includeNameA = "my_const_a.h";

const char* headerSrcB = R"(
#ifndef CONST_B_H
#define CONST_B_H
const int MY_CONST_B = 50;
#endif // CONST_B_H
)";

const char* includeNameB = "my_const_b.h";
const char* headersArray[] = { headerSrcA, headerSrcB };
const char* includeNameArray[] = { includeNameA, includeNameB };
aclError result = aclrtcCreateProg(&prog, src, "hello_world", 2,  headersArray,  includeNameArray);
```

