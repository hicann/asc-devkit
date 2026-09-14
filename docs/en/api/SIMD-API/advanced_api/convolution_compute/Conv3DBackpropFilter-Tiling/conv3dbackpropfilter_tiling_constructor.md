# Conv3DBackpropFilter Tiling Constructor

<!-- md-trans-meta sourceCommit=2737c34bbaf046b0c4e915117e38247d0f621f63 translatedAt=2026-09-05T14:24:24.440Z -->

## Description

Creates a single-core tiling object for **Conv3DBackpropFilter**

## Prototype

-   Parameterized constructor, which requires the hardware platform information to be passed in. This type of constructor is recommended for better compatibility.
    -   Passes information using the **PlatformAscendC** class.

        ```
        explicit Conv3dBpFilterTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

    -   Passes information using **PlatformInfo**.

        When platform\_ascendc::PlatformAscendC cannot be obtained at Tiling runtime, you need to construct the **PlatformInfo** structure yourself and pass it through to the **Conv3dBpFilterTiling** constructor.

        ```
        explicit Conv3dBpFilterTiling(const PlatformInfo& platform)
        ```

-   Parameterless constructor.

    ```
    Conv3dBpFilterTiling()
    ```

-   Base class constructor.

    **Conv3dBpFilterTiling** inherits from the base class **Conv3dBpFilterTilingBase**, whose constructor is as follows:

    ```
    Conv3dBpFilterTilingBase()
    ```

    ```
    explicit Conv3dBpFilterTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
    ```

    ```
    explicit Conv3dBpFilterTilingBase(const PlatformInfo& platform)
    ```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes in the hardware platform information. For the definition of **PlatformAscendC**, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| platform | Input | Passes in the hardware version and the memory size provided by each hardware unit in the AI Core. When **PlatformInfo** is constructed, it is obtained through the [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md).<br><br>The **PlatformInfo** structure is defined as shown in the following code. **socVersion** is obtained through [GetSocVersion](../../../../Utils-API/platform_info/PlatformAscendC/GetSocVersion.md) and passed through, and the sizes of various hardware storage spaces are obtained through [GetCoreMemSize](../../../../Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md) and passed through.<br><br>It is not recommended to call the constructor by directly filling in values to construct **PlatformInfo**, for example, PlatformInfo(, 1024, 1024, ..); |

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

When implementing the tiling function on the Host side, platform\_ascendc::PlatformAscendC is used to obtain some hardware platform information to support tiling computation, such as the number of cores of the hardware platform. The **PlatformAscendC** class provides the capability to obtain such platform information.

Unlike platform\_ascendc::PlatformAscendC, **PlatformInfo** is used to obtain information specific to a single AI Core, such as the chip version and the memory size provided by each hardware unit in the AI Core.

## Constraints

None

## Example

-   Parameterless constructor

    ```
    Convolution3DBackprop::Conv3dBpFilterTiling tiling;
    tiling.SetWeightType(ConvCommonApi::TPosition::GM,ConvCommonApi::ConvFormat::FRACTAL_Z_3D,ConvCommonApi::ConvDtype::FLOAT32);
    ...
    optiling::Conv3DBackpropFilterTilingData tilingData;
    int ret = tiling.GetTiling(tilingData);    // if ret = -1, gen tiling failed
    ...
    ```

-   Parameterized constructor

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    ConvBackpropApi::Conv3dBpFilterTiling tiling(ascendcPlatform);
    tiling.SetWeightType(ConvCommonApi::TPosition::GM,Convolution3DBackprop::ConvFormat::FRACTAL_Z_3D,ConvCommonApi::ConvDtype::FLOAT32);
    ...
    optiling::Conv3DBackpropFilterTilingData tilingData;
    int ret = tiling.GetTiling(tilingData);    // if ret = -1, gen tiling failed
    ```
