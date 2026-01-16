/**
 * This code is under GNU GENERAL PUBLIC LICENSE Version 2, 1991
 * Made by Mateus Cavalcante Teixeira(mateusct13)
 */

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define LOOP() for(;;){__asm__ volatile("hlt");}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    UINTN num_protocols = 0;
    VOID **buffer;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *gop_mode;
    UINT32 *fb;
    UINT32 *pixel_addr;
    UINT32 width; 
    UINT32 height;   
    UINTN x;
    UINTN y;
    UINTN PixelsPerScanLine;
    /**
     * TODO: It will be used, for now
     *  UINTN bytes_per_line;
    */ 

    EFI_STATUS status;
    status = EfiLocateProtocolBuffer(&gEfiGraphicsOutputProtocolGuid, &num_protocols, &buffer);
    if(EFI_ERROR(status))
    {   
        Print(L"Fatal: GOP is not available\n");
        return EFI_NOT_FOUND;
    }

    gop = buffer[0];
    gop_mode = gop->Mode;
    fb = (UINT32*)(gop_mode->FrameBufferBase);
    width = gop_mode->Info->HorizontalResolution;
    height = gop_mode->Info->VerticalResolution;
    PixelsPerScanLine = gop_mode->Info->PixelsPerScanLine;
    

    /**
     * Clears the screen initially
     */
    for(y = 0; y < height; y++)
    { 
        for(x = 0; x < width; x++)
        {
            pixel_addr = fb + (y * PixelsPerScanLine + x);
            *pixel_addr = 0x00000000;
        }
    }
 
    LOOP();
}