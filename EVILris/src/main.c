/*
  ToDo:
  
  v Реализовать сброс линий v
  v Разукрасить падающие tetramino v
  v Добавить функцию HOLD (on select) 
  v Улучшить генератор случайных чисел - часто повторяется одна и та же фигура
  v Реализовать infinite fall, посмотреть чтобы всегда была задержка перед закреплением
  v Сделать область падения не 20 а 23, и сделать потолок непрозрачным, а спрайт - фоновым v
  v Рисовать следующие фигуры
  v Считать очки
  v Рисовать очки
  v Реализовать title screen
  v Дорисовать графику
  v Пауза глючит, починить
  Всё-таки не идеален генератор случайных чисел, бывает заполнится целый экран а палки всё нет
  Всплывающие сообщения SINGLE, DOUBLE, TRIPLE, TETRIS, BACK 2 BACK, LEVEL UP
  Реализовать на титульном экране переход в окно настроек или старт игры
  Реализовать в паузе изменение графики, окошко посередине с возможностью начать сначала или выйти на титульный экран
  Реализовать звуки: закрепление, сброс, левелап, поворот (?)
  Реализовать проигрывание музыки: самопал ассемблером или плеер famitracker-а
*/

#include <knes.h>

void srand(void);
byte rand();
void SetDefaultGamePalette(void);
void DrawScreen(byte * screen_ptr);
void RandomizeNextTetraminos(void);
void RenderTetraminoMask(byte tetra_i);
byte GetNextTetramino(void);
void CreateNewTetramino(byte fromhold);
void UpdateFrame(void);
void UpdateTetraSprite(void);
void UpdateGhostSprite(void);
void UpdateNextSprite(byte k);
void UpdateHoldSprite(byte b);
void ClearTetraSprite(void);
byte CheckCollision(void);
void PlaceTetramino(void);
extern void place_tetramino(byte * tetramino_ptr, byte * playfield_ptr, byte x, byte y);
void DrawPlayfield(void);
void ClearPlayfield(void);
extern void draw_tetramino_at_playfield(byte * tetramino_ptr, byte x, byte y);
void DrawTetraminoAtPlayfield(void);
byte rand_t(void);
extern void clear_playfield(byte * playfield_ptr);
extern void clear_nt0(void);
extern void ppu_write(word length, word dest, const byte * source);
void Rotate(int mode);
void WaitForInput(void);
void FixPosition(void);
void FindLinesToClear(void);
void ClearMarkedLinesInPlayfield(void);
void AnimateLinesToClear(byte frame);
extern void animate_lines_to_clear(const byte * src_data, byte * lines_to_clear_ptr);
extern void blit_playfield(byte * playfield_ptr);
extern void clear_marked_lines_in_playfield(byte * playfield_ptr,
  byte * lines_to_clear_ptr);
extern byte find_lines_to_clear(byte * playfield_ptr, byte * lines_to_clear_ptr);
extern void display_rle_nt(byte * data_ptr, word record_count);
// delay_amount is in (ms / 256) 
extern void delay(word delay_amount);
// delay in ms (not asctronomically accurate. about 980 ms per sec)
extern void delay_ms(word delay_amount);
void UpdateScore(void);
void UpdateLevel(void);
void DrawScore(void);
void DrawLevel(void);
byte GetScoreAddition(void);
void UpdateLevelParams(void);
void PlayFixSound(void);
void WaitUnpause(void);

#define TETROMINO_TYPES 7
#define TETRA 4

#define JOY_THRESH_SLOW 10
#define JOY_THRESH_FAST 3
#define JOY_THRESH_SLOW_10LVL 7
#define JOY_THRESH_FAST_10LVL 3
#define MOV_DEFAULT_DELAY 32
#define MOV_FASTEST_LEVEL 20
#define MOV_DROP_THRESH 15
#define MOV_FIX_DELAY 7

#define NT_ADDR 0x2000
#define NT_SIZE 0x3C0
#define ATT_SIZE 0x40

#define ANIM_LINECLEAR_FRAMES 5

