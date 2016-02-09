    .autoimport	on
    .case       on
    .debuginfo  off
    .importzp   ptr1, ptr2
    .importzp   tmp1, tmp2, tmp3, tmp4
    .import     _wait_vblank

; Defines that must match those in tetris.h
    .define SCR_GAME_PART_SIZE 10
    .define SCR_GAME_PART_LEFT 8
    .define SCR_GAME_PART_Y 0
    .define SCR_GAME_PART_HEIGHT 24
    .define SCR_GAME_PART_HEIGHT_RESERVE 4


; ---------------------------------------------------------------
; void blit_board(unsigned char *board, unsigned char *screenpos)
; Copy the board array onto the screen at the offset provided
; Widths and heights are defined as constants above
; ---------------------------------------------------------------

.segment        "CODE"

; void draw_tetramino_at_playfield(byte * tetramino_ptr, byte x, byte y);

.export _draw_tetramino_at_playfield
.proc	_draw_tetramino_at_playfield
 
    jsr     popa         ; tetra_y
    sta     tmp4

    jsr     popa         ; tetra_x
    sta     tmp3

    jsr     popax        ; tetramino_ptr
    sta     ptr1
    stx     ptr1+1
   
    ldx     #$4 
    ldy     #$FF

	; assume y = 0
DRAWBLOCK:

    iny                         ; first y is 0, look at the code!
    lda     (ptr1), y           ; getting the y coordinate of a block into the a
    clc
    adc     tmp4                ; adding the y coordinate of a tetermino
    clc
	cmp		#$4                 ; if the resulting y coordinate of a block is less than 4, we don't draw it
	bpl     :+
	iny
	jmp     NEXTBLOCK
:
	clc                         ; adding the size of a screen frame to y coordinate  
    adc     #SCR_GAME_PART_Y
    sta     tmp2 
    
    iny     
    lda     (ptr1), y           ; loading x offset of a block in tetranimo
    adc     tmp3                ; adding tetra_x to it
    clc
    adc     #SCR_GAME_PART_LEFT ; adding the size of a screen frame to x coordinate
    sta     tmp1
 
    lda     tmp2                ; calculating HI address = $20 + Y / 8
    lsr     a
    lsr     a
    lsr     a
    ora     #$20
    sta     $2006               ; storing HI Address
    
    lda     tmp2                ; calculating LO address = Y * 32 + X
    asl     a
    asl     a
    asl     a
    asl     a
    asl     a
    ora     tmp1
    sta     $2006               ; storing LO address

    lda     #1
    sta     $2007

NEXTBLOCK:
    dex
    bne     DRAWBLOCK

    rts                       ; Params stack is already correct

.endproc

; void place_tetramino(byte * tetramino_ptr, byte * playfield_ptr, byte tetra_x, byte tetra_y);

.export _place_tetramino
.proc	_place_tetramino
 
    jsr     popa    ; Сохраняем tetra_y
    sta     tmp2 

    jsr     popa    ; Сохраняем tetra_x
    sta     tmp1   

    jsr     popax   ; Сохраняем playfield_ptr
    sta     ptr2
    stx     ptr2+1
   
    jsr     popax   ; Сохраняем tetramino_ptr
    sta     ptr1
    stx     ptr1+1
   
    ldx     #$0     ; Начинаем с нулевого блока (всего будет 4)

SETBLOCK:
    
    txa                 ;
    asl     a           ; 
    tay                 ; Y = X * 2
    lda     (ptr1), y   ; A = tetramino_ptr[Y]
    clc
    adc     tmp2        ; A = A + tetra_y (final_y)
    
    asl     a           ; A = final_y * 2
    sta     tmp3        ; TMP3 = final_y * 2
    
    asl     a           ; A = A * 2
    asl     a           ; A = A * 2 (= final_y * 8)
    
    clc
    adc     tmp3        ; A = (final_y * 8) + TMP3 = (final_y * 8) + (final_y * 2) = final_y * 10
    
    iny                 
    adc     (ptr1), y   ; A = (final_y * 10) + tetramino_ptr[Y + 1] = (final_y * 10) + block_x
    adc     tmp1        ; A = (final_y * 10) + block_x + tetra_x = final_y * 10 + final_x = offset
    
    tay                 ; Перенесем offset в y
    
    lda     #$1
    sta     (ptr2), y   ; playfield_ptr[offset] = 1
    
    inx                  
    cpx     #$4         ; LOOP UNTIL X = 4
    bne     SETBLOCK    ;

    rts                 ; Params stack is already correct

