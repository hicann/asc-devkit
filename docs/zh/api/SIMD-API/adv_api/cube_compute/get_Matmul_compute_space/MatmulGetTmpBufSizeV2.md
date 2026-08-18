# MatmulGetTmpBufSizeV2

## 功能说明

单核Matmul Tiling调用[GetTiling](../Matmul_Tiling/GetTiling.md)接口获取Tiling参数后，根据Tiling结构体信息获取L1 Buffer/Unified Buffer（UB）/L0C Buffer的使用大小。

## 函数原型

```
int32_t MatmulGetTmpBufSizeV2(AscendC::tiling::TCubeTiling& tiling, matmul_tiling::SysTilingTempBufSize& bufSize)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| tiling | 输入 | Matmul单核Tiling的结构体，即MatmulTiling对象得到的TCubeTiling结构体。<br><br>TCubeTiling为核函数（Kernel）侧定义的Matmul TilingData，与入参为带AscendC::tiling命名空间的TCubeTiling结构体的[GetTiling](../Matmul_Tiling/GetTiling.md)接口配合使用。 |
| bufSize | 输出 | 根据TCubeTiling结构体信息获取L1 Buffer/UB/L0C Buffer的使用大小。<br><br>SysTilingTempBufSize结构定义如下方代码所示。 |

```
struct SysTilingTempBufSize {
    int32_t ubSize = 0;  // UB大小
    int32_t l1Size = 0;  // L1 Buffer大小
    int32_t l0cSize = 0; // L0C Buffer大小
};
```

## 返回值说明

-1表示获取失败； 0表示获取成功。

## 约束说明

无

## 调用示例

```
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
matmul_tiling::MatmulApiTiling tiling(ascendcPlatform);
AscendC::tiling::TCubeTiling tilingData;
...                                         // 初始化tilingData，详见MatmulTiling类使用说明
    int ret = tiling.GetTiling(tilingData); // 获取Tiling参数
SysTilingTempBufSize bufSize;
MatmulGetTmpBufSizeV2(tilingData, bufSize);
```
