#include "Sounds.hxx"

using namespace Memory;

namespace Sounds
{
    typedef const void* (CDECLAPI* FUN_00514E50) (void* self);// TODO
    FUN_00514E50 FUN_00514e50 = (FUN_00514E50)0x00514e50;// TODO

    void* SoundDecoderDispose(SoundDecoder* self, const DisposeMode mode)
    {
        FUN_00514e50(self);

        return self;
    }
}