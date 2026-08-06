# aclrtcCreateProg<a name="ZH-CN_TOPIC_0000002466837741"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas 训练系列产品：不支持
<!-- end id9 -->

## 功能说明<a name="section618mcpsimp"></a>

通过给定的参数，创建编译程序的实例。

## 函数原型<a name="section620mcpsimp"></a>

```
aclError aclrtcCreateProg(aclrtcProg *prog, const char *src, const char *name, int numHeaders, const char **headers, const char **includeNames)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  接口参数说明

<a name="table549971610414"></a>
<table><thead align="left"><tr id="row15008166418"><th class="cellrowborder" valign="top" width="17.580000000000002%" id="mcps1.2.4.1.1"><p id="p19500141674120"><a name="p19500141674120"></a><a name="p19500141674120"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="15.559999999999999%" id="mcps1.2.4.1.2"><p id="p092775021313"><a name="p092775021313"></a><a name="p092775021313"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="66.86%" id="mcps1.2.4.1.3"><p id="p650071616415"><a name="p650071616415"></a><a name="p650071616415"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row05001016134116"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p12769163425119"><a name="p12769163425119"></a><a name="p12769163425119"></a>prog</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p0927135021312"><a name="p0927135021312"></a><a name="p0927135021312"></a>输出</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1717692211109"><a name="p1717692211109"></a><a name="p1717692211109"></a>运行时编译程序的句柄。</p>
</td>
</tr>
<tr id="row13879152135218"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p13367185611013"><a name="p13367185611013"></a><a name="p13367185611013"></a>src</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p1445910412109"><a name="p1445910412109"></a><a name="p1445910412109"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p17693308285"><a name="p17693308285"></a><a name="p17693308285"></a><span>以字符串形式提供的Ascend C Device侧源代码内容。</span></p>
</td>
</tr>
<tr id="row57431411164719"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p83671756151016"><a name="p83671756151016"></a><a name="p83671756151016"></a>name</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p34596412102"><a name="p34596412102"></a><a name="p34596412102"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p210014091116"><a name="p210014091116"></a><a name="p210014091116"></a>用户自定义的程序名称，用于标识和区分不同的编译程序，默认值为"default_program"。</p>
</td>
</tr>
<tr id="row145835011107"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p193681656171016"><a name="p193681656171016"></a><a name="p193681656171016"></a>numHeaders</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p659205016108"><a name="p659205016108"></a><a name="p659205016108"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p740481202916"><a name="p740481202916"></a><a name="p740481202916"></a>指定要包含的头文件数量，必须为非负整数。</p>
<p id="p63811057145412"><a name="p63811057145412"></a><a name="p63811057145412"></a>无需包含头文件或者<span>Ascend C Device侧源代码</span>中已包含所需头文件时，此参数需设置为0。</p>
</td>
</tr>
<tr id="row6593502107"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p236845641010"><a name="p236845641010"></a><a name="p236845641010"></a>headers</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p135925020105"><a name="p135925020105"></a><a name="p135925020105"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1431743912546"><a name="p1431743912546"></a><a name="p1431743912546"></a>一个指向数组的指针，数组中的每个元素都是以'\0'结尾的字符串，表示头文件的源代码内容。当numHeaders为0时，此参数可以设置为nullptr。</p>
</td>
</tr>
<tr id="row205911502104"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p173681656131016"><a name="p173681656131016"></a><a name="p173681656131016"></a>includeNames</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p185914505104"><a name="p185914505104"></a><a name="p185914505104"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p410040131118"><a name="p410040131118"></a><a name="p410040131118"></a>一个指向数组的指针，数组中的每个元素都是以'\0'结尾的字符串，表示头文件的名称。</p>
<p id="p81471034133119"><a name="p81471034133119"></a><a name="p81471034133119"></a>这些名称必须与源代码中#include指令中包含的头文件名称完全一致。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

aclError为int类型变量，详细说明请参考[RTC错误码](RTC_error_code.md)。

## 约束说明<a name="section633mcpsimp"></a>

无

## 调用示例<a name="section837496171220"></a>

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
