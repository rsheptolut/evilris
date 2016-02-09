snd_data:
  .word MUSE_L0
  .word MUSE_L1

MUSE_L0:
  .word MUSE_L2

MUSE_L1:
  .word MUSE_L3

MUSE_L4:
  .word MUSE_L27

MUSE_L27:
  .word MUSE_L28 - 1
  .word MUSE_L29 - 1
  .word MUSE_L30 - 1
  .word MUSE_L31 - 1

MUSE_L28:
  .byte 160, 240, 208, 160, 144, 144, 128, $FF, 0
MUSE_L29:
  .byte 52, 64, $FF, 0
MUSE_L30:
  .byte 0, $FF, 0
MUSE_L31:
  .byte 127, $FF, 0

MUSE_L5:
  .byte <( MUSE_L32 >> 6 ), $50
  .byte <( MUSE_L33 >> 6 ), $26

