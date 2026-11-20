# C++17 Language Features

<!-- md-trans-meta sourceCommit=f9418c38fe93669ca5095ef7e8f02cab935baa1e translatedAt=2026-11-09T11:11:59.638Z pushedAt=2026-11-19T06:32:19.998Z nl=2 -->

This document describes the support for C++17 language features on the Ascend C device side.

**Table 1** C++17 language feature support list

| C++17 Feature | C++17 Proposal | Supported | Description |
| --- | --- | --- | --- |
| Remove trigraphs | [N4086](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4086.html) | Yes | Trigraphs are no longer translated. |
| u8 character literal | [N4267](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4267.html) | Yes | The `u8'x'` form is available. |
| Attributes for namespaces and enumerators | [N4266](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4266.html) | Yes | `namespace [[maybe_unused]] ns {...}` and attributes on enumerators are available. |
| Nested namespace definition | [N4230](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4230.html) | Yes | The `namespace a::b::c {...}` form is available. |
| Constant evaluation for non-type template arguments | [N4268](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4268.html) | Yes | — |
| Allow typename in a template template parameter | [N4051](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4051.html) | Yes | The `template <template <typename> typename TT>` form is available. |
| `[[fallthrough]]` attribute | [P0188R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0188r1.pdf) | Yes | — |
| `[[nodiscard]]` attribute | [P0189R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0189r1.pdf) | Yes | Ignoring the return value generates a warning. |
| `[[maybe_unused]]` attribute | [P0212R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0212r1.pdf) | Yes | — |
| constexpr lambda | [P0170R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0170r1.pdf) | Yes | `constexpr auto f = [...] constexpr {...}` can be called at compile time. |
| Unary fold expressions and empty parameter packs | [P0036R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0036r0.pdf) | Yes | — |
| Lambda capture of `*this` by value | [P0018R3](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0018r3.html) | Yes | `[*this]` captures a copy of the current object. For the usage in SIMD and SIMT mixed programming scenarios, see [Lambda Expressions](./lambda_expressions.md). |
| Construction rules for enum class | [P0138R2](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0138r2.pdf) | Yes | An integer can directly list-initialize an enum class. |
| Hexadecimal floating-point literal | [P0245R1](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0245r1.html) | Yes | The `0x1.8p3` form is available. |
| `__has_include` | [P0061R1](https://wg21.link/p0061) | Yes | — |