#define NT0_ADDR NT_ADDR
#define NT0_ATT_ADDR NT0_ADDR + NT_SIZE
#define NT1_ADDR NT0_ATT_ADDR + ATT_SIZE 
#define NT1_ATT_ADDR NT1_ADDR + NT_SIZE 
#define NT2_ADDR NT1_ATT_ADDR + ATT_SIZE 
#define NT2_ATT_ADDR NT2_ADDR + NT_SIZE 
#define NT3_ADDR NT2_ATT_ADDR + ATT_SIZE 
#define NT3_ATT_ADDR NT3_ADDR + NT_SIZE 

#define PALETTE_ADDR 0x3F00
#define PALETTE_HALFSIZE 0x10
#define BKG_PALETTE_ADDR PALETTE_ADDR
#define SPR_PALETTE_ADDR PALETTE_ADDR + PALETTE_HALFSIZE

#define SCR_LEFT_PART_SIZE 7
#define SCR_RIGHT_PART_SIZE 8
#define SCR_GAME_PART_SIZE 10
#define SCR_GAME_PART_Y 0
#define SCR_GAME_PART_HEIGHT 24
#define SCR_GAME_PART_HEIGHT_RESERVE 4
#define SCR_NEXT_LEFT 24
#define SCR_NEXT_TOP 6
#define SCR_HOLD_LEFT 1
#define SCR_HOLD_TOP 6
#define SCR_SCORE_LEFT 23
#define SCR_SCORE_TOP 23
#define SCR_LEVEL_LEFT 25
#define SCR_LEVEL_TOP 19

#define SPR_GHOST 4
#define SPR_HOLD 16

#define CHR_BLANK 0x00
#define CHR_BLOCK 0x01
#define CHR_BKG_0 0x02
#define CHR_BKG_1 0x03
#define CHR_GHOST 0x04
#define CHR_BRDUP 0x05
#define CHR_BRDDN 0x06
#define CHR_BRDLT 0x07
#define CHR_BRDRT 0x08
#define CHR_BRDLU 0x09
#define CHR_BRDRU 0x0A
#define CHR_BRDLD 0x0B
#define CHR_BRDRD 0x0C

#define PPU_RESET()                             \
do {                                            \
    PPU.addr = 0;                               \
    PPU.addr = 0;                               \
} while (0)

#define PPU_SET(a)                              \
do {                                            \
    PPU.data = a;                               \
} while (0)

#define DRAW_SETPOS(x, y)                       \
do {                                            \
    PPU_ADDR(NT0_ADDR | x | (y << 5));          \
} while ( 0 )

#define DRAW_SETCHAR(a)                         \
do {                                            \
    PPU_SET(a);                                 \
} while (0)
 
#define DRAW_PLOT(x, y, a)                      \
do {                                            \
    DRAW_SETPOS(x, y);                          \
    DRAW_SETCHAR(a);                            \
} while (0)

#define DRAW_PLOT(x, y, a)                      \
do {                                            \
    DRAW_SETPOS(x, y);                          \
    DRAW_SETCHAR(a);                            \
} while (0)

extern byte nt_game_screen_rlec;
extern byte nt_title_screen_rlec;

static byte i, j; // couple of generic loop counters
static byte * ptr1;
static byte * ptr2;
static byte tmp1;
static byte randvalue;
static byte prev_joy;
static byte joy;
static byte f_t_move;
static byte f_t_tetramino;
static byte f_t_fix;
static byte f_t_fix_mode;
static byte joy_frames;
static byte joy_thresh;
static byte joy_repeat;
static byte joy_thresh_slow;
static byte joy_thresh_fast;
static byte joy_block;
static byte joy_dx;
static byte joy_drop_performed;
static byte gameover;
static byte btn_frames;
static byte lines_cleared;
static byte total_lines_cleared;
static byte level_lines_cleared;
static byte prev_lines_cleared;
static byte hold_performed;
static word score;
static byte level;
static byte movedelay;
static byte paused;

static byte next_tetraminos[2];
static byte digit_chars_score[6];
static byte digit_chars_level[2];

static byte playfield[SCR_GAME_PART_HEIGHT][SCR_GAME_PART_SIZE];
static byte lines_to_clear[SCR_GAME_PART_HEIGHT];

static byte tetramino[TETRA][2];
static byte seed_1;
static byte tetra_x;
static byte tetra_hold;
static byte ghost_y;
static byte tetra_y;
static byte tetra_i;
static byte tetra_rnd;
static byte block_x;
static byte block_y;
static byte block_i;
static byte playfield_changed;
static byte palette_changed;

