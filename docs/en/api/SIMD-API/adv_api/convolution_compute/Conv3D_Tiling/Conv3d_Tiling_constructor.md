# Conv3D Tiling Constructor

<!-- md-trans-meta sourceCommit=4c68db07c483e21d3d63ceb9ef38c2963da2162f translatedAt=2026-11-10T06:22:14.753Z pushedAt=2026-11-18T11:07:26.516Z nl=2 -->

## Description

Used to create a Conv3D single-core Tiling object.

## Prototype

-   Parameterized constructor, which requires hardware platform information to be . This type of constructor is recommended for better compatibility.
    -   Pass information using the PlatformAscendC class

        ```
        explicit Conv3dTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

    -   Pass information using PlatformInfo

        When platform\_ascendc::PlatformAscendC cannot be obtained at Tiling runtime, you need to construct the PlatformInfo struct yourself and pass it to the Conv3dTiling constructor.

        ```
        explicit Conv3dTiling(const PlatformInfo& platform)
        ```

-   Base class constructor

    Conv3dTiling inherits from the base class Conv3dTilingBase. Its constructor is as follows:

    ```
    explicit Conv3dTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
    ```

    ```
    explicit Conv3dTilingBase(const PlatformInfo& platform)
    ```

## Parameters

**Table 1**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Hardware platform information. For the definition of PlatformAscendC, see [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md). |
| platform | Input | Passes the hardware version and the memory sizes provided by each hardware unit in the AI Core. When constructing PlatformInfo, obtain them through [Constructors and Destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md).<br><br>The PlatformInfo struct is defined as shown in the following code. socVersion is obtained and passed through [GetSocVersion](../../../../Utils-API/platform_info/PlatformAscendC/GetSocVersion.md), and the sizes of various hardware storage spaces are obtained and passed through [GetCoreMemSize](../../../../Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md). |

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
// Instantiate the Conv3d API.
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
Conv3dTilingApi::Conv3dTiling conv3dApiTiling(ascendcPlatform);
conv3dApiTiling.SetGroups(groups);
conv3dApiTiling.SetOrgWeightShape(cout, kd, kh, kw);
...
conv3dApiTiling.GetTiling(conv3dCustomTilingData.conv3dApiTilingData);
```
