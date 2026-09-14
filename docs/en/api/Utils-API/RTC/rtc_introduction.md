# RTC Introduction<a name="ZH-CN_TOPIC_0000002487578597"></a>

<!-- md-trans-meta sourceCommit=d1fe9e831a69eaae67fc1f463ea1c4ea17a8f417 translatedAt=2026-09-06T15:36:08.449Z pushedAt=2026-09-09T12:01:44.315Z -->

RTC is the Ascend C runtime compilation library. Through the aclrtc APIs, it dynamically compiles intermediate code into target machine code at program runtime to improve program execution performance. For the development guide, see [RTC](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/guide/programming_guide/compilation_and_execution/operator_compilation/rtc_runtime_compilation.md).

Before using the aclrtc APIs, include the following header file:

```
#include "acl/acl_rt_compile.h"
```

The library file to be linked: libacl\_rtc.so.
