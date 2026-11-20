# RTC Overview<a name="ZH-CN_TOPIC_0000002487578597"></a>

<!-- md-trans-meta sourceCommit=e2a2709368f65375f9b4d904990c7884199bd22e translatedAt=2026-11-13T10:20:09.744Z pushedAt=2026-11-16T07:07:41.739Z nl=2 -->

RTC is the Ascend C runtime compilation library. Through the aclrtc API, it dynamically compiles intermediate code into target machine code at program runtime to improve program execution performance. For development guidance, see [RTC](../../../guide/programming_guide/compilation_and_execution/operator_compilation/rtc_runtime_compilation.md).

Before using the aclrtc API, include the following header file:

```
#include "acl/acl_rt_compile.h"
```

Library file to be linked: libacl\_rtc.so.

