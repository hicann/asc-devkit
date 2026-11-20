# C++11 Language Features

<!-- md-trans-meta sourceCommit=f9418c38fe93669ca5095ef7e8f02cab935baa1e translatedAt=2026-11-09T11:11:59.073Z pushedAt=2026-11-19T06:32:19.997Z nl=2 -->

This document describes the support status of C++11 language features on the Ascend C device side. The following table lists the support status of C++11 core language features on the device side.

**Table 1** C++11 core language features

| C++11 Feature | C++11 Proposal | Supported | Description |
| --- | --- | --- | --- |
| Rvalue references for \*this | [N2439](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2439.htm)| Yes | Supports `func() &`, `func() &&`, and `func() const &` ref-qualified member function overloading. |
| Rvalue-initialized class objects | [N1610](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1610.html) | Yes | When a factory function returns an rvalue, both move construction and return value optimization take effect. |
| Variadic templates | [N2242](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2242.pdf) | Yes | Supports parameter packs, `sizeof...`, pack expansion into array initialization, function arguments, base class lists, and recursive inheritance expansion. |
| static_assert | [N1720](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1720.html) | Yes | Supports `static_assert` at namespace scope and inside functions, and can be used in templates. |
| Auto-typed variables | [N1984](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1984.pdf) | Yes | Supports type deduction from initializer expressions and lambda expressions. Note that on the host side, a device pointer declared as `auto dev = nullptr;` and passed to a kernel function via `aclrtMalloc` does not take effect on the device side. Device pointers should be declared with an explicit pointer type. |
| Multiple declarators with auto | [N1737](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1737.pdf) | Yes | Declares multiple auto variables in a single statement. The deduced types of all declarators must be consistent; otherwise, a compilation error is reported. |
| Lambda expressions | [N2927](https://www.open-std.org/JTC1/SC22/WG21/docs/papers/2009/n2927.pdf) | Yes | In mixed SIMD and SIMT programming scenarios, function attributes must be explicitly marked. For details, see [Lambda Expressions](./lambda_expressions.md). |
| Declared type of an expression (decltype) | [N2343](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2343.pdf) | Yes | Value category semantics are consistent with the standard: named entities deduce the declared type, and expressions deduce references based on lvalue or rvalue. |
| Right angle brackets | [N1757](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1757.html) | Yes | The nested template closing delimiter `>>` does not require a space separator. |
| Alias templates | [N2258](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2258.pdf) | Yes | Supports `template <typename T> using Alias = ...` and metafunction cascading.|
| Null pointer constant (nullptr) | [N2431](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf) | Yes | Supports nullptr initialization, comparison, and function overload resolution. |
| Strongly typed enums | [N2347](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2347.pdf) | Yes | Supports enum class and underlying type specification. |
| Alignment support | [N2341](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2341.pdf) | Yes | Supports alignas and alignof, applicable to classes, global variables, and local variables. |
| Conditionally-supported behavior | [N1627](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1627.pdf) | Yes | The `__int128` extended integer type is available on the device side. |
| Delegating constructors | [N1986](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1986.pdf) | Yes | Supports constructor delegation chains. |
| Explicit conversion operators | [N2437](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2437.pdf) | Yes | Supports `explicit operator T`. |
| New character types | [N2249](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html) | Yes | Supports char16_t and char32_t as arithmetic types participating in operations. |
| Universal character names in literals | [N2170](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2170.html) | Yes | Supports the UTF-8 landing form of UCNs. |
| Standard-layout types | [N2342](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2342.htm) | Yes | `std::is_standard_layout` determination is consistent with the standard, and `<type_traits>` can be used on the device side. |
| Defaulted functions | [N2346](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2346.htm) | Yes | Construction, copy, move, assignment, and destruction all support `= default`. |
| Deleted functions | [N2346](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2346.htm) | Yes | Supports `= delete`, enabling the construction of move-only types. |
| Extended friend declarations | [N1791](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1791.pdf) | Yes | Supports the extended form of template friends without writing `<>`. |
| Extended sizeof | [N2253](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2253.html), [DR850](https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#850) | Yes | Supports sizeof on class members without instantiation, as well as the `sizeof...` parameter pack. |
| Defining move special member functions | [N3053](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2010/n3053.html) | Yes | Supports the definition and use of move constructors and move assignment operators. |

The following table summarizes the support status of C99 features incorporated into C++11 on the device side.

**Table 2** C99 features

| C99 Feature | C++11 Proposal | Supported | Description |
| --- | --- | --- | --- |
| \_\_func\_\_ predefined identifiers | [N2340](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2340.htm) | Yes | Implicit static string arrays inside functions are available. |
| C99 preprocessor | [N1653](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1653.htm) | Yes | Supports variadic macros `__VA_ARGS__`, token pasting `##`, and stringification `#`. The GNU extension `##__VA_ARGS__` empty-argument swallowing is unavailable when the variadic arguments are empty and must be replaced with alternatives such as function overloading. |
| long long | [N1811](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1811.pdf) | Yes | At least 64 bits, supporting LL and ULL suffixes and wide-width arithmetic. |
| Extended integer types | [N1988](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1988.pdf) | Yes | intmax\_t, uintmax\_t, and INT64\_C in `<stdint.h>` and `<cinttypes>` are available on the device side. |

