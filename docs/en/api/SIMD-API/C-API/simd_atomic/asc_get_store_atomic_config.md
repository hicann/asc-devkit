# asc_get_store_atomic_config

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-05T12:38:01.481Z pushedAt=2026-09-08T03:45:05.365Z -->

## Applicable Products

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products AI Core: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products Vector Core: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->

## Description

Obtains the values of the atomic operation enable bit and atomic operation type. It can be used to verify whether the atomic operation enable bit and atomic operation type values set by [asc_set_store_atomic_config_v1](./asc_set_store_atomic_config_v1.md) and [asc_set_store_atomic_config_v2 (deprecated)](./asc_set_store_atomic_config_v2_deprecated.md) meet expectations.

<!-- npu="950" id1 -->
**This API is deprecated on Ascend 950PR/Ascend 950DT. Atomic add operations can be implemented directly using [asc_atomic_add](../scalar_compute/asc_atomic_add.md). There is no need to configure and verify the atomic operation enable bit and atomic operation type values.**
<!-- end id1 -->

## Prototype

```cpp
__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config)
```

## Parameters

| Parameter  | Input/Output | Description |
| :----- | :------- | :------- |
| config | Output | Used to obtain the values of the atomic operation enable bit and atomic operation type. For details, see [asc_store_atomic_config](../struct/asc_store_atomic_config.md). |

## Return Value

None

## Pipeline Type

PIPE_S

## Constraints

- Must be used together with the [asc_set_store_atomic_config_v1](./asc_set_store_atomic_config_v1.md) and [asc_set_store_atomic_config_v2 (deprecated)](./asc_set_store_atomic_config_v2_deprecated.md) APIs, which are used to set the values of the atomic operation enable bit and atomic operation type.

## Example

```cpp
uint16_t atomic_type = 1; // Enable the atomic operation. The data type for the atomic operation is float, with a value of 1.
uint16_t atomic_op = 0; // Sum operation, with a value of 0.
asc_set_store_atomic_config_v1(atomic_type, atomic_op);
asc_store_atomic_config get_config;    // Used to obtain the values of the atomic operation enable bit and atomic operation type.
asc_get_store_atomic_config(get_config);    // get_config.atomic_type = 1; get_config.atomic_op = 0;
```
