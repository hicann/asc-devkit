# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Host regression for the actual 2002 sender, with device copies/FIFO consumption simulated.

Run: python3 tests/api/basic_api/dump_super_tensor/test_2002_sender.py
Requires a C++17 compiler. This does not replace device compilation or board tests.
"""

from pathlib import Path
import os
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[4]
SOURCE = ROOT / "impl/basic_api/dav_m200/kernel_operator_dump_tensor_impl.h"


def section(text, start, end):
    assert start in text, f"Missing sender implementation: {start}"
    return text[text.index(start) : text.index(end, text.index(start))]


def test_2002_sender():
    source = SOURCE.read_text()
    types = (ROOT / "impl/utils/debug/asc_debug_types.h").read_text()
    structs = section(types, "struct DebugBlockHeadInfo {", "struct TimeStampTlv {")
    copies = section(
        source,
        "__aicore__ inline void ClearGmData(",
        "template <typename T>\n__aicore__ inline void WriteRingBufTlvData(",
    )
    sender = section(source, "template <typename T>\n__aicore__ inline void WriteSuperTensorData(", "\n#endif")
    entry = section(
        source,
        "template <template <typename> class Tensor, typename T>\n__aicore__ inline void DumpTensorRingBufImpl(\n    const Tensor<T>& src, uint32_t desc, uint32_t dumpSize, const uint32_t* shape, const uint32_t shapeDim)\n{",
        "__aicore__ inline void WriteRingBufShapeInfo(const ShapeInfo& shapeInfo)\n{",
    )
    code = PRELUDE + structs + STUBS + copies + sender + entry + MAIN
    with tempfile.TemporaryDirectory(prefix="dump-2002-") as directory:
        cpp = Path(directory) / "sender.cpp"
        binary = Path(directory) / "sender"
        cpp.write_text(code)
        subprocess.run(
            [
                os.environ.get("CXX", "g++"),
                "-std=c++17",
                "-O1",
                "-g",
                "-fsanitize=address,undefined",
                "-fno-omit-frame-pointer",
                str(cpp),
                "-o",
                str(binary),
            ],
            check=True,
        )
        subprocess.run([str(binary)], check=True, env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


PRELUDE = r"""
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <vector>
#define __aicore__
#define __gm__
#define __ubuf__
#define __cbuf__
#define __cc__
#define ASCENDC_ASSERT(x, ...) assert(x)
constexpr uint32_t ONE_BLK_SIZE=32, ONE_DUMP_BACKUP_SIZE=1024, K_MAX_SHAPE_DIM=8;
constexpr int PIPE_ALL=0;
enum class Hardware {GM, UB, L1, L0C, MAX};
enum class TPosition {VECIN};
enum class DumpType {DUMP_BUFI=1,DUMP_BUFO=2,DUMP_SKIP=3,DUMP_SCALAR=4,DUMP_TENSOR=5,DUMP_SHAPE=6,
                     DUMP_SUPER_TENSOR=11,DUMP_SUPER_TENSOR_BODY=12};
