/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef CCU_ST_SELECTOR_SHIM_CCU_CCU_API_CCU_PRIMITIVES_H
#define CCU_ST_SELECTOR_SHIM_CCU_CCU_API_CCU_PRIMITIVES_H

#include "ccu_primitives_stub.h"

namespace asc {
namespace ccu {
using namespace ::AscendC::ccu;
template <typename T>
using array = ::AscendC::ccu::Array<T>;
using event = ::AscendC::ccu::Event;
using ccu_buffer = ::AscendC::ccu::CcuBuffer;
using local_addr = ::AscendC::ccu::LocalAddr;
using remote_addr = ::AscendC::ccu::RemoteAddr;
using func = ::AscendC::ccu::Func;
using loop = ::AscendC::ccu::Loop;
using loop_group = ::AscendC::ccu::LoopGroup;
using address = ::AscendC::ccu::Address;

inline Result load(variable address, variable destination) { return Load(address, destination); }
inline Result load(uint64_t address, variable destination) { return Load(address, destination); }
inline Result load(variable address, array<variable>& destinations, uint32_t count)
{
    return Load(address, destinations, count);
}
inline Result load(uint64_t address, array<variable>& destinations, uint32_t count)
{
    return Load(address, destinations, count);
}
inline Result store(variable address, variable source) { return Store(address, source); }
inline Result store(uint64_t address, variable source) { return Store(address, source); }

inline Result write_variable_with_notify(
    ChannelHandle channel, variable var, uint32_t remoteVarIdx, uint32_t remoteNotifyIdx, uint16_t mask = 1)
{
    return WriteVariableWithNotify(channel, var, remoteVarIdx, remoteNotifyIdx, mask);
}
inline Result notify_wait(ChannelHandle channel, uint32_t localNotifyIndex, uint16_t mask = 1)
{
    return NotifyWait(channel, localNotifyIndex, mask);
}
inline Result notify_record(ChannelHandle channel, uint32_t remoteNotifyIndex, uint16_t mask = 1)
{
    return NotifyRecord(channel, remoteNotifyIndex, mask);
}
inline Result event_record(event e, uint16_t mask = 1) { return EventRecord(e, mask); }
inline Result event_wait(event e, uint16_t mask = 1) { return EventWait(e, mask); }
inline Result event_record(const char* notifyTag, uint16_t mask = 1) { return EventRecord(notifyTag, mask); }
inline Result event_wait(const char* notifyTag, uint16_t mask = 1) { return EventWait(notifyTag, mask); }
inline Result local_copy(local_addr dst, local_addr src, variable len, event e, uint16_t mask = 1)
{
    return LocalCopy(dst, src, len, e, mask);
}
inline Result local_copy(ccu_buffer dst, local_addr src, variable len, event e, uint16_t mask = 1)
{
    return LocalCopy(dst, src, len, e, mask);
}
inline Result local_copy(local_addr dst, ccu_buffer src, variable len, event e, uint16_t mask = 1)
{
    return LocalCopy(dst, src, len, e, mask);
}
inline Result read(
    ChannelHandle channel, local_addr local, remote_addr remote, variable len, event e, uint16_t mask = 1)
{
    return Read(channel, local, remote, len, e, mask);
}
inline Result read(
    ChannelHandle channel, ccu_buffer local, remote_addr remote, variable len, event e, uint16_t mask = 1)
{
    return Read(channel, local, remote, len, e, mask);
}
template <typename DT, typename OP>
inline Result read_reduce(
    ChannelHandle channel, local_addr local, remote_addr remote, variable len, DT dataType, OP opType, event e,
    uint16_t mask = 1)
{
    return ReadReduce(
        channel, local, remote, len, static_cast<HcclDataType>(static_cast<int>(dataType)),
        static_cast<HcclReduceOp>(static_cast<int>(opType)), e, mask);
}
template <typename DT, typename OP>
inline Result local_reduce(
    local_addr dst, local_addr src, variable len, DT dataType, OP opType, event e, uint16_t mask = 1)
{
    return LocalReduce(
        dst, src, len, static_cast<HcclDataType>(static_cast<int>(dataType)),
        static_cast<HcclReduceOp>(static_cast<int>(opType)), e, mask);
}
template <typename DT, typename OT, typename OP>
inline Result local_reduce(
    ccu_buffer* buffers, uint32_t count, DT dataType, OT outputDataType, OP opType, variable len, event e,
    uint16_t mask = 1)
{
    return LocalReduce(
        buffers, count, static_cast<HcclDataType>(static_cast<int>(dataType)),
        static_cast<HcclDataType>(static_cast<int>(outputDataType)),
        static_cast<HcclReduceOp>(static_cast<int>(opType)), len, e, mask);
}
inline Result write(
    ChannelHandle channel, remote_addr remote, local_addr local, variable len, event e, uint16_t mask = 1)
{
    return Write(channel, remote, local, len, e, mask);
}
template <typename DT, typename OP>
inline Result write_reduce(
    ChannelHandle channel, remote_addr remote, local_addr local, variable len, DT dataType, OP opType, event e,
    uint16_t mask = 1)
{
    return WriteReduce(
        channel, remote, local, len, static_cast<HcclDataType>(static_cast<int>(dataType)),
        static_cast<HcclReduceOp>(static_cast<int>(opType)), e, mask);
}
} // namespace ccu
} // namespace asc

#endif // CCU_ST_SELECTOR_SHIM_CCU_CCU_API_CCU_PRIMITIVES_H
