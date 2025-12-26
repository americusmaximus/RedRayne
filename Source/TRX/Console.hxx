#pragma once

#include "Basic.hxx"

namespace Console
{
	struct Console
	{
		BOOL IsActive;
		s32 Values[4028];//TODO
	};

	struct ConsoleContainer
	{
		Console* _Console = (Console*)0x0066f0b0;//TODO
		Console** _ConsolePointer = (Console**)0x0061ddc0;//TODO
	};

	extern ConsoleContainer ConsoleState;
}