.endproc

; void clear_nt0(void);
.export _clear_nt0
.proc	_clear_nt0
 
    lda     #$20        ; Начинаем с адреса видеопамяти $2000 (Nametable 0)
    sta     $2006
    lda     #$00
    sta     $2006
    
    lda     #$0         ; Будем писать нули
    tay                 ; Начинаем итерацию также с нуля
    ldx     #$4         ; Всего будет записано $400 байт: offset(HIBYTE - x, LOBYTE - y)
    
:
    sta     $2007       ; Пишем ноль в видеопамять
    dey                 ; Декрементируем Y
    bne     :-          ; (0x100 раз)
    dex                 ; Декрементируем X
    bne     :-          ; (0x4 раза)

    sta     $2006       ; Сбрасываем адрес PPU
    sta     $2006
   
    rts   
.endproc

; void clear_playfield(byte * playfield_ptr);
.export _clear_playfield
.proc	_clear_playfield

    jsr     popax      ; Получаем playfield_ptr
    sta     ptr1       
    stx     ptr1+1
  
    lda     #0;         ; A = 0
    tay;               ; Y = 0
    ldx     #(SCR_GAME_PART_HEIGHT * SCR_GAME_PART_SIZE)       ; [For X = 240 downto 1]
:
    sta     (ptr1),y   ; Записываем 0 в текущую ячейку массива
    iny                ; Переходим на следующую ячейку
    dex                ; Засчитываем итерацию цикла (наверное быстрее чем каждый раз Y с 240 сравнивать)
    bne     :-         ; [Next]
    
    rts
    
    
.endproc

.export _delay
.proc _delay
	jsr	popax                ; a = LOBYTE(amount); x = HIBYTE(amount);

	sta tmp1                 ; tmp1 = a;
	txa                      ; a = x;
	ldx #0                   ; x = 0;
	tay                      ; y = a;
	
	; tmp1 - lo byte of delay, y - hi byte of delay, x - 0
	
	bne :+                   ; if (amount_hi == 0)
	jmp :++
:
	dex                      ; x--;
	bne :-                   ; if (x == 0) goto -1;
	
	ldx #0                   ; x = 0;
	dey                      ; y--;
	bne :-                   ; if (y == 0) goto -1;
:
	ldx tmp1                 ; x = amount_lo;
:
	dex
	bne :-
	
	rts
.endproc

.export _delay_ms
.proc _delay_ms
	jsr	popax                ; a = LOBYTE(amount); x = HIBYTE(amount);

	sta tmp1                 ; tmp1 = a;
	txa                      ; a = x;
	ldx #0                   ; x = 0;
	tay                      ; y = a;
	
	; tmp1 - lo byte of delay, y - hi byte of delay, x - 0
	
	bne :+                   ; if (amount_hi == 0)
	jmp :++
:
	txa
	ldx #0
:
    clc
	dex
	bne :-
	tax
	dex                      ; x--;
	bne :--                  ; if (x == 0) goto -1;
	
	ldx #0                   ; x = 0;
	dey                      ; y--;
	bne :--                  ; if (y == 0) goto -1;
:
	ldx tmp1                 ; x = amount_lo;
:
	txa
	ldx #0
:
    clc
	dex
	bne :-
	tax
	dex                      ; x--
	bne :--
	
	rts
.endproc

; Быстрое копирование памяти по указателю в ptr1 длиной в tmp1 (максимум 256 байт, если tmp=0)
.proc _ppu_write_fast
    
    ldy     #0          ; Обнуляем y

:                       ; <Цикл Y>
    lda     (ptr1), y   ; Берем очередной байт данных из источника
    sta     $2007       ; записываем его в видеопамять
    iny                 ; Инкрементируем y
    cpy     tmp1        ; Сравниваем с tmp1
    bne     :-          ; </Цикл Y>
    
    rts
.endproc

; extern void _ppu_write(word length, word dest, byte * source);
.export _ppu_write
.proc _ppu_write
    
    jsr     popax      ; Получаем source
    sta     ptr1       
    stx     ptr1+1  

    jsr     popax      ; Получаем dest
    stx     $2006      ; Пишем старший байт в регистр видеопроцессора
    sta     $2006      ; Пишем младший байт в регистр видеопроцессора
    
    jsr     popax       ; Получаем length
    sta     tmp1        ; Сохраняем младший байт в tmp1
    inx                 ; Старший байт у нас в X, и увеличим ка мы его на 1 (?!?!)
    
    ldy     #0          ; Итерация в пределах 256 байт начинается
    sty     tmp2        ; заодно обнулим значение с которым будет сравнена y (по умолчанию 256 байт)
