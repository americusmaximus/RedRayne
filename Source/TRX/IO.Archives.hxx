#pragma once

#include "IO.hxx"
#include "Assets.Archives.hxx"
#include "Objects.hxx"

#define MAX_ARCHIVE_VOLUME_COUNT 100

namespace IO::Archives
{
    typedef void* (CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const void(CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEMOUNTDEFAULTS) (void* self);
    typedef const BOOL(CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEISEXISTS) (void* self, IO::FileDescriptor* desc);
    typedef const BOOL(CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEMOUNT) (void* self, const char* name, char* outMessage);
    typedef const void(CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT) (void* self, const char* name);
    typedef const BOOL(CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED) (void* self, char* name);
    typedef const void(CDECLAPI* ARCHIVEFILEMANAGERMANAGERBASEREMOUNT) (void* self);

    void* ArchiveFileManagerBaseDisposeWrapper(void* self, const Memory::DisposeMode mode);
    void ArchiveFileManagerBaseInitializeDefaultsWrapper(void* self);
    BOOL ArchiveFileManagerBaseIsExistsWrapper(void* self, IO::FileDescriptor* desc);
    BOOL ArchiveFileManagerBaseMountWrapper(void* self, const char* name, char* outMessage);
    void ArchiveFileManagerBaseUnMountWrapper(void* self, const char* name);
    BOOL ArchiveFileManagerBaseIsMountedWrapper(void* self, const char* name);
    void ArchiveFileManagerBaseReMountWrapper(void* self);

    struct ArchiveFileManagerBaseSelf
    {
        //ARCHIVEFILEMANAGERMANAGERBASEDISPOSE Dispose = (ARCHIVEFILEMANAGERMANAGERBASEDISPOSE)0x0052a5e0;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEMOUNTDEFAULTS MountDefaults = (ARCHIVEFILEMANAGERMANAGERBASEMOUNTDEFAULTS)0x0052a610; // TODO
        //ARCHIVEFILEMANAGERMANAGERBASEISEXISTS IsExists = (ARCHIVEFILEMANAGERMANAGERBASEISEXISTS)0x0052add0;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEMOUNT Mount = (ARCHIVEFILEMANAGERMANAGERBASEMOUNT)0x0052a640;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT UnMount = (ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT)0x0052a7f0;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED IsMounted = (ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED)0x0052a890;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEREMOUNT ReMount = (ARCHIVEFILEMANAGERMANAGERBASEREMOUNT)0x0052a740;//TODO

        ARCHIVEFILEMANAGERMANAGERBASEDISPOSE Dispose = (ARCHIVEFILEMANAGERMANAGERBASEDISPOSE)&ArchiveFileManagerBaseDisposeWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEMOUNTDEFAULTS MountDefaults = (ARCHIVEFILEMANAGERMANAGERBASEMOUNTDEFAULTS)&ArchiveFileManagerBaseInitializeDefaultsWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEISEXISTS IsExists = (ARCHIVEFILEMANAGERMANAGERBASEISEXISTS)&ArchiveFileManagerBaseIsExistsWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEMOUNT Mount = (ARCHIVEFILEMANAGERMANAGERBASEMOUNT)&ArchiveFileManagerBaseMountWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT UnMount = (ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT)&ArchiveFileManagerBaseUnMountWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED IsMounted = (ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED)&ArchiveFileManagerBaseIsMountedWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEREMOUNT ReMount = (ARCHIVEFILEMANAGERMANAGERBASEREMOUNT)&ArchiveFileManagerBaseReMountWrapper;
    };

    struct ArchiveFileManagerBaseContainer
    {
        ArchiveFileManagerBaseSelf ArchiveFileManagerBaseSelf;
        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x006133c0;//TODO
    };

    extern ArchiveFileManagerBaseContainer ArchiveFileManagerBaseState;

    struct ArchiveFileManagerBase
    {
        u32 Count;
        Assets::Archives::ArchiveVolume* Volumes[MAX_ARCHIVE_VOLUME_COUNT];
        ArchiveFileManagerBaseSelf* Self;
    };

    void* ArchiveFileManagerBaseDispose(ArchiveFileManagerBase* self, const Memory::DisposeMode mode);
    void ArchiveFileManagerBaseInitializeDefaults(ArchiveFileManagerBase* self);
    BOOL ArchiveFileManagerBaseIsExists(ArchiveFileManagerBase* self, IO::FileDescriptor* desc);
    BOOL ArchiveFileManagerBaseMount(ArchiveFileManagerBase* self, const char* name, char* outMessage);
    void ArchiveFileManagerBaseUnMount(ArchiveFileManagerBase* self, const char* name);
    BOOL ArchiveFileManagerBaseIsMounted(ArchiveFileManagerBase* self, const char* name);
    void ArchiveFileManagerBaseReMount(ArchiveFileManagerBase* self);

    void ArchiveFileManagerBaseUnMount(ArchiveFileManagerBase* self, const s32 indx);
    ArchiveFileManagerBase* ArchiveFileManagerBaseInitialize(ArchiveFileManagerBase* self);
    void ArchiveFileManagerBaseRelease(ArchiveFileManagerBase* self);






    typedef const void* (CDECLAPI* ARCHIVEFILEMANAGERDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const void (CDECLAPI* ARCHIVEFILEMANAGERMOUNTDEFAULTS) (void* self);

    void* ArchiveFileManagerDisposeWrapper(void* self, const Memory::DisposeMode mode);
    void ArchiveFileManagerMountDefaultsWrapper(void* self);

    struct ArchiveFileManagerSelf
    {
        //ARCHIVEFILEMANAGERDISPOSE Dispose = (ARCHIVEFILEMANAGERDISPOSE)0x0052c5b0;//TODO
        //ARCHIVEFILEMANAGERMOUNTDEFAULTS MountDefaults = (ARCHIVEFILEMANAGERMOUNTDEFAULTS)0x0052c130; // TODO
        //ARCHIVEFILEMANAGERMANAGERBASEISEXISTS IsExists = (ARCHIVEFILEMANAGERMANAGERBASEISEXISTS)0x0052add0;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEMOUNT Mount = (ARCHIVEFILEMANAGERMANAGERBASEMOUNT)0x0052a640;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEUNKNOWN4 UnMount = (ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT)0x0052a7f0;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED IsMounted = (ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED)0x0052a890;//TODO
        //ARCHIVEFILEMANAGERMANAGERBASEREMOUNT ReMount = (ARCHIVEFILEMANAGERMANAGERBASEREMOUNT)0x0052a740;//TODO

        ARCHIVEFILEMANAGERDISPOSE Dispose = (ARCHIVEFILEMANAGERDISPOSE)&ArchiveFileManagerDisposeWrapper;
        ARCHIVEFILEMANAGERMOUNTDEFAULTS MountDefaults = (ARCHIVEFILEMANAGERMOUNTDEFAULTS)&ArchiveFileManagerMountDefaultsWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEISEXISTS IsExists = (ARCHIVEFILEMANAGERMANAGERBASEISEXISTS)&ArchiveFileManagerBaseIsExistsWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEMOUNT Mount = (ARCHIVEFILEMANAGERMANAGERBASEMOUNT)&ArchiveFileManagerBaseMountWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT UnMount = (ARCHIVEFILEMANAGERMANAGERBASEUNMOUNT)&ArchiveFileManagerBaseUnMountWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED IsMounted = (ARCHIVEFILEMANAGERMANAGERBASEISMOUNTED)&ArchiveFileManagerBaseIsMountedWrapper;
        ARCHIVEFILEMANAGERMANAGERBASEREMOUNT ReMount = (ARCHIVEFILEMANAGERMANAGERBASEREMOUNT)&ArchiveFileManagerBaseReMountWrapper;
    };

    struct ArchiveFileManagerContainer
    {
        ArchiveFileManagerSelf ArchiveFileManagerSelf;
        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)000613430;//TODO
    };

    extern ArchiveFileManagerContainer ArchiveFileManagerState;

    // TODO: Inherited from ArchiveFileManagerBase
    struct ArchiveFileManager
    {
        u32 Count;
        Assets::Archives::ArchiveVolume* Volumes[MAX_ARCHIVE_VOLUME_COUNT];
        ArchiveFileManagerSelf* Self;
    };

    void* ArchiveFileManagerDispose(ArchiveFileManager* self, const Memory::DisposeMode mode);
    void ArchiveFileManagerMountDefaults(ArchiveFileManager* self);

    ArchiveFileManager* ArchiveFileManagerInitialize(ArchiveFileManager* self);
    void ArchiveFileManagerInitialize2(ArchiveFileManager* self);

    struct ArchiveManagerContainer
    {
        ArchiveFileManager** _ArchiveFileManagerPointer = (ArchiveFileManager**)0x00624a70;//TODO

        // TODO NOTE: Initialized by a static constructor.
        ArchiveFileManager* _ArchiveFileManager = (ArchiveFileManager*)0x0075c190;//TODO
    };

    extern ArchiveManagerContainer ArchiveManagerState;
}