#pragma once

#include "Basic.hxx"
#include "Memory.hxx"

namespace Objects
{
	typedef const void* (CDECLAPI* ABSTRACTOBJECTINITIALIZERINITIALIZE) (void* self);
	typedef const void* (CDECLAPI* ABSTRACTOBJECTINITIALIZERASSIGN) (void* self, void* value);
	typedef const void* (CDECLAPI* ABSTRACTOBJECTINITIALIZERDISPOSE) (void* self, const Memory::DisposeMode mode);

	struct AbstractObjectInitializer
	{
		u32 Options;// TODO
		ABSTRACTOBJECTINITIALIZERINITIALIZE Initialize;
		ABSTRACTOBJECTINITIALIZERASSIGN Assign;
		ABSTRACTOBJECTINITIALIZERDISPOSE Dispose;
		u32 Size;
		const char* Name;
	};

	void* InitializeObject(void* self, const u32 count, const AbstractObjectInitializer* initializer);
	void* ReleaseObject1(void* self, const AbstractObjectInitializer* initializer);
	void* ReleaseObject2(void* self, const u32 count, const AbstractObjectInitializer* initializer);
}