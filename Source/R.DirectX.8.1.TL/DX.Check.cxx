/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DX.hxx"
#include "Quit.hxx"

#define D3DERR_CANNOTPROTECTCONTENT MAKE_D3DHRESULT(2173)
#define D3DERR_DEVICEHUNG MAKE_D3DHRESULT(2164)
#define D3DERR_DEVICEREMOVED MAKE_D3DHRESULT(2160)
#define D3DERR_PRESENT_STATISTICS_DISJOINT MAKE_D3DHRESULT(2180)
#define D3DERR_UNSUPPORTEDCRYPTO MAKE_D3DHRESULT(2174)
#define D3DERR_UNSUPPORTEDOVERLAY MAKE_D3DHRESULT(2171)
#define D3DERR_UNSUPPORTEDOVERLAYFORMAT MAKE_D3DHRESULT(2172)
#define D3DERR_WASSTILLDRAWING MAKE_D3DHRESULT(540)
#define D3DOK_NOAUTOGEN 0x876086F
#define S_NOT_RESIDENT 0x8760875
#define S_PRESENT_MODE_CHANGED 0x8760877
#define S_PRESENT_OCCLUDED 0x8760878
#define S_RESIDENT_IN_SHARED_MEMORY 0x8760876

using namespace Renderer::Module;

