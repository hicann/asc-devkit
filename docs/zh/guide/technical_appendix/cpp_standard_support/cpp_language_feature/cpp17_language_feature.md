# C++17语言特性

Ascend C Device侧对C++17语言特性的支持情况如下表所示：

| C++17特性 | C++17 Proposal | 是否支持 | 说明 |
| --- | --- | --- | --- |
| 移除trigraphs | [N4086](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4086.html) | 是 | 三字符组不再转译。 |
| u8字符字面量 | [N4267](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4267.html) | 是 | `u8'x'`形式可用。 |
| 折叠表达式 | [N4295](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4295.html) | 是 | 支持一元与二元折叠，如`(args + ... + 0)`。 |
| 命名空间与枚举器属性 | [N4266](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4266.html) | 是 | `namespace [[maybe_unused]] ns {...}`、枚举器携带属性可用。 |
| 嵌套命名空间定义 | [N4230](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4230.html) | 是 | `namespace a::b::c {...}`形式可用。 |
| 非类型模板参数的常量求值 | [N4268](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4268.html) | 是 | 非类型模板实参允许常量表达式求值，如`template <int N>`以`N/2`作实参。 |
| 扩展static_assert | [N3928](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3928.pdf) | 是 | static_assert可省略消息串。 |
| 花括号初始化列表的auto推导新规则 | [N3922](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3922.html) | 是 | `auto x{1};`推导为int而非initializer_list。 |
| 模板模板参数允许typename | [N4051](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4051.html) | 是 | `template <template <typename> typename TT>`形式可用。 |
| `[[fallthrough]]`属性 | [P0188R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0188r1.pdf) | 是 | switch分支末尾标注`[[fallthrough]]`后，贯穿不产生告警。 |
| `[[nodiscard]]`属性 | [P0189R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0189r1.pdf) | 是 | 忽略返回值会产生告警。 |
| `[[maybe_unused]]`属性 | [P0212R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0212r1.pdf) | 是 | 标注后未使用的变量与形参不产生告警。 |
| 聚合初始化扩展 | [P0017R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0017r1.html) | 是 | 含基类的聚合可直接花括号初始化。 |
| constexpr lambda | [P0170R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0170r1.pdf) | 是 | `constexpr auto f = [...] constexpr {...}`编译期调用可用 |
| 一元折叠与空参数包 | [P0036R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0036r0.pdf) | 是 | 空参数包的一元折叠仅`&&`、`||`、`,`有单位元（true、false、void()）；其他运算符的空包一元折叠为非法，需允许空包时用二元折叠，如`(args + ... + 0)`空包得0。 |
| 泛化基于范围的for循环 | [P0184R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0184r0.html) | 是 | begin、end可返回不同类型。 |
| lambda按值捕获`*this` | [P0018R3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0018r3.html) | 是 | `[*this]`捕获当前对象副本，SIMD与SIMT混合编程场景写法见[Lambda表达式](./lambda_expressions.md)。 |
| enum class构造规则 | [P0138R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0138r2.pdf) | 是 | 整型可直接列表初始化enum class。 |
| 十六进制浮点字面量 | [P0245R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0245r1.html) | 是 | `0x1.8p3`形式可用。 |
| 超对齐数据的动态内存分配 | [P0035R4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0035r4.html) | 否 | Device侧不支持动态内存分配。 |
| 保证拷贝消除 | [P0135R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0135r1.html) | 是 | 纯右值初始化对象时保证不产生拷贝。 |
| 表达式求值顺序细化 | [P0145R3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0145r3.pdf) | 是 | 后缀表达式先于实参、运算符左操作数先于右等求值顺序规则生效，而C++11标准中的“顺序点”特性指Device侧不提供顺序点相关库设施，与本条不冲突。 |
| constexpr if | [P0292R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0292r2.html) | 是 | if constexpr按条件裁剪实例化分支。 |
| 带初始化器的选择语句 | [P0305R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0305r1.html) | 是 | 支持`if (init; cond)`与`switch (init; cond)`。 |
| 类模板的模板实参推导 | [P0091R3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0091r3.html) | 是 | 构造函数实参可推导类模板实参，如`Box b(7);`。 |
| auto声明非类型模板参数 | [P0127R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r2.html) | 是 | 支持`template <auto V>`形式。 |
| 属性命名空间免重复 | [P0028R4](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0028r4.html) | 是 | using属性声明后可省略命名空间限定。 |
| 忽略不支持的非标准属性 | [P0283R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0283r2.html) | 是 | 未知属性仅告警不报错。 |
| 结构化绑定 | [P0217R3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0217r3.html) | 是 | 支持`auto [a, b] = expr;`绑定结构体成员。 |
| 移除register关键字弃用用法 | [P0001R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0001r1.html) | 是 | register不再是合法存储类说明符。 |
| 移除operator++(bool)弃用用法 | [P0002R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0002r1.html) | 是 | bool不再支持自增自减。 |
| 异常规格成为类型系统一部分 | [P0012R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0012r1.html) | 是 | noexcept参与函数类型；Device侧不支持异常抛出。 |
| `__has_include` | [P0061R1](https://wg21.link/p0061) | 是 | `#if __has_include(<header>)`按头文件是否存在取值，可用于条件引入。 |
| 继承构造函数措辞重写 | [P0136R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0136r1.html) | 是 | 继承构造函数按基类构造等价实现。 |
| 内联变量 | [P0386R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0386r2.pdf) | 是 | 支持`inline constexpr`变量定义。 |
| 模板模板实参匹配DR150 | [P0522R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0522r0.html) | 是 | 模板模板实参匹配规则放宽。 |
| 移除动态异常规格 | [P0003R5](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0003r5.html) | 是 | 动态异常规格语法移除。 |
| using声明中的包展开 | [P0195R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0195r2.html) | 是 | 支持`using Bs::operator()...;`包展开。 |
| 字节类型定义 | [P0298R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0298r0.pdf) | 是 | `std::byte`可用，`<cstddef>`可在Device侧包含。 |
| 类内显式实例化DR727 | [CWG727](https://cplusplus.github.io/CWG/issues/727.html) | 是 | 类内显式实例化按缺陷报告决议实现。 |
