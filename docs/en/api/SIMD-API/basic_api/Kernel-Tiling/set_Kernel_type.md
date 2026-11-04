# Setting the Kernel Type<a name="ZH-CN_TOPIC_0000001777592824"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T15:29:52.418Z pushedAt=2026-10-26T07:41:44.421Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/Kernel-Tiling/set_Kernel_type_res.md#id1 -->


## Description<a name="zh-cn_topic_0000001610027821_section212607105720"></a>

Used to customize the **kernel** type, so that only cores of the specified type are started during operator execution, avoiding starting cores that are not needed and reducing core startup overhead.

## Prototype<a name="zh-cn_topic_0000001610027821_section1630753514297"></a>

-   Sets the global default kernel type, which takes effect for all tiling keys.

    Currently, it is supported in custom operator projects and kernel function (Kernel) launch projects.

    ```
    KERNEL_TASK_TYPE_DEFAULT(value)
    ```

-   Sets the kernel type corresponding to a specific tiling key.

    Currently supported only in custom operator projects.

    ```
    KERNEL_TASK_TYPE(key, value)
    ```

## Parameters<a name="zh-cn_topic_0000001610027821_section129451113125413"></a>

**Table 1**  Parameters

<a name="table89201718635"></a>
<table><thead align="left"><tr id="row1992019181938"><th class="cellrowborder" valign="top" width="17.22%" id="mcps1.2.4.1.1"><p id="p109201118735"><a name="p109201118735"></a><a name="p109201118735"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="15.340000000000002%" id="mcps1.2.4.1.2"><p id="p8920018334"><a name="p8920018334"></a><a name="p8920018334"></a>Input/Output</p></th>
<th class="cellrowborder" valign="top" width="67.44%" id="mcps1.2.4.1.3"><p id="p199211318136"><a name="p199211318136"></a><a name="p199211318136"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row692113181531"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p1392117186310"><a name="p1392117186310"></a><a name="p1392117186310"></a>key</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p16921418832"><a name="p16921418832"></a><a name="p16921418832"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p12921131811315"><a name="p12921131811315"></a><a name="p12921131811315"></a>Key value of the tiling key. This parameter is a positive number and indicates a branch of a Kernel.</p></td>
</tr>
<tr id="row1792151817316"><td class="cellrowborder" valign="top" width="17.22%" headers="mcps1.2.4.1.1 "><p id="p119219181935"><a name="p119219181935"></a><a name="p119219181935"></a>value</p></td>
<td class="cellrowborder" valign="top" width="15.340000000000002%" headers="mcps1.2.4.1.2 "><p id="p18921181812316"><a name="p18921181812316"></a><a name="p18921181812316"></a>Input</p></td>
<td class="cellrowborder" valign="top" width="67.44%" headers="mcps1.2.4.1.3 "><p id="p14921918033"><a name="p14921918033"></a><a name="p14921918033"></a>Kernel type to set. For the value range and detailed description of the kernel type, see <a href="#table76335324910">Table 2</a>. Different hardware architectures support different parameter values. For the supported parameter values, see <a href="#li693212153417">kernel&nbsp;type value constraints</a>.</p>
<a name="screen2031620461932"></a><a name="screen2031620461932"></a><pre class="screen" codetype="Cpp" id="screen2031620461932">enum KernelMetaType {
    KERNEL_TYPE_AIV_ONLY,
    KERNEL_TYPE_AIC_ONLY,
    KERNEL_TYPE_MIX_AIV_1_0,
    KERNEL_TYPE_MIX_AIC_1_0,
    KERNEL_TYPE_MIX_AIC_1_1,
    KERNEL_TYPE_MIX_AIC_1_2,
<span>    KERNEL_TYPE_AICORE</span>,
<span>    KERNEL_TYPE_VECTORCORE,</span>
<span>    KERNEL_TYPE_MIX_AICORE,</span>
<span>    KERNEL_TYPE_MIX_VECTOR_CORE</span>,
<span>    KERNEL_TYPE_MAX</span>
};</pre></td>
</tr>
</tbody>
</table>

**Table 2**  kernel type value description

