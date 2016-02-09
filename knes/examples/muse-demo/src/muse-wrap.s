.zeropage
; Zeropage used by MUSE.
MUSE_ZEROPAGE:              .res 7

.bss
; RAM used by MUSE.
MUSE_RAM:                   .res 256

.code

; MUSE library, must be aligned to a 256 byte page.
; Could have another segment for this to avoid wasting bytes for aligning.
    .align 256
    .include "muse-ca65.h"

; Wrappers for some of the routines, as they take more than one parameter,
; or in registers other than A.
.import popa

; extern void __fastcall__ MUSE_startSfx( byte sfx, byte channel );
.proc _MUSE_startSfx
    ; "channel" in A, "sfx" on the CC65 stack.
    tax
    jsr popa
    jmp MUSE_startSfx
.endproc

; extern void __fastcall__ MUSE_stopSfx( byte channel );
.proc _MUSE_stopSfx
    tax
    jmp MUSE_stopSfx
.endproc

; extern byte __fastcall__ MUSE_isSfxPlaying( byte channel );
.proc _MUSE_isSfxPlaying
    tax
    jmp MUSE_isSfxPlaying    
.endproc
    
; Exports for C.
.export _MUSE_init          := MUSE_init
.export _MUSE_update        := MUSE_update
.export _MUSE_startMusic    := MUSE_startMusic
.export _MUSE_setVolume     := MUSE_setVolume
.export _MUSE_setFlags      := MUSE_setFlags
.export _MUSE_getSyncEvent  := MUSE_getSyncEvent
.export _MUSE_startSfx
.export _MUSE_stopSfx
.export _MUSE_isSfxPlaying
