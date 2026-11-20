# Conv3DBackpropInput Tiling Constructor

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T06:28:26.915Z pushedAt=2026-11-18T11:07:26.689Z nl=2 -->

## Description

Used to create a Conv3DBackpropInput single-core Tiling object.

## Prototype

-   Parameterized constructor, which requires hardware platform information to be . This type of constructor is recommended for better compatibility.
    -   Pass information using the PlatformAscendC class

        ```
        explicit Conv3DBpInputTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

    -   Pass information using PlatformInfo

        When platform\_ascendc::PlatformAscendC cannot be obtained at Tiling runtime, you need to construct the PlatformInfo structure yourself and pass it to the Conv3DBpInputTiling constructor.

        ```
        explicit Conv3DBpInputTiling(const PlatformInfo& platform)
        ```

-   Parameterless constructor

    ```
    Conv3DBpInputTiling()
    ```

-   Base class constructor

    Conv3DBpInputTiling inherits from the base class Conv3DBpInputTilingBase, whose constructor is as follows:

    ```
    Conv3DBpInputTilingBase()
    ```

    ```
    explicit Conv3DBpInputTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
    ```

    ```
    explicit Conv3DBpInputTilingBase(const PlatformInfo& platform)
    ```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Hardware platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md). |
| platform | Input | Hardware version and the memory size provided by each hardware unit in the AI Core. When PlatformInfo is constructed, it is obtained through [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md).<br><br>The PlatformInfo structure is defined as shown in the following code. socVersion is obtained and passed through [GetSocVersion](../../../../Utils-API/platform_info/PlatformAscendC/GetSocVersion.md), and the size of each type of hardware storage space is obtained and passed through [GetCoreMemSize](../../../../Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md).<br><br>It is not recommended to call the constructor by directly filling in values to construct PlatformInfo, for example, PlatformInfo(, 1024, 1024, ..); |

```
struct PlatformInfo {
    platform_ascendc::SocVersion socVersion;
    uint64_t l1Size = 0;
    uint64_t l0CSize = 0;
    uint64_t ubSize = 0;
    uint64_t l0ASize = 0;
    uint64_t l0BSize = 0;
};
```

When implementing the Tiling function on the host side, platform\_ascendc::PlatformAscendC is used to obtain some hardware platform information to support Tiling computation, such as the number of cores on the hardware platform. The PlatformAscendC class provides the functionality to obtain this platform information.

Unlike platform\_ascendc::PlatformAscendC, PlatformInfo is used to obtain information specific to a single AI Core, such as the chip version and the memory size provided by each hardware unit in the AI Core.

## Constraints

None

## Examples

-   Parameterless constructor

    ```
    ConvBackpropApi::Conv3DBpInputTiling tiling;
    tiling.SetWeightType(ConvCommonApi::TPosition::GM,ConvCommonApi::ConvFormat::FRACTAL_Z_3D,ConvCommonApi::ConvDtype::FLOAT16);
    ...
    optiling::Conv3DBackpropInputTilingData tilingData;
    int ret = tiling.GetTiling(tilingData);    // if ret = -1, gen tiling failed
    ...
    ```

-   Parameterized constructor

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    ConvBackpropApi::Conv3DBpInputTiling tiling(ascendcPlatform);
    tiling.SetWeightType(
        ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT16);
    ...
    optiling::Conv3DBackpropInputTilingData tilingData;
    int ret = tiling.GetTiling(tilingData); // if ret = -1, gen tiling failed
    ```
