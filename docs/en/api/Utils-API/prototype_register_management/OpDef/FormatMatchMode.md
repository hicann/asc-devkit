# FormatMatchMode<a name="ZH-CN_TOPIC_0000002128329073"></a>

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T10:57:13.338Z pushedAt=2026-11-13T08:46:49.376Z nl=2 -->

## Description<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the format matching mode for input and output tensors.

## Prototype<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpDef &FormatMatchMode(FormatCheckOption option)
```

## Parameters<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="p19712111142511"><a name="p19712111142511"></a><a name="p19712111142511"></a>option</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="p265693111279"><a name="p265693111279"></a><a name="p265693111279"></a>Format match mode configuration parameter, of the FormatCheckOption enum type. The following values are supported:</p>
<a name="zh-cn_topic_0000002091517061_ul1917131251512"></a><a name="zh-cn_topic_0000002091517061_ul1917131251512"></a><ul id="zh-cn_topic_0000002091517061_ul1917131251512"><li>DEFAULT: Converts the input and output in <span>NCHW/NHWC/DHWCN/NCDHW/NCL format to ND format for processing</span>.</li><li>STRICT: Distinguishes data formats strictly. <span>For NCHW/NHWC/DHWCN/NCDHW/NCL formats, the aclnn framework does not perform conversion</span><span>.</span></li></ul>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpDef operator definition. For details about OpDef, see [OpDef](OpDef.md).

## Constraints<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

If this API is not called, inputs and outputs in the NCHW/NHWC/DHWCN/NCDHW/NCL formats are converted to the ND format for processing by default.

## Examples<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_section320753512363"></a>

In the following example, input x of the AddCustom operator supports only the NCHW format, and input y supports only the NHWC format. Therefore, FormatMatchMode\(FormatCheckOption::STRICT\) must be configured. Otherwise, the aclnn framework converts the formats to ND before passing them to the operator tiling.

```
AddCustom(const char* name) : OpDef(name)
{
	this->Input("x")
		.ParamType(REQUIRED)
		.DataType({ge::DT_FLOAT})
		.FormatList({ge::FORMAT_NCHW});
	this->Input("y")
		.ParamType(REQUIRED)
		.DataType({ge::DT_FLOAT})
		.FormatList({ge::FORMAT_NHWC});
	this->Output("z")
		.ParamType(REQUIRED)
		.DataType({ge::DT_FLOAT})
		.FormatList({ge::FORMAT_ND});
	this->AICore().SetTiling(optiling::TilingFunc);
	this->AICore().AddConfig("ascendxxx");
        this->FormatMatchMode(FormatCheckOption::STRICT);
}
```

