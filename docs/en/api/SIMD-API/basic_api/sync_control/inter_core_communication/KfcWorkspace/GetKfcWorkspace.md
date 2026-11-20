# GetKfcWorkspace<a name="ZH-CN_TOPIC_0000002005165737"></a>

<!-- md-trans-meta sourceCommit=22a41aa8a74d7a2c3f454ea4bd5f0f8fb27aee29 translatedAt=2026-11-19T02:53:39.814Z pushedAt=2026-11-19T05:58:49.112Z nl=2 -->

## Supported Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/inter_core_communication/KfcWorkspace/GetKfcWorkspace_res.md#id1 -->


## Function Description<a name="zh-cn_topic_0000001526206862_section212607105720"></a>

Obtains the memory address used for the CubeResGroupHandle message communication area. When using the [CubeResGroupHandle](../../inter_core_communication/CubeResGroupHandle/CubeResGroupHandle_constructor.md) API, use this API to manage the space address independently.

## Prototype<a name="section765814724715"></a>

```
__aicore__ inline GM_ADDR GetKfcWorkspace()
```

## Parameters<a name="zh-cn_topic_0000001526206862_section129451113125413"></a>

None

## Return Value<a name="section358232015810"></a>

Workspace address.

## Constraints<a name="zh-cn_topic_0000001526206862_section65498832"></a>

This API cannot be used together with the [CreateCubeResGroup](../../inter_core_communication/CubeResGroupHandle/CreateCubeResGroup.md) API.

## Example<a name="zh-cn_topic_0000001526206862_section97001499599"></a>

```
AscendC::KfcWorkspace desc(workspaceGM);
GM_ADDR workspace = desc.GetKfcWorkspace();
```

