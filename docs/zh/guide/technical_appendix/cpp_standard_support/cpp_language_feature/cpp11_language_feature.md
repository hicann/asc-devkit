# C++11语言特性

本文介绍C++11语言特性在Ascend C Device侧的支持情况。

下表汇总了Device侧对C++11核心语言特性的支持情况：

| C++11特性 | C++11 Proposal | 是否支持 | 说明 |
| --- | --- | --- | --- |
| 右值引用 | [N2118](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2118.html) | 是 | 支持右值引用形参、移动构造与移动赋值。 |
| `*this`的右值引用（Rvalue references for `*this`） | [N2439](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2439.htm)| 是 | 支持`func() &`、`func() &&`、`func() const &`引用限定成员函数重载。 |
| 右值初始化类对象 | [N1610](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1610.html) | 是 | 工厂函数返回右值时移动构造或返回值优化均生效。 |
| 非静态数据成员初始化器 | [N2756](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2756.htm) | 是 | 类内成员默认初始化器可用，构造对象时未显式初始化的成员取默认值。 |
| 变参模板 | [N2242](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2242.pdf) | 是 | 支持参数包、`sizeof...`、包展开进数组初始化、函数实参、基类列表，递归继承展开。 |
| 扩展变参模板模板参数 | [N2555](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2555.pdf) | 是 | 模板模板形参可接受变参模板实参。 |
| 初始化列表 | [N2672](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2672.htm) | 部分 | 聚合、数组与类对象的花括号初始化在Device侧可用；`std::initializer_list`的begin、size为Host函数，基于它的构造与遍历在Device侧编译报`reference to __host__ function`，需要变长初始化时以数组或变参模板替代。 |
| 静态断言 | [N1720](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1720.html) | 是 | 支持命名空间级与函数内`static_assert`，可在模板中使用。 |
| auto类型变量 | [N1984](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1984.pdf) | 是 | 支持从初始化表达式、lambda表达式推导类型。注意Host侧`auto dev = nullptr;`声明的设备指针经`aclrtMalloc`传入核函数时Device地址不生效，设备指针应以显式指针类型声明。 |
| 多声明符auto | [N1737](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1737.pdf) | 是 | 一条语句声明多个auto变量，要求各声明符推导类型一致，不一致时编译报错。 |
| 移除auto存储类说明符 | [N2546](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2546.htm) | 是 | auto不再作为存储类说明符，仅保留类型推导语义。 |
| 新函数声明符语法 | [N2541](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2541.htm) | 是 | 支持尾置返回类型`auto f(...) -> T`，与decltype配合推导返回类型。 |
| Lambda表达式 | [N2927](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2927.pdf) | 是 | SIMD与SIMT混合编程场景需显式标记函数属性，详见[Lambda表达式](./lambda_expressions.md)。 |
| 表达式的声明类型（Declared type of an expression，decltype） | [N2343](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2343.pdf) | 是 | 值类别语义与标准一致：具名实体推导声明类型，表达式按左值、右值推导引用。 |
| 不完整返回类型 | [N3276](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3276.pdf) | 是 | 允许函数声明返回不完整类型，定义时类型须完整。 |
| 右尖括号 | [N1757](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1757.html) | 是 | 嵌套模板关闭符`>>`无需空格分隔。 |
| 函数模板默认模板实参 | [DR226](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#226) | 是 | 函数模板形参可带默认模板实参。 |
| 表达式SFINAE | [DR339](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2634.html) | 是 | 替换失败发生在表达式与类型推导中时按SFINAE处理，不报硬错误。 |
| 别名模板 | [N2258](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2258.pdf) | 是 | 支持`template <typename T> using Alias = ...`及元函数级联。|
| 外部模板 | [N1987](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1987.htm) | 是 | 支持`extern template`显式实例化声明，抑制隐式实例化。 |
| 空指针常量（Null pointer constant，nullptr） | [N2431](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf) | 是 | 支持nullptr初始化、比较与函数重载决议。 |
| 强类型枚举 | [N2347](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2347.pdf) | 是 | 支持enum class及底层类型指定。 |
| 枚举前向声明 | [N2764](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2764.pdf)、[DR1206](https://cplusplus.github.io/CWG/issues/1206.html) | 是 | `enum class E : 底层类型;`可先声明后定义，底层类型须显式指定。 |
| 标准化属性语法 | [N2761](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2761.pdf) | 是 | 支持`[[...]]`标准属性语法，如`[[noreturn]]`。 |
| 广义常量表达式 | [N2235](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2235.pdf) | 是 | constexpr变量可作数组尺寸与模板实参，constexpr函数可在编译期求值。 |
| 对齐支持 | [N2341](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2341.pdf) | 是 | 支持alignas、alignof，可作用于类、全局与局部变量。 |
| 条件支持行为 | [N1627](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1627.pdf) | 是 | `__int128`扩展整型在Device侧可用。 |
| 未定义行为转为可诊断错误 | [N1727](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1727.pdf) | 是 | 标准要求可诊断的未定义行为按编译错误或告警处理。 |
| 委托构造函数 | [N1986](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1986.pdf) | 是 | 支持构造函数委托链。 |
| 继承构造函数 | [N2540](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2540.htm) | 是 | 派生类可经`using Base::Base;`继承基类构造函数。 |
| 显式转换运算符 | [N2437](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2437.pdf) | 是 | 支持`explicit operator T`。 |
| 新字符类型 | [N2249](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html) | 是 | 支持char16_t、char32_t作为算术类型参与运算。 |
| Unicode字符串字面量 | [N2442](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm) | 是 | u8、u、U前缀字符串字面量可编译。 |
| 原始字符串字面量 | [N2442](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm) | 是 | R"(...)"内转义序列不生效，按字面字符处理。 |
| 字面量中的通用字符名 | [N2170](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2170.html) | 是 | 支持UCN的UTF-8落地形式。 |
| 用户自定义字面量 | [N2765](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2765.pdf) | 是 | 支持operator""后缀字面量运算符，Device侧可定义并调用。 |
| 标准布局类型 | [N2342](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2342.htm) | 是 | `std::is_standard_layout`判定与标准一致，`<type_traits>`可在Device侧使用。 |
| defaulted函数 | [N2346](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2346.htm) | 是 | 构造、拷贝、移动、赋值、析构均支持`= default`。 |
| deleted函数 | [N2346](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2346.htm) | 是 | 支持`= delete`，可构造move-only类型。 |
| 扩展friend声明 | [N1791](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1791.pdf) | 是 | 支持模板友元不写`<>`的扩展形式。 |
| 扩展sizeof | [N2253](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2253.html)、[DR850](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#850) | 是 | 支持类成员sizeof且无需实例，以及`sizeof...`参数包。 |
| 内联命名空间 | [N2535](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2535.htm) | 是 | inline namespace成员对外直接可见，可用于版本管理。 |
| 非受限联合体 | [N2544](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2544.pdf) | 是 | 联合体成员可含非平凡成员与非静态数据成员初始化器。 |
| 局部与无名类型作为模板实参 | [N2657](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2657.htm) | 是 | 块作用域内定义的类型可作模板实参。 |
| 基于范围的for循环 | [N2930](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2930.html) | 是 | 数组与定义了begin、end的类型可用范围for遍历；Device侧数组遍历可用。 |
| 显式虚函数重载 | [N2928](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2928.htm)、[N3206](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3206.htm)、[N3272](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3272.htm) | 否 | Device侧不支持虚函数 |
| 最小GC与泄漏检测支持 | [N2670](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2670.htm) | 否 | 不适用。 |
| 允许移动构造抛出异常（noexcept） | [N3050](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3050.html) | 是 | 支持noexcept说明符与noexcept运算符，`static_assert(noexcept(f))`可用。 |
| 定义移动特殊成员函数 | [N3053](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3053.html) | 是 | 支持移动构造与移动赋值的定义、使用。 |

下表汇总了Device侧对C++11并发相关特性的支持情况：

| C++11特性 | C++11 Proposal | 是否支持 | 说明 |
| --- | --- | --- | --- |
| 顺序点 | [N2239](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2239.html) | 否 | C++11起以“先于”关系取代顺序点描述单线程内求值顺序（语言定义，非库设施）。Device侧编译器按同一规则实现指令流顺序，不提供顺序点相关的库设施。 |
| 原子操作 | [N2427](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2427.html) | 否 | `std::atomic`不可用；原子能力经SIMT原子API提供，如`asc_atomic_add`、`asc_atomic_cas`。 |
| 强比较与交换 | [N2748](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2748.html) | 否 | CAS能力经`asc_atomic_cas`提供，非`std::atomic`接口。 |
| 双向栅栏 | [N2752](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2752.htm) | 否 | 内存栅栏经SIMT同步API提供，如`asc_threadfence`。 |
| 内存模型 | [N2429](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2429.htm) | 否 | Device侧不实现C++标准内存模型。 |
| 数据依赖顺序：原子与内存模型 | [N2664](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2664.htm) | 否 | Device侧不实现C++标准内存模型。 |
| 异常传播 | [N2179](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2179.html) | 否 | Device侧不支持异常。 |
| 信号处理器中使用原子 | [N2547](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2547.htm) | 否 | Device侧无信号处理模型。 |
| 线程局部存储 | [N2659](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2659.htm) | 否 | `thread_local`关键字不可用。 |
| 并发动态初始化与析构 | [N2660](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2660.htm) | 否 | Device侧不支持动态初始化与析构的并发语义。 |

下表汇总了Device侧对C++11收录的C99特性的支持情况：

| C99特性 | C++11 Proposal | 支持情况 | 说明 |
| --- | --- | --- | --- |
| `__func__`预定义标识符 | [N2340](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2340.htm) | 是 | 函数内隐式静态字符串数组可用。 |
| C99预处理器 | [N1653](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1653.htm) | 是 | 支持可变参数宏`__VA_ARGS__`、记号粘贴`##`、字符串化`#`。GNU扩展`##__VA_ARGS__`空参吞并在可变参数为空时不可用，需以函数重载等方式替代。 |
| long long | [N1811](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1811.pdf) | 是 | 64位，支持LL、ULL后缀与宽位数算术。 |
| 扩展整型 | [N1988](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1988.pdf) | 是 | `<stdint.h>`、`<cinttypes>`可包含，`intmax_t`、`uintmax_t`、`INT64_C`与`int32_t`/`uint32_t`等定宽类型在Device侧可用。 |
