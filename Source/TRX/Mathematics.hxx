#pragma once

#include "Mathematics.Basic.hxx"
#include "Memory.hxx"
#include "Objects.hxx"

namespace Mathematics
{
	struct V3ObjectContainer
	{
		Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x0060ee30;//TODO
	};

	extern V3ObjectContainer V3ObjectState;
}