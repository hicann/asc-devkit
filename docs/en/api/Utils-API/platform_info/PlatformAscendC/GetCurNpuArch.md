# GetCurNpuArch<a name="ZH-CN_TOPIC_0000002543243123"></a>

<!-- md-trans-meta sourceCommit=134c8a713e94913719724061b56ccd2fc3a577d7 translatedAt=2026-09-06T09:28:49.454Z pushedAt=2026-09-11T02:17:43.058Z -->

## Description<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the architecture version number of the current hardware platform chip.

## Prototype<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
NpuArch GetCurNpuArch(void) const
```

## Parameters<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

Enumeration class of the current hardware platform architecture number. For the mapping between this enumeration class and AI processor models, refer to the **include/platform/soc\_spec.h** header file in the file storage path after CANN software installation.

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: DAV\_3510
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: DAV\_2201
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: DAV\_2201
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: DAV\_3002
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: DAV\_2002
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: DAV\_2002
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: DAV\_1001
<!-- end id7 -->

## Constraints<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto npuArch = ascendcPlatform.GetCurNpuArch();
    // Design the tiling strategy based on the obtained version model.
    // Replace DAV_XXX with the actual architecture number.
    if (socVersion == NpuArch::DAV_XXXX) {
        // ...
    }
    return ret;
}
```

