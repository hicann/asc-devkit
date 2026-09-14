# GetSocVersion<a name="ZH-CN_TOPIC_0000002078653638"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-06T09:37:20.330Z pushedAt=2026-09-08T10:07:47.284Z -->

## Description<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the version model of the current hardware platform.

## Prototype<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
SocVersion GetSocVersion(void) const
```

## Parameters<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

Enumeration class for the current hardware platform version model. For the mapping between this enumeration class and AI processor models, refer to the **include/tiling/platform/platform\_ascendc.h** header file in the file storage path after CANN software installation.

Obtain the AI processor model as follows:

<!-- npu="910b,910,310p,310b" id1 -->
-   For the following products: run the **npu-smi info** command on the server where the AI processor is installed to query the **Name** information. The actual configuration value is AscendName. For example, if the **Name** value is xxxyyy, the actual configuration value is Ascendxxxyy.

    <!-- npu="910b" id2 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id2 -->

    <!-- npu="310b" id3 -->
    Atlas 200I/500 A2 inference products
    <!-- end id3 -->

    <!-- npu="310p" id4 -->
    Atlas inference products
    <!-- end id4 -->

    <!-- npu="910" id5 -->
    Atlas training products
    <!-- end id5 -->
<!-- end id1 -->

<!-- npu="A3" id6 -->
-   For Atlas A3 training products/Atlas A3 inference products, run the npu-smi info -t board -i **_id_** -c **_chip\_id_** command on the server where the AI processor is installed to query the **Chip Name** and **NPU Name** information. The actual configuration value is Chip Name\_NPU Name. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234. Where:
    -   id: Device ID. The NPU ID queried by the **npu-smi info -l** command is the device ID.
    -   chip\_id: Chip ID. The Chip ID queried by the **npu-smi info -m** command is the chip ID.
<!-- end id6 -->

<!-- npu="950" id7 -->
-   For Ascend 950PR/Ascend 950DT, run the npu-smi info -t board -i **_id_** command on the server where the AI processor is installed to obtain the **Chip Name** and **NPU Name** information. The actual configuration value is the Chip Name\_NPU Name. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascend_xxx_1234.

    Here, id is the device ID. The NPU ID obtained by running the **npu-smi info -l** command is the device ID.
<!-- end id7 -->

## Constraints<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

To support operator binary reuse, when using Atlas A3 training products/Atlas A3 inference products, the enumeration value returned by calling the **GetSocVersion** API is the same as that of Atlas A2 training products/Atlas A2 inference products. If you need to distinguish them, use the **aclrtGetSocName** API.

## Example<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

```
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto socVersion = ascendcPlatform.GetSocVersion();
    // Design the Tiling strategy based on the obtained version model.
    // Replace ASCENDXXX with the actual version model.
    if (socVersion == platform_ascendc::SocVersion::ASCENDXXX) {
        // ...
    }
    return ret;
}
```

