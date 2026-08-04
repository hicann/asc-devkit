# ViewEngine

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

ViewEngine是Tensor API中的视图引擎，负责持有一个指针对象Iterator，通过Begin()返回这段视图的起始位置。在LocalTensor中，ViewEngine和Layout一起组成一个Tensor的视图。

```cpp
LocalTensor<TensorAttribute<ViewEngine<Iterator>, LayoutType>>
```

## 函数原型

  `ViewEngine`定义如下：

  ```cpp
  template <typename Iterator>
  struct ViewEngine
  {
      using iterator    = Iterator;
      using reference   = typename IterRef<iterator>::type;
      using elementType = typename IterEle<iterator>::type;
      using valueType   = typename IterVal<iterator>::type;

      __aicore__ inline constexpr iterator const& Begin() const;
      __aicore__ inline constexpr iterator& Begin();
      __aicore__ inline constexpr ViewEngine(iterator storage = {});
  };
  ```

## 成员函数说明

### `ViewEngine()`

- 功能说明

  根据给定的指针/迭代器对象构造ViewEngine对象。

- 函数原型

  ```cpp
  __aicore__ inline constexpr ViewEngine(iterator storage = {});

  ```

- 参数说明

- `storage`：视图起始位置对应的指针/迭代器对象。

- 返回值说明

  构造函数无返回值。

### `Begin()`

- 功能说明

  获取构造ViewEngine对象时的起始位置指针/迭代器。

- 函数原型

  ```cpp
  __aicore__ inline constexpr iterator const& Begin() const;

  __aicore__ inline constexpr iterator& Begin();

  ```

- 返回值说明

- 返回构造时保存的storage，即当前视图的起始指针/迭代器。

## 约束说明

传入的iterator对象必须是有效的随机访问迭代器，以确保能够正确访问和操作数据。

## 调用示例

```cpp
  using namespace AscendC::Te;

  constexpr int32_t size = 256;
  __ubuf__ float data[size];
  auto ptr = MakeMemPtr(data);

  auto layout = MakeLayout(MakeShape(2, 2), MakeStride(2, 1));
  auto tensor = MakeTensor(ptr, layout);

  auto engine = tensor.Engine();
  auto begin = engine.Begin();

  float a = begin[0];
  float b = tensor[MakeCoord(1, 1)];

```
