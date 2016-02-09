.rodata

; General data.
    .include "snd-data/snd-data.s"
; Song data.
    .include "snd-data/snd-data-cheetah.s"
; Sound effect data.
    .include "snd-data/snd-data-sfx-metal.s"

; DPCM data (must be at $C000-FFFF and aligned to 64 bytes).
.segment "DPCM"
    .include "snd-data/snd-data-dpcm.s"

; Exports for C.
.export _snd_data           := snd_data
