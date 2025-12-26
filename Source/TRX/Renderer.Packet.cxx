#include "Renderer.Packet.hxx"
#include "Memory.hxx"
#include "Logger.hxx"
#include "App.Options.hxx"
#include "Renderer.hxx"
#include "Renderer.Module.hxx"
#include "Renderer.Allocator.hxx"

using namespace Renderer::Allocator;
using namespace Graphics;

using namespace Renderers;
using namespace IO::Streams;
using namespace Logger;
using namespace Memory;
using namespace App::Options;
using namespace Renderer::Module;

namespace Renderer::Packet
{
    RendererPacketContainer RendererPacketState;

    void InitializeRendererPacket1(RendererPacket* packet)
    {
        packet->Type = RendererPacketType::Unk0;
        packet->Vertexes.Unknown = NULL;
        packet->VertexCount = 0;
        packet->Indexes = NULL;
        packet->IndexCount = 0;
        packet->Unk3 = 0;
        packet->VertexBuffer = NULL;
        packet->Bones = NULL;
        packet->Unk1 = 0;
    }

    u32 AcquireRendererPacketVertexSize(const RendererPacket* packet)
    {
        // TODO is this complete?
        switch (packet->Type)
        {
        case RendererPacketType::RVX: { return sizeof(RVX); }
        case RendererPacketType::RTLVX: { return sizeof(RTLVX); }
        case RendererPacketType::RBNVX: { return sizeof(RBNVX); }
        case RendererPacketType::RBVX: { return sizeof(RBVX); }
        }

        LogError("Unable to provide vertex size for an unknown or unsupported type.");

        return INVALID_RENDER_PACKET_VERTEX_SIZE;
    }

    void AllocateRendererPacketMemory1(RendererPacket* packet)
    {
        packet->Vertexes.Unknown = AllocateMemory(packet->VertexCount * AcquireRendererPacketVertexSize(packet));

        if (packet->Vertexes.Unknown == NULL)
        {
            LogError("Unable to allocate render packet memory for %d vertexes.", packet->VertexCount);
        }
    }

    void AllocateRendererPacketMemory2(RendererPacket* packet)
    {
        packet->Indexes = (u16*)AllocateMemory(packet->IndexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT * sizeof(u16));

        if (packet->Indexes == NULL)
        {
            LogError("Unable to allocate render packet memory %d faces.", packet->IndexCount);
        }
    }

    void* AllocateRendererPacketMemory(const u32 size)
    {
        const auto result = AllocateMemory(size);

        if (result == NULL)
        {
            LogError("Unable to allocate render packet memory.");
        }

        return result;
    }

    void LoadBinaryRendererPacket1(RendererPacket* packet, AbstractInStream* stream)
    {
        LoadBinaryRendererPacket2(packet, stream);
        InitializeRendererPacket2(packet, stream);
    }

    void LoadBinaryRendererPacket2(RendererPacket* packet, AbstractInStream* stream)
    {
        u32 version = 0;

        stream->Self->ReadBlock(stream, &version, sizeof(u32));

        if (version != RENDER_PACKET_VERSION)
        {
            LogError("Unable to load binary render packet. Invalid version %d.", version);
        }

        stream->Self->ReadBlock(stream, &packet->Unk1, 4); // TODO size
        stream->Self->ReadBlock(stream, &packet->Type, sizeof(RendererPacketType));
        stream->Self->ReadBlock(stream, &packet->VertexCount, sizeof(s32));// TODO type
        stream->Self->ReadBlock(stream, &packet->IndexCount, sizeof(s32));// TODO type
        stream->Self->ReadBlock(stream, &packet->Unk2, 4); // TODO size
        stream->Self->ReadBlock(stream, &packet->Unk3, 4); // TODO size
    }

    void InitializeRendererPacket2(RendererPacket* packet, AbstractInStream* stream)
    {
        AllocateRendererPacketMemory1(packet);
        AllocateRendererPacketMemory2(packet);

        const auto size = AcquireRendererPacketVertexSize(packet);

        stream->Self->ReadBlock(stream, packet->Vertexes.Unknown, packet->VertexCount * size);
        stream->Self->ReadBlock(stream, packet->Indexes, packet->IndexCount * GRAPHICS_TRIANGLE_VERTEX_COUNT * sizeof(u16));

        InitializeRendererPacketVertexBuffer(packet);
    }

