# PlatformAscendC Introduction<a name="ZH-CN_TOPIC_0000002224684138"></a>

<!-- md-trans-meta sourceCommit=b3e2e715646b9fecf3e353f980743f9a959a7286 translatedAt=2026-09-06T09:40:45.552Z pushedAt=2026-09-07T11:48:09.640Z -->

When implementing the Tiling function on the Host side, you may need to obtain some hardware platform information to support Tiling computation, such as the core count of the hardware platform. The **PlatformAscendC** class provides the functionality to obtain such platform information.

## Header Files<a name="section78885814919"></a>

To use this feature, include the **tiling/platform/platform\_ascendc.h** header file. The following is an example:

```
#include "tiling/platform/platform_ascendc.h"
```