static const byte tetraminos[TETROMINO_TYPES] = 
{
  0x23, 0x26, 0x24, 0x45, 0x45, 0x25, 0x56 
// I, J, L, O, S, T, Z
};

static const byte tetramino_sizes[TETROMINO_TYPES] = 
{
  4, 3, 3, 2, 3, 3, 3, 
};

static const byte level_speeds[MOV_FASTEST_LEVEL] = 
{
  25, 21, 18, 16, 14, 13, 12, 11, 10, 9, 8, 8, 7, 7, 6, 6, 5, 5, 5, 4
};

static const byte tetramino_colors[TETROMINO_TYPES + 1][3] = 
{
  {0x1c, 0x21, 0x31},
  {0x02, 0x11, 0x21},
  {0x07, 0x17, 0x27}, 
  {0x17, 0x27, 0x37},
  {0x0B, 0x1A, 0x2A},
  {0x04, 0x14, 0x24},
  {0x06, 0x15, 0x35},
  {0x31, 0x2A, 0x24}, 
};

static const byte line_clear_graphic[ANIM_LINECLEAR_FRAMES][SCR_GAME_PART_SIZE] = 
{
  {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, },
  {0x0D, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x0E, },
  {0x00, 0x00, 0x0D, 0x03, 0x03, 0x03, 0x03, 0x0E, 0x00, 0x00, },
  {0x00, 0x00, 0x00, 0x00, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00, },
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
};

static const byte paused_text[6] = {'P', 'A', 'U', 'S', 'E', 'D'};

int main( void )
{
  PPU.ctrl = BASE_NT0|ADDRINC_1|SPR_CHR0|BG_CHR0|SPR_8X8|NMI_ON;
  PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_OFF|SPRREND_OFF;
  joy_thresh = JOY_THRESH_SLOW;
  SetDefaultGamePalette();
  DrawScreen(&nt_title_screen_rlec);
  PPU.ctrl = BASE_NT0|ADDRINC_1|SPR_CHR0|BG_CHR0|SPR_8X8|NMI_ON;
  PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_ON|SPRREND_ON;
  WaitForInput();
  for ( ;; )
  {
    PPU.ctrl = BASE_NT0|ADDRINC_1|SPR_CHR0|BG_CHR0|SPR_8X8|NMI_ON;
    PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_OFF|SPRREND_OFF;
    tetra_i = 0;
    paused = 0;
    playfield_changed = 0;
    total_lines_cleared = 0;
    gameover = 0;
		tetra_hold = TETROMINO_TYPES;
    RenderTetraminoMask(tetra_hold);
		UpdateHoldSprite(0);
    RandomizeNextTetraminos();
    SetDefaultGamePalette();
    DrawScreen(&nt_game_screen_rlec);
    CreateNewTetramino(0);
    joy_block = 0;
    score = 0;
    level = 1;
    UpdateLevelParams();
    UpdateScore();
    UpdateLevel();
    prev_lines_cleared = 0;
    
    wait_vblank();
    DrawScore();
    DrawLevel();
    
    PPU_RESET();
    wait_vblank();
		PPU.ctrl = BASE_NT0|ADDRINC_1|SPR_CHR0|BG_CHR0|SPR_8X8|NMI_ON;
    PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_ON|SPRREND_ON;
    // Loop forever.
    for ( ;; )
    {
      UpdateFrame();

      if (gameover) 
      {
        PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_OFF|SPRREND_OFF;
        clear_playfield(playfield[0]);
        clear_nt0();
        break;
      }
	  
			if (paused)
			{
				wait_vblank();
				ppu_write(sizeof(paused_text), NT0_ADDR | 10 | (10 << 5), paused_text);
				PPU_RESET();
				wait_vblank();
				WaitUnpause();
				wait_vblank();
				blit_playfield(playfield[SCR_GAME_PART_HEIGHT_RESERVE]);
				PPU_RESET();
			}
      
      wait_vblank();
      OAM_DMA();
      if (playfield_changed)
      {
        ppu_write(3, SPR_PALETTE_ADDR + 1, tetramino_colors[TETROMINO_TYPES]);
        PPU_RESET(); 
        wait_vblank();
        ClearTetraSprite();
        draw_tetramino_at_playfield(tetramino[0], tetra_x, tetra_y);
        PPU_RESET(); 
        PlayFixSound();
        wait_vblank();
        OAM_DMA();
        wait_vblank();
        
        if (lines_cleared)
        {
          for (block_x = 0; block_x < ANIM_LINECLEAR_FRAMES; block_x++)
          {
            wait_vblank();
            animate_lines_to_clear(line_clear_graphic[block_x], lines_to_clear);
            PPU_RESET();
            wait_vblank();
          } 
          
          blit_playfield(playfield[SCR_GAME_PART_HEIGHT_RESERVE]);
          DrawScore();
          DrawLevel();
          
          PPU_RESET();
          wait_vblank();
        }
      }
      if (palette_changed)
      {
        ppu_write(3, SPR_PALETTE_ADDR + 1, tetramino_colors[tetra_i]);
				ppu_write(3, SPR_PALETTE_ADDR + 5, tetramino_colors[next_tetraminos[0]]);
				ppu_write(3, SPR_PALETTE_ADDR + 9, tetramino_colors[next_tetraminos[1]]);
				ppu_write(3, SPR_PALETTE_ADDR + 13, tetramino_colors[tetra_hold]);
        palette_changed = 0;
      }
      PPU_RESET(); 
    }
  }

  return 0;
}

