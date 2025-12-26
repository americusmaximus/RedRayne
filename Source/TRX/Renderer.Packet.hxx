#pragma once

#include "Basic.hxx"
#include "Renderers.Basic.hxx"
#include "Renderers.Basic.Vertexes.hxx"
#include "IO.Streams.hxx"

#define RENDER_PACKET_VERSION 2

#define RENDER_PACKET_BONE_PALETTE_UNK2_COUNT 128
#define RENDER_PACKET_BONE_PALETTE_UNK3_COUNT 128

#define RENDER_PACKET_BONE_PALETTE_UNK2_DEFAULT_VALUE -1

#define INVALID_RENDER_PACKET_VERTEX_SIZE 0

namespace Renderer::Packet
{
    // TODO: better name
    struct RendererPacketBonePalette
    {
        s32 Count;//TODO
        s32 Unk2[RENDER_PACKET_BONE_PALETTE_UNK2_COUNT];//TODO
        s32 Unk3[RENDER_PACKET_BONE_PALETTE_UNK3_COUNT];//TODO
    };

    // TODO: what values are valid?
    enum class RendererPacketType : u32
    {
        Unk0 = 0, // TODO: Better name.
        RVX = 1,
        RTLVX = 2,
        Unk3 = 3, // TODO: Better name.
        RBNVX = 4,
        Unk5 = 5, // TODO: Better name.
        RBVX = 6
    };

    struct RendererPacket
    {
        s32 Unk1;// TODO
        RendererPacketType Type;
        union
        {
            void* Unknown; // TODO: Better name.
            Renderers::RVX* Simple; // TODO: Better name.
            Renderers::RBNVX* Bone; // TODO: Better name.
            Renderers::RBVX* Basis; // TODO: Better name.
            Renderers::RTLVX* D3DTLVertexes; // TODO: Better name.
        } Vertexes; // TODO: Better name.
        s32 VertexCount;
        u16* Indexes;
        s32 IndexCount;
        s32 Unk2;//TODO
        s32 Unk3;// TODO
        void* VertexBuffer;
        RendererPacketBonePalette* Bones; //TODO
    };

    // TODO: better data types
    struct RendererPacketBoneConstant
    {
        f32x4 Bones[3]; // TODO, why 3?
    };

    struct RendererPacketContainer
    {
        RendererPacketBoneConstant* _Unknown1 = (RendererPacketBoneConstant*)0x008496fc;//TODO [128 elements]
        RendererPacketBoneConstant** _Unknown2 = (RendererPacketBoneConstant**)0x0084af0c;
        s32* _Unknown2Count = (s32*)0x0084af08;//TODO type

        struct
        {
            u32* _Counter = (u32*)0x007302dc;//TODO
            u32* _IndexCount = (u32*)0x007302d4;//TODO
            u32* _VertexCount = (u32*)0x007302d8;//TODO
        } Counters;
    };

    extern RendererPacketContainer RendererPacketState;

    void* AllocateRendererPacketMemory(const u32 size);
    u32 AcquireRendererPacketVertexSize(const RendererPacket* packet);
    void AllocateRendererPacketMemory1(RendererPacket* packet);
    void AllocateRendererPacketMemory2(RendererPacket* packet);
    void InitializeRendererPacketVertexBuffer(RendererPacket* packet);

    void LoadBinaryRendererPacket1(RendererPacket* packet, IO::Streams::AbstractInStream* stream);
    void LoadBinaryRendererPacket2(RendererPacket* packet, IO::Streams::AbstractInStream* stream);

    void InitializeRendererPacket1(RendererPacket* packet);
    void InitializeRendererPacket2(RendererPacket* packet, IO::Streams::AbstractInStream* stream);

    f32 InitializeRendererPacketBonePalette2(RendererPacketBonePalette* palette, s32 indx);

    s32 RenderRendererPacket(RendererPacket* packet);
    s32 RenderRendererPacketBasisVertexes(const Renderers::RBVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount);
    s32 RenderRendererPacketSimpleVertexes1(Renderers::RVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount);

    s32 RenderRendererPacketD3DTLVertexes1(Renderers::RTLVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount);
    s32 RenderRendererPacketBoneVertexes1(Renderers::RBNVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount, RendererPacketBoneConstant* constants, u32 constantCount);
    s32 RenderRendererPacketSimpleVertexes2(const char* name, Renderers::RVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount);

    void ReleaseRendererPacket(RendererPacket* packet);
}