/**
 * This code is under GNU GENERAL PUBLIC LICENSE Version 2, 1991
 * Made by Mateus Cavalcante Teixeira(mateusct13)
 */

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SimplePointer.h>

/**
 * Global:
 */
STATIC EFI_STATUS status;
STATIC UINTN num_protocols = 0;
STATIC EFI_SIMPLE_POINTER_PROTOCOL *mouse;
STATIC EFI_SIMPLE_POINTER_STATE mouse_state;

/**
 * GOP:
 */
STATIC UINT32 *pixel_addr;
STATIC UINT32 width;
STATIC UINT32 height;
STATIC UINTN PixelsPerScanLine;
STATIC UINT32 *fb;
STATIC INTN x;
STATIC INTN y;

STATIC const UINT8 CursorArrow[6][6] = {
    {1, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 0},
    {1, 0, 0, 1, 0, 0},
    {1, 0, 0, 0, 1, 0},
    {1, 1, 1, 1, 1, 1}};

STATIC EFI_STATUS mouse_setup()
{
    VOID **mouse_buffer;

    status = EfiLocateProtocolBuffer(&gEfiSimplePointerProtocolGuid, &num_protocols, &mouse_buffer);
    if (EFI_ERROR(status))
    {
        Print(L"Mouse is not successfully initilialized\n");
        return EFI_DEVICE_ERROR;
    }

    mouse = mouse_buffer[0];
    mouse->Reset(mouse, FALSE);

    return EFI_SUCCESS;
}

STATIC VOID DrawCursor(INTN cx, INTN cy, UINT32 color)
{
    for (INTN row = 0; row < 6; row++)
    {
        for (INTN col = 0; col < 6; col++)
        {
            if (CursorArrow[row][col])
            {
                pixel_addr = fb + ((cy + row) * PixelsPerScanLine + (cx + col));
                *pixel_addr = color;
            }
        }
    }
}

STATIC VOID ClearCursor(INTN cx, INTN cy)
{
    for (INTN row = 0; row < 6; row++)
    {
        for (INTN col = 0; col < 6; col++)
        {
            if (CursorArrow[row][col])
            {
                pixel_addr = fb + ((cy + row) * PixelsPerScanLine + (cx + col));
                *pixel_addr = 0x00000000;
            }
        }
    }
}

STATIC VOID Clear(UINT32 color)
{

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            pixel_addr = fb + (y * PixelsPerScanLine + x);
            *pixel_addr = color;
        }
    }
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *gop_mode;
    INTN cursor_x = 0;
    INTN cursor_y = 0;
    INTN old_x = 0;
    INTN old_y = 0;

    VOID **buffer;

    /**
     * TODO: It will be used, for now
     *  UINTN bytes_per_line;
     */

    status = EfiLocateProtocolBuffer(&gEfiGraphicsOutputProtocolGuid, &num_protocols, &buffer);
    if (EFI_ERROR(status))
    {
        Print(L"Fatal: GOP is not available\n");
        return EFI_NOT_FOUND;
    }

    gop = buffer[0];
    gop_mode = gop->Mode;
    fb = (UINT32 *)(gop_mode->FrameBufferBase);
    width = gop_mode->Info->HorizontalResolution;
    height = gop_mode->Info->VerticalResolution;
    PixelsPerScanLine = gop_mode->Info->PixelsPerScanLine;

    /**
     * Clears the screen initially
     */
    Clear(0x000000000);

    /**
     * Setup the mouse
     */
    status = mouse_setup();
    if (EFI_ERROR(status))
    {
        Print(L"Mouse can't init\n");
        return EFI_DEVICE_ERROR;
    }

    DrawCursor(cursor_x, cursor_y, 0x00FFFFFF);
    old_x = cursor_x;
    old_y = cursor_y;

    for (;;)
    {
       UINTN Index;
       gBS->WaitForEvent(1, mouse->WaitForInput, &Index);
       status = mouse->GetState(mouse, &mouse_state);

       if(status == EFI_NOT_READY)
       {
        continue;
       }
       if(EFI_ERROR(status))
       {
        continue;
       }

       ClearCursor(old_x, old_y);

       cursor_x += mouse_state.RelativeMovementX;
       cursor_y += mouse_state.RelativeMovementY;
       
       /**
        * X
        */
       if(cursor_x > (INTN)width - 6)
       {
        cursor_x = (INTN)width - 6;
       }
       else if(cursor_x < 0)
       {
        cursor_x = 0;
       }
       /**
        * Y
        */
       else if(cursor_y < 0)
       {
        cursor_y = 0;
       }

       if(cursor_y > (INTN)height - 6)
       {
        cursor_y = (INTN)height - 6;
       }

       old_x = cursor_x;
       old_y = cursor_y;

       DrawCursor(cursor_x, cursor_y, 0x00FFFFFF);
    }
}