void SetDefaultGamePalette(void)
{
    #define BGCOLOR 0x0F
    static const byte palette[] = {
        // background palette
				BGCOLOR, 0x17, 0x27, 0x30, // playfield
        BGCOLOR, 0x0A, 0x1A, 0x2A, // frame
        BGCOLOR, 0x30, 0x1A, 0xF,  // the rest
        BGCOLOR, 0x17, 0x27, 0xF,  // unused
    };
    #undef BGCOLOR

    // avoid artifacts by only writing palette in vblank
    wait_vblank();

    PPU_ADDR(PALETTE_ADDR);
    for(i = 0; i < sizeof palette; ++i) {
        PPU.data = palette[i];
    }
}

void DrawScreen(byte * screen_ptr)
{
  word len;
 
  wait_vblank();
  PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_OFF|SPRREND_OFF;

  len = *((word *)screen_ptr) / 2;
  screen_ptr += sizeof(word);
  display_rle_nt(screen_ptr, len);
	PPU_RESET();
  wait_vblank();
  PPU.mask = GRAYSCL_OFF|BGCLIP_OFF|SPRCLIP_OFF|BGREND_ON|SPRREND_ON;
}

void RandomizeNextTetraminos(void)
{
  srand(); 
  
  for (i = 0; i < 2; i++)
    next_tetraminos[i] = rand_t();
}

byte GetNextTetramino(void)
{
	byte shifted = next_tetraminos[0];
  next_tetraminos[0] = next_tetraminos[1];
  next_tetraminos[1] = rand_t();
  RenderTetraminoMask(next_tetraminos[0]);
	UpdateNextSprite(0);
	RenderTetraminoMask(next_tetraminos[1]);
	UpdateNextSprite(1);
	return shifted;
}

void CreateNewTetramino(byte fromhold)
{
  f_t_move = 0;
  f_t_fix_mode = 0;
  f_t_tetramino = 0;
  srand(); 
	
	if (fromhold && tetra_hold != TETROMINO_TYPES)
	{
		tmp1 = tetra_hold;
		tetra_hold = tetra_i;
		tetra_i = tmp1;
		RenderTetraminoMask(tetra_hold);
		UpdateHoldSprite(1);
	}
	else
	{
		if (fromhold)
		{
			tetra_hold = tetra_i;
			RenderTetraminoMask(tetra_hold);
			UpdateHoldSprite(1);
		}
		else	
			hold_performed = 0;
		tetra_i = GetNextTetramino();
	}
	
	RenderTetraminoMask(tetra_i);
	tetra_x = 3;
  tetra_y = 2;
  Rotate(1);
  Rotate(2);

  if (CheckCollision())
  {
    gameover = 1;
  }
  UpdateTetraSprite();
  UpdateGhostSprite();
	palette_changed = 1;
}

