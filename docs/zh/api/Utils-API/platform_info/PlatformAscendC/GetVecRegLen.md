# GetVecRegLen<a name="ZH-CN_TOPIC_0000002556048947"></a>

## 功能说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section36583473819"></a>

获取当前硬件平台芯片架构Vec计算单元位宽。

## 函数原型<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section13230182415108"></a>

```
uint32_t GetVecRegLen(void) const
```

## 参数说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section189014013619"></a>

无

## 返回值<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section25791320141317"></a>

-   对于RegBase硬件平台芯片架构，返回当前硬件平台Vector计算单元位宽。
-   对于非RegBase硬件平台芯片架构，返回0 。

## 约束说明<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section19165124931511"></a>

无

## 调用示例<a name="zh-cn_topic_0000001664705472_zh-cn_topic_0000001442758437_section320753512363"></a>

```
void GetLayerNormMaxMinTmpSize(...)
{
       platform_ascendc::PlatformAscendC* platform = platform_ascendc::PlatformAscendCManager::GetInstance();
        ...
        const uint32_t vecLenB32 = platform->GetVecRegLen() / LAYERNORM_SIZEOF_FLOAT;
        const uint32_t vecLenB16 = platform->GetVecRegLen() / LAYERNORM_SIZEOF_HALF;
        ...
}
```

