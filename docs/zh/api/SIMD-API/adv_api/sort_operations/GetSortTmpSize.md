# GetSortTmpSize

## 功能说明

获取Sort接口所需的临时空间大小。

## 函数原型

```
uint32_t GetSortTmpSize(const platform_ascendc::PlatformAscendC& ascendcPlatform, const uint32_t elemCount, const uint32_t dataTypeSize)
```

## 参数说明

**表1**  接口参数列表

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ascendcPlatform | 输入 | 传入硬件平台的信息，PlatformAscendC定义请参见[构造及析构函数](../../../Utils-API/平台信息获取/PlatformAscendC/构造及析构函数.md)。 |
| elemCount | 输入 | 输入元素个数。 |
| dataTypeSize | 输入 | 输入数据大小（单位为字节）。 |

## 返回值说明

Sort接口所需的临时空间大小。

## 约束说明

无

## 调用示例

```
fe::PlatFormInfos platform_info;
auto plat = platform_ascendc::PlatformAscendC(&platform_info);
const uint32_t elemCount = 128;
AscendC::GetSortTmpSize(plat, elemCount, 4);
```
