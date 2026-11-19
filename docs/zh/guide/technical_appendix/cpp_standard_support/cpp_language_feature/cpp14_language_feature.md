# C++14语言特性

Ascend C Device侧对C++14语言特性的支持情况如下表所示：

| C++14特性 | C++14 Proposal | 是否支持 | 说明 |
| --- | --- | --- | --- |
| 上下文转换调整 | [N3323](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3323.pdf) | 是 | if等上下文中explicit bool转换可直接使用。 |
| 二进制字面量 | [N3472](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3472.pdf) | 是 | 支持0b、0B两种前缀。 |
| 返回类型推导函数 | [N3638](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3638.html) | 是 | 无尾置返回类型的函数可用auto推导返回类型。 |
| 广义lambda捕获、初始化捕获 | [N3648](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3648.html) | 是 | `[sum = expr]`形式可用。SIMD与SIMT混合编程场景写法详见[Lambda表达式](./lambda_expressions.md)。 |
| 泛型lambda表达式 | [N3649](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3649.html) | 是 | SIMD与SIMT混合编程场景写法详见[Lambda表达式](./lambda_expressions.md)。 |
| 变量模板 | [N3651](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3651.pdf) | 是 | 支持`template <typename T> constexpr T var = ...;`变量模板。 |
| constexpr函数增强 | [N3652](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3652.html) | 是 | constexpr函数内允许局部变量、循环、多返回路径。 |
| 成员初始化器与聚合 | [N3653](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3653.html) | 是 | 含默认成员初始化器的类可作为聚合用花括号初始化。 |
| 内存分配澄清 | [N3664](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3664.html) | 否 | Device侧不支持动态内存分配。 |
| 带尺寸的释放 | [N3778](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3778.html) | 否 | Device侧不支持动态内存释放。 |
| `[[deprecated]]`属性 | [N3760](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3760.html) | 是 | 标注弃用后，使用该实体会产生编译告警。 |
| 单引号数字分隔符 | [N3781](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3781.pdf) | 是 | `1'000'000`形式可用。 |
