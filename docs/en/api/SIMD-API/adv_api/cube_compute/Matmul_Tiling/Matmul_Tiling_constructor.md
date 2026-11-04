# Matmul Tiling Class Constructor

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-08T13:07:53.904Z pushedAt=2026-10-27T11:36:19.336Z nl=2 -->

## Description

Creates a Matmul single-core Tiling object, a multi-core Tiling object, or a BatchMatmul Tiling object.

## Prototype

-   Parameterized constructor, which requires the hardware platform information to be passed in. It is recommended to use this type of constructor for better compatibility.
    -   Passes information using the **PlatformAscendC** class.

        ```
        explicit MatmulApiTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

        ```
        explicit MultiCoreMatmulTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

        ```
        explicit BatchMatmulTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

    -   Passes information using **PlatformInfo**.

        When platform\_ascendc::PlatformAscendC cannot be obtained at Tiling runtime, you need to construct the **PlatformInfo** structure yourself and pass it to the **MatmulApiTiling** constructor.

        ```
        explicit MatmulApiTiling(const PlatformInfo& platform)
        ```

        ```
        explicit MultiCoreMatmulTiling(const PlatformInfo& platform)
        ```

<!-- npu="A3,910b" id3 -->
-   No-argument constructor

    ```
    MatmulApiTiling()
    ```

    ```
    MultiCoreMatmulTiling()
    ```

    ```
    BatchMatmulTiling()
    ```

    The no-argument constructor supports only the following product models:

    <!-- npu="910b" id1 -->
    Atlas A2 products
    <!-- end id1 -->

    <!-- npu="A3" id2 -->
    Atlas A3 products
    <!-- end id2 -->
<!-- end id3 -->

-   Base class constructor

    **MatmulApiTiling**, **MultiCoreMatmulTiling**, and **BatchMatmulTiling** all inherit from the base class **MatmulApiTilingBase**, whose constructor is as follows:

    ```
    MatmulApiTilingBase()
    ```

    ```
    explicit MatmulApiTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
    ```

    ```
    explicit MatmulApiTilingBase(const PlatformInfo& platform)
    ```

## Parameters

**Table 1**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| ascendcPlatform | Input | Passes the hardware platform information. For the definition of **PlatformAscendC**, see [Constructor and Destructor](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md). |
| platform | Input | Passes the hardware version and the memory size provided by each hardware unit in the AI Core. **PlatformInfo** obtains this information through [Constructor and Destructor](../../../../Utils-API/platform_info/PlatformAscendC/constructor_and_destructor.md) during construction.<br><br>The **PlatformInfo** structure is defined as shown in the following code. **socVersion** is obtained and passed through [GetSocVersion](../../../../Utils-API/platform_info/PlatformAscendC/GetSocVersion.md), and the sizes of various hardware storage spaces are obtained and passed through [GetCoreMemSize](../../../../Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md).<br><br>It is not recommended to call the constructor by directly filling in values to construct **PlatformInfo**, for example, PlatformInfo(socVersion, 1024, 1024, ..); |

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

When implementing the Tiling function on the host side, platform\_ascendc::PlatformAscendC is used to obtain some hardware platform information to support Tiling computation, such as the number of cores on the hardware platform. The **PlatformAscendC** class provides the functionality to obtain such platform information.

Unlike platform\_ascendc::PlatformAscendC, **PlatformInfo** is used to obtain information specific to a single AI Core, such as the chip version and the memory size provided by each hardware unit in the AI Core.

## Constraints

None

## Examples

-   No-argument constructor

    ```
    // Single-core tiling.
    matmul_tiling::MatmulApiTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    // Multi-core tiling.
    matmul_tiling::MultiCoreMatmulTiling tiling;
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    // BatchMatmul Tiling
    matmul_tiling::BatchMatmulTiling bmmTiling;
    bmmTiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    ```

-   parameterized constructor

    ```
    // Single-core tiling.
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    // Multi-core tiling.
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::MultiCoreMatmulTiling tiling(ascendcPlatform);
    tiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);

    // BatchMatmul Tiling
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    matmul_tiling::BatchMatmulTiling bmmTiling(ascendcPlatform);
    bmmTiling.SetAType(matmul_tiling::TPosition::GM, matmul_tiling::CubeFormat::ND, matmul_tiling::DataType::DT_FLOAT16);
    ```
