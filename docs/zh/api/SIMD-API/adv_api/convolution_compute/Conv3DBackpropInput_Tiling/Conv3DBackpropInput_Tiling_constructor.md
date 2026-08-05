# Conv3DBackpropInput Tiling构造函数

## 功能说明

用于创建一个Conv3DBackpropInput单核Tiling对象。

## 函数原型

-   带参构造函数，需要传入硬件平台信息，推荐使用这类构造函数来获得更好的兼容性。
    -   使用PlatformAscendC类传入信息

        ```
        explicit Conv3DBpInputTiling(const platform_ascendc::PlatformAscendC& ascendcPlatform)
        ```

    -   使用PlatformInfo传入信息

        当platform\_ascendc::PlatformAscendC无法在Tiling运行时获取时，需要用户自己构造PlatformInfo结构体，透传给Conv3DBpInputTiling构造函数。

        ```
        explicit Conv3DBpInputTiling(const PlatformInfo& platform)
        ```

-   无参构造函数

    ```
    Conv3DBpInputTiling()
    ```

-   基类构造函数

    Conv3DBpInputTiling继承自基类Conv3DBpInputTilingBase，其构造函数如下：

    ```
    Conv3DBpInputTilingBase()
    ```

    ```
    explicit Conv3DBpInputTilingBase(const platform_ascendc::PlatformAscendC& ascendcPlatform)
    ```

    ```
    explicit Conv3DBpInputTilingBase(const PlatformInfo& platform)
    ```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ascendcPlatform | 输入 | 传入硬件平台的信息，PlatformAscendC定义请参见[构造及析构函数](../../../../Utils-API/平台信息获取/PlatformAscendC/构造及析构函数.md)。 |
| platform | 输入 | 传入硬件版本以及AI Core中各个硬件单元提供的内存大小。PlatformInfo构造时通过[构造及析构函数](../../../../Utils-API/平台信息获取/PlatformAscendC/构造及析构函数.md)获取。<br><br>PlatformInfo结构定义如下代码所示，socVersion通过[GetSocVersion](../../../../Utils-API/平台信息获取/PlatformAscendC/GetSocVersion.md)获取并透传，各类硬件存储空间大小通过[GetCoreMemSize](../../../../Utils-API/平台信息获取/PlatformAscendC/GetCoreMemSize.md)获取并透传。<br><br>不推荐通过直接填值构造PlatformInfo的方式调用构造函数，例如PlatformInfo(, 1024, 1024, ..); |

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

在实现Host侧的Tiling函数时，platform\_ascendc::PlatformAscendC用于获取一些硬件平台的信息，来支撑Tiling的计算，比如获取硬件平台的核数等信息。PlatformAscendC类提供获取这些平台信息的功能。

与platform\_ascendc::PlatformAscendC不同的是，PlatformInfo则用于获取芯片版本以及AI Core中各个硬件单元提供的内存大小等只针对单个AI Core的信息。

## 约束说明

无

## 调用示例

-   无参构造函数

    ```
    ConvBackpropApi::Conv3DBpInputTiling tiling;
    tiling.SetWeightType(ConvCommonApi::TPosition::GM,ConvCommonApi::ConvFormat::FRACTAL_Z_3D,ConvCommonApi::ConvDtype::FLOAT16);
    ...
    optiling::Conv3DBackpropInputTilingData tilingData;
    int ret = tiling.GetTiling(tilingData);    // if ret = -1, gen tiling failed
    ...
    ```

-   带参构造函数

    ```
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    ConvBackpropApi::Conv3DBpInputTiling tiling(ascendcPlatform);
    tiling.SetWeightType(
        ConvCommonApi::TPosition::GM, ConvCommonApi::ConvFormat::FRACTAL_Z_3D, ConvCommonApi::ConvDtype::FLOAT16);
    ...
    optiling::Conv3DBackpropInputTilingData tilingData;
    int ret = tiling.GetTiling(tilingData); // if ret = -1, gen tiling failed
    ```