<a name="table76335324910"></a>
<table><thead align="left"><tr id="row13633133211918"><th class="cellrowborder" valign="top" width="35.8%" id="mcps1.2.3.1.1"><p id="p196346329913"><a name="p196346329913"></a><a name="p196346329913"></a>Parameter</p></th>
<th class="cellrowborder" valign="top" width="64.2%" id="mcps1.2.3.1.2"><p id="p1163403212911"><a name="p1163403212911"></a><a name="p1163403212911"></a>Description</p></th>
</tr>
</thead>
<tbody><tr id="row1663413326917"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p12634173214918"><a name="p12634173214918"></a><a name="p12634173214918"></a>KERNEL_TYPE_AIV_ONLY</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p14634183212912"><a name="p14634183212912"></a><a name="p14634183212912"></a>During operator execution, only the Vector cores on the AI Cores are started. For example, if the user sets **numBlocks** to 10 on the host side, 10 Vector cores are started.</p></td>
</tr>
<tr id="row166346321898"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p1863413321917"><a name="p1863413321917"></a><a name="p1863413321917"></a>KERNEL_TYPE_AIC_ONLY</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p17634113218918"><a name="p17634113218918"></a><a name="p17634113218918"></a>During operator execution, only the Cube cores on the AI Cores are started. For example, if the user sets **numBlocks** to 10 on the host side, 10 Cube cores are started.</p></td>
</tr>
<tr id="row196349321693"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p1863411329915"><a name="p1863411329915"></a><a name="p1863411329915"></a>KERNEL_TYPE_MIX_AIV_1_0</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p101533312255"><a name="p101533312255"></a><a name="p101533312255"></a>In the AIC and AIV hybrid scenario, when <a href="../sync_control/inter_core_sync/inter_core_sync.md">multi-core control instructions</a> are used, set kernel function type to MIX AIV:AIC 1:0 (with hard synchronization). During operator execution, only the Vector cores on the AI Core are started. For example, if the user sets **numBlocks** to 10 on the host side, 10 Vector cores are started.</p>
<p id="p15178718334"><a name="p15178718334"></a><a name="p15178718334"></a>The concept of hard synchronization is explained as follows: When different cores operate on the same global memory and data dependency issues such as read-after-write, write-after-read, and write-after-write may exist, call the <a href="../sync_control/inter_core_sync/SyncAll.md">SyncAll()</a> function to insert synchronization statements to avoid data read/write errors that may occur due to the aforementioned data dependencies. Currently, multi-core synchronization is divided into hard synchronization and soft synchronization. Hard synchronization uses the hardware's built-in full-core synchronization instructions, with the hardware ensuring multi-core synchronization.</p></td>
</tr>
<tr id="row12635183213919"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p7635932295"><a name="p7635932295"></a><a name="p7635932295"></a>KERNEL_TYPE_MIX_AIC_1_0</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p121619511252"><a name="p121619511252"></a><a name="p121619511252"></a>In the AIC and AIV hybrid scenario, when <a href="../sync_control/inter_core_sync/inter_core_sync.md">multi-core control instructions</a> are used, set the Kernel type to MIX AIC:AIV 1:0 (with hard synchronization). During operator execution, only the Cube cores on the AI Core are started. For example, if the user sets **numBlocks** to 10 on the host side, 10 Cube cores are started.</p></td>
</tr>
<tr id="row76354321096"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p146354324910"><a name="p146354324910"></a><a name="p146354324910"></a>KERNEL_TYPE_MIX_AIC_1_1</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p1763563216919"><a name="p1763563216919"></a><a name="p1763563216919"></a>In the AIC and AIV hybrid scenario, set the Kernel type to MIX AIC:AIV 1:1. During operator execution, both the Cube cores and Vector cores on the AI Cores are started. For example, if the user sets **numBlocks** to 10 on the host side, 10 Cube cores and 10 Vector cores are started.</p></td>
</tr>
<tr id="row15635332191"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p13635113220918"><a name="p13635113220918"></a><a name="p13635113220918"></a>KERNEL_TYPE_MIX_AIC_1_2</p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p16635332397"><a name="p16635332397"></a><a name="p16635332397"></a>In the AIC and AIV hybrid scenario, set the Kernel type to MIX AIC:AIV 1:2. During operator execution, both the Cube cores and Vector cores on the AI Cores are started. For example, if the user sets **numBlocks** to 10 on the host side, 10 Cube cores and 20 Vector cores are started.</p></td>
</tr>
<tr id="row66355321498"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p863516323915"><a name="p863516323915"></a><a name="p863516323915"></a><span>KERNEL_TYPE_AICORE </span></p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p1990333814243"><a name="p1990333814243"></a><a name="p1990333814243"></a>During operator execution, only the AI Cores are started. For example, if the user sets **numBlocks** to 5 on the host side, 5 AI Cores are started.</p></td>
</tr>
<tr id="row86359321091"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p363511321396"><a name="p363511321396"></a><a name="p363511321396"></a><span>KERNEL_TYPE_VECTORCORE</span></p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p82581757123"><a name="p82581757123"></a><a name="p82581757123"></a>This parameter is reserved and not supported in the current version.</p></td>
</tr>
<tr id="row2635113215912"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p18921145171312"><a name="p18921145171312"></a><a name="p18921145171312"></a><span>KERNEL_TYPE_MIX_AICORE</span></p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p1842315851216"><a name="p1842315851216"></a><a name="p1842315851216"></a>This parameter is reserved and not supported in the current version.</p></td>
</tr>
<tr id="row3636193212912"><td class="cellrowborder" valign="top" width="35.8%" headers="mcps1.2.3.1.1 "><p id="p663633211910"><a name="p663633211910"></a><a name="p663633211910"></a><span>KERNEL_TYPE_MIX_VECTOR_CORE</span></p></td>
<td class="cellrowborder" valign="top" width="64.2%" headers="mcps1.2.3.1.2 "><p id="p1487434912401"><a name="p1487434912401"></a><a name="p1487434912401"></a>Operators related to vector computation developed based on Ascend C can run on <span id="ph182831320115814"><a name="ph182831320115814"></a><a name="ph182831320115814"></a>Vector Core</span>. Call this API and pass this parameter to enable <span id="ph12899342131115"><a name="ph12899342131115"></a><a name="ph12899342131115"></a>Vector Core</span>.</p>
<p id="p14506850174012"><a name="p14506850174012"></a><a name="p14506850174012"></a><span id="ph5667172611155"><a name="ph5667172611155"></a><a name="ph5667172611155"></a>After <span id="ph19742192610399"><a name="ph19742192610399"></a><a name="ph19742192610399"></a>Vector Core</span> is enabled, both AI Core and Vector Core are started during operator execution for parallel computation. For example, if the user sets **numBlocks** to 10 on the host side, a total of 10 AI Cores and Vector Cores combined are started.</span></p>
<p id="p1166792611510"><a name="p1166792611510"></a><a name="p1166792611510"></a>Note that when setting the number of cores through **SetSimdNumBlocks**, it must be greater than the number of AI Cores; otherwise, VectorCore will not be started.</p></td>
</tr>
</tbody>
</table>

