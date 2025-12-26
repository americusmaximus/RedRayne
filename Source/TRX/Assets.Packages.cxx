#include "Assets.Packages.hxx"
#include "Assets.Textures.hxx"
#include "Sounds.hxx"
#include "Strings.hxx"
#include "Logger.hxx"
#include "Assets.Sounds.hxx"

using namespace Logger;
using namespace Strings;
using namespace Sounds;
using namespace Assets::Textures;
using namespace Assets::Sounds;
using namespace IO::Streams;

namespace Assets::Packages
{
    AssetPackageContainer AssetPackageState;

    void RegisterHandler(char ext[MAX_ASSET_PACKAGE_EXTENSION_LENGTH], void* lambda)
    {
        if ((MAX_ASSET_PACKAGE_HANDLER_COUNT - 1) < *AssetPackageState._Count)
        {
            LogError("Unable to register package handler. The maximum amount of registered handlers reached.");
        }

        AssetPackageState._Extensions[*AssetPackageState._Count] = ext;
        AssetPackageState._Handlers[*AssetPackageState._Count] = lambda;

        *AssetPackageState._Count = *AssetPackageState._Count + 1;
    }

    void TextureHandler(void* todo, const char* name, InStreamFile* stream)
    {
        LoadTexture3(name, stream);
    }

    void SoundHandler(void* todo, const char* name, InStreamFile* stream)
    {
        LockSounds();

        SoundSampleDescriptor ssdesc;
        SoundSampleDescriptorInitialize1(&ssdesc);
        SelectSoundSampleDescriptorName(&ssdesc, name);

        for (u32 x = 0; x < MAX_SOUND_SAMPLE_CACHE_COUNT; x++)
        {
            const auto sample = &SoundState.Samples._Cache[x];

            if (EqualStrings(sample->Descriptor.Definition.Name, ssdesc.Definition.Name))
            {
                if (IsNotNullOrEmpty(sample->Descriptor.Definition.Name))
                {
                    UnlockSound1();

                    return;
                }

                break;
            }
        }

        if (!AcquireSoundSampleDescriptor1(&ssdesc))
        {
            LogMessage("[ERROR] [SOUND] %s in package, but not in the sound sample collection.", name);
        }

        auto sample = AcquireSoundSample1();

        if (sample == NULL)
        {
            UnlockSound1();

            return;
        }
        
        CopyMemory(&sample->Descriptor, &ssdesc, sizeof(SoundSampleDescriptor));

        sample->StreamIndex = 0;

        sample->Unk7 = -1;
        sample->Unk5 = 0;
        sample->Unk6 = 0;

        sample->Length = sample->Descriptor.Definition.Length;
        sample->Unk10 = sample->Descriptor.Definition.Length;
        sample->Unk11 = sample->Descriptor.Definition.Length;

        if (AllocateSoundsSample(sample))
        {
            if (stream->Self->Seek(stream, sample->Descriptor.Offset, StreamSeek::Current))
            {
                sample->Descriptor.CacheControl = SoundCacheMode::Static;

                LogMessage("[SOUND] Sound sample %s  cached (%d bytes)\n",
                    sample->Descriptor.Definition.Name, sample->AllocatedMemorySize);

                UnlockSound1();

                return;
            }
        }

        SoundSampleRelease(sample);

        UnlockSound1();
    }
}