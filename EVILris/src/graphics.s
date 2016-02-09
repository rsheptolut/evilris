.segment "CHR"
    .incbin "gfx/tiles.chr"
.segment "CODE"
.export _nt_title_screen_rlec
.export _nt_game_screen_rlec
_nt_title_screen_rlec:    .incbin "titlescreen.cnt"
_nt_game_screen_rlec:   .incbin "gamescreen.cnt"
