# aclrtcGetLoweredName<a name="ZH-CN_TOPIC_0000002601671087"></a>

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

获取已经注册的\_\_global\_\_函数的mangling name。

## 函数原型<a name="section620mcpsimp"></a>

```
aclError aclrtcGetLoweredName(aclrtcProg prog, const char *nameExpression, const char **loweredName)
```

## 参数说明<a name="section622mcpsimp"></a>

**表1**  接口参数说明

<a name="table10560024131915"></a>
<table><thead align="left"><tr id="row35607245197"><th class="cellrowborder" valign="top" width="17.580000000000002%" id="mcps1.2.4.1.1"><p id="p1356016243193"><a name="p1356016243193"></a><a name="p1356016243193"></a>参数名</p>
</th>
<th class="cellrowborder" valign="top" width="15.559999999999999%" id="mcps1.2.4.1.2"><p id="p092775021313"><a name="p092775021313"></a><a name="p092775021313"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="66.86%" id="mcps1.2.4.1.3"><p id="p20560122410195"><a name="p20560122410195"></a><a name="p20560122410195"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row15560192415195"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p12769163425119"><a name="p12769163425119"></a><a name="p12769163425119"></a>prog</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p0927135021312"><a name="p0927135021312"></a><a name="p0927135021312"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1717692211109"><a name="p1717692211109"></a><a name="p1717692211109"></a>运行时编译程序的句柄。</p>
</td>
</tr>
<tr id="row13879152135218"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p2344720151015"><a name="p2344720151015"></a><a name="p2344720151015"></a>nameExpression</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p834314201105"><a name="p834314201105"></a><a name="p834314201105"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p1328182051012"><a name="p1328182051012"></a><a name="p1328182051012"></a>一个常量表达式，通常为字符串字面量，用于指明某个__global__函数。</p>
</td>
</tr>
<tr id="row4554191717102"><td class="cellrowborder" valign="top" width="17.580000000000002%" headers="mcps1.2.4.1.1 "><p id="p1955421781015"><a name="p1955421781015"></a><a name="p1955421781015"></a>loweredName</p>
</td>
<td class="cellrowborder" valign="top" width="15.559999999999999%" headers="mcps1.2.4.1.2 "><p id="p1255417170103"><a name="p1255417170103"></a><a name="p1255417170103"></a>输出</p>
</td>
<td class="cellrowborder" valign="top" width="66.86%" headers="mcps1.2.4.1.3 "><p id="p6495134121119"><a name="p6495134121119"></a><a name="p6495134121119"></a>与所传入名称表达式对应的底层mangling name；该名称所占用的内存将在通过aclrtcDestroyProg销毁程序对象时自动释放。</p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="section640mcpsimp"></a>

aclError为int类型变量，详细说明请参考[RTC错误码](RTC_error_code.md)。

## 约束说明<a name="section633mcpsimp"></a>

使用前必须先通过[aclrtcAddNameExpr](aclrtcAddNameExpr.md)注册原始名称，且两次传入的nameExpression字符串必须完全相同。

## 调用示例<a name="section837496171220"></a>

```
aclrtcProg prog;
aclrtcCreateProg(&prog, src, "add_custom", 0, nullptr, nullptr);

const char* kernelNameExpr = "add_custom<float>";
aclrtcAddNameExpr(prog, kernelNameExpr);

const char *options[] = {
    "--npu-arch=dav-2201",
};
int numOptions = sizeof(options) / sizeof(options[0]);
aclrtcCompileProg(prog, numOptions, options);

const char* manglingName = "";
aclrtcGetLoweredName(prog, kernelNameExpr, &manglingName);
```
