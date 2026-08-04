# Pointer

## 功能说明

Pointer定义指针迭代器，用于遍历和访问张量数据。

## 结构体定义

```cpp
template <typename T, typename P>
struct Pointer {
    T* ptr;
    P position;
};
```

## 字段说明

| 字段名 | 类型 | 描述 |
|--------|------|------|
| ptr | T* | 指向数据的指针。 |
| position | P | 指针的位置信息。 |

## 约束说明

- ptr指针必须指向有效的内存空间。
- position必须正确描述指针的当前位置。
- Pointer支持多种内存空间的指针类型。

## 调用示例

```cpp
constexpr int32_t size = 256;
__ubuf__ float ubData[size];
__cbuf__ float l1Data[size];
__ca__ float l0AData[size];

// 创建Unified Buffer指针
auto ubPtr = AscendC::Te::MakeMemPtr(ubData);

// 创建L1 Buffer指针
auto l1Ptr = AscendC::Te::MakeMemPtr(l1Data);

// 创建L0A Buffer指针
auto caPtr = AscendC::Te::MakeMemPtr(l0AData);
```