void RenderTetraminoMask(byte tetra_i)
{
  tetramino[0][0] = 0;
  tetramino[1][0] = 0;
  if (tetra_i == 4)
  {
    tetramino[0][1] = 1;
    tetramino[1][1] = 2;
  }
  else
  {
    tetramino[0][1] = 0;
    tetramino[1][1] = 1;
  }
  
  j = tetraminos[tetra_i];
  
  tetramino[2][0] = j >> 6 & 3;
  tetramino[2][1] = j >> 4 & 3;
  tetramino[3][0] = j >> 2 & 3;
  tetramino[3][1] = j & 3;
}

void WaitForInput(void)
{ 
  while (!joy)
  {
    joy = read_joy(0);
    btn_frames++;
  }
}

void UpdateFrame(void)
{
  if (playfield_changed)
  {
    CreateNewTetramino(0);
    playfield_changed = 0;  
  }
  j = tetra_x;
  prev_joy = joy;
  joy = read_joy(0);
  
  if (prev_joy != joy)
    btn_frames = 0;
  else
    btn_frames++;
   
  joy_block &= joy;   
  
  if ((joy & JOY_START) && (!(prev_joy & JOY_START)) && (!(joy_block & JOY_START)))
  {
    paused = 1;
  }
  
  if (paused)
    return;
  
  if ((joy & JOY_LEFT) && (!f_t_fix_mode) && (!(joy_block & JOY_LEFT)))
  { 
    joy_dx = -1;
    joy_repeat = prev_joy & JOY_LEFT;
  }
  else if ((joy & JOY_RIGHT) && (!f_t_fix_mode) && (!(joy_block & JOY_RIGHT)))
  {
    joy_dx = 1;
    joy_repeat = prev_joy & JOY_RIGHT;
  }
  else
  {
    joy_dx = 0;
    joy_repeat = 0;
  }
    
  if (joy_dx)
  {
    if (joy_repeat)
      joy_frames++;
    if (!joy_repeat || (joy_frames == joy_thresh))
    {
      if (joy_frames == joy_thresh)
        joy_thresh = joy_thresh_fast;
      else
        joy_thresh = joy_thresh_slow;
      joy_frames = 0;
      tetra_x += joy_dx;
    }
  }
  
  if ((joy & JOY_B) && !(prev_joy & JOY_B) && (!(joy_block & JOY_B)))
  {
    f_t_fix = 0;
    Rotate(1);
    FixPosition();
    if (!CheckCollision())
    { 
      f_t_move = movedelay - MOV_FIX_DELAY;
      f_t_fix_mode = 0;
    }
  }
  
  if ((joy & JOY_A) && !(prev_joy & JOY_A) && (!(joy_block & JOY_A)))
  {
    f_t_fix = 0;
    Rotate(2);
    FixPosition();
    if (!CheckCollision())
    {
      f_t_move = movedelay - MOV_FIX_DELAY;
      f_t_fix_mode = 0;
    }
  }
    
  if ((joy & JOY_DOWN) && (!f_t_fix_mode) && (f_t_move < movedelay - 2) && (!(joy_block & JOY_DOWN)))
    f_t_move = movedelay - 2;
		
  if ((joy & JOY_SELECT) && (!(joy_block & JOY_SELECT)))
	{
		if (!hold_performed)
		{
			hold_performed = 1;
			CreateNewTetramino(1);
		}
  }
    
  if ((joy & JOY_UP) && (f_t_tetramino > MOV_DROP_THRESH) && (!(joy_block & JOY_UP)))
  {
    joy_drop_performed = 1;
    while (!CheckCollision())
      tetra_y++;
    tetra_y--;
    
    f_t_fix_mode = 1;
    f_t_fix = MOV_FIX_DELAY;
  }
    
  if (CheckCollision())
  { 
    tetra_x = j;
  }
  
  if (f_t_fix_mode)
  {
    f_t_fix++;
    if (f_t_fix >= MOV_FIX_DELAY)
    {
      f_t_fix_mode = 0;
      tetra_y++;
      if (CheckCollision())
      {
        tetra_y--;
        joy_drop_performed = 0;
        joy_block = 0;
        place_tetramino(tetramino[0], playfield[0], tetra_x, tetra_y);
            
        if (lines_cleared)
          prev_lines_cleared = lines_cleared;
          
        FindLinesToClear();
        
        if (lines_cleared)
        {
          total_lines_cleared += lines_cleared;
          level_lines_cleared += lines_cleared;
          if (level_lines_cleared >= 10)
          {
            level++;
            UpdateLevelParams();
            level_lines_cleared -= 10;
          }

          score += GetScoreAddition();
          UpdateScore();
          UpdateLevel();
          clear_marked_lines_in_playfield(playfield[0], 
            lines_to_clear);
        }
        playfield_changed = 1;
      }
      else
        tetra_y--;
    }
  }
  else
  
  
  {
    f_t_move++;
    f_t_tetramino++;
    if (f_t_move == movedelay) 
    {
      f_t_move = 0;
      tetra_y++;
      
      if (CheckCollision())
      {
        f_t_fix = 0;
        f_t_fix_mode = 1;
        tetra_y--;
      }
    }
    else
    {
      tmp1 = tetra_y;
      while (!CheckCollision())
        tetra_y++;
      ghost_y = tetra_y - 1;
      tetra_y = tmp1;

    }
  }
  UpdateTetraSprite();
  UpdateGhostSprite(); 
}

