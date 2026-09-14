# asc_ndim_pad_count_config

<!-- md-trans-meta sourceCommit=282ae070f9d90289ec519e0d57dbf32c7e9ac1e9 translatedAt=2026-09-05T12:47:41.742Z pushedAt=2026-09-08T03:45:05.381Z -->

**asc_ndim_pad_count_config** is used in the [asc_set_ndim_pad_count](../vector_datamove/asc_set_ndim_pad_count.md) API to set the number of padding elements on the left and right sides of each dimension of the [asc_ndim_copy_gm2ub](../vector_datamove/asc_ndim_copy_gm2ub.md) API.

## Struct Definition

```cpp
constexpr uint64_t ASC_DEFAULT_NDIM_PAD_COUNT_CONFIG_VALUE = 0;
union asc_ndim_pad_count_config {
    uint64_t config = ASC_DEFAULT_NDIM_PAD_COUNT_CONFIG_VALUE;
    struct {
        uint8_t loop1_lp_count;
        uint8_t loop1_rp_count;
        uint8_t loop2_lp_count;
        uint8_t loop2_rp_count;
        uint8_t loop3_lp_count;
        uint8_t loop3_rp_count;
        uint8_t loop4_lp_count;
        uint8_t loop4_rp_count;
    };
};
```

## Fields

|Field |Description|
|----------|----------|
| loop1_lp_count | Number of elements to be padded on the left side of dimension 1.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop1_rp_count | Number of elements to be padded on the right side of dimension 1.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop2_lp_count | Number of elements to be padded on the left side of dimension 2.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop2_rp_count | Number of elements to be padded on the right side of dimension 2.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop3_lp_count | Number of elements to be padded on the left side of dimension 3.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop3_rp_count | Number of elements to be padded on the right side of dimension 3.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop4_lp_count | Number of elements to be padded on the left side of dimension 4.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
| loop4_rp_count | Number of elements to be padded on the right side of dimension 4.<br> The unit is the number of elements. The value range is [0, 255], and the default value is 0. |
