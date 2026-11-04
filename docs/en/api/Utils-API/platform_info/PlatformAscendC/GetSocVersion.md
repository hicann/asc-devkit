# GetSocVersion<a name="ZH-CN_TOPIC_0000002078653638"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T03:11:09.380Z pushedAt=2026-10-23T07:01:40.550Z nl=2 -->

## Description<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

Obtains the version model of the current hardware platform.

## Prototype<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
SocVersion GetSocVersion(void) const
```

## Parameters<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

None

## Return Value<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

Returns the enumeration class of the current hardware platform version model. For the mapping between this enumeration class and the AI processor model, see the platform_ascendc.h header file in the include/tiling/platform directory under the file storage path after CANN software installation.

Obtain the AI processor model as follows:

<!-- npu="910b,910,310p,310b" id1 -->
-   For the following products: execute the `npu-smi info` command on the server where the AI processor is installed to query the **Name** information. The actual configuration value is AscendName. For example, if the **Name** value is xxxyy, the actual configuration value is Ascendxxxyy.

    <!-- npu="910b" id2 -->
    Atlas A2 products
    <!-- end id2 -->

    <!-- npu="310b" id3 -->
    Atlas 200I/500 A2 inference product
    <!-- end id3 -->

    <!-- npu="310p" id4 -->
    Atlas inference products
    <!-- end id4 -->

    <!-- npu="910" id5 -->
    Atlas training products
    <!-- end id5 -->
<!-- end id1 -->

<!-- npu="A3" id6 -->
-   For Atlas A3 products, execute the `npu-smi info -t board -i <id> -c <chip_id>` command on the server where the AI processor is installed to query the **Chip Name** and **NPU Name** information. The actual configuration value is `Chip Name_NPU Name`. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234. Where:
    -   `id`: device ID. The NPU ID queried through the `npu-smi info -l` command is the device ID, which is represented as `<id>` in the command.
    -   `chip_id`: chip ID. The Chip ID queried through the `npu-smi info -m` command is the chip ID, which is represented as `<chip_id>` in the command.
<!-- end id6 -->

<!-- npu="950" id7 -->
-   For Ascend 950PR&950DT products, execute the `npu-smi info -t board -i <id>` command on the server where the AI processor is installed to query the **Chip Name** and **NPU Name** information. The actual configuration value is `Chip Name_NPU Name`. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234.

    Here, `id` is the device ID. The NPU ID queried through the `npu-smi info -l` command is the device ID, which is represented as `<id>` in the command.
<!-- end id7 -->

## Constraints<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

To support operator binary reuse, when Atlas A3 products are used, the enumeration value returned by calling the GetSocVersion API is the same as that for Atlas A2 products. To distinguish between them, use the aclrtGetSocName API.

## Examples<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

### Custom Operator Project

```cpp
ge::graphStatus TilingXXX(gert::TilingContext* context) {
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    auto socVersion = ascendcPlatform.GetSocVersion();
    // Design the tiling strategy based on the obtained version model.
    // Replace ASCENDXXX with the actual version model.
    if (socVersion == platform_ascendc::SocVersion::ASCENDXXX) {
        // ...
    }
    return ret;
}
```

### Kernel Direct Invocation

```cpp
void GetInfoFun()
{
    auto* ascendcPlatform = platform_ascendc::PlatformAscendCManager::GetInstance();
    if (ascendcPlatform == nullptr) {
        return;
    }
    auto socVersion = ascendcPlatform->GetSocVersion();
    // Replace ASCENDXXX with the actual version model.
    if (socVersion == platform_ascendc::SocVersion::ASCENDXXX) {
        // ...
    }
}
```
