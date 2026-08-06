# is\_base\_of

## 产品支持情况

- Ascend 950PR/Ascend 950DT：支持
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
- Atlas 200I/500 A2 推理产品：不支持
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
- Atlas 训练系列产品：不支持

## 功能说明

is\_base\_of是定义于<type\_traits\>头文件的一个类型特征工具，它能够在程序编译时检查一个类型是否为另一个类型的基类。本接口可应用在模板元编程、类型检查和条件编译等场景，用于在编译阶段捕获潜在的类型错误，提高代码的鲁棒性。

## 函数原型

```
template <typename Base, typename Derived>
struct is_base_of;
```

## 参数说明

**表1**  模板参数说明

| 参数名 | 含义 |
| --- | --- |
| Base | 待检查的基类类型，即Base类型是否为Derived类型的基类。 |
| Derived | 待检查的派生类类型，即Base类型是否为Derived类型的基类。 |

## 约束说明

无

## 返回值说明

is\_base\_of的静态常量成员value用于获取返回的布尔值，is\_base\_of<Base, Derived\>::value取值如下：

-   true：Base类型是Derived类型的基类（包括Base类型和Derived类型为同一类型的情况）。
-   false：Base类型不是Derived类型的基类。

## 调用示例

```
class Base {};
class Derived : public Base {};
class Unrelated {};

// 虚继承的派生类
class Derived2 : virtual public Base {};

// 定义虚继承的派生类
class VirtualDerived : virtual public Base {};

// 定义多重继承的派生类
class MultiDerived : public Base, public VirtualDerived {};

// 模板基类
template <typename T>
class BaseTemplate {
public:
    T value;
};

// 模板派生类
template <typename T>
class DerivedTemplate : public BaseTemplate<T> {};

// 检查Base是否是Derived的基类
AscendC::PRINTF("Is Base a base of Derived? %d\n" , AscendC::Std::is_base_of<Base, Derived>::value);

// 检查Derived是否是Base的基类（应该为false）
AscendC::PRINTF("Is Derived a base of Base? %d\n" , AscendC::Std::is_base_of<Derived, Base>::value);

// 检查Base是否是Unrelated的基类（应该为false）
AscendC::PRINTF("Is Base a base of Unrelated? %d\n" , AscendC::Std::is_base_of<Base, Unrelated>::value);

AscendC::PRINTF("Is Base a base of Derived (virtual inheritance)? %d\n", AscendC::Std::is_base_of<Base, Derived2>::value);

AscendC::PRINTF("Is BaseTemplate<int> a base of DerivedTemplate<int>? %d\n", AscendC::Std::is_base_of<BaseTemplate<int>, DerivedTemplate<int>>::value);

// 测试Base是否为VirtualDerived的基类（虚继承情况）
AscendC::PRINTF("Is Base a base of VirtualDerived? %d\n" , AscendC::Std::is_base_of<Base, VirtualDerived>::value);
// 测试Base是否为MultiDerived的基类（多重继承情况）
AscendC::PRINTF("Is Base a base of MultiDerived? %d\n" , AscendC::Std::is_base_of<Base, MultiDerived>::value);
```

```
// 执行结果：
Is Base a base of Derived? 1
Is Derived a base of Base? 0
Is Base a base of Unrelated? 0
Is Base a base of Derived (virtual inheritance)? 1
Is BaseTemplate<int> a base of DerivedTemplate<int>? 1
Is Base a base of VirtualDerived? 1
Is Base a base of MultiDerived? 1
```
