# PlatformAscendCManager<a name="ZH-CN_TOPIC_0000002078498914"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-23T03:16:42.457Z pushedAt=2026-10-23T07:01:40.567Z nl=2 -->

## Description<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section36583473819"></a>

In a Kernel Launch operator project, when an operator is invoked through direct Kernel Launch, you may need to obtain hardware platform information, such as the number of cores on the hardware platform. The PlatformAscendCManager class provides the capability to obtain platform information: you can obtain a pointer to the PlatformAscendC class through the GetInstance method of this class, and then use the pointer to obtain hardware platform information. For the information that can be obtained, see [PlatformAscendC](PlatformAscendC/PlatformAscendC.md).

> [!CAUTION] Caution 
>-   To use this capability, include the "tiling/platform/platform\_ascendc.h" header file and link the tiling\_api and platform dynamic libraries in the build script.
>    -   The following is an example of including the header file:
>        ```
>        #include "tiling/platform/platform_ascendc.h"
>        ```
>    -   The following is an example of linking the dynamic libraries:
>        ```
>        add_executable(main main.cpp)
>        target_link_libraries(main PRIVATE
>          kernels
>          tiling_api
>          platform
>        )
>        ```

## Prototype<a name="zh-cn_topic_0000001796358754_section7979556121414"></a>

```
class PlatformAscendCManager {
public:
    static PlatformAscendC* GetInstance();
    // In a CPU-only environment without the corresponding NPU hardware, pass customSocVersion to specify the target AI processor model. Note: Because GetInstance uses the singleton pattern, only the customSocVersion passed on the first call takes effect.
    static PlatformAscendC* GetInstance(const char *customSocVersion);
private:
...
}
```

## Parameters<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section189014013619"></a>

| Parameter | Description |
|:------|:-----|
|customSocVersion|AI processor model. |

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
    -   `id`: Device ID. The NPU ID queried via the `npu-smi info -l` command is the device ID, represented as `<id>` in the command.
    -   `chip_id`: Chip ID. The Chip ID queried via the `npu-smi info -m` command is the chip ID, represented as `<chip_id>` in the command.
<!-- end id6 -->

<!-- npu="950" id7 -->
-   For Ascend 950PR&950DT products, execute the `npu-smi info -t board -i <id>` command on the server where the AI processor is installed to query the **Chip Name** and **NPU Name** information. The actual configuration value is `Chip Name_NPU Name`. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234.

    Where `id` is the device ID. The NPU ID queried via the `npu-smi info -l` command is the device ID, represented as `<id>` in the command.
<!-- end id7 -->

## Return Value<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Examples<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section320753512363"></a>

```
GetInfoFun() {
    ...
    auto coreNum = platform_ascendc::PlatformAscendCManager::GetInstance()->GetCoreNum();
    ...
    return;
}
```
