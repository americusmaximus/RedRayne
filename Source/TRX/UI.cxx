#include "UI.hxx"
#include "Renderer.hxx"

using namespace Renderer;

namespace UI
{
	UIContainer UIState;

	/// <summary>
	/// Initialize BloodRayne UI Colors.
	/// </summary>
	void InitializeColors1(void* self)
	{
		*UIState.Colors._UnknownColor1 = 0xffc0c0c0;
		*UIState.Colors._UnknownColor2 = 0xffc0c0c0;
		*UIState.Colors._UnknownColor3 = 0xff000000;
		*UIState.Colors._UnknownColor4 = 0xff000000;
		*UIState.Colors._UnknownColor5 = 0xffffff00;
		*UIState.Colors._UnknownColor6 = 0xffffffff;
		*UIState.Colors._UnknownColor7 = 0xffffffff;
		*UIState.Colors._UnknownColor8 = 0xffffffff;
		*UIState.Colors._UnknownColor9 = 0xffffffff;
		*UIState.Colors._UnknownColor10 = 0xffffffff;
		*UIState.Colors._UnknownColor11 = 0xffffffff;
		*UIState.Colors._UnknownColor12 = 0xffffffff;
		*UIState.Colors._UnknownColor13 = 0xffffffff;
		*UIState.Colors._UnknownColor14 = 0xff000080;
		*UIState.Colors._UnknownColor15 = 0xffffffff;
		*UIState.Colors._UnknownColor16 = 0xffffffff;
		*UIState.Colors._UnknownColor17 = 0xffffffff;
		*UIState.Colors._UnknownColor18 = 0xffffffff;
		*UIState.Colors._UnknownColor19 = 0xff000000;
		*UIState.Colors._UnknownColor20 = 0xff000000;
		*UIState.Colors._UnknownColor21 = 0xff000000;
		*UIState.Colors._ControlColor1 = 0xffffffff;
		*UIState.Colors._UnknownColor22 = 0xff000080;
		*UIState.Colors._UnknownColor23 = 0xff00ff00;
		*UIState.Colors._UnknownColor24 = 0xff00ff00;
		*UIState.Colors._UnknownColor25 = 0xffc0c0c0;
		*UIState.Colors._UnknownColor26 = 0xffc0c0c0;
		*UIState.Colors._ControlColor3 = 0xffc0c0c0;
		*UIState.Colors._UnknownColor27 = 0xff000000;
		*UIState.Colors._UnknownColor28 = 0xff808080;
		*UIState.Colors._ControlColor2 = 0xff808080;
		*UIState.Colors._UnknownColor29 = 0xff008000;
	}

	/// <summary>
	/// Initialize Legacy UI Colors.
	/// Exactly same colors are used in 4x4 Evo.
	/// </summary>
	void InitializeColors2(void* self)
	{
		*UIState.Colors._UnknownColor4 = 0xff323c64;
		*UIState.Colors._UnknownColor1 = 0xff3c4678;
		*UIState.Colors._UnknownColor2 = 0xff1e1e46;
		*UIState.Colors._UnknownColor5 = 0xffdcffdc;
		*UIState.Colors._UnknownColor3 = 0xff180f19;
		*UIState.Colors._UnknownColor6 = 0xffcdcdff;
		*UIState.Colors._UnknownColor7 = 0xffffff00;
		*UIState.Colors._UnknownColor9 = 0xffcdcdff;
		*UIState.Colors._UnknownColor8 = 0xffff7d00;
		*UIState.Colors._UnknownColor22 = 0xff000080;
		*UIState.Colors._UnknownColor10 = 0xffff1919;
		*UIState.Colors._UnknownColor12 = 0xffffffff;
		*UIState.Colors._UnknownColor13 = 0xffffffff;
		*UIState.Colors._UnknownColor23 = 0xff00ff00;
		*UIState.Colors._UnknownColor15 = 0xff2d2ddc;
		*UIState.Colors._UnknownColor17 = 0xffffffff;
		*UIState.Colors._UnknownColor18 = 0xffffffff;
		*UIState.Colors._ControlColor3 = 0xffaaa0ac;
		*UIState.Colors._UnknownColor27 = 0xff191996;
		*UIState.Colors._UnknownColor14 = 0xff370080;
		*UIState.Colors._UnknownColor16 = 0xff050519;
		*UIState.Colors._UnknownColor28 = 0xff808080;
		*UIState.Colors._UnknownColor25 = 0xffc0c0c0;
		*UIState.Colors._UnknownColor26 = 0xff5c5cfc;
		*UIState.Colors._UnknownColor29 = 0xff321c78;
		*UIState.Colors._UnknownColor24 = 0xff9605ff;
		*UIState.Colors._UnknownColor19 = 0xff000032;
		*UIState.Colors._UnknownColor20 = 0xff0a0000;
		*UIState.Colors._UnknownColor21 = 0xff0a0000;
		*UIState.Colors._ControlColor2 = 0xff645050;
		*UIState.Colors._ControlColor1 = 0xffbeaac8;
		*UIState.Colors._UnknownColor11 = 0xffffffff;
	}

	void RenderVerticalMargin(const s32 x, const s32 y, const s32 width)
	{
		RenderRectangle(x, y, width, y);
	}

	void RenderHorizontalMargin(const s32 x, const s32 y, const s32 height)
	{
		RenderRectangle(x, y, x, height);
	}

	void RenderControl(const s32 x, const s32 y, const s32 width, const s32 height, const BOOL mode)
	{
		SelectRendererClearColor(mode ? *UIState.Colors._ControlColor2 : *UIState.Colors._ControlColor1);

		RenderVerticalMargin(x, y, width + -1);
		RenderHorizontalMargin(x, y, height + -1);

		SelectRendererClearColor(mode ? *UIState.Colors._ControlColor1 : *UIState.Colors._ControlColor2);

		RenderVerticalMargin(x, height + -1, width + -1);
		RenderHorizontalMargin(width + -1, y, height + -1);

		SelectRendererClearColor(*UIState.Colors._ControlColor3);

		RenderRectangle(x + 1, y + 1, width + -2, height + -2);
	}
}