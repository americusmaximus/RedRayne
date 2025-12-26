#include "UI.hxx"

using namespace Memory;

namespace UI
{
    ScrollControl* ScrollControlInitialize(ScrollControl* self)
    {
        self->Unk2 = 1;
        self->Unk1 = 0;
        self->Unk3 = 1;
        self->Unk4 = 1;
        self->Unk5 = 0;

        self->X = 0;
        self->Y = 0;
        self->Width = 0;
        self->Height = 0;

        return self;
    }

    void* ScrollControlDispose(ScrollControl* self, const DisposeMode mode)
    {
        ScrollControlRelease(self);

        return self;
    }

    void ScrollControlRelease(ScrollControl* self)
    {
        if (self->Unk1 != *UIState._SelectedControl) { return; }

        *UIState._SelectedControl = 0;
    }
}