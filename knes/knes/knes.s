.import __nmi_count : zeropage

.export _wait_vblank
.proc _wait_vblank
    ; TODO: Should we save A?
    lda __nmi_count
:
    cmp __nmi_count
    beq :-
    rts
.endproc

.export _poll_vblank
.proc _poll_vblank
    ; Read once first to make sure the flag is initially clear (we don't want
    ;   to return in the middle of a frame).
    bit $2002
:
    bit $2002
    bpl :-
    rts
.endproc

.import __read_joy0
.import __read_joy1

.export _read_joy
.proc _read_joy
    cmp #0
    bne :+
    jmp __read_joy0
:
    cmp #1
    bne :+
    jmp __read_joy1
:
    lda #0
    rts
.endproc

.export _enable_interrupts
.proc _enable_interrupts
    cli
    rts
.endproc

.export _disable_interrupts
.proc _disable_interrupts
    sei
    rts
.endproc