void UpdateTetraSprite(void)
{
  for (i = 0; i < TETRA; i++)
  {
    OAM[i].y = (tetra_y + tetramino[i][0] + SCR_GAME_PART_Y) * 8 - 1;
    OAM[i].x = (tetra_x + tetramino[i][1] + SCR_LEFT_PART_SIZE + 1) * 8;
    OAM[i].tile = 1;
    OAM[i].attrib = _B(00100000);
  }
}

void UpdateNextSprite(byte k)
{
	byte offset;
	offset = k * 4 + 8;
	for (i = 0; i < TETRA; i++)
	{
		OAM[i + offset].y = (k * 4 + tetramino[i][0] + SCR_NEXT_TOP) * 8;
		OAM[i + offset].x = (tetramino[i][1] + SCR_NEXT_LEFT) * 8;
		OAM[i + offset].tile = 1;
		OAM[i + offset].attrib = k + 1;
	}
}

void UpdateHoldSprite(byte b)
{
	for (i = 0; i < TETRA; i++)
	{
		OAM[i + SPR_HOLD].y = (tetramino[i][0] + SCR_HOLD_TOP) * 8;
		OAM[i + SPR_HOLD].x = (tetramino[i][1] + SCR_HOLD_LEFT) * 8;
		OAM[i + SPR_HOLD].tile = b;
		OAM[i + SPR_HOLD].attrib = 3;
	}
}

void UpdateGhostSprite(void)
{
  for (i = 0; i < TETRA; i++)
  {
		if (ghost_y == tetra_y)
		{
		  OAM[i + SPR_GHOST].tile = 0;
			OAM[i + SPR_GHOST].x = 0;
			OAM[i + SPR_GHOST].y = 0;
			OAM[i + SPR_GHOST].attrib = _B(00000000);
		}
		else
		{
			OAM[i + SPR_GHOST].y = (ghost_y + tetramino[i][0] + SCR_GAME_PART_Y) * 8 - 1;
			OAM[i + SPR_GHOST].x = (tetra_x + tetramino[i][1] + SCR_LEFT_PART_SIZE + 1) * 8;
			OAM[i + SPR_GHOST].tile = 4;
			OAM[i + SPR_GHOST].attrib = _B(00000000);
		}
  }
}

void ClearTetraSprite(void)
{
  for (i = 0; i < TETRA * 2; i++)
    OAM[i].tile = 0;
}

byte CheckCollision(void)
{
  for (block_i = 0; block_i < TETRA; block_i++)
  {
    block_x = tetramino[block_i][1] + tetra_x;
    if (block_x > SCR_GAME_PART_SIZE - 1)
      return 1;
    block_y = tetramino[block_i][0] + tetra_y;
    if (block_y >= SCR_GAME_PART_HEIGHT)
      return 1;
      
    if (playfield[block_y][block_x])
      return 1;
  }

  return 0;
}

void PlaceTetramino(void)
{
  for (block_i = 0; block_i < TETRA; block_i++)
  {
    block_x = tetramino[block_i][1] + tetra_x;
    block_y = tetramino[block_i][0] + tetra_y;    

    playfield[block_y][block_x] = 1;
  }
}

