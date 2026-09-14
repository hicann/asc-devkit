# PlatformAscendCManager<a name="ZH-CN_TOPIC_0000002078498914"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-06T09:46:16.574Z pushedAt=2026-09-11T02:31:02.803Z -->

## Description<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section36583473819"></a>

In the scenario where operators are invoked through Kernel Launch based on a Kernel Launch operator project, you may need to obtain hardware platform information, such as the number of cores on the hardware platform. The **PlatformAscendCManager** class provides the capability to obtain platform information: you can obtain a pointer to the **PlatformAscendC** class through the **GetInstance** method of this class, and then use this pointer to obtain hardware platform information. For the information that can be obtained, see [PlatformAscendC](PlatformAscendC/PlatformAscendC.md).

> [!CAUTION]
> -   To use this capability, include the **tiling/platform/platform\_ascendc.h** header file and link the tiling\_api and platform dynamic libraries in the compilation script.
>     -   The following is an example of including the header file:
>         ```
>         #include "tiling/platform/platform_ascendc.h"
>         ```
>     -   The following is an example of linking the dynamic libraries:
>         ```
>         add_executable(main main.cpp)
>         target_link_libraries(main PRIVATE
>           kernels
>           tiling_api
>           platform
>         )
>         ```

## Prototype<a name="zh-cn_topic_0000001796358754_section7979556121414"></a>

```
class PlatformAscendCManager {
public:
    static PlatformAscendC* GetInstance();
    // In a CPU-only environment without the corresponding NPU hardware, pass customSocVersion to specify the target AI processor model. Note: Because GetInstance is implemented as a singleton, the customSocVersion passed only takes effect on the first call.
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
-   For the following products: Run the **npu-smi info** command on the server where the AI processor is installed to query and obtain the **Name** information. The actual configuration value is AscendName. For example, if the **Name** value is xxxyy, the actual configuration value is Ascendxxxyy.

    <!-- npu="910b" id2 -->
    Atlas A2 training products/Atlas A2 inference products
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
-   For Atlas A3 training products/Atlas A3 inference products, run the npu-smi info -t board -i **_id_** -c **_chip\_id_** command on the server where the AI processor is installed to query and obtain the **Chip Name** and **NPU Name** information. The actual configuration value is Chip Name\_NPU Name. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascendxxx_1234. Where:
    -   id: Device ID. The NPU ID obtained by running the **npu-smi info -l** command is the device ID.
    -   chip\_id: Chip ID. The Chip ID obtained by running the **npu-smi info -m** command is the chip ID.
<!-- end id6 -->

<!-- npu="950" id7 -->
-   For Ascend 950PR/Ascend 950DT, run the npu-smi info -t board -i **_id_** command on the server where the AI processor is installed to query the **Chip Name** and **NPU Name** information. The actual configuration value is the Chip Name\_NPU Name. For example, if the **Chip Name** value is Ascendxxx and the **NPU Name** value is 1234, the actual configuration value is Ascend_xxx_1234.

    Here, id is the device ID. The NPU ID obtained by running the **npu-smi info -l** command is the device ID.
<!-- end id7 -->

## Return Value<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section25791320141317"></a>

None

## Constraints<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section19165124931511"></a>

None

## Example<a name="zh-cn_topic_0000001796358754_zh-cn_topic_0000001442758437_section320753512363"></a>

```
GetInfoFun() {
    ...
    auto coreNum = platform_ascendc::PlatformAscendCManager::GetInstance()->GetCoreNum();
    ...
    return;
}
```

