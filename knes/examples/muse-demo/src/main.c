/*

    Minimal example of using the MUSE sound library with KNES.
    by thefox//aspekt 2011

*/

#include <knes.h>
#include "muse-wrap.h"
#include "snd-data-wrap.h"

int main( void )
{
    // Initialize MUSE.
    MUSE_init( &snd_data );
    
    // Load the song.
    MUSE_startMusic( 0 );
    
    // MUSE_init sets the MUSE_PAUSE flag by default, so clear that now.
    // I'm also setting the MUSE_NTSC_MODE flag here for demonstration purposes.
    // In a real program you should detect the system and set the flag based on that.
    MUSE_setFlags( MUSE_NTSC_MODE );

    // Enable NMI so that wait_vblank works.
    PPU.ctrl = NMI_ON;
    
    for ( ;; )
    {
        static byte joy, prev_joy, newly_pressed;
    
        wait_vblank();
        MUSE_update();
        
        prev_joy        = joy;
        joy             = read_joy( 0 );
        newly_pressed   = joy & ( prev_joy ^ 0xFF );
        
        // If Start is pressed, start sound effect 0 on channel 0.
        if ( newly_pressed & JOY_START )
        {
            // Just demonstrating use of MUSE_isSfxPlaying -- only
            // start the sound effect if it's not already playing.
            if ( !MUSE_isSfxPlaying( 0 ) )
            {
                MUSE_startSfx( 0, 0 );
            }
        }
        
        // If Select is pressed, stop the sound effect.
        if ( newly_pressed & JOY_SELECT )
        {
            MUSE_stopSfx( 0 );
        }
    }
    
    return 0;
}