## Constraints<a name="zh-cn_topic_0000001610027821_section65498832"></a>

-   <a name="li693212153417"></a>kernel type value constraints
    <!-- npu="950" id100 -->
    -   Ascend 950PR&950DT products supports KERNEL\_TYPE\_AIV\_ONLY, KERNEL\_TYPE\_AIC\_ONLY, KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2.
    <!-- end id100 -->
    <!-- npu="A3" id101 -->
    -   Atlas A3 products supports KERNEL\_TYPE\_AIV\_ONLY, KERNEL\_TYPE\_AIC\_ONLY, KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2.
    <!-- end id101 -->
    <!-- npu="910b" id102 -->
    -   Atlas A2 products supports KERNEL\_TYPE\_AIV\_ONLY, KERNEL\_TYPE\_AIC\_ONLY, KERNEL\_TYPE\_MIX\_AIV\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_0, KERNEL\_TYPE\_MIX\_AIC\_1\_1, KERNEL\_TYPE\_MIX\_AIC\_1\_2.
    <!-- end id102 -->
    <!-- npu="310p" id103 -->
    -   Atlas inference products support KERNEL\_TYPE\_AICORE, KERNEL\_TYPE\_MIX\_VECTOR\_CORE.
    <!-- end id103 -->

-   **KERNEL\_TASK\_TYPE** has a higher priority than **KERNEL\_TASK\_TYPE\_DEFAULT**. When both the global kernel type and the kernel type of a specific tiling key are set, the kernel type of that tiling key takes the one set by **KERNEL\_TASK\_TYPE**.
-   If the global default kernel type is not set and developers set the kernel type only for some tiling keys, that is, some tiling keys have no kernel type set, the operator kernel compilation will report an error.
-   When a specific kernel task type is set, the user's operator implementation must match the kernel type. For example, if the user sets the kernel type to KERNEL\_TYPE\_MIX\_AIC\_1\_2, the internal operator implementation should correspond to a core ratio of AIC:AIV = 1:2. If the user sets the kernel type to KERNEL\_TYPE\_AIC\_ONLY, the internal operator implementation should be pure cube logic and should not contain any vector logic. The same applies to other kernel types.
-   When a pure cube or pure vec operator is forced to set the kernel type to MIX, the workspace size cannot be set to 0 and must be set to a value greater than 0 (for example, 16 or 32).
-   When using [Tiling Template Programming](../../../Utils-API/Tiling_template_programming/Tiling_template_programming.md), set the kernel function (Kernel) type through ASCENDC\_TPL\_KERNEL\_TYPE\_SEL. There is no need to set it through this API, and this API does not take effect.

