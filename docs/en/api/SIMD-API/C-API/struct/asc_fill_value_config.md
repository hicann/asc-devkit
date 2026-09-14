# asc_fill_value_config

<!-- md-trans-meta sourceCommit=282ae070f9d90289ec519e0d57dbf32c7e9ac1e9 translatedAt=2026-09-05T12:45:23.426Z pushedAt=2026-09-08T03:45:05.374Z -->

**asc_fill_value_config** is the initialization parameter structure for **fill_value**, containing the various initialization parameters that need to be configured for the [asc_fill_l0a](../cube_datamove/asc_fill_l0a.md)/[asc_fill_l0b](../cube_datamove/asc_fill_l0b.md)/[asc_fill_l1](../cube_datamove/asc_fill_l1.md) APIs.

## Structure Definition

```cpp
constexpr uint64_t ASC_FILL_VALUE_DEFAULT_VALUE = 0x0000000000000000;
union asc_fill_value_config {
    uint64_t config = ASC_FILL_VALUE_DEFAULT_VALUE;
    struct {
        uint64_t repeat : 15;
        uint64_t reserved1 : 1;
        uint64_t blk_num : 15;
        uint64_t reserved2 : 1;
        uint64_t dst_gap : 15;
        uint64_t reserved3 : 17;
    };
};
```

## Fields

|Field |Description|
|----------|----------|
| **repeat** | Number of iterations. Default value: 0. Value Range: [0, 32767]. |
| **blk_num** | Number of data blocks initialized per iteration. Default value: 0. Value Range: [0, 32767].<br>When the target operand is in the L1 Buffer, the data block size is 32 bytes; when the target operand is in the L0A Buffer/L0B Buffer, the data block size is 512 bytes. |
| **dst_gap** | Distance between the end address of the previous iteration and the start address of the next iteration of the target operand. Default value: 0. Value Range: [0, 32767].<br>When the target operand is in the L1 Buffer, the unit is 32 bytes; when the target operand is in the L0A Buffer/L0B Buffer, the unit is 512 bytes. |