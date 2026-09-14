# asc_load3d_v2_config

<!-- md-trans-meta sourceCommit=282ae070f9d90289ec519e0d57dbf32c7e9ac1e9 translatedAt=2026-09-05T12:46:57.362Z pushedAt=2026-09-08T03:45:05.380Z -->

**asc_load3d_v2_config** is used to set the **repeat** parameter of the **Load3Dv2** API.

## Structure Definition

```cpp
constexpr uint64_t ASC_DEFAULT_LOAD3D_V2_CONFIG_VALUE = 0x0000000000010000;
union asc_load3d_v2_config {
    uint64_t config = ASC_DEFAULT_LOAD3D_V2_CONFIG_VALUE;
    struct {
        uint64_t rpt_stride : 16;
        uint64_t rpt_time : 8;
        uint64_t rpt_mode : 1;
        uint64_t reserved2 : 39;
    };
};
```

## Fields

|Field|Description|
|----------|----------|
| **rpt_stride** | Distance between the start addresses of the previous iteration and the next iteration in the height/width direction. Value Range: [0, 65535]. Default Value: 0. |
| **rpt_time** | Number of iterations in the height/width direction. Value Range: [0, 255]. Default Value: 1. |
| **rpt_mode** | Controls the direction of repeat iteration. Value Range: [0, 1]. Default Value: 0.<br>0: Iteration proceeds along the height direction.<br>1: Iteration proceeds along the width direction. |
