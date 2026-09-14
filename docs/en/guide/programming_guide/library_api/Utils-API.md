# Utils API<a name="ZH-CN_TOPIC_0000002509865667"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:19:34.671Z pushedAt=2026-09-06T03:11:03.865Z -->

Ascend C provides a rich set of general-purpose utility classes for development, covering the standard library, platform information acquisition, context construction, runtime compilation, and log output, helping developers efficiently implement operator development and performance optimization.

-   [C++ standard library API](../../../api/Utils-API/cpp_stdlib/cpp_standard_library.md): Provides C++ standard library functions such as algorithms, mathematical functions, and container functions.
-   [Platform information acquisition API](../../../api/Utils-API/platform_info/platform_information.md): Provides the capability to obtain platform information, such as the number of cores on the hardware platform.
-   [RTC API](../../../api/Utils-API/RTC/RTC.md): Ascend C runtime compilation library. Through the aclrtc API, it dynamically compiles intermediate code into target machine code at program runtime to improve program execution performance.
-   [log API](../../../api/Utils-API/log/log.md): Provides the capability to print logs on the host side. Developers can use the ASC\_CPU\_LOG\_XXX APIs in the TilingFunc code of an operator to output relevant content.
-   [Debugging API](../../../api/Utils-API/tuning_interface/tuning_interface.md): Interfaces used in SIMT VF debugging scenarios.
