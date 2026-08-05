# GetSortLen

## 功能说明

根据元素数量，获取Sort数据的大小（单位为字节）。

## 函数原型

```
template <typename T>
__aicore__ inline uint32_t GetSortLen(const uint32_t elemCount)
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：half、float。 |

**表2**  接口参数列表

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| elemCount | 输入 | 输入元素个数。 |

## 返回值说明

sort数据的大小（单位为字节）。

## 约束说明

无

## 调用示例

```
AscendC::GetSortLen<half>(128);
```
