# Utils API<a name="ZH-CN_TOPIC_0000002509865667"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-10-23T15:21:38.102Z pushedAt=2026-11-03T07:05:13.471Z nl=2 -->

Ascend C provides a rich set of general-purpose utility classes for development, covering the standard library, platform information acquisition, context construction, runtime compilation, and log output, helping developers efficiently implement operator development and performance optimization.

-   [C++ Standard Library API](../../../api/Utils-API/cpp_stdlib/cpp_stdlib.md): Provides C++ standard library functions such as algorithms, math functions, and container functions.
-   [Platform Information API](../../../api/Utils-API/platform_info/platform_info.md): Provides functions for obtaining platform information, such as the number of cores on the hardware platform.
-   [RTC API](../../../api/Utils-API/RTC/RTC.md): Ascend C runtime compilation library. Through the aclrtc API, it dynamically compiles intermediate code into target machine code at program runtime to improve program execution performance.
-   [log API](../../../api/Utils-API/log/log.md): Provides the capability to print logs on the host side. Developers can use the ASC\_CPU\_LOG\_XXX APIs in the TilingFunc code of an operator to output relevant content.
-   [Tuning API](../../../api/Utils-API/tuning_interface/tuning_interface.md): Provides interfaces for debugging scenarios, supporting various scenarios such as SIMD and SIMT.
