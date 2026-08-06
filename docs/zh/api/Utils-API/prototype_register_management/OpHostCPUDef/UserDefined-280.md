# UserDefined<a name="ZH-CN_TOPIC_0000002489252008"></a>

## 功能说明<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section36583473819"></a>

配置该算子是否为用户自定义算子，HostCPU自定义算子的userDefined仅支持配置为“True”，若开发者没有配置的情况下，该参数会被默认设置为“True”。

## 函数原型<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section13230182415108"></a>

```
OpHostCPUDef &UserDefined(bool flag)
```

## 参数说明<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section75395119104"></a>

<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p10223674448"></a>参数</p>
</th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p645511218169"></a>输入/输出</p>
</th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p1922337124411"></a>说明</p>
</th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p318615392613"></a>flag</p>
</td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001575944081_p320343694214"></a>输入</p>
</td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_p12935163055011"><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_p12935163055011"></a><a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_p12935163055011"></a><span>HostCPU自定义算子的userDefined仅支持配置为“True”，</span><span>若开发者没有配置的情况下，该参数会被默认设置为“True”。</span></p>
</td>
</tr>
</tbody>
</table>

## 返回值说明<a name="zh-cn_topic_0000001575929572_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section25791320141317"></a>

OpHostCPUDef算子定义，OpHostCPUDef请参考[OpHostCPUDef](OpHostCPUDef.md)。

## 约束说明<a name="zh-cn_topic_0000001626409757_zh-cn_topic_0000001526442954_zh-cn_topic_0000001525424352_section19165124931511"></a>

无

