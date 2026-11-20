# Overview

<!-- md-trans-meta sourceCommit=1398d4baddf82d4b412922bb4a32a05abf2ada8a translatedAt=2026-11-18T07:40:48.328223+00:00 pushedAt=2026-11-18T07:42:30.263Z nl=2 -->

## API List

### Bitwise Operations

**Table 1** Bitwise operation APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_clear_nthbit](scalar_bit/asc_clear_nthbit.md) | Clears the specified bit of a 64-bit unsigned integer to 0 and leaves the remaining bits unchanged. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_clz](scalar_bit/asc_clz.md) | Counts the number of leading zeros in the binary representation of a 64-bit unsigned integer. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_ffs](scalar_bit/asc_ffs.md) | Finds the first bit with value 1 starting from the least significant bit and returns its bit index; returns -1 if not found. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_ffz](scalar_bit/asc_ffz.md) | Finds the first bit with value 0 starting from the least significant bit and returns its bit index; returns -1 if not found. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_popc](scalar_bit/asc_popc.md) | Counts the number of bits with value 1 in the binary representation of a 64-bit unsigned integer. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_set_nthbit](scalar_bit/asc_set_nthbit.md) | Sets the specified bit of a 64-bit unsigned integer to 1 and leaves the remaining bits unchanged. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_sflbits](scalar_bit/asc_sflbits.md) | Starting from the second-highest bit, counts the number of consecutive bits identical to the sign bit; returns -1 when the input is all 0s or all 1s. | `c_api/scalar_compute/compute/scalar_bit.h` |
| [asc_zero_bits_cnt](scalar_bit/asc_zero_bits_cnt.md) | Counts the number of bits with value 0 in the binary representation of a 64-bit unsigned integer. | `c_api/scalar_compute/compute/scalar_bit.h` |

### Type Conversion

**Table 2** Type conversion APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_float2half](scalar_convert/asc_float2half.md) | Converts a scalar of type `float` to type `half`, with the rounding mode fixed to ODD (round to nearest odd). | `c_api/scalar_compute/compute/scalar_convert.h` |
| [asc_float2int32](scalar_convert/asc_float2int32.md) | Converts a scalar of type `float` to type `int32_t`. | `c_api/scalar_compute/compute/scalar_convert.h` |

### Data Load

**Table 3** Data load APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_load_dev](scalar_load/asc_load_dev.md) | Bypasses DCache and reads integer data directly from a GM address. | `c_api/scalar_compute/scalar_load.h` |

### Data Store Out

**Table 4** Data store-out APIs

| API Name | API Description | Finest-Grained Common Header File |
| --- | --- | --- |
| [asc_store_dev](scalar_store/asc_store_dev.md) | Bypasses DCache and writes data directly to a global memory address. | `c_api/scalar_compute/scalar_store.h` |
