#ifndef MUSE_WRAP_H
#define MUSE_WRAP_H

#include <knes.h>

// Flags for MUSE_setFlags.
enum MUSE_Flags
{
    // If set, music updates are paused.
    MUSE_PAUSE_MUSIC             = _B( 10000000 ),
    // If set, sound effect updates are paused.
    MUSE_PAUSE_SFX               = _B( 01000000 ),
    // If set, pauses both music and sound effects.
    MUSE_PAUSE                   = MUSE_PAUSE_MUSIC | MUSE_PAUSE_SFX,
    // If set, global volume (MUSE_setVolume) doesn't apply to sound effects.
    MUSE_GLOBAL_VOL_BEFORE_SFX   = _B( 00100000 ),
    // If set, speed and note frequencies are compensated for NTSC.
    MUSE_NTSC_MODE               = _B( 00010000 ),
};

extern void __fastcall__ MUSE_init( const void* sound_data );
extern void __fastcall__ MUSE_update( void );
extern void __fastcall__ MUSE_startMusic( byte song );
extern void __fastcall__ MUSE_startSfx( byte sfx, byte channel );
extern void __fastcall__ MUSE_stopSfx( byte channel );
extern byte __fastcall__ MUSE_isSfxPlaying( byte channel );
extern void __fastcall__ MUSE_setVolume( byte volume );
extern void __fastcall__ MUSE_setFlags( byte flags );
extern byte __fastcall__ MUSE_getSyncEvent( void );

#endif // !MUSE_WRAP_H