namespace Internal {enum class DumpTensorDataType {ACL_UINT32=4, ACL_MAX=99};}
namespace __asc_aicore {
using ::DumpType;
"""
STUBS = r"""
}
using BlockRingBufInfo=__asc_aicore::DebugBlockHeadInfo;
using RingBufWriteInfo=__asc_aicore::DebugBlockWriteInfo;
using RingBufReadInfo=__asc_aicore::DebugBlockReadInfo;
using DumpTensorTlvInfoHead=__asc_aicore::DumpTensorTlv;
namespace cache_line_t {constexpr int ENTIRE_DATA_CACHE=0;}
void dcci(uint64_t*,int) {}
template<int> void PipeBarrier() {}
template<class A,class B> using IsSameType=std::is_same<A,B>;
template<class T> constexpr auto GetTensorDataType(){return Internal::DumpTensorDataType::ACL_UINT32;}
uint32_t AlignUp(uint32_t n,uint32_t a){return (n+a-1)/a*a;}
unsigned GetBlockIdxImpl(){return 7;}
void EnablePrintf(){}
uint8_t ub[1024];
template<class T> struct GlobalTensor {
 T* ptr; T* GetPhyAddr() const{return ptr;}
 GlobalTensor operator[](uint64_t n) const{return {ptr+n};}
};
template<class T> struct LocalTensor {
 T* ptr=nullptr; Hardware pos=Hardware::UB;
 T* GetPhyAddr() const{return ptr;} Hardware GetPosition()const{return pos;}
 LocalTensor operator[](uint32_t n)const{return {ptr+n,pos};}
};
Hardware GetPhyType(Hardware p){return p;}
template<class T> Hardware CheckDumpTensorPosition(const LocalTensor<T>& s){return s.pos;}
template<class T> void InitTmpTensor(LocalTensor<T>& s,uint8_t){s.ptr=reinterpret_cast<T*>(ub);}
struct DataCopyParams {uint16_t blockCount=1,blockLen=0,srcStride=0,dstStride=0;};
enum class BlockMode {BLOCK_MODE_MATRIX};
struct DataCopyEnhancedParams {BlockMode blockMode;};
template<class A,class B> void DataCopyUB2GMImpl(A* d,B* s,DataCopyParams p){memcpy(d,s,p.blockLen*32);}
template<class A,class B> void DataCopyGM2UBImpl(A* d,B* s,DataCopyParams p){memcpy(d,s,p.blockLen*32);}
template<class A,class B> void DataCopyL12UBImpl(A* d,B* s,DataCopyParams p){memcpy(d,s,p.blockLen*32);}
template<class A,class B> void DataCopyL0C2UBImpl(A* d,B* s,DataCopyParams p,DataCopyEnhancedParams){memcpy(d,s,p.blockLen*1024);}
void MemCopyGm2Gm(uint8_t* d,const uint8_t* s,const uint32_t& n){memcpy(d,s,n);}
BlockRingBufInfo block;
RingBufWriteInfo writer;
std::vector<uint8_t> ring, collected;
unsigned reserved=0,commits=0,failAfter=~0u,normalCalls=0,wraps=0;
BlockRingBufInfo* GetBlockRingBufInfo(){return &block;}
namespace __asc_aicore {
uint32_t align_up(uint32_t n,uint32_t a){return AlignUp(n,a);}
void asc_entire_dcci(uint64_t*){}
bool check_ringbuf_space(DebugBlockHeadInfo* b,const uint32_t& n){
 assert(n<=b->ringBufLen);
 if(commits==failAfter)return false;
 if(writer.bufOffset+n>b->ringBufLen){writer.bufOffset=0;++wraps;}
 reserved=n;
 std::fill(ring.begin()+writer.bufOffset+n,ring.end(),0xA5);
 return true;
}
uint8_t* get_ringbuf_tlv_addr(DebugBlockHeadInfo*){return ring.data()+writer.bufOffset;}
DebugBlockWriteInfo* get_block_write_info(DebugBlockHeadInfo*){return &writer;}
void update_write_info(DebugBlockWriteInfo*,const uint32_t& n){
 assert(n<=reserved);
 for(size_t i=writer.bufOffset+reserved;i<ring.size();++i)assert(ring[i]==0xA5);
 collected.insert(collected.end(),ring.data()+writer.bufOffset,ring.data()+writer.bufOffset+n);
 writer.bufOffset+=n;++writer.packIdx;++commits;
}
}
bool CheckAndWaitRingBufSpace(BlockRingBufInfo*,const uint32_t& n){++normalCalls;return n<=block.ringBufLen;}
uint8_t* GetRingBufTlv(BlockRingBufInfo*){return ring.data();}
RingBufWriteInfo* GetRingBufWriteInfo(BlockRingBufInfo*){return &writer;}
void UpdateWriteInfo(RingBufWriteInfo*,const uint32_t&){}
template<class... A>void WriteRingBufTlvHead(A...){}
template<class... A>void WriteRingBufTlvShape(A...){}
template<class... A>void WriteRingBufTlvData(A...){}
"""
MAIN = r"""
void reset(unsigned size){
 block.ringBufLen=size;ring.assign(size+4096,0xA5);collected.clear();writer={};
 reserved=commits=normalCalls=wraps=0;failAfter=~0u;
 std::fill(std::begin(ub),std::end(ub),0x71);
}
void verify(const std::vector<uint32_t>& src,uint32_t count,Hardware pos){
 size_t cursor=0;uint64_t offset=0;std::vector<uint8_t> result;
 while(cursor<collected.size()){
  uint32_t header,size,span;uint64_t total,off;
  if(offset==0){
   __asc_aicore::DumpSuperTensorTlv h;memcpy(&h,&collected[cursor],sizeof(h));
   assert(h.type==11&&h.desc==17&&h.position==static_cast<uint16_t>(pos)&&h.blockIdx==7);
   assert(h.dim==8&&h.shape[0]==count&&h.shape[7]==1);
   assert(h.tensorAddr==static_cast<uint32_t>(reinterpret_cast<uintptr_t>(src.data())));
   assert(h.dataType==4);
   header=sizeof(h);size=h.dumpSize;span=h.length+8;total=h.tensorLength;off=h.tensorOffset;
  }else{
   __asc_aicore::DumpSuperTensorBodyTlv h;memcpy(&h,&collected[cursor],sizeof(h));
   assert(h.type==12);header=sizeof(h);size=h.dumpSize;span=h.length+8;total=h.tensorLength;off=h.tensorOffset;
  }
  assert(total==uint64_t(count)*4&&off==offset&&size>0&&header+size<=span);
  result.insert(result.end(),collected.begin()+cursor+header,collected.begin()+cursor+header+size);
  offset+=size;cursor+=span;
 }
 assert(offset==uint64_t(count)*4&&memcmp(result.data(),src.data(),offset)==0);
 for(auto b:ub)assert(b==0x71);
}
int main(){
 for(auto pos:{Hardware::GM,Hardware::UB,Hardware::L1,Hardware::L0C}){
  for(unsigned count:{1025u,4097u,16385u})for(unsigned ringSize:{4096u,8192u}){
   reset(ringSize);std::vector<uint32_t> src(AlignUp(count*4,1024)/4);
   for(unsigned i=0;i<src.size();++i)src[i]=i*1234567u;
   uint32_t shape[8]={count,1,1,1,1,1,1,1};
   if(pos==Hardware::GM)DumpTensorRingBufImpl(GlobalTensor<uint32_t>{src.data()},17,count,shape,8);
   else DumpTensorRingBufImpl(LocalTensor<uint32_t>{src.data(),pos},17,count,shape,8);
   assert(commits>0);verify(src,count,pos);
   if(count*4>ringSize*2)assert(wraps>0);
  }
 }
 reset(4096);std::vector<uint32_t> src(4096,42);failAfter=1;
 DumpTensorRingBufImpl(GlobalTensor<uint32_t>{src.data()},17,4096,nullptr,0);assert(commits==1);
 reset(4096);failAfter=1;
 DumpTensorRingBufImpl(GlobalTensor<uint32_t>{src.data()},17,0x40000001u,nullptr,0);
 assert(commits==1);__asc_aicore::DumpSuperTensorTlv h;
 memcpy(&h,collected.data(),sizeof(h));assert(h.tensorLength==0x100000004ULL);
 reset(4096);DumpTensorRingBufImpl(GlobalTensor<uint32_t>{src.data()},17,16,nullptr,0);
 assert(normalCalls==1&&commits==0);
 reset(4096);DumpTensorRingBufImpl(GlobalTensor<uint32_t>{src.data()},17,0,nullptr,0);
 assert(normalCalls==0&&commits==0);
 reset(96);DumpTensorRingBufImpl(GlobalTensor<uint32_t>{src.data()},17,4096,nullptr,0);assert(commits==0);
 reset(2048);DumpTensorRingBufImpl(LocalTensor<uint32_t>{src.data(),Hardware::L0C},17,4096,nullptr,0);assert(commits==0);
 std::cout<<"2002 sender: 24 round trips, wrap, timeout, >4GiB, ordinary/zero/tiny FIFO passed\n";
}
"""

if __name__ == "__main__":
    test_2002_sender()
