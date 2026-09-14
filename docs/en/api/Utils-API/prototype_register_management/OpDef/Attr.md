# Attr<a name="ZH-CN_TOPIC_0000002114097045"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T10:42:12.785Z pushedAt=2026-09-11T02:57:20.011Z -->

## Description<a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section36583473819"></a>

Registers operator attribute parameters.

When the parameter to be set does not participate in kernel-side computation, it can be registered as an operator attribute parameter.

## Prototype<a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section13230182415108"></a>

```
OpAttrDef &Attr(const char *name)
```

## Parameters<a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section75395119104"></a>

<a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"></a><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"></a><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"></a><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"></a><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p318615392613"></a>name</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"></a><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p320343694214"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"></a><a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_p453018873120"></a>Operator attribute name.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section25791320141317"></a>

Operator attribute definition. For details about **OpAttrDef**, see [OpAttrDef](../OpAttrDef/OpAttrDef-272.md).

## Constraints<a name="zh-cn_topic_0000001705099477_zh-cn_topic_0000001576870901_zh-cn_topic_0000001575944081_section19165124931511"></a>

The **Attr** attribute name cannot be the same as the following Python keywords and built-in variable names; otherwise, an undefined error occurs.

-   Common Python keywords

    and, as, assert, break, class, continue, def, del, elif, else, except, finally, for, from, global, if, import, in, is, lambda, not, or, pass, raise, return, try, while, with, yield, False, None, True, nonlocal, arg.

-   Built-in variable names

    \_\_inputs\_\_, \_\_outputs\_\_, \_\_attrs\_\_, options, bisheng, bisheng\_path, tikcpp\_path, impl\_mode, custom\_compile\_options, custom\_all\_compile\_options, soc\_version, soc\_short, custom\_compile\_options\_soc, custom\_all\_compile\_options\_soc, origin\_func\_name, ascendc\_src\_dir\_ex, ascendc\_src\_dir, ascendc\_src\_file, src, op\_type, code\_channel, op\_info, compile\_op, get\_code\_channel, result, isinstance, attr, get\_current\_build\_config, \_build\_args, get\_dtype\_fmt\_options, shutil, os, get\_kernel\_source,ascendc\_api\_version\_header\_path,ascendc\_api\_version\_file,ascendc\_api\_version,re.