:                       ; <Цикл X><Цикл Y>
    cpx     #1          ; Если X <> 1 (а значит имеется в виду старший разряд 0)
    bne     :+          ; то идем на :
    lda     tmp1        ; А иначе, копируем младший разряд length в tmp2
    sta     tmp2 
:
    lda     (ptr1), y   ; Берем очередной байт данных из источника
    sta     $2007       ; записываем его в видеопамять
    iny                 ; Инкрементируем y
    cpy     tmp2        ; Сравниваем с tmp2 (256 или меньше, если это последняя итерация по x)
    bne     :-          ; </Цикл Y>
    inc     ptr1+1      ; Инкрементируем старший разряд в указателе источника
    dex                 ; Декрементируем x
    bne     :--         ; </Цикл X>

    rts
.endproc

.proc position_at_tmp1_and_scr_game_part_left
  lda tmp1
  lsr a                  ; block_y >> 3 (cause low ppu address has 3 bits for "y" left from 5 bits of "x")
  lsr a 
  lsr a
  ora #$20               ; add $20 to result (cause NT0 starts at $2000)
  sta $2006              ; store HI vram address 
  
  lda tmp1               ; load block_y
  asl a                  ; block_y * 32 (cause max x is 31)
  asl a
  asl a
  asl a
  asl a
  ora #SCR_GAME_PART_LEFT ; block_y + 10
  sta $2006              ; store LO vram address
  
  rts
.endproc

; extern void blit_playfield(byte * playfield_ptr);
.export _blit_playfield
.proc _blit_playfield
  
  jsr popax              ; Получаем source
  sta ptr1       
  stx ptr1 + 1  
  
  lda #(SCR_GAME_PART_Y + SCR_GAME_PART_HEIGHT_RESERVE) 
  sta tmp1               ; block_y = 4

  ldy #0                  ; playfield_offset = 0
  sty tmp2
:                        ; <Цикл block_y (20)>
  ldx #(SCR_GAME_PART_SIZE) ; block_x = 10;
  
  jsr position_at_tmp1_and_scr_game_part_left

:                       ; <Цикл X (10)>
  lda (ptr1),y        ; write next byte
  sta $2007             ;
  iny                   ; inc playfield_offset
  dex                   ; dec count (10 times)

  bne :-                ; </Цикл X>
 
  inc tmp2
  lda tmp2
  cmp #8
  bne :+
  lda #0
  sta $2006
  sta $2006
  sta tmp2
  jsr _wait_vblank
:
  inc tmp1              ; inc block_y
  lda tmp1              ; load it
  ; compare it to 24
  cmp #(SCR_GAME_PART_HEIGHT + SCR_GAME_PART_Y)
  bne :---               ; </Цикл block_y>
  
  rts            ; Возврат из процедуры
.endproc

; extern void _clear_marked_lines_in_playfield(byte * playfield_ptr;
;   byte * lines_to_clear_ptr)
.export _clear_marked_lines_in_playfield
.proc _clear_marked_lines_in_playfield
  jsr popax              ; Получаем lines_to_clear_ptr
  sta ptr1       
  stx ptr1 + 1  

  jsr popax              ; Получаем playfield_ptr
  sta ptr2      
  stx ptr2 + 1  

  ldx #(SCR_GAME_PART_HEIGHT - 1) ; Начинаем с последней строки
  stx tmp1               ; tmp1 = Строка-источник
  stx tmp2               ; tmp2 = Строка-приемник
  
NextLine:
  
  ldy tmp1               ; 
  lda (ptr1), y          ;
  beq Move               ; if (lines_to_clear[tmp1] == 0) goto Move;
  dec tmp1               ; else Dec(tmp1);
  jmp NextLine           ; Continue;
  
Move:  
  lda tmp1
  cmp tmp2               
  beq SkipLine           ; if (tmp1 == tmp2) then goto SkipLine
  
  lda tmp1
  asl a
  sta tmp3
  asl a
  asl a
  clc
  adc tmp3               
  sta tmp3               ; tmp3 = tmp1 * 10
  
  lda tmp2
  asl a
  sta tmp4
  asl a
  asl a
  clc
  adc tmp4               
  sta tmp4               ; tmp4 = tmp2 * 10
  
  ldx #(SCR_GAME_PART_SIZE) ; For X = 10 downto 1
  