## Example<a name="zh-cn_topic_0000001610027821_section97001499599"></a>

-   Example 1: Enable the VectorCore sample
    1.  When the operator kernel-side development is complete, use this API to enable Vector Core. During operator execution, both AI Core and Vector Core are started, and AI Core is used as Vector Core. The example is as follows:

        ```
        extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *workspace, __gm__ uint8_t *tiling)
        {
            GET_TILING_DATA(tilingData, tiling);
            if (workspace == nullptr) {
                return;
            }
            KernelAdd op;
            op.Init(x, y, z, tilingData.numBlocks, tilingData.totalLength, tilingData.tileNum);
            KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_MIX_VECTOR_CORE); // Enable VectorCore.
            if (TILING_KEY_IS(1)) {
                op.Process1();
            } else if (TILING_KEY_IS(2)) {
                op.Process2();
            }
            // ...
        }
        ```

    2.  When the operator host-side Tiling development is complete, the set **numBlocks** represents the total number of AI Cores and Vector Cores. For example, if the user sets **numBlocks** to 10 on the host side, a total of 10 AI Cores and Vector Cores are started. To ensure that Vector Cores are started, the set value should be greater than the number of AI Cores. You can obtain the number of AI Cores through the [GetCoreNumAic](../../../Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md) API and the number of Vector Cores through the [GetCoreNumVector](../../../Utils-API/platform_info/PlatformAscendC/GetCoreNumVector.md) API. The following code snippet shows how to set **numBlocks**. Here it is set to the sum of AI Cores and Vector Cores, indicating that all AI Cores and Vector Cores are started.

        ```
        // Example of the matching host-side tiling function:
        ge::graphStatus TilingFunc(gert::TilingContext* context)
        {	
            // Enable VectorCore and set numBlocks to the number of vector cores in the AI Core plus the number of vector cores in the Vector Core.
            auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
            auto totalCoreNum = ascendcPlatform.GetCoreNumAiv();
            // Replace ASCENDXXX with the actual version model.
            if (ascendcPlatform.GetSocVersion() == platform_ascendc::SocVersion::ASCENDXXX) {
               totalCoreNum = totalCoreNum + ascendcPlatform.GetCoreNumVector();
            }
            context->SetSimdNumBlocks(totalCoreNum);
        }
        ```

-   Example 2: Set the kernel type corresponding to a specific tiling key. The following code is pseudocode and cannot be run directly.

    ```
    extern "C" __global__ __aicore__ void add_custom(__gm__ uint8_t *x, __gm__ uint8_t *y, __gm__ uint8_t *z, __gm__ uint8_t *workspace, __gm__ uint8_t *tiling)
    {
        GET_TILING_DATA(tilingData, tiling);
        if (workspace == nullptr) {
            return;
        }
        KernelAdd op;
        op.Init(x, y, z, tilingData.numBlocks, tilingData.totalLength, tilingData.tileNum);
        KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIV_ONLY); // Set the default kernel type to the AIV-only type.
        if (TILING_KEY_IS(1)) {
            KERNEL_TASK_TYPE(1, KERNEL_TYPE_MIX_AIV_1_0); // Set the kernel type corresponding to tiling key=1 to MIX AIV 1:0.
            op.Process1();
        } else if (TILING_KEY_IS(2)) {
            KERNEL_TASK_TYPE(2, KERNEL_TYPE_AIV_ONLY); // Set the kernel type corresponding to tiling key=2 to AIV-only type.
            op.Process2();
        }
        // ...
    }
    // Example of the matching host-side tiling function:
    ge::graphStatus TilingFunc(gert::TilingContext* context)
    {	
        // ...
        if (context->GetInputShape(0) > 10) {
            context->SetTilingKey(1);
        } else if (some condition) {
            context->SetTilingKey(2);
        }
    }
    ```
