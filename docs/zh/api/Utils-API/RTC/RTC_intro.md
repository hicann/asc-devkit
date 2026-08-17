# RTC简介<a name="ZH-CN_TOPIC_0000002487578597"></a>

RTC是Ascend C运行时编译库，通过aclrtc接口，在程序运行时，将中间代码动态编译成目标机器码，提升程序运行性能。开发指导请参考[RTC](../../../guide/programming_guide/compilation_and_execution/operator_compilation/rtc_runtime_compilation.md)。

使用aclrtc接口前需要包含如下头文件：

```
#include "acl/acl_rt_compile.h"
```

需要链接的库文件：libacl\_rtc.so。

