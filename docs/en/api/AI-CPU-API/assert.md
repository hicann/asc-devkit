# assert<a name="ZH-CN_TOPIC_0000002507523647"></a>

<!-- md-trans-meta sourceCommit=13f1af5a09ac01160909f73c31bb7f8d23f4ec30 translatedAt=2026-11-17T03:45:42.111Z pushedAt=2026-11-17T09:52:41.778Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->

<!-- @ref: asc-devkit/res/docs/zh/api/AI-CPU-API/assert_res.md#id1 -->

## Description<a name="section259105813316"></a>

This API implements the **assert** assertion functionality in the debugging scenarios of AI CPU operator kernel functions (Kernel).

During operator execution, if the internal condition of **assert** is not true, information such as the assert condition, the triggering file name, and the line number is output.

## Required Header Files<a name="section78885814919"></a>

```
#include "aicpu_api.h"
```

## Prototype<a name="section2067518173415"></a>

```
assert(expr)
```

## Parameters<a name="section158061867342"></a>

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="16.49%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.93%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.58%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.1.4.1.1 "><p id="p541413413465"><a name="p541413413465"></a><a name="p541413413465"></a>expr</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.1.4.1.2 "><p id="p1441334144620"><a name="p1441334144620"></a><a name="p1441334144620"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.1.4.1.3 "><p id="p84131146466"><a name="p84131146466"></a><a name="p84131146466"></a>Whether the assert terminates the program. If true, the program continues; if false, the program terminates.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

-   This API can only be invoked through <<<...\>\>\> and is used in heterogeneous compilation scenarios.
-   Do not include the system assert.h in kernel function (Kernel) development, as this causes macro definition conflicts.
-   The **assert** API is invoked in the same form as in the C language and does not require the **AscendC** namespace.

-   This API uses the Dump function. For all APIs that use the Dump function, the amount of data dumped at a time on each core must not exceed 1 MB. You must control the amount of content to be printed; otherwise, the content will not be printed.
-   When using this API, if compilation is performed with the bisheng command line, developers need to manually link the relevant static libraries. When compilation is performed with CMake, the framework automatically handles the linking, and no additional attention is required from developers. The specific compilation commands are as follows: link libaicpu\_api.a for the Device through --cce-aicpu-laicpu\_api, and specify the library path of libaicpu\_api.a through --cce-aicpu-L.

    ```
    $bisheng -O2 foo.aicpu --cce-aicpu-L${INSTALL_DIR}/lib64/device/lib64 --cce-aicpu-laicpu_api -I${INSTALL_DIR}/asc/include/aicpu_api -c -o foo.aicpu.o
    ```

    Replace `${INSTALL_DIR}` with the file storage path after CANN software installation. If the software is installed as the root user, the default file storage path is /usr/local/Ascend/cann.

## Example<a name="section82241477610"></a>

Use **assert** to add an assertion check at the location in the operator kernel-side implementation code where an assertion is needed, as shown in the following example:

```
int assertFlag = 10;
// Assertion condition.
assert(assertFlag == 12);
```

When the program runs, **assert** is triggered and prints output similar to the following:

```
[ASSERT]` assertFlag == 12 ' at /home/.../test.cpp:36
```

