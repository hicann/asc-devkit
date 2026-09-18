# Matmul Tiling Class Constructor

<!-- md-trans-meta sourceCommit=c17b32aa8d725bcb43135ab25add5a26d62ad5c0 translatedAt=2026-09-09T07:16:41.592Z pushedAt=2026-09-12T09:55:18.176Z -->

## Description

Creates a **Matmul** single-core tiling object, a multi-core tiling object, or a **BatchMatmul** tiling object.

## Prototype

-   Parameterized constructors, which require the hardware platform information to be passed in. These constructors are recommended for better compatibility.

    -   Pass information using the **PlatformAscendC** class.

        ```
        explicit MatmulApiTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

        ```
        explicit MultiCoreMatmulTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

        ```
        explicit BatchMatmulTiling(const platform_ascendc::PlatformAscendC &ascendcPlatform)
        ```

    -   Pass information using **PlatformInfo**.

        When platform\_ascendc::PlatformAscendC cannot be obtained at Tiling runtime, you need to construct the **PlatformInfo** structure yourself and pass it to the **MatmulApiTiling** constructor.

        ```
        explicit MatmulApiTiling(const PlatformInfo& platform)
        ```

        ```
        explicit MultiCoreMatmulTiling(const PlatformInfo &platform)
        ```

-   Default constructor

    ```
    MatmulApiTiling()
    ```

    ```
    MultiCoreMatmulTiling()
    ```

    ```
    BatchMatmulTiling()
    ```

    The default constructor supports only the following product models:

    Atlas A2 training products/Atlas 800I A2 inference products

    Atlas A3 training products

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
| **ascendcPlatform** | Input | Passes the hardware platform information. For the definition of **PlatformAscendC**, see [constructors_and_destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md). |
| **platform** | Input | Passes the hardware version and the memory size provided by each hardware unit in the AI Core. When **PlatformInfo** is constructed, it is obtained through the [constructors_and_destructors](../../../../Utils-API/platform_info/PlatformAscendC/constructors_and_destructors.md).<br><br>The **PlatformInfo** structure is defined as shown in the following code. **socVersion** is obtained and passed through [GetSocVersion](../../../../Utils-API/platform_info/PlatformAscendC/GetSocVersion.md), and the storage space size of each hardware unit is obtained and passed through [GetCoreMemSize](../../../../Utils-API/platform_info/PlatformAscendC/GetCoreMemSize.md). <br><br>It is not recommended to call the constructor by directly filling in values to construct **PlatformInfo**, for example, `PlatformInfo(socVersion, 1024, 1024, ..);`. |

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

When implementing the Tiling function on the host side, platform\_ascendc::PlatformAscendC is used to obtain some hardware platform information to support Tiling computation, such as the number of cores on the hardware platform. The **PlatformAscendC** class provides the capability to obtain this platform information.

Unlike platform\_ascendc::PlatformAscendC, **PlatformInfo** is used to obtain information specific to a single AI Core, such as the chip version and the memory size provided by each hardware unit in the AI Core.

## Constraints

None

## Examples

-   Default constructor

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

-   Parameterized constructor

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