NextBlock:
  ldy tmp3               ; load byte from source line
  lda (ptr2), y
  inc tmp3

  ldy tmp4               ; store byte at dest line
  sta (ptr2), y
  inc tmp4
  dex
  bne NextBlock
  
SkipLine:
  dec tmp2
  dec tmp1
  lda tmp1
  cmp #$FF
  bne NextLine
  
  rts
.endproc

; extern void animate_lines_to_clear(byte * src_data, byte * lines_to_clear_ptr);
.export _animate_lines_to_clear
.proc _animate_lines_to_clear
  jsr popax              ; Получаем lines_to_clear_ptr
  sta ptr1       
  stx ptr1 + 1  

  jsr popax              ; Получаем src_data (тоже указатель)
  sta ptr2
  stx ptr2 + 1  
  
  ldy #(SCR_GAME_PART_HEIGHT - 1) ; Начнем с последней строки поля
  sty tmp1               ; tmp1 = block_y
  
NextLine:
  lda (ptr1), y          ; if (lines_to_clear[block_y] == 0) goto SkipLine
  beq SkipLine
  
  jsr position_at_tmp1_and_scr_game_part_left ; store HI(vram_addr) and LO(vram_addr)
  
  ldy #0                 ; start copying from 0
:
  lda (ptr2), y          ; copy next byte from source to vram
  sta $2007              
  iny                   
  cpy #(SCR_GAME_PART_SIZE) ; copy total of 10 bytes
  bne :- 
  
SkipLine:
  dec tmp1               ; cycling until block_y = 4
  ldy tmp1
  cpy #(SCR_GAME_PART_HEIGHT_RESERVE)
  bne NextLine
  
  rts
.endproc

; extern byte find_lines_to_clear(byte * playfield_ptr, byte * lines_to_clear_ptr)
.export _find_lines_to_clear
.proc _find_lines_to_clear
  
  jsr popax              ; Получаем lines_to_clear_ptr
  sta ptr1       
  stx ptr1 + 1

  jsr popax              ; Получаем playfield_ptr
  sta ptr2
  stx ptr2 + 1
  
  ldx #0
  stx tmp1               ; lines_to_clear index
  stx tmp2               ; playfield_ptr index
  stx tmp3
  
NextLine:
  ldx #10                ; for x = 1 to 10
  ldy tmp2               ; y = playfield_ptr index
:               
  lda (ptr2), y          ; if (playfield_ptr[y] == 0) goto NotFilled
  beq NotFilled
  iny                    ; y++
  dex                    ;
  bne :-                 ; Next X
  jmp SaveResults
  
NotFilled:
  iny
  dex
  bne NotFilled
  
SaveResults:
  sty tmp2               ; saving y back in playfield_ptr index
  ldy tmp1               ; y = lines_to_clear index
  sta (ptr1), y          ; lines_to_clear[y] = 0 if _any_ a was 0, or 1 if _all_ a was 1
  inc tmp3
 
  inc tmp1               ; lines_to_clear_index++
  lda tmp1               
  cmp #(SCR_GAME_PART_HEIGHT)
  bne NextLine           ; Next lines_to_clear_index (Until 24)
  
  lda tmp3
  rts
 
.endproc

; extern void display_rle_nt(byte * data_ptr, word record_count);
.export _display_rle_nt
.proc _display_rle_nt
	jsr popax
	sta tmp1           ; record_count
	stx tmp2           ; HI record_count
	inc tmp2           ; increment HI part, cause its meaning is (how much times will lesser cycle be executed)
	
	jsr popax
	sta ptr1
	stx ptr1 + 1       ; data_ptr
	
	lda #$20
	sta $2006
	lda #$00
	sta $2006          ; set the first byte of the name table 0
	        
	ldy #0
	
NextRecord:
	iny
	lda (ptr1), y      ; get the second byte, transfer it to x - it's count
	tax
	dey
	lda (ptr1), y      ; get first byte, it's the value - into the a
	iny                ; inc stream pos by 2
	iny
NextByte:
	sta $2007          ; store #a x times
	dex
	bne NextByte
	
	cpy #0             ; inc hi order of pointer when we're past 256 bytes
	bne DontIncPointer
	inc ptr1 + 1
DontIncPointer:
	dec tmp1           ; dec record_count, repeat until 0
	bne NextRecord
	dec tmp2
	bne NextRecord
	rts
.endproc