# printf<a name="ZH-CN_TOPIC_0000002507683569"></a>

<!-- md-trans-meta sourceCommit=74b51a589fae17ae2efa1fa2834a7b5d8d5284dd translatedAt=2026-08-28T06:19:10.673Z pushedAt=2026-08-28T06:39:20.258Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported


## Description<a name="section259105813316"></a>

This interface provides formatted output for AI CPU operator Kernel debugging scenarios, parsing the output content and printing it to the screen by default.

## Header Files to Include<a name="section78885814919"></a>

```
#include "aicpu_api.h"
```

## Prototype<a name="section2067518173415"></a>

```
void printf(const char* fmt, ...)
```

## Parameters<a name="section158061867342"></a>

<a name="zh-cn_topic_0235751031_table33761356"></a>
<table><thead align="left"><tr id="zh-cn_topic_0235751031_row27598891"><th class="cellrowborder" valign="top" width="16.49%" id="mcps1.1.4.1.1"><p id="zh-cn_topic_0235751031_p20917673"><a name="zh-cn_topic_0235751031_p20917673"></a><a name="zh-cn_topic_0235751031_p20917673"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="11.93%" id="mcps1.1.4.1.2"><p id="zh-cn_topic_0235751031_p16609919"><a name="zh-cn_topic_0235751031_p16609919"></a><a name="zh-cn_topic_0235751031_p16609919"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="71.58%" id="mcps1.1.4.1.3"><p id="zh-cn_topic_0235751031_p59995477"><a name="zh-cn_topic_0235751031_p59995477"></a><a name="zh-cn_topic_0235751031_p59995477"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row42461942101815"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.1.4.1.1 "><p id="p45208478318"><a name="p45208478318"></a><a name="p45208478318"></a>fmt</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.1.4.1.2 "><p id="p135196472314"><a name="p135196472314"></a><a name="p135196472314"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.1.4.1.3 "><p id="p1965816506312"><a name="p1965816506312"></a><a name="p1965816506312"></a>Format control string, which contains two types of objects: ordinary characters and conversion specifications.</p>
<a name="ul419411543310"></a><a name="ul419411543310"></a><ul id="ul419411543310"><li>Ordinary characters are printed as-is.</li><li>Conversion specifications are not output directly but are used to control the conversion and printing of the arguments in printf. Each conversion specification starts with a percent sign (%) and ends with the conversion specification, indicating the type of the output data.<div class="p" id="p158820115597"><a name="p158820115597"></a><a name="p158820115597"></a>The supported conversion types include:<a name="ul541124915329"></a><a name="ul541124915329"></a><ul id="ul541124915329"><li>%d / %i: Outputs a decimal number. Supported data types: bool/int8_t/int16_t/int32_t/int64_t.</li><li>%f: Outputs a real number. Supported data types: float/half.</li><li>%x: Outputs a hexadecimal integer. Supported data types: int8_t/int16_t/int32_t/int64_t/uint8_t/uint16_t/uint32_t/uint64_t.</li><li>%s: Outputs a string.</li><li>%u: Outputs unsigned data. Supported data types: bool/uint8_t/uint16_t/uint32_t/uint64_t.</li><li>%p: Outputs a pointer address.</li></ul>
</div>
</li></ul></td>
</tr>
<tr id="row163919564263"><td class="cellrowborder" valign="top" width="16.49%" headers="mcps1.1.4.1.1 "><p id="p1563916565265"><a name="p1563916565265"></a><a name="p1563916565265"></a>...</p></td>
<td class="cellrowborder" valign="top" width="11.93%" headers="mcps1.1.4.1.2 "><p id="p59396564285"><a name="p59396564285"></a><a name="p59396564285"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="71.58%" headers="mcps1.1.4.1.3 "><p id="p1257115311337"><a name="p1257115311337"></a><a name="p1257115311337"></a>Additional parameters, a variable-length parameter list: Depending on the fmt string, the function may require a series of additional parameters, each containing a value to be inserted, replacing each % tag specified in the fmt parameter. The number of parameters must be the same as the number of % tags.</p></td>
</tr>
</tbody>
</table>

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section43265506459"></a>

-   This API supports only invocation through <<<...>>> and is used in heterogeneous compilation scenarios.
-   This API does not support printing escape characters other than the newline character.
-   This API uses the Dump function. For all APIs that use the Dump function, the total amount of data dumped on each core must not exceed 1 MB. Developers must control the amount of content to be printed; otherwise, the content will not be printed.
-   When using this API, if the bisheng command line is used for compilation, developers need to manually link the relevant static library. When CMake is used for compilation, the framework automatically handles the linking, and developers do not need to pay extra attention. The specific compilation commands are as follows: link libaicpu_api.a for the Device through --cce-aicpu-laicpu_api, and specify the library path of libaicpu_api.a through --cce-aicpu-L.

    ```
    $bisheng -O2 foo.aicpu --cce-aicpu-L${INSTALL_DIR}/lib64/device/lib64 --cce-aicpu-laicpu_api -I${INSTALL_DIR}/include/ascendc/aicpu_api -c -o foo.aicpu.o
    ```

    Replace ${INSTALL_DIR} with the file storage path after CANN software installation. For example, when installed as the root user, the default file storage path after installation is /usr/local/Ascend/cann.

## Example<a name="section82241477610"></a>

Call the **printf** interface at the location in the operator Kernel-side implementation code where logs need to be output to print the relevant content. The sample is as follows:

```
#include "aicpu_api.h"

// Print an integer.
AscendC::printf("fmt string %d\n", 0x123);

// Print a floating-point number.
float a = 3.14;
AscendC::printf("fmt string %f\n", a);

// Print a pointer.
int b = 10;
int *c = &b;
AscendC::printf("TEST %p\n", c);
```

The print effect when the program runs is as follows:

```
fmt string 291
fmt string 3.140000
TEST 0xdfffd6fddd1c
```

