/*

    KNES Template
    by thefox//aspekt 2010

    This project can be used as a template for new projects.

*/

#include <knes.h>

int main( void )
{
    // Set the background color to blue.
    PPU_ADDR( PPU_BG_PALETTE );
    PPU.data = 1;
    // Point the PPU address to the color so that PPU renders using it.
    PPU_ADDR( PPU_BG_PALETTE );

    // Loop forever.
    for ( ;; );

    return 0;
}
