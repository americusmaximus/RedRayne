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

#include "Graphics.Basic.hxx"
#include "Module.hxx"
#include "Quit.hxx"
#include "Renderers.Basic.hxx"
#include "Textures.hxx"

using namespace Renderer::Module;
using namespace Renderer;
using namespace Renderers::Modules;
using namespace Renderers;

#define MAX_PIXEL_FORMAT_COUNT 256

namespace GL
{
    GLFState GLF;

    BOOL IsExtensionSupported(const char* name)
    {
        const char* const extensions = (const char*)glGetString(GL_EXTENSIONS);

        if (strstr(extensions, name) != NULL) { return TRUE; }

        if (GLF.wglGetExtensionsStringARB != NULL)
        {
            const char* const arbs = GLF.wglGetExtensionsStringARB(State.Window.DeviceContext);

            if (strstr(arbs, name) != NULL) { return TRUE; }
        }

        if (GLF.wglGetExtensionsStringEXT != NULL)
        {
            const char* const exts = GLF.wglGetExtensionsStringEXT();

            if (strstr(exts, name)) { return TRUE; }
        }

        return FALSE;
    }

    void InitializeExtensions(void)
    {
        GLF.wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
        GLF.wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

        if (!IsExtensionSupported("GL_ARB_multitexture"))
        {
            Quit("OpenGL extension <GL_ARB_MultiTexture> not supported.");
        }

        GLF.glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
        GLF.glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
        GLF.glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");

        GLF.glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);
        GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
        GLF.glClientActiveTextureARB(GL_TEXTURE0_ARB);

        if (!IsExtensionSupported("WGL_ARB_render_texture"))
        {
            Quit("OpenGL extension <WGL_ARB_Render_Texture> not supported.");
        }

