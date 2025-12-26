#pragma once

#include "IO.hxx"
#include "Memory.hxx"
#include "Native.Basic.hxx"
#include "Objects.hxx"


#include <stdio.h>

#define STREAM_RESULT_SUCCESS 0
#define STREAM_RESULT_END_OF_DATA (-1)
#define STREAM_RESULT_FAILURE (-2)

#define MAX_STREAM_ITEM_VALUE 256

// TODO: standardize s32/u32 size/count.
namespace IO::Streams
{
    enum class StreamSeek : u32
    {
        Set = 0,
        Current = 1,
        End = 2
    };

    typedef const void* (CDECLAPI* ABSTRACTINSTREAMDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMREAD) (void* self);
    typedef const BOOL(CDECLAPI* ABSTRACTINSTREAMPUTBACK) (void* self, const s32 value);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMPEEK) (void* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMREADBLOCK) (void* self, void* data, const s32 count);
    typedef const BOOL(CDECLAPI* ABSTRACTINSTREAMSEEK) (void* self);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMTELL) (void* self);
    typedef const BOOL(CDECLAPI* ABSTRACTINSTREAMISAVAILABLE) (void* self);
    typedef const s32(CDECLAPI* ABSTRACTINSTREAMISADVANCEABLE) (void* self);



    void* AbstractInStreamDisposeWrapper(void* self, const Memory::DisposeMode mode);// TODO
    s32 AbstractInStreamNotImplemented(void* self);// TODO
    s32 AbstractInStreamZero(void* self);//TODO
    s32 AbstractInStreamNegativeOne(void* self);//TODO

    struct AbstractInStreamSelf
    {
        /*
        ABSTRACTINSTREAMDISPOSE Dispose = (ABSTRACTINSTREAMDISPOSE)0x00614874;// TODO
        ABSTRACTINSTREAMREAD Read = (ABSTRACTINSTREAMREAD)0x00614878;//TODO
        ABSTRACTINSTREAMSEEK Back = (ABSTRACTINSTREAMPUTBACK)0x0061487c;//TODO
        ABSTRACTINSTREAMPEEK Peek = (ABSTRACTINSTREAMPEEK)0x00614880;//TODO
        ABSTRACTINSTREAMREADBLOCK ReadBlock = (ABSTRACTINSTREAMREADBLOCK)0x00614884;//TODO
        ABSTRACTINSTREAMSEEK Seek = (ABSTRACTINSTREAMSEEK)0x00614888;//TODO
        ABSTRACTINSTREAMTELL Tell = (ABSTRACTINSTREAMTELL)0x0061488c;//TODO
        ABSTRACTINSTREAMISAVAILABLE IsAvailable = (ABSTRACTINSTREAMISAVAILABLE)0x00614890;//TODO
        ABSTRACTINSTREAMISADVANCEABLE IsAdvanceable = (ABSTRACTINSTREAMISADVANCEABLE)0x00614894;//TODO
        */
        ABSTRACTINSTREAMDISPOSE Dispose = (ABSTRACTINSTREAMDISPOSE)&AbstractInStreamDisposeWrapper;
        ABSTRACTINSTREAMREAD Read = (ABSTRACTINSTREAMREAD)&AbstractInStreamNotImplemented;
        ABSTRACTINSTREAMPUTBACK PutBack = (ABSTRACTINSTREAMPUTBACK)&AbstractInStreamZero;
        ABSTRACTINSTREAMPEEK Peek = (ABSTRACTINSTREAMPEEK)&AbstractInStreamNotImplemented;
        ABSTRACTINSTREAMREADBLOCK ReadBlock = (ABSTRACTINSTREAMREADBLOCK)&AbstractInStreamNotImplemented;
        ABSTRACTINSTREAMSEEK Seek = (ABSTRACTINSTREAMSEEK)&AbstractInStreamZero;
        ABSTRACTINSTREAMTELL Tell = (ABSTRACTINSTREAMTELL)&AbstractInStreamNegativeOne;
        ABSTRACTINSTREAMISAVAILABLE IsAvailable = (ABSTRACTINSTREAMISAVAILABLE)&AbstractInStreamNotImplemented;
        ABSTRACTINSTREAMISADVANCEABLE IsAdvanceable = (ABSTRACTINSTREAMISADVANCEABLE)&AbstractInStreamNotImplemented;
    };

    struct AbstractInStreamContainer
    {
        AbstractInStreamSelf AbstractInStreamSelf;
    };

    extern AbstractInStreamContainer AbstractInStreamState;

    // TODO: Make a class
    struct AbstractInStream
    {
        AbstractInStreamSelf* Self;
    };

    AbstractInStream* AbstractInStreamInitialize(AbstractInStream* self);
    void* AbstractInStreamDispose(AbstractInStream* self, const Memory::DisposeMode mode);

    typedef const void* (CDECLAPI* INSTREAMMEMORYDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const s32(CDECLAPI* INSTREAMMEMORYREAD) (void* self);
    typedef const BOOL(CDECLAPI* INSTREAMMEMORYPUTBACK) (void* self, const s32 value);
    typedef const s32(CDECLAPI* INSTREAMMEMORYPEEK) (void* self);
    typedef const s32(CDECLAPI* INSTREAMMEMORYREADBLOCK) (void* self, void* data, const u32 count);
    typedef const BOOL(CDECLAPI* INSTREAMMEMORYSEEK) (void* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* INSTREAMMEMORYTELL) (void* self);
    typedef const BOOL(CDECLAPI* INSTREAMMEMORYISAVAILABLE) (void* self);
    typedef const s32(CDECLAPI* INSTREAMMEMORYISADVANCEABLE) (void* self);
    typedef const void (CDECLAPI* INSTREAMMEMORYRELEASE) (void* self);
    typedef const void (CDECLAPI* INSTREAMMEMORYSET) (void* self, void* data, const u32 size);
    typedef const void (CDECLAPI* INSTREAMMEMORYOWN) (void* self, void* data, const u32 size);


    void* InStreamMemoryDisposeWrapper(void* self, const Memory::DisposeMode mode);
    s32 InStreamMemoryReadWrapper(void* self);
    s32 InStreamMemoryBackWrapper(void* self, const s32 value);
    s32 InStreamMemoryPeekWrapper(void* self);
    s32 InStreamMemoryReadBlockWrapper(void* self, void* data, const u32 count);
    BOOL InStreamMemorySeekWrapper(void* self, const s32 offset, const StreamSeek mode);
    s32 InStreamMemoryTellWrapper(void* self);
    BOOL InStreamMemoryIsAvailableWrapper(void* self);
    s32 InStreamMemoryIsAdvanceableWrapper(void* self);
    void InStreamMemoryReleaseWrapper(void* self);
    void InStreamMemorySetWrapper(void* self, void* data, const u32 size);
    void InStreamMemoryOwnWrapper(void* self, void* data, const u32 size);

    struct InStreamMemorySelf
    {
        //INSTREAMMEMORYDISPOSE Dispose = (INSTREAMMEMORYDISPOSE)0x006148e4;// TODO
        //INSTREAMMEMORYREAD Read = (INSTREAMMEMORYREAD)0x006148e8;//TODO
        //ABSTRACTINSTREAMBACK Back = (ABSTRACTINSTREAMPUTBACK)0x006148ec;//TODO
        //INSTREAMMEMORYPEEK Peek = (INSTREAMMEMORYPEEK)0x006148f0;//TODO
        //INSTREAMMEMORYREADBLOCK ReadBlock = (INSTREAMMEMORYREADBLOCK)0x006148f4;//TODO
        //INSTREAMMEMORYSEEK Seek = (INSTREAMMEMORYSEEK)0x006148f8;//TODO
        //INSTREAMMEMORYTELL Tell = (INSTREAMMEMORYTELL)0x006148fc;//TODO
        //INSTREAMMEMORYISAVAILABLE IsAvailable = (INSTREAMMEMORYISAVAILABLE)0x00614900;//TODO
        //INSTREAMMEMORYISADVANCEABLE IsAdvanceable = (INSTREAMMEMORYISADVANCEABLE)0x00614904;//TODO
        //INSTREAMMEMORYRELEASE Release = (INSTREAMMEMORYRELEASE)0x00614908;//TODO
        //INSTREAMMEMORYSET Set = (INSTREAMMEMORYSET)0x0061490c;//TODO
        //INSTREAMMEMORYOWN Own = (INSTREAMMEMORYOWN)0x00614910;//TODO

        INSTREAMMEMORYDISPOSE Dispose = (INSTREAMMEMORYDISPOSE)&InStreamMemoryDisposeWrapper;
        INSTREAMMEMORYREAD Read = (INSTREAMMEMORYREAD)&InStreamMemoryReadWrapper;
        INSTREAMMEMORYPUTBACK PutBack = (INSTREAMMEMORYPUTBACK)&InStreamMemoryBackWrapper;
        INSTREAMMEMORYPEEK Peek = (INSTREAMMEMORYPEEK)&InStreamMemoryPeekWrapper;
        INSTREAMMEMORYREADBLOCK ReadBlock = (INSTREAMMEMORYREADBLOCK)&InStreamMemoryReadBlockWrapper;
        INSTREAMMEMORYSEEK Seek = (INSTREAMMEMORYSEEK)&InStreamMemorySeekWrapper;
        INSTREAMMEMORYTELL Tell = (INSTREAMMEMORYTELL)&InStreamMemoryTellWrapper;
        INSTREAMMEMORYISAVAILABLE IsAvailable = (INSTREAMMEMORYISAVAILABLE)&InStreamMemoryIsAvailableWrapper;
        INSTREAMMEMORYISADVANCEABLE IsAdvanceable = (INSTREAMMEMORYISADVANCEABLE)&InStreamMemoryIsAdvanceableWrapper;
        INSTREAMMEMORYRELEASE Release = (INSTREAMMEMORYRELEASE)&InStreamMemoryReleaseWrapper;
        INSTREAMMEMORYSET Set = (INSTREAMMEMORYSET)&InStreamMemorySetWrapper;
        INSTREAMMEMORYOWN Own = (INSTREAMMEMORYOWN)&InStreamMemoryOwnWrapper;
    };

    struct InStreamMemoryContainer
    {
        InStreamMemorySelf InStreamMemorySelf;

        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x0060f540;//TODO
    };

    extern InStreamMemoryContainer InStreamMemoryState;

    // TODO: Make a class
    struct InStreamMemory
    {
        InStreamMemorySelf* Self;
        u8* Data;
        u32 Size;//TODO: Data type
        u32 Offset;//TODO: Data type
        BOOL IsOwned; // TODO: Enum, name
    };

    InStreamMemory* InStreamMemoryAssign(InStreamMemory* self, InStreamMemory* value);
    InStreamMemory* InStreamMemoryInitialize(InStreamMemory* self);
    void InStreamMemoryInitialize2(InStreamMemory* self, void* data, const u32 count);

    void* InStreamMemoryDispose(InStreamMemory* self, const Memory::DisposeMode mode);
    s32 InStreamMemoryRead(InStreamMemory* self);
    BOOL InStreamMemoryPutBack(InStreamMemory* self, const s32 value);
    s32 InStreamMemoryPeek(InStreamMemory* self);
    s32 InStreamMemoryReadBlock(InStreamMemory* self, void* data, const u32 count);
    s32 InStreamMemorySeek(InStreamMemory* self, const s32 offset, const StreamSeek mode);
    s32 InStreamMemoryTell(InStreamMemory* self);
    BOOL InStreamMemoryIsAvailable(InStreamMemory* self);
    s32 InStreamMemoryIsAdvanceable(InStreamMemory* self);
    void InStreamMemoryRelease(InStreamMemory* self);
    void InStreamMemorySet(InStreamMemory* self, void* data, const u32 size);
    void InStreamMemoryOwn(InStreamMemory* self, void* data, const u32 size);

    typedef const void* (CDECLAPI* INSTREAMFILEDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const s32(CDECLAPI* INSTREAMFILEREAD) (void* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILEPUTBACK) (void* self, const s32 value);
    typedef const s32(CDECLAPI* INSTREAMFILEPEEK) (void* self);
    typedef const s32(CDECLAPI* INSTREAMFILEREADBLOCK) (void* self, void* data, const s32 count);
    typedef const BOOL(CDECLAPI* INSTREAMFILESEEK) (void* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* INSTREAMFILETELL) (void* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILEISAVAILABLE) (void* self);
    typedef const s32(CDECLAPI* INSTREAMFILEISADVANCEABLE) (void* self);
    typedef const s32(CDECLAPI* INSTREAMFILERELEASE) (void* self);
    typedef const BOOL(CDECLAPI* INSTREAMFILESET) (void* self, const char* file, const BOOL mode);
    typedef const BOOL(CDECLAPI* INSTREAMFILEOWN) (void* self, const char* dir, const char* file, const BOOL mode);
    typedef const BOOL(CDECLAPI* INSTREAMFILEOPENFILE) (void* self, FileDescriptor* desc, const BOOL mode);

    void* InStreamFileDisposeWrapper(void* self, const Memory::DisposeMode mode);
    s32 InStreamFileReadWrapper(void* self);
    s32 InStreamFileBackWrapper(void* self, const s32 value);
    s32 InStreamFilePeekWrapper(void* self);
    s32 InStreamFileReadBlockWrapper(void* self, void* data, const s32 count);
    BOOL InStreamFileSeekWrapper(void* self, const s32 offset, const StreamSeek mode);
    s32 InStreamFileTellWrapper(void* self);
    BOOL InStreamFileIsAvailableWrapper(void* self);
    s32 InStreamFileIsAdvanceableWrapper(void* self);
    s32 InStreamFileReleaseWrapper(void* self);
    BOOL InStreamFileSetWrapper(void* self, const char* file, const BOOL mode);
    BOOL InStreamFileOwnWrapper(void* self, const char* dir, const char* file, const BOOL mode);
    BOOL InStreamFileOpenFileWrapper(void* self, FileDescriptor* desc, const BOOL mode);

    struct InStreamFileSelf
    {
        //INSTREAMFILEDISPOSE Dispose = (INSTREAMFILEDISPOSE)0x005ade50;// TODO
        //INSTREAMFILEREAD Read = (INSTREAMFILEREAD)0x005adeb0;// TODO
        //INSTREAMFILEBACK Back = (INSTREAMFILEPUTBACK)0x005adf10;// TODO
        //INSTREAMFILEPEEK Peek = (INSTREAMFILEPEEK)0x005adf90;// TODO
        //INSTREAMFILEREADBLOCK ReadBlock = (INSTREAMFILEREADBLOCK)0x005ae050;// TODO
        //INSTREAMFILESEEK Seek = (INSTREAMFILESEEK)0x005ae170;// TODO
        //INSTREAMFILETELL Tell = (INSTREAMFILETELL)0x005ae1c0;// TODO
        //INSTREAMFILEISAVAILABLE IsAvailable = (INSTREAMFILEISAVAILABLE)0x005ae1e0;// TODO
        //INSTREAMFILEISADVANCEABLE IsAdvanceable = (INSTREAMFILEISADVANCEABLE)0x005ae200;// TODO
        //INSTREAMFILERELEASE Release = (INSTREAMFILERELEASE)0x005ae230;// TODO
        //INSTREAMFILESET Set = (INSTREAMFILESET)0x005ae4c0;// TODO
        //INSTREAMFILEOWN Own = (INSTREAMFILEOWN)0x005ae4e0;// TODO
        //INSTREAMFILEOPENFILE OpenFile = (INSTREAMFILEOPENFILE)0x005ae560;// TODO

        INSTREAMFILEDISPOSE Dispose = (INSTREAMFILEDISPOSE)&InStreamFileDisposeWrapper;
        INSTREAMFILEREAD Read = (INSTREAMFILEREAD)&InStreamFileReadWrapper;
        INSTREAMFILEPUTBACK PutBack = (INSTREAMFILEPUTBACK)&InStreamFileBackWrapper;
        INSTREAMFILEPEEK Peek = (INSTREAMFILEPEEK)&InStreamFilePeekWrapper;
        INSTREAMFILEREADBLOCK ReadBlock = (INSTREAMFILEREADBLOCK)&InStreamFileReadBlockWrapper;
        INSTREAMFILESEEK Seek = (INSTREAMFILESEEK)&InStreamFileSeekWrapper;
        INSTREAMFILETELL Tell = (INSTREAMFILETELL)&InStreamFileTellWrapper;
        INSTREAMFILEISAVAILABLE IsAvailable = (INSTREAMFILEISAVAILABLE)&InStreamFileIsAvailableWrapper;
        INSTREAMFILEISADVANCEABLE IsAdvanceable = (INSTREAMFILEISADVANCEABLE)&InStreamFileIsAdvanceableWrapper;
        INSTREAMFILERELEASE Release = (INSTREAMFILERELEASE)&InStreamFileReleaseWrapper;
        INSTREAMFILESET Set = (INSTREAMFILESET)&InStreamFileSetWrapper;
        INSTREAMFILEOWN Own = (INSTREAMFILEOWN)&InStreamFileOwnWrapper;
        INSTREAMFILEOPENFILE OpenFile = (INSTREAMFILEOPENFILE)&InStreamFileOpenFileWrapper;
    };

    struct InStreamFileContainer
    {
        InStreamFileSelf InStreamFileSelf;

        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x0060ed10;//TODO
    };

    extern InStreamFileContainer InStreamFileState;

    // TODO: Make a class
    struct InStreamFile
    {
        InStreamFileSelf* Self;
        HANDLE FileHandle;
        char FileName[STANDARD_IO_FILE_NAME_LENGTH];
        u32 FileSize; // TODO: Type
        u32 FileOffset; // TODO: Type
        u32 BufferOffset; // TODO: Type
        u8* Buffer;
        u8* AlignedBuffer;
        u32 Version; //TODO
        s32 BufferSize;// TODO
        s32 BufferSize1;// TODO
        s32 Unknown2;// TODO
    };

    void* InStreamFileDispose(InStreamFile* self, const Memory::DisposeMode mode);
    s32 InStreamFileRead(InStreamFile* self);
    BOOL InStreamFilePutBack(InStreamFile* self, const s32 value);
    s32 InStreamFilePeek(InStreamFile* self);
    s32 InStreamFileReadBlock(InStreamFile* self, void* data, const s32 count);
    s32 InStreamFileSeek(InStreamFile* self, const s32 offset, const StreamSeek mode);
    s32 InStreamFileTell(InStreamFile* self);
    BOOL InStreamFileIsAvailable(InStreamFile* self);
    s32 InStreamFileIsAdvanceable(InStreamFile* self);
    s32 InStreamFileRelease(InStreamFile* self);// TODO: no need to return value?
    BOOL InStreamFileSet(InStreamFile* self, const char* file, const BOOL mode);
    BOOL InStreamFileOwn(InStreamFile* self, const char* dir, const char* file, const BOOL mode);

    void InStreamFileRelease1(InStreamFile* self);
    BOOL InStreamFileOpenFile(InStreamFile* self, FileDescriptor* desc, const BOOL mode);
    void InStreamFileAllocateBuffer(InStreamFile* self);
    BOOL InStreamFileReadFile1(InStreamFile* self, const s32 count);
    s32 InStreamFileReadFile1(InStreamFile* self, const s32 count);
    s32 InStreamFileReadFile2(InStreamFile* self, const s32 count);

    InStreamFile* InStreamFileInitialize(InStreamFile* self);
    void InStreamFileInitialize2(InStreamFile* self, const char* file, const BOOL mode);
    InStreamFile* InStreamFileAssign(InStreamFile* self, InStreamFile* value);

    s32 AbstractStreamSkipLine(AbstractInStream* self, const u32 count);
    s32 InStreamFileReadString(InStreamFile* self, char* buffer, const u32 size);

    s32 AbstractInStreamScan(AbstractInStream* self, const char* format, ...);

    s32 AbstractInStreamIsSpace(AbstractInStream* self);









    typedef const void* (CDECLAPI* ABSTRACTOUTSTREAMDISPOSE) (void* self);
    typedef const s32(CDECLAPI* ABSTRACTOUTSTREAMWRITE) (void* self, const u8 value);
    typedef const BOOL(CDECLAPI* ABSTRACTOUTSTREAMWRITEBLOCK) (void* self, const void* data, const s32 count);
    typedef const BOOL(CDECLAPI* ABSTRACTOUTSTREAMSEEK) (void* self);
    typedef const s32(CDECLAPI* ABSTRACTOUTSTREAMTELL) (void* self);


    void* AbstractOutStreamDisposeWrapper(void* self, const Memory::DisposeMode mode);// TODO
    s32 AbstractOutStreamNotImplemented(void* self);// TODO
    s32 AbstractOutStreamZero(void* self);//TODO
    s32 AbstractOutStreamNegativeOne(void* self);//TODO

    struct AbstractOutStreamSelf
    {
        ABSTRACTOUTSTREAMDISPOSE Dispose = (ABSTRACTOUTSTREAMDISPOSE)&AbstractOutStreamDisposeWrapper;
        ABSTRACTOUTSTREAMWRITE Write = (ABSTRACTOUTSTREAMWRITE)&AbstractOutStreamNotImplemented;
        ABSTRACTOUTSTREAMWRITEBLOCK WriteBlock = (ABSTRACTOUTSTREAMWRITEBLOCK)&AbstractOutStreamNotImplemented;
        ABSTRACTOUTSTREAMSEEK Seek = (ABSTRACTOUTSTREAMSEEK)&AbstractOutStreamZero;
        ABSTRACTOUTSTREAMTELL Tell = (ABSTRACTOUTSTREAMTELL)&AbstractOutStreamNegativeOne;
    };

    struct AbstractOutStreamContainer
    {
        AbstractOutStreamSelf AbstractOutStreamSelf;
    };

    extern AbstractOutStreamContainer AbstractOutStreamState;

    // TODO: Make a class
    struct AbstractOutStream
    {
        AbstractOutStreamSelf* Self;
    };

    AbstractOutStream* AbstractOutStreamConstructor(AbstractOutStream* self);
    void* AbstractOutStreamDispose(AbstractOutStream* self, const Memory::DisposeMode mode);

    typedef const void* (CDECLAPI* OUTSTREAMMEMORYDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const s32(CDECLAPI* OUTSTREAMMEMORYWRITE) (void* self, const u8 value);
    typedef const s32(CDECLAPI* OUTSTREAMMEMORYWRITEBLOCK) (void* self, const void* data, const u32 size);
    typedef const BOOL(CDECLAPI* OUTSTREAMMEMORYSEEK) (void* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* OUTSTREAMMEMORYTELL) (void* self);
    typedef const s32(CDECLAPI* OUTSTREAMMEMORYRELEASE) (void* self);
    typedef const s32(CDECLAPI* OUTSTREAMMEMORYSET) (void* self, void* data, const u32 size);


    void* OutStreamMemoryDisposeWrapper(void* self, const Memory::DisposeMode mode);
    s32 OutStreamMemoryWriteWrapper(void* self, const u8 value);
    s32 OutStreamMemoryWriteBlockWrapper(void* self, const void* data, const u32 size);
    BOOL OutStreamMemorySeekWrapper(void* self, const s32 offset, const StreamSeek mode);
    s32 OutStreamMemoryTellWrapper(void* self);
    void OutStreamMemoryReleaseWrapper(void* self);
    void OutStreamMemorySetWrapper(void* self, void* data, const u32 size);

    struct OutStreamMemorySelf
    {
        //OUTSTREAMMEMORYDISPOSE Dispose = (OUTSTREAMMEMORYDISPOSE)0x005af340;// TODO
        //OUTSTREAMMEMORYWRITE Write = (OUTSTREAMMEMORYWRITE)0x005af3a0;//TODO
        //OUTSTREAMMEMORYWRITEBLOCK WriteBlock = (OUTSTREAMMEMORYWRITEBLOCK)0x005af410;//TODO
        //OUTSTREAMMEMORYSEEK Seek = (OUTSTREAMMEMORYSEEK)0x005af4a0;//TODO
        //OUTSTREAMMEMORYTELL Tell = (OUTSTREAMMEMORYTELL)0x005af4f0;//TODO
        //OUTSTREAMMEMORYRELEASE Release = (OUTSTREAMMEMORYRELEASE)0x005af500;//TODO
        //OUTSTREAMMEMORYSET Set = (OUTSTREAMMEMORYSET)0x005af540;//TODO

        OUTSTREAMMEMORYDISPOSE Dispose = (OUTSTREAMMEMORYDISPOSE)&OutStreamMemoryDisposeWrapper;
        OUTSTREAMMEMORYWRITE Write = (OUTSTREAMMEMORYWRITE)&OutStreamMemoryWriteWrapper;
        OUTSTREAMMEMORYWRITEBLOCK WriteBlock = (OUTSTREAMMEMORYWRITEBLOCK)&OutStreamMemoryWriteBlockWrapper;
        OUTSTREAMMEMORYSEEK Seek = (OUTSTREAMMEMORYSEEK)&OutStreamMemorySeekWrapper;
        OUTSTREAMMEMORYTELL Tell = (OUTSTREAMMEMORYTELL)&OutStreamMemoryTellWrapper;
        OUTSTREAMMEMORYRELEASE Release = (OUTSTREAMMEMORYRELEASE)OutStreamMemoryReleaseWrapper;
        OUTSTREAMMEMORYSET Set = (OUTSTREAMMEMORYSET)&OutStreamMemorySetWrapper;
    };

    struct OutStreamMemoryContainer
    {
        OutStreamMemorySelf OutStreamMemorySelf;
        
        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x0060fda0;//TODO

    };

    extern OutStreamMemoryContainer OutStreamMemoryState;

    // TODO: Make a class
    struct OutStreamMemory
    {
        OutStreamMemorySelf* Self;
        u8* Data;
        s32 Size;//TODO: Data type
        s32 Offset;//TODO: Data type
        s32 Unk3;// TODO
        BOOL IsEmpty; // TODO
    };

    void* OutStreamMemoryDispose(OutStreamMemory* self, const Memory::DisposeMode mode);
    s32 OutStreamMemoryWrite(OutStreamMemory* self, const u8 value);
    s32 OutStreamMemoryWriteBlock(OutStreamMemory* self, const void* data, const u32 size);
    BOOL OutStreamMemorySeek(OutStreamMemory* self, const s32 offset, const StreamSeek mode);
    s32 OutStreamMemoryTell(OutStreamMemory* self);
    void OutStreamMemoryRelease(OutStreamMemory* self);
    void OutStreamMemorySet(OutStreamMemory* self, void* data, const u32 size);

    OutStreamMemory* OutStreamMemoryInitialize(OutStreamMemory* self);
    void OutStreamMemoryInitialize2(OutStreamMemory* self, void* data, const u32 size);
    OutStreamMemory* OutStreamMemoryAssign(OutStreamMemory* self, OutStreamMemory* value);


    typedef const void (CDECLAPI* OUTSTREAMFILEDISPOSE) (void* self, const Memory::DisposeMode mode);
    typedef const s32(CDECLAPI* OUTSTREAMFILEWRITE) (void* self, const u8 value);
    typedef const s32(CDECLAPI* OUTSTREAMFILEWRITEBLOCK) (void* self, const void* data, const u32 size);
    typedef const BOOL(CDECLAPI* OUTSTREAMFILESEEK) (void* self, const s32 offset, const StreamSeek mode);
    typedef const s32(CDECLAPI* OUTSTREAMFILETELL) (void* self);
    typedef const BOOL(CDECLAPI* OUTSTREAMFILEISAVAILABLE) (void* self);
    typedef const BOOL(CDECLAPI* OUTSTREAMFILEISADVANCEABLE) (void* self);
    typedef const BOOL(CDECLAPI* OUTSTREAMFILERELEASE) (void* self);
    typedef const BOOL(CDECLAPI* OUTSTREAMFILEOPENFILE1) (void* self, const char* file, const char* attrs);
    typedef const BOOL(CDECLAPI* OUTSTREAMFILEOPENFILE2) (void* self);

    void* OutStreamFileDisposeWrapper(void* self, const Memory::DisposeMode mode);
    s32 OutStreamFileWriteWrapper(void* self, const u8 value);
    s32 OutStreamFileWriteBlockWrapper(void* self, const void* data, const u32 size);
    BOOL OutStreamFileSeekWrapper(void* self, const s32 offset, const StreamSeek mode);
    s32 OutStreamFileTellWrapper(void* self);
    BOOL OutStreamFileIsAvailableWrapper(void* self);
    BOOL OutStreamFileIsAdvanceableWrapper(void* self);
    BOOL OutStreamFileReleaseWrapper(void* self);
    BOOL OutStreamFileOpenFile1Wrapper(void* self, const char* file, const char* attrs);
    BOOL OutStreamFileOpenFile2Wrapper(void* self, const char* dir, const char* file, const char* attrs);

    struct OutStreamFileSelf
    {
        //OUTSTREAMFILEDISPOSE Dispose = (OUTSTREAMFILEDISPOSE)0x005aede0;// TODO
        //OUTSTREAMFILEWRITE Write = (OUTSTREAMFILEWRITE)0x005aee40;//TODO
        //OUTSTREAMFILEWRITEBLOCK WriteBlock = (OUTSTREAMFILEWRITEBLOCK)0x005aee90;//TODO
        //OUTSTREAMFILESEEK Seek = (OUTSTREAMFILESEEK)0x005aef00;//TODO
        //OUTSTREAMFILETELL Tell = (OUTSTREAMFILETELL)0x005aef40;//TODO
        //OUTSTREAMFILEISAVAILABLE IsAvailable = (OUTSTREAMFILEISAVAILABLE)0x005aef70;//TODO
        //OUTSTREAMFILEISADVANCEABLE IsAdvanceable = (OUTSTREAMFILEISADVANCEABLE)0x005aef90;//TODO
        //OUTSTREAMFILERELEASE Release = (OUTSTREAMFILERELEASE)0x005aefc0;//TODO
        //OUTSTREAMFILEOPENFILE1 OpenFile1 = (OUTSTREAMFILEOPENFILE1)0x005af050;//TODO
        //OUTSTREAMFILEOPENFILE2 OpenFile2 = (OUTSTREAMFILEOPENFILE2)0x005af1b0;//TODO

        OUTSTREAMFILEDISPOSE Dispose = (OUTSTREAMFILEDISPOSE)&OutStreamFileDisposeWrapper;
        OUTSTREAMFILEWRITE Write = (OUTSTREAMFILEWRITE)&OutStreamFileWriteWrapper;
        OUTSTREAMFILEWRITEBLOCK WriteBlock = (OUTSTREAMFILEWRITEBLOCK)&OutStreamFileWriteBlockWrapper;
        OUTSTREAMFILESEEK Seek = (OUTSTREAMFILESEEK)&OutStreamFileSeekWrapper;
        OUTSTREAMFILETELL Tell = (OUTSTREAMFILETELL)&OutStreamFileTellWrapper;
        OUTSTREAMFILEISAVAILABLE IsAvailable = (OUTSTREAMFILEISAVAILABLE)&OutStreamFileIsAvailableWrapper;
        OUTSTREAMFILEISADVANCEABLE IsAdvanceable = (OUTSTREAMFILEISADVANCEABLE)&OutStreamFileIsAdvanceableWrapper;
        OUTSTREAMFILERELEASE Release = (OUTSTREAMFILERELEASE)&OutStreamFileReleaseWrapper;
        OUTSTREAMFILEOPENFILE1 OpenFile1 = (OUTSTREAMFILEOPENFILE1)&OutStreamFileOpenFile1Wrapper;
        OUTSTREAMFILEOPENFILE2 OpenFile2 = (OUTSTREAMFILEOPENFILE2)&OutStreamFileOpenFile2Wrapper;
    };

    struct OutStreamFileContainer
    {
        OutStreamFileSelf OutStreamFileSelf;

        Objects::AbstractObjectInitializer* _Initializer = (Objects::AbstractObjectInitializer*)0x0060ed30;//TODO

    };

    extern OutStreamFileContainer OutStreamFileState;

    struct OutStreamFile
    {
        OutStreamFileSelf* Self;
        BOOL BackUp;
        FILE* Handle;// TODO: Use winapi
        char Name[STANDARD_IO_FILE_NAME_LENGTH];
    };

    void* OutStreamFileDispose(OutStreamFile* self, const Memory::DisposeMode mode);
    s32 OutStreamFileWrite(OutStreamFile* self, const u8 value);
    s32 OutStreamFileWriteBlock(OutStreamFile* self, const void* data, const u32 size);
    BOOL OutStreamFileSeek(OutStreamFile* self, const s32 offset, const StreamSeek mode);
    s32 OutStreamFileTell(OutStreamFile* self);
    BOOL OutStreamFileIsAvailable(OutStreamFile* self);
    BOOL OutStreamFileIsAdvanceable(OutStreamFile* self);
    BOOL OutStreamFileRelease(OutStreamFile* self);
    BOOL OutStreamFileOpenFile1(OutStreamFile* OutStreamFile, const char* file, const char* attrs);
    BOOL OutStreamFileOpenFile2(OutStreamFile* self, const char* dir, const char* file, const char* attrs);

    OutStreamFile* OutStreamFileInitialize(OutStreamFile* self);
    void OutStreamFileInitialize2(OutStreamFile* self, const char* file, const char* attrs);
    OutStreamFile* OutStreamFileAssign(OutStreamFile* self, OutStreamFile* value);

    s32 AbstractOutStreamFormat(AbstractOutStream* self, const char* format, ...);
}