void DrawTetraminoAtPlayfield(void)
{
  for (block_i = 0; block_i < TETRA; block_i++)
  {
    block_x = tetramino[block_i][1] + tetra_x + SCR_GAME_PART_SIZE;
    block_y = tetramino[block_i][0] + tetra_y + SCR_GAME_PART_Y;
    
    DRAW_PLOT(block_x, block_y, 1);
  }
}

void ClearPlayfield(void)
{
  ptr1 = playfield[0];
  i = SCR_GAME_PART_SIZE * SCR_GAME_PART_HEIGHT;
  while (i)
  {
    *ptr1 = 0;
    ptr1++;
    i--;
  }
}

void srand(void)
{
  seed_1 = btn_frames ^ (NMI_COUNT() + randvalue);
  if (!seed_1)
    seed_1 = 1;
}

byte rand()
{
  seed_1 = (seed_1 >> 1) ^ ( (seed_1 & 1) ? 0xF0 : 0 );
  randvalue = (seed_1 & 1) << 2;
  seed_1 = (seed_1 >> 1) ^ ( (seed_1 & 1) ? 0xF0 : 0 );
  randvalue |= (seed_1 & 1) << 1;
  seed_1 = (seed_1 >> 1) ^ ( (seed_1 & 1) ? 0xF0 : 0 );
  randvalue |= (seed_1 & 1);
  return randvalue ^ (btn_frames & TETROMINO_TYPES) ^ TETROMINO_TYPES;
}

byte rand_t(void)
{
	byte pre_rnd = tetra_rnd;
  byte counter = 0;
  tetra_rnd = rand();
  while ((tetra_rnd == pre_rnd) || !tetra_rnd) 
	{
    counter++;
    if (counter == 0x10)
    {
      counter = 0;
      srand();
    }
    tetra_rnd = rand();
	}

  return tetra_rnd - 1;
}

void Rotate(int mode)
{
  byte min_y;
  byte min_x;
  byte t_size;
  min_y = 4;
  min_x = 4;
  
  t_size = tetramino_sizes[tetra_i];
  if (t_size == 2) 
    return;
    
  for (block_i = 0; block_i < TETRA; block_i++)
  {
    if (mode == 1)
    {
      block_x = tetramino[block_i][0];
      tetramino[block_i][0] = tetramino[block_i][1];
      tetramino[block_i][1] = t_size - block_x;
    };
    if (mode == 2)
    {
      block_y = tetramino[block_i][1];
      tetramino[block_i][1] = tetramino[block_i][0];
      tetramino[block_i][0] = t_size - block_y;    
    }
    if (tetramino[block_i][0] < min_y)
      min_y = tetramino[block_i][0];
    if (tetramino[block_i][1] < min_x)
      min_x = tetramino[block_i][1];
  }
  
  if (min_y > 0 || min_x > 0)
  {
    min_x -= tetramino_sizes[tetra_i] / 2;
    min_y -= tetramino_sizes[tetra_i] / 2;
    //if (((min_x > 0) && (min_x < TETRA)) || ((min_y > 0) && (min_y < TETRA)))
      for (block_i = 0; block_i < TETRA; block_i++)
      {
        tetramino[block_i][0] -= min_y;
        tetramino[block_i][1] -= min_x;
      }
  }
}

void FixPosition(void)
{  
  for (block_i = 0; block_i < TETRA; block_i++)
  {
    block_x = tetramino[block_i][1] + tetra_x;
    while (block_x > 0xF0)
    {
      tetra_x = -block_x;
      block_x = tetramino[block_i][1] + tetra_x;
    }
    while (block_x > SCR_GAME_PART_SIZE - 1)
    {
      tetra_x --;
      block_x = tetramino[block_i][1] + tetra_x;
    }
    block_y = tetramino[block_i][0] + tetra_y;
    while (block_y >= SCR_GAME_PART_HEIGHT)
    {
      tetra_y--;
      block_y = tetramino[block_i][0] + tetra_y;
    }
      
    while (playfield[block_y][block_x])
    {
      tetra_y--;
      block_y = tetramino[block_i][0] + tetra_y;
    }
  }
}

