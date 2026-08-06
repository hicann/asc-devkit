# Utils API<a name="ZH-CN_TOPIC_0000002509865667"></a>

Ascend C开发提供了丰富的通用工具类，涵盖标准库、平台信息获取、上下文构建、运行时编译及日志输出等功能，支持开发者高效实现算子开发与性能优化。

-   [C++标准库API](../../../api/Utils-API/cpp_stdlib/cpp_stdlib.md)：提供算法、数学函数、容器函数等C++标准库函数。
-   [平台信息获取API](../../../api/Utils-API/platform_info/platform_info.md)：提供获取平台信息的功能，比如获取硬件平台的核数等信息。
-   [RTC API](../../../api/Utils-API/RTC/RTC.md)：Ascend C运行时编译库，通过aclrtc接口，在程序运行时，将中间代码动态编译成目标机器码，提升程序运行性能。
-   [log API](../../../api/Utils-API/log/log.md)：提供Host侧打印Log的功能。开发者可以在算子的TilingFunc代码中使用ASC\_CPU\_LOG\_XXX接口来输出相关内容。
-   [调测接口](../../../api/Utils-API/tuning_interface/tuning_interface.md)：面向调试场景提供相关接口，支持SIMD、SIMT等各类场景。
