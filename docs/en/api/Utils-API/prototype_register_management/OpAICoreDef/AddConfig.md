# AddConfig<a name="ZH-CN_TOPIC_0000002114052057"></a>

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-06T10:10:48.232Z pushedAt=2026-09-11T02:35:55.818Z -->

## Description<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section36583473819"></a>

Registers the AI processor models supported by the operator and the [OpAICoreConfig](../OpAICoreConfig/OpAICoreConfig.md) information.

## Prototype<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
void AddConfig(const char *soc)
void AddConfig(const char *soc, OpAICoreConfig &aicore_config)
```

## Parameters<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001575929572_row261104521415"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001575929572_p13611145151412"><a name="zh-cn_topic_0000001575929572_p13611145151412"></a><a name="zh-cn_topic_0000001575929572_p13611145151412"></a>soc</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575929572_p9611345141418"><a name="zh-cn_topic_0000001575929572_p9611345141418"></a><a name="zh-cn_topic_0000001575929572_p9611345141418"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001575929572_p14611945131415"><a name="zh-cn_topic_0000001575929572_p14611945131415"></a><a name="zh-cn_topic_0000001575929572_p14611945131415"></a>Supported AI processor model. For the filling rules, refer to the <span>ASCEND_COMPUTE_UNIT field in the CMakePresets.json compilation configuration file under the operator project directory. The value of this field is automatically generated when the project is created using msOpGen</span>.</p></td>
</tr>
<tr id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a>aicore_config</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001575929572_p1087563416157"><a name="zh-cn_topic_0000001575929572_p1087563416157"></a><a name="zh-cn_topic_0000001575929572_p1087563416157"></a>For AI Core configuration information, see the definition of <a href="../OpAICoreConfig/OpAICoreConfig.md">OpAICoreConfig</a>.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section19165124931511"></a>

When the aicore\_config parameter is not passed, some parameters in the **OpAICoreConfig** structure are configured with default values. The specific parameters and their default values are listed in the following table:

**Table 1**  Default configuration of **OpAICoreConfig** when the aicore\_config parameter is not passed

<a name="table56008147710"></a>
<table><thead align="left"><tr id="row3601014470"><th class="cellrowborder" valign="top" width="18.09%" id="mcps1.2.4.1.1"><p id="p19601111411713"><a name="p19601111411713"></a><a name="p19601111411713"></a>Configuration Parameter</p></th>
<th class="cellrowborder" valign="top" width="60.589999999999996%" id="mcps1.2.4.1.2"><p id="p14573171417333"><a name="p14573171417333"></a><a name="p14573171417333"></a>Description</p></th>
<th class="cellrowborder" valign="top" width="21.32%" id="mcps1.2.4.1.3"><p id="p56011114871"><a name="p56011114871"></a><a name="p56011114871"></a>Default Value</p></th>
</tr>
</thead>
<tbody><tr id="row460191414720"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.2.4.1.1 "><p id="p136012142712"><a name="p136012142712"></a><a name="p136012142712"></a><a href="../OpAICoreConfig/DynamicCompileStaticFlag.md">DynamicCompileStaticFlag</a></p></td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001575931912_p183604264191"><a name="zh-cn_topic_0000001575931912_p183604264191"></a><a name="zh-cn_topic_0000001575931912_p183604264191"></a>Whether the operator implementation supports static Shape compilation during operator integration into the graph.</p></td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.2.4.1.3 "><p id="p15601114777"><a name="p15601114777"></a><a name="p15601114777"></a>true</p></td>
</tr>
<tr id="row2060161415717"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.2.4.1.1 "><p id="p26019142078"><a name="p26019142078"></a><a name="p26019142078"></a><a href="../OpAICoreConfig/DynamicFormatFlag.md">DynamicFormatFlag</a></p></td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.2.4.1.2 "><p id="p145734149339"><a name="p145734149339"></a><a name="p145734149339"></a>Whether to automatically derive the supported dtype and format of the operator input and output based on the function set by <a href="SetOpSelectFormat.md">SetOpSelectFormat</a>.</p></td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.2.4.1.3 "><p id="p56015149714"><a name="p56015149714"></a><a name="p56015149714"></a>true</p></td>
</tr>
<tr id="row9601014375"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.2.4.1.1 "><p id="p116011714075"><a name="p116011714075"></a><a name="p116011714075"></a><a href="../OpAICoreConfig/DynamicRankSupportFlag.md">DynamicRankSupportFlag</a></p></td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_p10448657201310"><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_p10448657201310"></a><a name="zh-cn_topic_0000001575612432_zh-cn_topic_0000001576875005_zh-cn_topic_0000001525424352_p10448657201310"></a>Whether the operator supports dynamicRank (dynamic dimension).</p></td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.2.4.1.3 "><p id="p66017146713"><a name="p66017146713"></a><a name="p66017146713"></a>true</p></td>
</tr>
<tr id="row126011142077"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.2.4.1.1 "><p id="p2601101420710"><a name="p2601101420710"></a><a name="p2601101420710"></a><a href="../OpAICoreConfig/DynamicShapeSupportFlag.md">DynamicShapeSupportFlag</a></p></td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.2.4.1.2 "><p id="p5890739574"><a name="p5890739574"></a><a name="p5890739574"></a>Whether the operator supports the dynamic shape scenario during operator integration into the graph.</p></td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.2.4.1.3 "><p id="p1160131416714"><a name="p1160131416714"></a><a name="p1160131416714"></a>true</p></td>
</tr>
<tr id="row160116140712"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.2.4.1.1 "><p id="p16601914377"><a name="p16601914377"></a><a name="p16601914377"></a><a href="../OpAICoreConfig/NeedCheckSupportFlag.md">NeedCheckSupportFlag</a></p></td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0000001626211657_p1114624162912"><a name="zh-cn_topic_0000001626211657_p1114624162912"></a><a name="zh-cn_topic_0000001626211657_p1114624162912"></a>Whether to call the operator parameter validation function during operator fusion to validate data type and Shape.</p></td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.2.4.1.3 "><p id="p1360141410720"><a name="p1360141410720"></a><a name="p1360141410720"></a>false</p></td>
</tr>
<tr id="row5601181415716"><td class="cellrowborder" valign="top" width="18.09%" headers="mcps1.2.4.1.1 "><p id="p13601181417712"><a name="p13601181417712"></a><a name="p13601181417712"></a><a href="../OpAICoreConfig/PrecisionReduceFlag.md">PrecisionReduceFlag</a></p></td>
<td class="cellrowborder" valign="top" width="60.589999999999996%" headers="mcps1.2.4.1.2 "><p id="p957351413338"><a name="p957351413338"></a><a name="p957351413338"></a>This field controls the precision mode of the operator during ATC model conversion or network debugging.</p></td>
<td class="cellrowborder" valign="top" width="21.32%" headers="mcps1.2.4.1.3 "><p id="p46013144712"><a name="p46013144712"></a><a name="p46013144712"></a>true</p></td>
</tr>
</tbody>
</table>

