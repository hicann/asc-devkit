# asc_store_atomic_config

<!-- md-trans-meta sourceCommit=282ae070f9d90289ec519e0d57dbf32c7e9ac1e9 translatedAt=2026-09-05T12:48:37.179Z pushedAt=2026-09-08T03:45:05.383Z -->

The **asc_store_atomic_config** parameter contains the atomic operation enable bit and the atomic operation type, and is used by the [asc_get_store_atomic_config](../simd_atomic/asc_get_store_atomic_config.md) API.

## Structure Definition

```cpp
constexpr uint64_t ASC_STORE_ATOMIC_DEFAULT_VALUE = 0x0000000000000000;
union asc_store_atomic_config {
    uint64_t config = ASC_STORE_ATOMIC_DEFAULT_VALUE;
    struct {
        uint64_t atomic_type : 3;
        uint64_t atomic_op : 2;
        uint64_t reserved1 : 59;
    };
};
```

## Fields

| Field | Description |
|----------|----------|
| **atomic_type** | Atomic operation enable bit. Default value: 0.<br>**0**: No atomic operation.<br>**1**: Enables atomic operation, with the data type **float**.<br>**2**: Enables atomic operation, with the data type **half**.<br>**3**: Enables atomic operation, with the data type **int16_t**.<br>**4**: Enables atomic operation, with the data type **int32_t**.<br>**5**: Enables atomic operation, with the data type **int8_t**.<br>**6**: Enables atomic operation, with the data type **bfloat16_t**.<br>Other values have no specific meaning. |
| **atomic_op** | Atomic operation type. Default value: 0.<br>**0**: Sum operation.<br>Other values have no specific meaning. |
