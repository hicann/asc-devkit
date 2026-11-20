# Comment<a name="ZH-CN_TOPIC_0000002078492696"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-11-10T10:56:58.804Z pushedAt=2026-11-13T08:46:49.375Z nl=2 -->

## Description<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section36583473819"></a>

Sets the operator group information and operator prototype comments, including the operator brief description and operator constraints. It is used to generate operator prototype comments synchronously when automatically generating the operator prototype header file.

Based on the OpDef operator prototype definition, a custom operator project can provide the following automation capabilities: automatically generate the operator prototype header file used in graph mode scenarios, so that you can use the generated operator prototype to perform operations such as graph construction, graph compilation, and graph execution.

The generated comments help you understand the operator prototype, and can be used to automatically generate documentation for the operator prototype. In most cases, built-in CANN operators use these comments extensively. You can use them as needed.

## Prototype<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpDef &Comment(CommentSection section, const char *comment)
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
<tbody><tr id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p318615392613"></a>section</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_p096733515614"><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_p096733515614"></a><a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_p096733515614"></a>Specifies the function of this API through the CommentSection class. The following values are supported:</p>
<a name="zh-cn_topic_0000002091517061_ul1917131251512"></a><a name="zh-cn_topic_0000002091517061_ul1917131251512"></a><ul id="zh-cn_topic_0000002091517061_ul1917131251512"><li>CATEGORY: the comment content sets the operator group name;</li><li>BRIEF: the comment content sets the operator @brief comment content, that is, a brief description of the operator function;</li><li>CONSTRAINTS: the comment content sets the operator @Attention Constraints comment content, that is, the constraints of the operator;</li><li>RESTRICTIONS: the comment content sets the operator @Restrictions comment content. This option is currently an experimental parameter and is not recommended;</li><li>SEE: the comment content sets the operator @see comment content, which can indicate operators related to this operator;</li><li>THIRDPARTYFWKCOMPAT: the comment content sets the third-party operator referenced by the operator.</li></ul>
</td>
</tr>
<tr id="zh-cn_topic_0000002091517061_row1946220499205"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002091517061_p104626492207"><a name="zh-cn_topic_0000002091517061_p104626492207"></a><a name="zh-cn_topic_0000002091517061_p104626492207"></a>comment</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002091517061_p7462104912020"><a name="zh-cn_topic_0000002091517061_p7462104912020"></a><a name="zh-cn_topic_0000002091517061_p7462104912020"></a>Input</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002091517061_p84621049172017"><a name="zh-cn_topic_0000002091517061_p84621049172017"></a><a name="zh-cn_topic_0000002091517061_p84621049172017"></a>Adds comment content.</p>
</td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpDef operator definition. For details about OpDef, see [OpDef](OpDef.md).

## Constraints<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001525424352_section19165124931511"></a>

When you use the CATEGORY parameter to set the operator group name, a code file with the same name is generated. If the file name is too long, it may exceed the tar package file name length limit during compilation, causing an error.

For details, see [Operator project compilation error due to file name too long](../../../../guide/programming_guide/appendix/faq/filename_too_long_error.md).

## Examples<a name="zh-cn_topic_0000002091517061_zh-cn_topic_0000001526111046_zh-cn_topic_0000001575944081_section320753512363"></a>

```
 AddCustomComment(const char* name) : OpDef(name)
{
	this->Comment(CommentSection::CATEGORY, "catg"); // Operator group.
	this->Comment(CommentSection::BRIEF, "Brief cmt") // BRIEF comment.
	        .Comment(CommentSection::CONSTRAINTS, "Constraints cmt1") // CONSTRAINTS comment.
	        .Comment(CommentSection::CONSTRAINTS, "Constraints cmt2");
	this->Comment(CommentSection::RESTRICTIONS, "Restrictions cmt1") // RESTRICTIONS comment.
		.Comment(CommentSection::RESTRICTIONS, "Restrictions cmt2")
		.Comment(CommentSection::THIRDPARTYFWKCOMPAT, "Third-party framework compatibility cmt1") // THIRDPARTYFWKCOMPAT comment.
		.Comment(CommentSection::THIRDPARTYFWKCOMPAT, "Third-party framework compatibility cmt2")
		.Comment(CommentSection::SEE, "See cmt1")// SEE comment.
		.Comment(CommentSection::SEE, "See cmt2");
	this->Input("x")
		.ParamType(REQUIRED)
		.DataType({ge::DT_FLOAT, ge::DT_INT32})
		.FormatList({ge::FORMAT_ND});
	this->Input("y")
		.ParamType(REQUIRED)
		.DataType({ge::DT_FLOAT, ge::DT_INT32})
		.FormatList({ge::FORMAT_ND});

	this->Output("z")
		.ParamType(REQUIRED)
		.DataType({ge::DT_FLOAT, ge::DT_INT32})
		.FormatList({ge::FORMAT_ND});
	this->AICore()
		.SetTiling(optiling::TilingFunc);
	this->AICore().AddConfig("ascendxxx");
}
```

