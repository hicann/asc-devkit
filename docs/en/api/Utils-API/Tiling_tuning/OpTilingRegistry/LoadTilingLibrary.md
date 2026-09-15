# LoadTilingLibrary<a name="ZH-CN_TOPIC_0000002333459065"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T16:12:53.720Z pushedAt=2026-09-11T06:41:24.203Z -->

## Description<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section36583473819"></a>

Loads the corresponding Tiling dynamic library based on the input path. After developers complete the operator implementation using the engineering-based operator development approach, they can obtain the corresponding Tiling dynamic library file through **operator package compilation** or **operator dynamic library compilation**.

-   Operator package compilation: The dynamic library corresponding to the Tiling implementation is liboptiling.so in the operator package deployment directory. For the specific path, see [operator package deployment](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/advanced_programming/aclnn_operator_development/operator_package_compilation/operator_package_deployment.md).

-   Dynamic library compilation: The Tiling implementation is integrated into the operator dynamic library libcust\_opapi.so. For the specific path, see [operator dynamic library and static library compilation](../../../../guide/programming_guide/advanced_programming/aclnn_operator_development/operator_dynamic_and_static_library_compilation.md).

## Prototype<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section13230182415108"></a>

```
bool LoadTilingLibrary(const char *tilingSoPath) const
```

## Parameters<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section75395119104"></a>

<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_table111938719446"></a>
<table><thead align="left"><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row6223476444"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p10223674448"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p10223674448"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p10223674448"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p645511218169"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p645511218169"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p645511218169"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p1922337124411"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p1922337124411"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p1922337124411"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_row152234713443"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.1.4.1.1 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p8563195616313"></a>tilingSoPath</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.1.4.1.2 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p15663137127"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.1.4.1.3 "><p id="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p2684123934216"><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p2684123934216"></a><a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_p2684123934216"></a>Path of the Tiling dynamic library, supporting both relative and absolute paths.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section25791320141317"></a>

true: The Tiling dynamic library is loaded successfully; false: The Tiling dynamic library fails to load. For specific errors, see the log information.

For log configuration and viewing, see the log-related environment variables in [Environment Variable Reference](https://www.hiascend.com/document/redirect/CannCommunityEnvRef).

## Constraints<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000002078486173_zh-cn_topic_0000001576727153_zh-cn_topic_0000001389787297_section320753512363"></a>

```
context_ascendc::OpTilingRegistry tmpIns;
bool flag = tmpIns.LoadTilingLibrary("/your/path/to/so_path/liboptiling.so");
if (flag == false) {
    std::cout << "Load tiling so failed" << std::endl;
    ...        
}
// ...
```