namespace DX
{
    void DXC(const HRESULT code, const char* message)
    {
        if (code == D3D_OK) { return; }

        const char* description = NULL;

        switch (code)
        {
        case D3DERR_CANNOTPROTECTCONTENT: { description = "The specified content cannot be protected."; break; }
        case D3DERR_CONFLICTINGRENDERSTATE: { description = "The currently set render states cannot be used together."; break; }
        case D3DERR_CONFLICTINGTEXTUREFILTER: { description = "The current texture filters cannot be used together."; break; }
        case D3DERR_CONFLICTINGTEXTUREPALETTE: { description = "The current textures cannot be used simultaneously."; break; }
        case D3DERR_DEVICEHUNG: { description = "The device that returned this code caused the hardware adapter to be reset by the OS. Most applications should destroy the device and quit. Applications that must continue should destroy all video memory objects (surfaces, textures, state blocks etc) and call Reset() to put the device in a default state. If the application then continues rendering in the same way, the device will return to this state."; break; }
        case D3DERR_DEVICELOST: { description = "The device has been lost but cannot be reset at this time.Therefore, rendering is not possible.A Direct3D device object other than the one that returned this code caused the hardware adapter to be reset by the OS.Delete all video memory objects(surfaces, textures, state blocks) and call Reset() to return the device to a default state.If the application continues rendering without a reset, the rendering calls will succeed."; break; }
        case D3DERR_DEVICENOTRESET: { description = "The device has been lost but can be reset at this time."; break; }
        case D3DERR_DEVICEREMOVED: { description = "The hardware adapter has been removed. Application must destroy the device, do enumeration of adapters and create another Direct3D device. If application continues rendering without calling Reset, the rendering calls will succeed."; break; }
        case D3DERR_DRIVERINTERNALERROR: { description = "Internal driver error. Applications should destroy and recreate the device when receiving this error. For hints on debugging this error, see Driver Internal Errors (Direct3D 9)."; break; }
        case D3DERR_DRIVERINVALIDCALL: { description = "Driver invalid call. Not used."; break; }
        case D3DERR_INVALIDCALL: { description = "The method call is invalid. For example, a method's parameter may not be a valid pointer."; break; }
        case D3DERR_INVALIDDEVICE: { description = "The requested device type is not valid."; break; }
        case D3DERR_MOREDATA: { description = "There is more data available than the specified buffer size can hold."; break; }
        case D3DERR_NOTAVAILABLE: { description = "This device does not support the queried technique."; break; }
        case D3DERR_NOTFOUND: { description = "The requested item was not found."; break; }
        case D3DERR_OUTOFVIDEOMEMORY: { description = "Direct3D does not have enough display memory to perform the operation. The device is using more resources in a single scene than can fit simultaneously into video memory. Present, PresentEx, or CheckDeviceState can return this error. Recovery is similar to D3DERR_DEVICEHUNG, though the application may want to reduce its per-frame memory usage as well to avoid having the error recur."; break; }
        case D3DERR_PRESENT_STATISTICS_DISJOINT: { description = "The present statistics have no orderly sequence."; break; }
        case D3DERR_TOOMANYOPERATIONS: { description = "The application is requesting more texture-filtering operations than the device supports."; break; }
        case D3DERR_UNSUPPORTEDALPHAARG: { description = "The device does not support a specified texture-blending argument for the alpha channel."; break; }
        case D3DERR_UNSUPPORTEDALPHAOPERATION: { description = "The device does not support a specified texture-blending operation for the alpha channel."; break; }
        case D3DERR_UNSUPPORTEDCOLORARG: { description = "The device does not support a specified texture-blending argument for color values."; break; }
        case D3DERR_UNSUPPORTEDCOLOROPERATION: { description = "The device does not support a specified texture-blending operation for color values."; break; }
        case D3DERR_UNSUPPORTEDCRYPTO: { description = "The specified cryptographic algorithm is not supported."; break; }
        case D3DERR_UNSUPPORTEDFACTORVALUE: { description = "The device does not support the specified texture factor value. Not used; provided only to support older drivers."; break; }
        case D3DERR_UNSUPPORTEDOVERLAY: { description = "The device does not support overlay for the specified size or display mode."; break; }
        case D3DERR_UNSUPPORTEDOVERLAYFORMAT: { description = "The device does not support overlay for the specified surface format."; break; }
        case D3DERR_UNSUPPORTEDTEXTUREFILTER: { description = "The device does not support the specified texture filter."; break; }
        case D3DERR_WASSTILLDRAWING: { description = "The previous blit operation that is transferring information to or from this surface is incomplete."; break; }
        case D3DERR_WRONGTEXTUREFORMAT: { description = "The pixel format of the texture surface is not valid."; break; }
        case D3DOK_NOAUTOGEN: { description = "This is a success code. However, the autogeneration of mipmaps is not supported for this format. This means that resource creation will succeed but the mipmap levels will not be automatically generated."; break; }
        case E_FAIL: { description = "An undetermined error occurred inside the Direct3D subsystem."; break; }
        case E_INVALIDARG: { description = "An invalid parameter was passed to the returning function."; break; }
        case E_NOINTERFACE: { description = "No object interface is available."; break; }
        case E_NOTIMPL: { description = "Not implemented."; break; }
        case E_OUTOFMEMORY: { description = "Direct3D could not allocate sufficient memory to complete the call."; break; }
        case S_NOT_RESIDENT: { description = "At least one allocation that comprises the resources is on disk."; break; }
        case S_PRESENT_MODE_CHANGED: { description = "The desktop display mode has been changed. The application can continue rendering, but there might be color conversion/stretching. Pick a back buffer format similar to the current display mode, and call Reset to recreate the swap chains. The device will leave this state after a Reset is called."; break; }
        case S_PRESENT_OCCLUDED: { description = "The presentation area is occluded. Occlusion means that the presentation window is minimized or another device entered the fullscreen mode on the same monitor as the presentation window and the presentation window is completely on that monitor. Occlusion will not occur if the client area is covered by another Window. Occluded applications can continue renderingand all calls will succeed, but the occluded presentation window will not be updated.Preferably the application should stop rendering to the presentation window using the deviceand keep calling CheckDeviceState until D3D_OK, S_OK, or S_PRESENT_MODE_CHANGED returns."; break; }
        case S_RESIDENT_IN_SHARED_MEMORY: { description = "No allocations that comprise the resources are on disk. However, at least one allocation is not in GPU-accessible memory."; break; }
        }

        Quit("DirectX: %s %s", message, description ? description : "An unknown error code was returned.");
    }
}