        GLF.wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)wglGetProcAddress("wglBindTexImageARB");
        GLF.wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)wglGetProcAddress("wglReleaseTexImageARB");
        GLF.wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)wglGetProcAddress("wglSetPbufferAttribARB");
        GLF.wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        GLF.wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)wglGetProcAddress("wglCreatePbufferARB");
        GLF.wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)wglGetProcAddress("wglGetPbufferDCARB");
        GLF.wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)wglGetProcAddress("wglQueryPbufferARB");
        GLF.wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)wglGetProcAddress("wglReleasePbufferDCARB");
        GLF.wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)wglGetProcAddress("wglDestroyPbufferARB");
    }

    BOOL InitializeARBTexture(GLTexture* texture, const s32 width, const s32 height)
    {
        s32 pixelFormats[MAX_PIXEL_FORMAT_COUNT];
        u32 pixelFormatCount = 0;

        const s32 cpfAttributes[] =
        {
            WGL_DRAW_TO_PBUFFER_EXT, GL_TRUE,
            WGL_BIND_TO_TEXTURE_RGB_ARB, GL_TRUE,
            WGL_DEPTH_BITS_ARB, GRAPHICS_BITS_PER_PIXEL_24,
            WGL_DOUBLE_BUFFER_ARB, GL_FALSE,
            GL_ZERO, GL_ZERO
        };

        if (!GLF.wglChoosePixelFormatARB(State.Window.DeviceContext,
            cpfAttributes, NULL, ARRAYSIZE(pixelFormats), pixelFormats, &pixelFormatCount)) {
            return FALSE;
        }

        const s32 cbAttributes[] =
        {
            WGL_TEXTURE_FORMAT_ARB, WGL_TEXTURE_RGB_ARB,
            WGL_TEXTURE_TARGET_ARB, WGL_TEXTURE_2D_ARB,
            WGL_MIPMAP_TEXTURE_ARB, GL_FALSE,
            WGL_PBUFFER_LARGEST_EXT, GL_FALSE,
            GL_ZERO, GL_ZERO
        };

        texture->Buffer = GLF.wglCreatePbufferARB(State.Window.DeviceContext, pixelFormats[0], width, height, cbAttributes);

        texture->DeviceContext = GLF.wglGetPbufferDCARB(texture->Buffer);
        texture->RenderContext = wglCreateContext(texture->DeviceContext);

        wglShareLists(wglGetCurrentContext(), texture->RenderContext);

        GLF.wglQueryPbufferARB(texture->Buffer, WGL_PBUFFER_WIDTH_EXT, &texture->Width);
        GLF.wglQueryPbufferARB(texture->Buffer, WGL_PBUFFER_HEIGHT_EXT, &texture->Height);

        return TRUE;
    }

    void Initialize(void)
    {
        ResetTextureCache();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glViewport(0, 0, State.Window.Width, State.Window.Height);
        glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 1600.0);
        glDepthRange(0, 1.0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0039215689f);
        glDisable(GL_FOG);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DITHER);
        glFrontFace(GL_CCW);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);

        InitializeExtensions();

        for (u32 x = 0; x < MAX_RENDER_TEXTURE_COUNT; x++)
        {
            const auto texture = &State.GL.Textures.Render.Textures[x];

            InitializeARBTexture(texture, TEXTURE_DIMENSIONS_SIZE_256, TEXTURE_DIMENSIONS_SIZE_256);

            glGenTextures(1, &texture->ID);
        }

        State.GL.Textures.Render.IsActive = FALSE;

        for (u32 x = 0; x < MAX_SELECTED_TEXTURE_COUNT; x++)
        {
            State.GL.Textures.Selected.Textures[x] = 0;
        }

        State.GL.Textures.Render.Direction = 1.0f;
    }

    void ReleaseTexture(GLTexture* texture)
    {
        if (texture->ID != INVALID_TEXTURE_ID)
        {
            const auto hglrc = wglGetCurrentContext();

            if (hglrc == texture->RenderContext)
            {
                wglMakeCurrent(NULL, NULL);
            }

            wglDeleteContext(texture->RenderContext);

            GLF.wglReleasePbufferDCARB(texture->Buffer, texture->DeviceContext);
            GLF.wglDestroyPbufferARB(texture->Buffer);

            texture->ID = INVALID_TEXTURE_ID;
        }
    }

    void Release(void)
    {
        ResetTextureCache();

        for (u32 x = 0; x < MAX_RENDER_TEXTURE_COUNT; x++)
        {
            ReleaseTexture(&State.GL.Textures.Render.Textures[x]);
        }

        wglMakeCurrent(NULL, NULL);

        if (State.Window.RenderContext != NULL)
        {
            wglDeleteContext(State.Window.RenderContext);
            State.Window.RenderContext = NULL;
        }

        if (State.Window.DeviceContext != NULL)
        {
            ReleaseDC(State.Window.HWND, State.Window.DeviceContext);
            State.Window.DeviceContext = NULL;
        }
    }

    void SetCullMode(const RendererCullMode mode)
    {
        switch (mode)
        {
        case RendererCullMode::CCW:
        {
            glFrontFace(GL_CCW);

            if (State.GL.Textures.Render.IsActive)
            {
                glCullFace(GL_BACK);
            }
            else
            {
                glCullFace(GL_FRONT);
            }

            glEnable(GL_CULL_FACE);

            break;
        }
        case RendererCullMode::CW:
        {
            glFrontFace(GL_CW);

            if (State.GL.Textures.Render.IsActive)
            {
                glCullFace(GL_FRONT);
            }
            else
            {
                glCullFace(GL_BACK);
            }

            glEnable(GL_CULL_FACE);

            break;
        }
        case RendererCullMode::None: { glDisable(GL_CULL_FACE); break; }
        }
    }

    void SelectRendererMode(const RendererMode mode)
    {
        GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);

        if ((mode & RendererMode::U0x20) != RendererMode::None)
        {
            glEnable(GL_BLEND);
            glEnable(GL_ALPHA_TEST);
        }
        else
        {
            glDisable(GL_BLEND);
            glDisable(GL_ALPHA_TEST);
        }

        glBlendFunc(State.GL.Mode.SourceBlendMode, State.GL.Mode.DestinationBlendMode);

        if ((mode & RendererMode::U0x8) != RendererMode::None)
        {
            glEnable(GL_FOG);
        }
        else
        {
            glDisable(GL_FOG);
        }

        if ((mode & RendererMode::U0x80) != RendererMode::None && (mode & RendererMode::U0x40) != RendererMode::None)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }
        else if ((mode & RendererMode::U0x80) != RendererMode::None)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);
        }
        else if ((mode & RendererMode::U0x40) != RendererMode::None)
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_ALWAYS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_ALWAYS);
        }

        if (State.GL.Light.Mode == RendererLightingMode::On)
        {
            glEnable(GL_LIGHTING);
        }
        else
        {
            glDisable(GL_LIGHTING);
        }

        SetCullMode(State.GL.Mode.Cull);

        if (State.GL.Textures.MultiBlendOperation == RendererBlendOperation::None
            || State.GL.Textures.MultiBlendOperation == RendererBlendOperation::AddSigned
            || State.GL.Textures.MultiBlendOperation == RendererBlendOperation::Subtract
            || State.GL.Textures.MultiBlendOperation == RendererBlendOperation::AddSmooth)
        {
            GLF.glClientActiveTextureARB(GL_TEXTURE1_ARB);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
            GLF.glClientActiveTextureARB(GL_TEXTURE0_ARB);
        }

        GLF.glActiveTextureARB(GL_TEXTURE1_ARB);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLF.glActiveTextureARB(GL_TEXTURE1_ARB);
        glDisable(GL_TEXTURE_2D);

        GLF.glActiveTextureARB(GL_TEXTURE2_ARB);
        glDisable(GL_TEXTURE_2D);

        GLF.glActiveTextureARB(GL_TEXTURE3_ARB);
        glDisable(GL_TEXTURE_2D);

        GLF.glActiveTextureARB(GL_TEXTURE0_ARB);
        glEnable(GL_TEXTURE_2D);

        u32 indexes[MAX_BIND_TEXTURE_COUNT] = { 0, U32_MAX };

        switch (State.GL.Textures.MultiBlendOperation)
        {
        case RendererBlendOperation::Disable:
        case RendererBlendOperation::BlendTextureAlpha:
        {
            indexes[0] = TEXTURE_STAGE_1_INDEX;
            indexes[1] = TEXTURE_STAGE_0_INDEX;
            break;
        }
        case RendererBlendOperation::SelectArgument1:
        case RendererBlendOperation::BlendFactorAlpha:
        case RendererBlendOperation::BlendTextureAlphaPreMultiplied:
        case RendererBlendOperation::BlendCurrentAlpha:
        {
            indexes[0] = TEXTURE_STAGE_1_INDEX;
            indexes[1] = TEXTURE_STAGE_4_INDEX;
            break;
        }
        case RendererBlendOperation::SelectArgument2:
        {
            indexes[0] = TEXTURE_STAGE_2_INDEX;
            break;
        }
        case RendererBlendOperation::Modulate:
        {
            indexes[0] = TEXTURE_STAGE_1_INDEX;
            break;
        }
        case RendererBlendOperation::Add:
        {
            indexes[0] = TEXTURE_STAGE_0_INDEX;
            indexes[1] = TEXTURE_STAGE_1_INDEX;
            break;
        }
        case RendererBlendOperation::AddSigned2X:
        case RendererBlendOperation::PreModulate:
        {
            indexes[0] = TEXTURE_STAGE_0_INDEX;
            indexes[1] = TEXTURE_STAGE_3_INDEX;
            break;
        }
        case RendererBlendOperation::BlendDiffuseAlpha:
        {
            indexes[0] = TEXTURE_STAGE_1_INDEX;
            indexes[1] = TEXTURE_STAGE_4_INDEX;
            break;
        }
        }

        u32 textures[MAX_BIND_TEXTURE_COUNT] = { INVALID_TEXTURE_ID, INVALID_TEXTURE_ID };

        for (u32 x = 0; x < MAX_BIND_TEXTURE_COUNT; x++)
        {
            textures[x] = indexes[x] == U32_MAX
                ? TEXTURE_STAGE_0_INDEX : State.GL.Textures.Selected.Textures[indexes[x]];

            if (textures[x] != State.GL.Textures.Bind.Textures[x]
                && (textures[x] != INVALID_TEXTURE_ID))
            {
                BindTexture(x, textures[x]);
            }

            State.GL.Textures.Bind.Textures[x] = textures[x];
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
}