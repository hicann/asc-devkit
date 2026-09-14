# Conv3D Tiling Constructor

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-05T14:10:58.294Z -->

## Description

Creates a Conv3D single-core Tiling object.

## Prototype

-   Parameterized constructor, which requires the hardware platform information to be passed in. This type of constructor is recommended for better compatibility.
    -   Pass information using the PlatformAscendC class.

        ```
        explicit Conv3dTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

    -   Pass information using PlatformInfo.

        When platform\_ascendc::PlatformAscendC cannot be obtained during Tiling runtime, you need to construct the **PlatformInfo** structure yourself and pass it to the **Conv3dTiling** constructor.

        ```
        explicit Conv3dTiling(const PlatformInfo& platform)
        ```

-   Base class constructor

    **Conv3dTiling** inherits from the base class **Conv3dTilingBase**, whose constructor is as follows:

    ```
    explicit Conv3dTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
    ```

    ```
    explicit Conv3dTilingBase(const PlatformInfo& platform)
    ```

## Parameters

**Table 1**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes in the hardware platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| platform | Input | Passes in the hardware version and the memory size provided by each hardware unit in the AI Core. When constructing PlatformInfo, obtain the information through [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md).<br><br>The **PlatformInfo** structure is defined as shown in the following code. **socVersion** is obtained and passed through [GetSocVersion](../../../../Utils-API/platform_info/PlatformAscendC/GetSocVersion.md), and the sizes of various hardware storage spaces are obtained and passed through [GetCoreMemSize](../../../../Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md). |

```
struct PlatformInfo {
    platform_ascendc::SocVersion socVersion;
    uint64_t l1Size = 0;
    uint64_t l0CSize = 0;
    uint64_t ubSize = 0;
    uint64_t l0ASize = 0;
    uint64_t l0BSize = 0;
    uint64_t btSize = 0;
    uint64_t fbSize = 0;
};
```

## Constraints

None

## Examples

```
// Instantiate the Conv3d Api.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetGroups(groups);
conv3dApiTiling.SetOrgWeightShape(cout, kd, kh, kw);
...
conv3dApiTiling.GetTiling(conv3dCustomTilingData.conv3dApiTilingData);
```
