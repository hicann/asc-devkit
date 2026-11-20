# C++14 Language Features

<!-- md-trans-meta sourceCommit=f9418c38fe93669ca5095ef7e8f02cab935baa1e translatedAt=2026-11-09T11:11:46.725Z pushedAt=2026-11-19T06:32:19.996Z nl=2 -->

This document describes the support for C++14 language features on the Ascend C device side.

**Table 1** C++14 language features support list

| C++14 Feature | C++14 Proposal | Supported | Description |
| --- | --- | --- | --- |
| Binary literals | [N3472](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3472.pdf) | Yes | Both the 0b and 0B prefixes are supported. |
| Generalized lambda capture, init capture | [N3648](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3648.html) | Yes | The `[sum = expr]` form is available. For the syntax in SIMD and SIMT hybrid programming scenarios, see [Lambda expressions](./lambda_expressions.md). |
| Generic lambda expressions | [N3649](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3649.html) | Yes | For the syntax in SIMD and SIMT hybrid programming scenarios, see [Lambda expressions](./lambda_expressions.md). |
| constexpr function enhancements | [N3652](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3652.html) | Yes | Local variables, loops, and multiple return paths are allowed in constexpr functions. |
| Member initializers and aggregates | [N3653](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3653.html) | Yes | A class with default member initializers can be initialized with braces as an aggregate. |
| \[\[deprecated\]\] Attributes | [N3760](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3760.html) | Yes | - |
| Single-quote digit separators | [N3781](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3781.pdf) | Yes | The `1'000'000` form is available. |

