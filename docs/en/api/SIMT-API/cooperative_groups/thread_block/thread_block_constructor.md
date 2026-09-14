# **thread_block** Constructor

<!-- md-trans-meta sourceCommit=e290f6324f3caf31e5b53e9c14d1dc9a5b1efebb translatedAt=2026-09-07T06:51:12.548Z pushedAt=2026-09-07T09:11:27.113Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

`thread_block` does not provide a default constructor. Users use the `this_thread_block` function to obtain the `thread_block` object to which the current thread belongs.

## Function Prototype

```c++
thread_block this_thread_block()
```

## Parameters

None

## Return Value

Returns the `thread_block` object to which the current thread belongs.

## Constraints

None

## Example

See [Example](sync.md#example)
