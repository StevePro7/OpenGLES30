#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include "esUtil.h"
#include "esUtil_win.h"
#include "resource.h"

#ifdef _WIN64
#define GWL_USERDATA GWLP_USERDATA
#endif

LRESULT WINAPI ESWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    LRESULT  lRet = 1;

    switch( uMsg )
    {
    case WM_CREATE:
        break;

    case WM_PAINT:
    {
        ESContext *esContext = (ESContext *)(LONG_PTR)GetWindowLongPtr( hWnd, GWL_USERDATA );
        if( esContext && esContext->drawFunc )
        {
            esContext->drawFunc( esContext );
            eglSwapBuffers( esContext->eglDisplay, esContext->eglSurface );
        }

        ValidateRect( esContext->eglNativeWindow, NULL );
        break;
    }

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_CHAR:
    {
        POINT      point;
        ESContext *esContext = (ESContext *)(LONG_PTR)GetWindowLongPtr( hWnd, GWL_USERDATA );

        GetCursorPos( &point );
        if( esContext && esContext->keyFunc )
        {
            esContext->keyFunc( esContext, (unsigned char)wParam, (int)point.x, (int)point.y );
        }

        break;
    }

    default:
        lRet = DefWindowProc( hWnd, uMsg, wParam, lParam );
        break;
    }

    return lRet;
}

GLboolean WinCreate( ESContext* esContext, const char* title )
{
    WNDCLASS wndclass = { 0 };
    DWORD wStyle = 0;
    RECT windowRect;
    HINSTANCE hInstance = GetModuleHandle( NULL );

    wndclass.style = CS_OWNDC;
    wndclass.lpfnWndProc = (WNDPROC)ESWindowProc;
    wndclass.hInstance = hInstance;

    wndclass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON ) );
    //wndclass.hIconSm = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON_SM ) );

    wndclass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wndclass.lpszClassName = "opengles3.0";

    if( !RegisterClass( &wndclass ) )
    {
        return FALSE;
    }

    wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

    // Adjust the window rectangle so that the client area has the correct number of pixels
    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = esContext->width;
    windowRect.bottom = esContext->height;

    AdjustWindowRect( &windowRect, wStyle, FALSE );

    esContext->eglNativeWindow = CreateWindow(
        "opengles3.0",
        title,
        wStyle,
        0,
        0,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        hInstance,
        NULL );

    // Set the ESContext* to the GWL_USERDATA so that it is available to the ESWindowProc
#ifdef _WIN64
    SetWindowLongPtr( esContext->eglNativeWindow, GWL_USERDATA, (LONGLONG)(LONG_PTR)esContext );
#else
    SetWindowLongPtr( esContext->eglNativeWindow, GWL_USERDATA, (LONG)(LONG_PTR)esContext );
#endif

    if( esContext->eglNativeWindow == NULL )
    {
        return GL_FALSE;
    }

    ShowWindow( esContext->eglNativeWindow, TRUE );
    return GL_TRUE;
}

void WinLoop( ESContext* esContext )
{
    MSG msg = { 0 };
    int done = 0;
    DWORD lastTime = GetTickCount();

    while( !done )
    {
        int gotMsg = (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) != 0);
        DWORD curTime = GetTickCount();
        float deltaTime = (float)(curTime - lastTime) / 1000.0f;
        lastTime = curTime;

        if( gotMsg )
        {
            if( msg.message == WM_QUIT )
            {
                done = 1;
            }
            else
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        else
        {
            SendMessage( esContext->eglNativeWindow, WM_PAINT, 0, 0 );
        }

        // Call update function if registered
        if( esContext->updateFunc != NULL )
        {
            esContext->updateFunc( esContext, deltaTime );
        }
    }
}

extern int esMain( ESContext* esContext );

int main( int argc, char* argv[] )
{
    ESContext esContext;
    memset( &esContext, 0, sizeof( ESContext ) );

    if( esMain( &esContext ) != GL_TRUE )
    {
        return 1;
    }

    WinLoop( &esContext );

    if( NULL != esContext.shutdownFunc )
    {
        esContext.shutdownFunc( &esContext );
    }

    if( NULL != esContext.userData )
    {
        free( esContext.userData );
    }
    return 0;
}