    void InitializeRendererPacketBonePalette(RendererPacketBonePalette* palette)
    {
        palette->Count = 0;

        for (u32 x = 0; x < RENDER_PACKET_BONE_PALETTE_UNK2_COUNT; x++)
        {
            palette->Unk2[x] = RENDER_PACKET_BONE_PALETTE_UNK2_DEFAULT_VALUE;
        }
    }

    // TODO: Better name
    f32 InitializeRendererPacketBonePalette2(RendererPacketBonePalette* palette, const s32 indx)
    {
        if (palette->Unk2[indx] == RENDER_PACKET_BONE_PALETTE_UNK2_DEFAULT_VALUE)
        {
            palette->Unk2[indx] = palette->Count;

            palette->Unk3[palette->Count] = indx;

            palette->Count = palette->Count + 1;
        }

        return (f32)(palette->Unk2[indx] * 3 + 25); // TODO: what is the meaning of this?
    }

    void InitializeRendererPacketVertexBuffer(RendererPacket* packet)
    {
        if (*AppOptionsState.Graphics._TextureBumpMappingMode == ApplicationTextureBumpMapOptionsMode::Off
            && packet->Type == RendererPacketType::RBVX && *RendererState._Unknown1 == 0) // TODO
        {
            auto sv = (RVX*)AllocateMemory(packet->VertexCount * sizeof(RVX));

            if (sv == NULL)
            {
                LogError("Unable to allocate memory for vertex buffer.");
            }

            const auto bv = packet->Vertexes.Basis;

            for (u32 x = 0; x < packet->VertexCount; x++)
            {
                sv[x].XYZ.X = bv[x].XYZ.X;
                sv[x].XYZ.Y = bv[x].XYZ.Y;
                sv[x].XYZ.Z = bv[x].XYZ.Z;

                sv[x].Normal.X = bv[x].UV1.X;
                sv[x].Normal.Y = bv[x].UV1.Y;
                sv[x].Normal.Z = 1.0f;

                sv[x].UV.X = bv[x].UV.X;
                sv[x].UV.Y = bv[x].UV.Y;
            }

            ReleaseMemory1(packet->Vertexes.Unknown);

            packet->Type = RendererPacketType::RVX;
            packet->Vertexes.Simple = sv;
        }

        if (RendererModule.HasShaderSupport != NULL)
        {
            *RendererModule.State._HasShaderSupport = RendererModule.HasShaderSupport();
        }

        if (*AppOptionsState.Graphics._TextureBumpMappingMode == ApplicationTextureBumpMapOptionsMode::On)
        {
            if (packet->Type == RendererPacketType::RBNVX)
            {
                packet->Bones = (RendererPacketBonePalette*)AllocateMemory1(sizeof(RendererPacketBonePalette));

                if (packet->Bones == NULL)
                {
                    LogError("Unable to allocate render packet bone palette.");
                }

                InitializeRendererPacketBonePalette(packet->Bones);

                const auto vertexes = packet->Vertexes.Bone;

                for (u32 x = 0; x < packet->VertexCount; x++)
                {
                    const auto v = vertexes[x];

                    InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[0]);

                    if (1 < v.BoneCount)
                    {
                        InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[1]);
                    }

                    if (2 < v.BoneCount)
                    {
                        InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[2]);
                    }

                    if (3 < v.BoneCount)
                    {
                        InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[3]);
                    }
                }

                if (0x16 < packet->Bones->Count)// TODO why 0x16?
                {
                    LogMessage("[WARNING] [RENDERER] The loaded model has no parts.\n");

                    ReleaseMemory2(packet->Bones);

                    packet->Bones = NULL;

                    return;
                }

                const auto size = packet->VertexCount * sizeof(RBNVX);

                auto bvs = (RBNVX*)AllocateMemory(size);

                if (bvs == NULL)
                {
                    LogError("Unable to allocate memory for bone vertex buffer.");
                }

                // TODO: combine CopyMemory with the loop below, so the copying and modifying happens in one loop.
                CopyMemory(bvs, vertexes, size);

                for (u32 x = 0; x < packet->VertexCount; x++)
                {
                    const auto v = vertexes[x];

                    bvs[x].Bones.Values[0] = InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[0]);
                    bvs[x].Bones.Values[1] = InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[1]);
                    bvs[x].Bones.Values[2] = InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[2]);
                    bvs[x].Bones.Values[3] = InitializeRendererPacketBonePalette2(packet->Bones, v.Bones.Indexes[3]);

                }

                if (*RendererModule._AllocateVertexBuffer != NULL)
                {
                    (*RendererModule._AllocateVertexBuffer)(bvs, packet->VertexCount, sizeof(RBNVX), &packet->VertexBuffer);
                }

                ReleaseMemory1(bvs);

                return;
            }

            if (*RendererModule._AllocateVertexBuffer != NULL)
            {
                (*RendererModule._AllocateVertexBuffer)(packet->Vertexes.Unknown,
                    packet->VertexCount, AcquireRendererPacketVertexSize(packet), &packet->VertexBuffer);
            }
        }
    }

    s32 RenderRendererPacket(RendererPacket* packet)
    {
        if (*RendererState.Fog._Mode)
        {
            SelectRendererTransform();
        }

        auto mode = *RendererState.Mode._UnknownMode2;

        if (*RendererState.Mode._UnknownMode6 != 0)
        {
            mode = *RendererState.Mode._UnknownMode2 | RendererMode::U0x20;
        }

        if (*RendererState.Mode._UnknownMode5 != 0)
        {
            mode = mode | RendererMode::U0x8;
        }

        switch (packet->Type)
        {
        case RendererPacketType::RVX: { break; }
        case RendererPacketType::RTLVX:
        {
            if (packet->VertexBuffer != NULL)
            {
                *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + packet->VertexCount;
                *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + packet->IndexCount;
                *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;

                return (*RendererModule._RenderVertexBufferPrelit)(packet->VertexBuffer, packet->VertexCount, packet->Indexes, packet->IndexCount, mode);
            }

            return RenderRendererPacketD3DTLVertexes1(packet->Vertexes.D3DTLVertexes, packet->VertexCount, packet->Indexes, packet->IndexCount);
        }
        case RendererPacketType::RBNVX:
        {
            if (packet->VertexBuffer != NULL)
            {
                for (u32 x = 0; x < packet->Bones->Count; x++)
                {
                    const auto indx = packet->Bones->Unk3[x];
                    const auto value1 = (*RendererPacketState._Unknown2)[indx];
                    auto value2 = &RendererPacketState._Unknown1[x];

                    for (u32 xx = 0; xx < 3; xx++) // TODO count, each struct has 3 elements
                    {
                        value2->Bones[xx].X = value1.Bones[xx].X;
                        value2->Bones[xx].Y = value1.Bones[xx].Y;
                        value2->Bones[xx].Z = value1.Bones[xx].Z;
                        value2->Bones[xx].W = value1.Bones[xx].W;
                    }
                }

                *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + packet->VertexCount;
                *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + packet->IndexCount;
                *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;

                // TODO types
                return (*RendererModule._RenderVertexBufferBone)(packet->VertexBuffer, packet->VertexCount,
                    packet->Indexes, packet->IndexCount, (f32x4*)RendererPacketState._Unknown1, packet->Bones->Count, mode);
            }

            return RenderRendererPacketBoneVertexes1(packet->Vertexes.Bone, packet->VertexCount,
                packet->Indexes, packet->IndexCount, *RendererPacketState._Unknown2, *RendererPacketState._Unknown2Count);
        }
        case RendererPacketType::RBVX:
        {
            if (packet->VertexBuffer != NULL)
            {
                *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + packet->VertexCount;
                *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + packet->IndexCount;
                *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;

                return (*RendererModule._RenderVertexBufferBasis)(packet->VertexBuffer, packet->VertexCount, packet->Indexes, packet->IndexCount, mode);
            }

            return RenderRendererPacketBasisVertexes(packet->Vertexes.Basis, packet->VertexCount, packet->Indexes, packet->IndexCount);
        }
        default:
        {
            LogError("Unable to render. Renderer packet type %d is not supported.", packet->Type);
        }
        }

        if (packet->VertexBuffer != NULL)
        {
            *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + packet->VertexCount;
            *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + packet->IndexCount;
            *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;

            return (*RendererModule._RenderVertexBuffer)(packet->VertexBuffer, packet->VertexCount, packet->Indexes, packet->IndexCount, mode);
        }

        return RenderRendererPacketSimpleVertexes1(packet->Vertexes.Simple, packet->VertexCount, packet->Indexes, packet->IndexCount);
    }

    typedef const s32(CDECLAPI* FUN_00509F50) (AbstractVertex*, s32, u16*, u32);//TODO
    FUN_00509F50 FUN_00509f50 = (FUN_00509F50)0x00509f50;//TODO

    typedef const s32(CDECLAPI* FUN_005091C0) (ProperTLVertex*, s32, u16*, u32, u32 mode);//TODO
    FUN_005091C0 FUN_005091c0 = (FUN_005091C0)0x005091c0;//TODO

    // TODO: Better name.
    s32 RenderRendererPacketD3DTLVertexes1(RTLVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount)
    {
        *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;
        *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + indexCount;
        *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + vertexCount;

        if (*RendererState.ViewPort._IsComplete)
        {
            return FUN_00509f50((AbstractVertex*)vertexes, vertexCount, indexes, indexCount);
        }

        SelectRendererTransform();
        SelectRendererMode();

        if (*RendererModule._PolyListD3DL != NULL && *RendererState._IsActive)
        {
            return (*RendererModule._PolyListD3DL)(vertexes, vertexCount, indexes, indexCount, *RendererState.Mode._UnknownMode1);
        }

        AllocateVertexBuffer2();

        if (RENDERER_TEN_THOUSAND_ELEMENT_COUNT < vertexCount)
        {
            LogError("Unable to render %d vertexes, maximum is %d.", vertexCount, RENDERER_TEN_THOUSAND_ELEMENT_COUNT);
        }

        u32 mode = 0;// TODO enum

        auto tlvs = (ProperTLVertex*)*RendererState.Allocated.Vertexes._AlignedAllocatedMemory1;

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto tlv = &tlvs[x];
            auto vertex = &vertexes[x];

            InitializeTLVertex2(&vertex->XYZ, tlv);

            tlv->Color.R = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 0) & 0xff);
            tlv->Color.G = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 8) & 0xff);
            tlv->Color.B = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 16) & 0xff);
            tlv->Color.A = ALMOST_ONE_TWO_HUNDRED_FIFTY_FIFTH * ((vertex->Color >> 24) & 0xff);

            tlv->UV.X = (vertex->UV).X;
            tlv->UV.Y = (vertex->UV).Y;

            InitializeTLVertex1(tlv);

            mode = mode | tlv->Unk5;
        }

        return FUN_005091c0(tlvs, vertexCount, indexes, indexCount, mode); // TODDO
    }

    s32 RenderRendererPacketBasisVertexes(const RBVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount)
    {
        *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;
        *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + indexCount;
        *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + vertexCount;

        SelectRendererTransform();
        SelectRendererMode();
        SelectRendererTransformO2W(FALSE);

        if (RendererModule.PolyListBasis != NULL && *RendererState._IsActive)
        {
            return RendererModule.PolyListBasis(vertexes, vertexCount, indexes, indexCount, *RendererState.Mode._UnknownMode1);
        }

        // NOTE: Originally an error.
        LogMessage("[WARNING] [RENDERER] No hardware support available. Method \"PolyListBasis\" is absent.\n");

        return FALSE;
    }

    // TODO name
    void InitializeTLVertexColor1(RVX* vertex, f32xRGBA* color)
    {
        color->R = *RendererState.Light.Color._R;
        color->G = *RendererState.Light.Color._G;
        color->B = *RendererState.Light.Color._B;
        color->A = *RendererState.Light.Color._A;

        const auto transform = *RendererState.Transform._TransformPointer;

        for (u32 x = 0; x < 3; x++)
        {
            // TODO make proper light type
            const auto alpha =
                (vertex->Normal.X * transform->LightMatrix.M[x][1])
                + (vertex->Normal.Y * transform->LightMatrix.M[x][2])
                + (vertex->Normal.Z * transform->LightMatrix.M[x][3]);

            if (alpha < 0.0f)
            {
                // TODO, why mismatch in R != R?
                color->R = color->R - alpha * RendererState.Light.Colors._B[x]; // TODO
                color->G = color->G - alpha * RendererState.Light.Colors._G[x]; // TODO
                color->B = color->B - alpha * RendererState.Light.Colors._B[x]; // TODO
            }
        }

        if (1.0f < color->R) { color->R = 1.0f; }
        if (1.0f < color->G) { color->G = 1.0f; }
        if (1.0f < color->B) { color->B = 1.0f; }
    }

    s32 RenderRendererPacketSimpleVertexes2(const char* name, RVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount)
    {
        return RenderRendererPacketSimpleVertexes1(vertexes, vertexCount, indexes, indexCount);
    }

    s32 RenderRendererPacketSimpleVertexes1(RVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount)
    {
        *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;
        *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + indexCount;
        *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + vertexCount;

        if (*RendererState.ViewPort._IsComplete)
        {
            return FUN_00509f50((AbstractVertex*)vertexes, vertexCount, indexes, indexCount);
        }

        SelectRendererTransform();
        SelectRendererMode();
        SelectRendererTransformO2W(FALSE);

        if (RendererModule.PolyList != NULL && *RendererState._IsActive)
        {
            return RendererModule.PolyList(vertexes, vertexCount, indexes, indexCount, *RendererState.Mode._UnknownMode1);
        }

        AllocateVertexBuffer2();

        if (RENDERER_TEN_THOUSAND_ELEMENT_COUNT < vertexCount)
        {
            LogError("Unable to render %d vertexes, maximum is %d.", vertexCount, RENDERER_TEN_THOUSAND_ELEMENT_COUNT);
        }

        u32 mode = 0;// TODO enum

        auto tlvs = (ProperTLVertex*)*RendererState.Allocated.Vertexes._AlignedAllocatedMemory1;

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto tlv = &tlvs[x];
            auto vertex = &vertexes[x];

            InitializeTLVertex2(&vertex->XYZ, tlv);

            tlv->UV.X = (vertex->UV).X;
            tlv->UV.Y = (vertex->UV).Y;

            if (*RendererState.Light._Mode == RendererLightingMode::Off)
            {
                tlv->Color.R = 1.0f;
                tlv->Color.G = 1.0f;
                tlv->Color.B = 1.0f;
                tlv->Color.A = 1.0f;
            }
            else
            {
                InitializeTLVertexColor1(vertex, &tlv->Color);
            }

            InitializeTLVertex1(tlv);

            mode = mode | tlv->Unk5;
        }

        return FUN_005091c0(tlvs, vertexCount, indexes, indexCount, mode); // TODDO
    }
    
    s32 RenderRendererPacketBoneVertexes1(RBNVX* vertexes, u32 vertexCount, u16* indexes, u32 indexCount, RendererPacketBoneConstant* constants, u32 constantCount)
    {
        *RendererPacketState.Counters._Counter = *RendererPacketState.Counters._Counter + 1;
        *RendererPacketState.Counters._IndexCount = *RendererPacketState.Counters._IndexCount + indexCount;
        *RendererPacketState.Counters._VertexCount = *RendererPacketState.Counters._VertexCount + vertexCount;

        SelectRendererTransform();
        SelectRendererMode();
        SelectRendererTransformO2W(FALSE);

        if (RendererModule.PolyListBone != NULL && *RendererState._IsActive)
        {
            // TODO: data types
            const auto result = RendererModule.PolyListBone(vertexes, vertexCount, indexes, indexCount, (f32x4*)constants, constantCount);

            if (result) { return result; }
        }

        AllocateVertexBuffer2();

        if (RENDERER_TEN_THOUSAND_ELEMENT_COUNT < vertexCount)
        {
            LogError("Unable to render %d vertexes, maximum is %d.", vertexCount, RENDERER_TEN_THOUSAND_ELEMENT_COUNT);
        }

        auto svs = (RVX*)*RendererState.Allocated.Vertexes._AlignedAllocatedMemory1;

        for (u32 x = 0; x < vertexCount; x++)
        {
            auto sv = &svs[x];
            auto bv = &vertexes[x];

            switch (bv->BoneCount)
            {
            case 1:
            {
                const auto indx = bv->Bones.Indexes[0];
                const auto bones = constants[indx].Bones;

                sv->XYZ.X = bv->XYZ.Z * bones[0].Z + bv->XYZ.X * bones[0].X + bv->XYZ.Y * bones[0].Y + bones[0].W;
                sv->XYZ.Y = bv->XYZ.Z * bones[1].Z + bv->XYZ.X * bones[1].X + bv->XYZ.Y * bones[1].Y + bones[1].W;
                sv->XYZ.Z = bv->XYZ.Z * bones[2].Z + bv->XYZ.X * bones[2].X + bv->XYZ.Y * bones[2].Y + bones[2].W;

                sv->Normal.X = bv->Diffuse.Z * bones[0].Z + bv->Diffuse.X * bones[0].X + bv->Diffuse.Y * bones[0].Y;
                sv->Normal.Y = bv->Diffuse.Z * bones[1].Z + bv->Diffuse.X * bones[1].X + bv->Diffuse.Y * bones[1].Y;
                sv->Normal.Z = bv->Diffuse.Z * bones[2].Z + bv->Diffuse.X * bones[2].X + bv->Diffuse.Y * bones[2].Y;

                break;
            }
            case 2:
            {
                const u32 indx[] = { bv->Bones.Indexes[0], bv->Bones.Indexes[1] };
                const f32x4* bones[] = { constants[indx[0]].Bones, constants[indx[1]].Bones };

                sv->XYZ.X =
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][0].Z + bv->BlendWeight.X * bones[1][0].X + bv->BlendWeight.Y * bones[1][0].Y + bones[1][0].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][0].Z + bv->XYZ.X * bones[0][0].X + bv->XYZ.Y * bones[0][0].Y + bones[0][0].W);

                sv->XYZ.Y =
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][1].Z + bv->BlendWeight.Y * bones[1][1].Y + bv->BlendWeight.X * bones[1][1].X + bones[1][1].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][1].Z + bv->XYZ.Y * bones[0][1].Y + bv->XYZ.X * bones[0][1].X + bones[0][1].W);

                sv->XYZ.Z =
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][2].Z + bv->BlendWeight.Y * bones[1][2].Y + bv->BlendWeight.X * bones[1][2].X + bones[1][2].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][2].Z + bv->XYZ.Y * bones[0][2].Y + bv->XYZ.X * bones[0][2].X + bones[0][2].W);

                sv->Normal.X =
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][0].Z + bv->Diffuse.X * bones[1][0].X + bv->Diffuse.Y * bones[1][0].Y) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][0].Z + bv->Diffuse.X * bones[0][0].X + bv->Diffuse.Y * bones[0][0].Y);

                sv->Normal.Y =
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][1].Z + bv->Diffuse.X * bones[1][1].X + bv->Diffuse.Y * bones[1][1].Y) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][1].Z + bv->Diffuse.X * bones[0][1].X + bv->Diffuse.Y * bones[0][1].Y);

                sv->Normal.Z =
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][2].Z + bv->Diffuse.X * bones[1][2].X + bv->Diffuse.Y * bones[1][2].Y) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][2].Z + bv->Diffuse.X * bones[0][2].X + bv->Diffuse.Y * bones[0][2].Y);

                break;
            }
            case 3:
            {
                const u32 indx[] = { bv->Bones.Indexes[0], bv->Bones.Indexes[1], bv->Bones.Indexes[2] };
                const f32x4* bones[] = { constants[indx[0]].Bones, constants[indx[1]].Bones, constants[indx[2]].Bones };

                sv->XYZ.X =
                    bv->PointSize.Z * (bv->BlendIndexes.Z * bones[2][0].Z + bv->BlendIndexes.X * bones[2][0].X + bv->BlendIndexes.Y * bones[2][0].Y + bones[2][0].W) +
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][0].Z + bv->BlendWeight.X * bones[1][0].X + bv->BlendWeight.Y * bones[1][0].Y + bones[1][0].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][0].Z + bv->XYZ.X * bones[0][0].X + bv->XYZ.Y * bones[0][0].Y + bones[0][0].W);

                sv->XYZ.Y =
                    bv->PointSize.Z * (bv->BlendIndexes.Z * bones[2][1].Z + bv->BlendIndexes.Y *bones[2][1].Y + bv->BlendIndexes.X * bones[2][1].X +bones[2][1].W) +
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][1].Z + bv->BlendWeight.Y * bones[1][1].Y + bv->BlendWeight.X * bones[1][1].X + bones[1][1].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][1].Z + bv->XYZ.Y * bones[0][1].Y + bv->XYZ.X * bones[0][1].X + bones[0][1].W);

                sv->XYZ.Z =
                    bv->PointSize.Z * (bv->BlendIndexes.Z * bones[2][2].Z + bv->BlendIndexes.Y * bones[2][2].Y + bv->BlendIndexes.X * bones[2][2].X + bones[2][2].W) +
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][2].Z + bv->BlendWeight.Y * bones[1][2].Y + bv->BlendWeight.X * bones[1][2].X + bones[1][2].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][2].Z + bv->XYZ.Y * bones[0][2].Y + bv->XYZ.X * bones[0][2].X + bones[0][2].W);

                sv->Normal.X =
                    bv->PointSize.Z * (bv->Diffuse.Z * bones[2][0].Z + bv->Diffuse.X * bones[2][0].X + bv->Diffuse.Y * bones[2][0].Y) +
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][0].Z + bv->Diffuse.X * bones[1][0].X + bv->Diffuse.Y * bones[1][0].Y) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][0].Z + bv->Diffuse.X * bones[0][0].X + bv->Diffuse.Y * bones[0][0].Y);

                sv->Normal.Y =
                    bv->PointSize.Z * (bv->Diffuse.Z * bones[2][1].Z + bv->Diffuse.Y * bones[2][1].Y + bv->Diffuse.X * bones[2][1].X) +
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][1].Z + bv->Diffuse.Y * bones[1][1].Y + bv->Diffuse.X * bones[1][1].X) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][1].Z + bv->Diffuse.Y * bones[0][1].Y + bv->Diffuse.X * bones[0][1].X);

                sv->Normal.Z =
                    bv->PointSize.Z * (bv->Diffuse.Z * bones[2][2].Z + bv->Diffuse.Y * bones[2][2].Y + bv->Diffuse.X * bones[2][2].X) +
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][2].Z + bv->Diffuse.Y * bones[1][2].Y + bv->Diffuse.X * bones[1][2].X) +
                    bv->PointSize.X + (bv->Diffuse.Z * bones[0][2].Z + bv->Diffuse.Y * bones[0][2].Y + bv->Diffuse.X * bones[0][2].X);

                break;
            }
            default:
            {
                const u32 indx[] = { bv->Bones.Indexes[0], bv->Bones.Indexes[1], bv->Bones.Indexes[2], bv->Bones.Indexes[3] };
                const f32x4* bones[] = { constants[indx[0]].Bones, constants[indx[1]].Bones, constants[indx[2]].Bones, constants[indx[3]].Bones };
                
                sv->XYZ.X = 
                    bv->PointSize.W * (bv->Normal.Z * bones[3][0].Z + bv->Normal.X * bones[3][0].X + bv->Normal.Y * bones[3][0].Y + bones[3][0].W) +
                    bv->PointSize.Z * (bv->BlendIndexes.Z * bones[2][0].Z + bv->BlendIndexes.X * bones[2][0].X + bv->BlendIndexes.Y * bones[2][0].Y + bones[2][0].W) +
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][0].Z + bv->BlendWeight.X * bones[1][0].X + bv->BlendWeight.Y * bones[1][0].Y + bones[1][0].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][0].Z + bv->XYZ.X * bones[0][0].X + bv->XYZ.Y * bones[0][0].Y + bones[0][0].W);

                sv->XYZ.Y =
                    bv->PointSize.W * (bv->Normal.Z * bones[3][1].Z + bv->Normal.Y * bones[3][1].Y + bv->Normal.X * bones[3][1].X + bones[3][1].W) +
                    bv->PointSize.Z * (bv->BlendIndexes.Z * bones[2][1].Z + bv->BlendIndexes.Y * bones[2][1].Y + bv->BlendIndexes.X * bones[2][1].X + bones[2][1].W) +
                    bv->PointSize.Y + (bv->BlendWeight.Z * bones[1][1].Z + bv->BlendWeight.Y * bones[1][1].Y + bv->BlendWeight.X * bones[1][1].X + bones[1][1].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][1].Z + bv->XYZ.Y * bones[0][1].Y + bv->XYZ.X * bones[0][1].X + bones[0][1].W);

                sv->XYZ.Z =
                    bv->PointSize.W * (bv->Normal.Z * bones[3][2].Z + bv->Normal.Y * bones[3][2].Y + bv->Normal.X * bones[3][2].X + bones[3][2].W) +
                    bv->PointSize.Z * (bv->BlendIndexes.Z * bones[2][2].Z + bv->BlendIndexes.Y * bones[2][2].Y + bv->BlendIndexes.X * bones[2][2].X + bones[2][2].W) +
                    bv->PointSize.Y * (bv->BlendWeight.Z * bones[1][2].Z + bv->BlendWeight.Y * bones[1][2].Y + bv->BlendWeight.X * bones[1][2].X + bones[1][2].W) +
                    bv->PointSize.X * (bv->XYZ.Z * bones[0][2].Z + bv->XYZ.Y * bones[0][2].Y + bv->XYZ.X * bones[0][2].X + bones[0][2].W);

                sv->Normal.X =
                    bv->PointSize.W * (bv->Diffuse.Z * bones[3][0].Z + bv->Diffuse.X * bones[3][0].X + bv->Diffuse.Y * bones[3][0].Y) +
                    bv->PointSize.Z * (bv->Diffuse.Z * bones[2][0].Z + bv->Diffuse.X * bones[2][0].X + bv->Diffuse.Y * bones[2][0].Y) +
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][0].Z + bv->Diffuse.X * bones[1][0].X + bv->Diffuse.Y * bones[1][0].Y) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][0].Z + bv->Diffuse.X * bones[0][0].X + bv->Diffuse.Y * bones[0][0].Y);

                sv->Normal.Y =
                    bv->PointSize.W * (bv->Diffuse.Z * bones[3][1].Z + bv->Diffuse.Y * bones[3][1].Y + bv->Diffuse.X * bones[3][1].X) +
                    bv->PointSize.Z * (bv->Diffuse.Z * bones[2][1].Z + bv->Diffuse.Y * bones[2][1].Y + bv->Diffuse.X * bones[2][1].X) +
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][1].Z + bv->Diffuse.Y * bones[1][1].Y + bv->Diffuse.X * bones[1][1].X) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][1].Z + bv->Diffuse.Y * bones[0][1].Y + bv->Diffuse.X * bones[0][1].X);
                
                sv->Normal.Z =
                    bv->PointSize.W * (bv->Diffuse.Z * bones[3][2].Z + bv->Diffuse.Y * bones[3][2].Y + bv->Diffuse.X * bones[3][2].X) +
                    bv->PointSize.Z * (bv->Diffuse.Z * bones[2][2].Z + bv->Diffuse.Y * bones[2][2].Y + bv->Diffuse.X * bones[2][2].X) +
                    bv->PointSize.Y * (bv->Diffuse.Z * bones[1][2].Z + bv->Diffuse.Y * bones[1][2].Y + bv->Diffuse.X * bones[1][2].X) +
                    bv->PointSize.X * (bv->Diffuse.Z * bones[0][2].Z + bv->Diffuse.Y * bones[0][2].Y + bv->Diffuse.X * bones[0][2].X);

                break;
            }
            }

            sv->UV.X = bv->UV.X;
            sv->UV.Y = bv->UV.Y;

        }

        if (RendererModule.PolyList != NULL)
        {
            return RendererModule.PolyList(svs, vertexCount, indexes, indexCount, *RendererState.Mode._UnknownMode1);
        }

        // NOTE: Originally an error.
        LogMessage("[WARNING] [RENDERER] No hardware support available. Method \"PolyList\" is absent.\n");

        return FALSE;
    }

    void ReleaseRendererPacket(RendererPacket* packet)
    {
        packet->Indexes = NULL;
        packet->Vertexes.Unknown = NULL;

        if (packet->VertexBuffer != NULL)
        {
            (*RendererModule._FreeVertexBuffer)(packet->VertexBuffer);
            packet->VertexBuffer = NULL;
        }
    }
}