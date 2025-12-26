#include "UI.hxx"
#include "Input.hxx"

using namespace Memory;
using namespace Input;

namespace UI
{
    ButtonControl* ButtonControlInitialize(ButtonControl* self)
    {
        self->Unk3 = NULL;

        self->Unk8 = 0;

        self->ActiveMode = FALSE;

        self->Unk1 = 0;
        self->Unk7 = 0;
        self->Unk2 = 0;

        self->X = 0;
        self->Y = 0;
        self->Width = 0;
        self->Height = 0;

        return self;
    }

    void* ButtonControlDispose(ButtonControl* self, const DisposeMode mode)
    {
        ButtonControlRelease(self);

        return self;
    }

    void ButtonControlRelease(ButtonControl* self)
    {
        if (self->Unk1 != *UIState._SelectedControl) { return; }

        *UIState._SelectedControl = 0;
    }

    BOOL ButtonControlAcquireActiveMode(ButtonControl* self)
    {
        BOOL active = self->Unk2 == 0
            && self->X <= *InputState.Mouse._X && self->Y <= *InputState.Mouse._Y
            && *InputState.Mouse._X < self->Width && *InputState.Mouse._Y < self->Height;

        if (*UIState._SelectedControl == self->Unk1)
        {
            self->ActiveMode = active;

            if ((*InputState._Unk2 & 1) == 0)
            {
                *UIState._SelectedControl = *UIState._SelectedControl ^ self->Unk1;

                if (active)
                {
                    self->ActiveMode = FALSE;

                    if (self->Unk1 == 1 && self->Unk2 == 0)
                    {
                        self->Unk7 = self->Unk7 == 0;
                    }

                    return TRUE;
                }
            }

            return FALSE;
        }

        if (active && *UIState._SelectedControl == NULL && ((*InputState._Unk2 & 1) != 0))
        {
            self->ActiveMode = TRUE;
            *UIState._SelectedControl = self->Unk1;
        }

        return FALSE;
    }
}