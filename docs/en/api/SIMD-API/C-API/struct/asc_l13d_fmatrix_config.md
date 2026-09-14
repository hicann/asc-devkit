# asc_l13d_fmatrix_config

<!-- md-trans-meta sourceCommit=282ae070f9d90289ec519e0d57dbf32c7e9ac1e9 translatedAt=2026-09-05T12:46:37.511Z pushedAt=2026-09-08T03:45:05.378Z -->

**asc_l13d_fmatrix_config** is used to set the Feature map parameters of the 3D format data transfer API of [asc_copy_l12l0a](../cube_datamove/asc_copy_l12l0a)/[asc_copy_l12l0b](../cube_datamove/asc_copy_l12l0b).

## Structure Definition

```cpp
constexpr uint64_t ASC_DEFAULT_L13D_FMATRIX_CONFIG_VALUE = 0;
union asc_l13d_fmatrix_config {
    uint64_t config = ASC_DEFAULT_L13D_FMATRIX_CONFIG_VALUE;
    struct {
        uint16_t l1_height;
        uint16_t l1_width;
        uint8_t padding_left_size;
        uint8_t padding_right_size;
        uint8_t padding_top_size;
        uint8_t padding_bottom_size;
    };
};
```

## Constraints

After a union variable of type `asc_l13d_fmatrix_config` is defined, `l1_height` and `l1_width` must be assigned values that fit the actual scenario and fall within the valid range.

## Fields

|Field |Description|
|----------|----------|
| **l1_height** | Height of the feature map. Value range: [1, 32767]. |
| **l1_width** | Width of the feature map. Value range: [1, 32767]. |
| **padding_left_size** | Size of the left padding. Value range: [0, 255]. |
| **padding_right_size** | Size of the right padding. Value range: [0, 255]. |
| **padding_top_size** | Size of the top padding. Value range: [0, 255]. |
| **padding_bottom_size** | Size of the bottom padding. Value range: [0, 255]. |