void FindLinesToClear(void)
{
  lines_cleared = 0;
  ptr1 = playfield[0];
  for (block_y = 0; block_y < SCR_GAME_PART_HEIGHT; block_y++)
  {
    i = 1;
    block_x = 0;
    while ((i) && (block_x < SCR_GAME_PART_SIZE))
    {
      i = *ptr1;
      ptr1++;
      block_x++;
    }
    ptr1 += SCR_GAME_PART_SIZE - block_x;
    lines_to_clear[block_y] = i;
    if (i)
      lines_cleared++;
  }    
}

void AnimateLinesToClear(byte frame)
{
  word dest;

  for (block_y = 0; block_y < SCR_GAME_PART_HEIGHT; block_y++)
  {
    if (lines_to_clear[block_y])
    {
      dest = NT0_ADDR + (block_y + SCR_GAME_PART_Y) * 32 + SCR_LEFT_PART_SIZE + 1;
      ppu_write(SCR_GAME_PART_SIZE, dest, line_clear_graphic[frame]);
      PPU_RESET();
      wait_vblank();  
    }    
  }
}

void ClearMarkedLinesInPlayfield(void)
{
  block_y = SCR_GAME_PART_HEIGHT;
  block_x = block_y;
  
  while (block_x)
  {
    if (lines_to_clear[block_x - 1])
      block_x--;
    else
    {  
      if (block_x != block_y)
      {
        ptr1 = playfield[block_y - 1];
        ptr2 = playfield[block_x - 1];
        for (i = 0; i < SCR_GAME_PART_SIZE; i++)
        {
          *ptr1 = *ptr2;
          ptr1++;
          ptr2++;
        }
      }
      
      block_x--;
      block_y--;
    }
  }
}

void IntToStr(word value, byte * digits, byte maxlength)
{
  byte digit;
  byte val;
  word divisor;
  for (digit = maxlength; digit; digit--)
  {
    divisor = 1;
    for (i = digit - 1; i; i--)
    {
      divisor *= 10;
    }
    val = 0;
    while (value >= divisor)
    {
      val++;
      value -= divisor;
    }
    digits[maxlength - digit] = 0x30 + val;
  }
}

void UpdateScore(void)
{
  digit_chars_score[4] = 0x30;
  digit_chars_score[5] = 0x30;
  IntToStr(score, digit_chars_score, 4);
}

void UpdateLevel(void)
{
  IntToStr(level, digit_chars_level, 2); 
}

void DrawScore(void)
{
  ppu_write(6, NT0_ADDR | SCR_SCORE_LEFT | (SCR_SCORE_TOP << 5), digit_chars_score);
}

void DrawLevel(void)
{
  ppu_write(2, NT0_ADDR | SCR_LEVEL_LEFT | (SCR_LEVEL_TOP << 5), digit_chars_level);
}

byte GetScoreAddition(void)
{
  switch (lines_cleared)
  {
    case 1:
      return 1;
    case 2:
      return 2;
    case 3:
      return 4;
    case 4:
      if (prev_lines_cleared == 4)
        return 12;
      else
        return 8;
  }
}

void UpdateLevelParams(void)
{
  if (level <= MOV_FASTEST_LEVEL)
    movedelay = level_speeds[level - 1];
  else
    movedelay = level_speeds[MOV_FASTEST_LEVEL - 1];
  if (level < 10)
  {
    joy_thresh_slow = JOY_THRESH_SLOW; 
    joy_thresh_fast = JOY_THRESH_FAST;
  }
  else
  {
    joy_thresh_slow = JOY_THRESH_SLOW_10LVL; 
    joy_thresh_fast = JOY_THRESH_FAST_10LVL;
  }
}

void PlayFixSound(void)
{ 
  CPU.apu.tri.linear = _B(10000111);
  CPU.apu.tri.lo = _B(10100101);
  CPU.apu.tri.hi = _B(00111111);
  CPU.apu_chn = _B(00001100);
  for (i = 16; i != 0; i--) 
	{
		CPU.apu.tri.hi = i;
		delay(2);
	}
  CPU.apu_chn = _B(00000000);
}

void WaitUnpause(void)
{
  while (1)
  {
		prev_joy = joy;
		joy = read_joy(0);  
	  
	  if ((joy & JOY_START) && (!(prev_joy & JOY_START)))
	  {
			paused = 0;
			joy_block |= JOY_START;
			break;
	  }
	}
}