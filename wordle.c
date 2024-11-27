#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"


//#link "vrambuf.c"
void put_str(unsigned int adr, const char *str) {
  vram_adr(adr);        // set PPU read/write address
  vram_write(str, strlen(str)); // write bytes to PPU
}


void fill_nametableA(x,y){
  
  
  put_str(NTADR_A(x-1,y-1),"\xA2");
  put_str(NTADR_A(x-1,y),"\xA2");
  put_str(NTADR_A(x-1,y+1),"\xA2");
  
  
  put_str(NTADR_A(x-1,y-2),"\xA1");
  put_str(NTADR_A(x,y-2),"\xA1");
  put_str(NTADR_A(x+1,y-2),"\xA1");
  
  put_str(NTADR_A(x+1,y-1),"\xA3");
  put_str(NTADR_A(x+1,y),"\xA3");
  put_str(NTADR_A(x+1,y+1),"\xA3");
  
  put_str(NTADR_A(x+1,y+2),"\xA0");
  put_str(NTADR_A(x,y+2),"\xA0");
  put_str(NTADR_A(x-1,y+2),"\xA0");
  
  put_str(NTADR_A(x,y),"\x41");
  
  put_str(NTADR_A(x,y-1),"\x04");
  put_str(NTADR_A(x,y+1),"\x04");
  
  
}


void fill_letter(x, y, color, i) {
 
  if (color == 1) {
   
    put_str(NTADR_A(x-1,y-1),"\xA7");
    put_str(NTADR_A(x-1,y),"\xA7");
    put_str(NTADR_A(x-1,y+1),"\xA7");
    
    
    put_str(NTADR_A(x+1,y-1),"\xA6");
    put_str(NTADR_A(x+1,y),"\xA6");
    put_str(NTADR_A(x+1,y+1),"\xA6");
    
    put_str(NTADR_A(x,y-1),"\x01");
    put_str(NTADR_A(x,y+1),"\x01");
    vram_adr(NTADR_A(x, y));
    vram_put('\xC1' + i);
  }
  
  if (color == 2) {
    put_str(NTADR_A(x-1,y-1),"\xA5");
    put_str(NTADR_A(x-1,y),"\xA5");
    put_str(NTADR_A(x-1,y+1),"\xA5");
    
    
    put_str(NTADR_A(x+1,y-1),"\xA4");
    put_str(NTADR_A(x+1,y),"\xA4");
    put_str(NTADR_A(x+1,y+1),"\xA4");
    
    put_str(NTADR_A(x,y-1),"\x02");
    put_str(NTADR_A(x,y+1),"\x02");
    vram_adr(NTADR_A(x, y));
    vram_put('\x61'+i);
  }
  
}



void print_word(x,y){
  
  fill_nametableA(x,y);
  
  fill_nametableA(x+5,y);
  
  fill_nametableA(x+10,y);
  
  fill_nametableA(x+15,y);
  
  fill_nametableA(x+20,y);
  
}


void cycle_letters(i,x,y) {
  ppu_off();
  vram_adr(NTADR_A(x, y));
  vram_put('A' + i);
  ppu_on_all();
}




int check_word(const char *word, char letter, int i) {
  
  
  int color = 0;
  int j; 
    if(word[i] == letter) {
	color = 1;
    }
    else {
      for (j = 0; j < 5; j++) {
       if (word[j] == letter) {
         color = 2;
       }
      }
  }
       
  return color;
  
}



// main function, run after console reset
void main(void) {
  
  bool playing = true;
  
  int tries = 0;
  int game_over = 5;
  int i = 0;
  int j = 0;
  int arr[5] = {0,0,0,0,0};
  char pad;
  int x = 5;
  int y = 4;
  int color;
  
  char word[5] = {'S', 'L', 'I', 'D', 'E'};
  
  
  
  char entered_word[5];
  
  

  // set palette colors
  pal_col(0,0x0F);	// set screen to dark blue
  pal_col(1,0x28);
  pal_col(2,0x19);
  pal_col(3,0x30);	// white

  // write text to name table
  
  vram_adr(NTADR_A(12,3));
  vram_write("WORDLE", 6);
  
  vram_adr(NTADR_A(6,7));
  vram_write("PRESS UP OR DOWN TO", 19);
  
  vram_adr(NTADR_A(9,9));
  vram_write("CHOOSE LETTER", 13);
  
  
  
  
  vram_adr(NTADR_A(5,12));
  vram_write("PRESS RIGHT OR LEFT TO", 22);
  
  vram_adr(NTADR_A(9,14));
  vram_write("MOVE LETTERS", 12);
  
  vram_adr(NTADR_A(10,17));
  vram_write("PRESS A TO", 10);
  
  vram_adr(NTADR_A(10,19));
  vram_write("ENTER WORD", 10);
  
  vram_adr(NTADR_A(8,24));
  vram_write("PRESS A TO START", 16);
  
  
  
  ppu_on_all();

  
  while (1) {
  
  pad = pad_trigger(0);
  
  if(pad&PAD_A){
    ppu_off();
    vram_adr(NTADR_A(0, 0));
    vram_fill(0x00, 32*30);
    print_word(x, y);
    print_word(x,y+5);
    print_word(x,y+10);
    print_word(x,y+15);
    print_word(x,y+20);
    break;
  }
  }
  
  

  // enable PPU rendering (turn on screen)
  ppu_on_all();

  // infinite loop
  while (1) {
   
   pad = pad_trigger(0);
   if(playing == true){ 
    if (pad&PAD_DOWN && arr[j]<25){
      arr[j]+=1;
      cycle_letters(arr[j],x,y);
    }
    
    if (pad&PAD_UP && arr[j]>0){
      arr[j]-=1;
      cycle_letters(arr[j],x,y);
    }
    
    if (pad&PAD_LEFT && x > 0) {

      x-=5;
      j-=1;
    }
    
    if (pad&PAD_RIGHT && x < 25) {
      x+= 5;
      j+=1;
    }
    
    if (pad&PAD_A && y < 25) {
      tries+=1;
      game_over = 0;
      x=5;
      for (i=0;i<5;i++){ 
       entered_word[i] = 'A' + arr[i];
       color = check_word(word, entered_word[i], i);
       if (color == 1) {
        game_over += 1; 
       }
       ppu_off();
       fill_letter(x, y, color, arr[i]);
       ppu_on_all();
       arr[i] = 0;
       x += 5;
      }
      x = 5;
      y += 5;
      j = 0;
      
      if(game_over == 5 || tries > 4){
      	playing = false;
      }
      
      
      
    }
   }
   else {
      
     ppu_off();
     
     vram_adr(NTADR_A(0, 0));
     vram_fill(0x00, 32*30);
      
     if (game_over == 5) {
     vram_adr(NTADR_A(8,10));
     vram_write("CONGRATULATIONS", 15);
       
     vram_adr(NTADR_A(6,13));
     vram_write("YOU GUESSED THE WORD", 20);
     
      
     }
     
     else {
       
     vram_adr(NTADR_A(4,10));
     vram_write("THANKS FOR PLAYING TODAY", 25);
     vram_adr(NTADR_A(5,15));
     vram_write("BETTER LUCK NEXT TIME", 21);
       
     }
     ppu_on_all();
     
     break;
      
    }
    
    

    ppu_wait_frame();
    
    
  }
  
  
  while (1) {
    
    
  }
  
}
