# assert<a name="ZH-CN_TOPIC_0000002507523647"></a>

<!-- md-trans-meta sourceCommit=74b51a589fae17ae2efa1fa2834a7b5d8d5284dd translatedAt=2026-08-28T06:19:06.831Z pushedAt=2026-08-28T06:39:20.247Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported


## Description<a name="section259105813316"></a>

This interface implements the assert assertion functionality in the AI CPU operator Kernel debugging scenario.

During operator execution, if the condition inside the assert evaluates to false, information such as the assert condition, the triggering file name, and the line number is output.

## Header Files to Include<a name="section78885814919"></a>

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
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.1.4.1.1 "><p id="p541413413465"><a name="p541413413465"></a><a name="p541413413465"></a>**expr**</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.1.4.1.2 "><p id="p1441334144620"><a name="p1441334144620"></a><a name="p1441334144620"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.1.4.1.3 "><p id="p84131146466"><a name="p84131146466"></a><a name="p84131146466"></a>Condition that determines whether the assert terminates the program. If the condition is true, the program continues; if false, the program terminates.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

-   This API can only be invoked through <<<...\>\>\> and is used in heterogeneous compilation scenarios.
-   Do not include the system **assert.h** in kernel development, as it causes macro definition conflicts.
-   The **assert** API is invoked in the same form as in C and does not require the **AscendC** namespace.

-   This API uses the Dump function. For all APIs that use the Dump function, the total amount of data dumped on each core must not exceed 1M. Developers must control the amount of content to be printed; otherwise, the content will not be printed.
-   When using this API, if compilation is performed through the bisheng command line, developers need to manually link the relevant static library. When compilation is performed through CMake, the framework automatically handles the linking, and no additional attention is required from developers. The specific compilation commands are as follows: link **libaicpu\_api.a** for the Device through **--cce-aicpu-laicpu\_api**, and specify the library path of **libaicpu\_api.a** through **--cce-aicpu-L**.

    ```
    $bisheng -O2 foo.aicpu --cce-aicpu-L${INSTALL_DIR}/lib64/device/lib64 --cce-aicpu-laicpu_api -I${INSTALL_DIR}/include/ascendc/aicpu_api -c -o foo.aicpu.o
    ```

    Replace $\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. For example, when installed as the root user, the default file storage path is /usr/local/Ascend/cann.

## Example<a name="section82241477610"></a>

Use **assert** to check the code at the location where an assertion needs to be added in the operator kernel implementation, as shown in the following example:

```
int assertFlag = 10;
// Assertion condition.
assert(assertFlag == 12);
```

When the program runs, the assert is triggered and the following is printed:

```
[ASSERT]` assertFlag == 12 ' at /home/.../test.cpp:36
```

