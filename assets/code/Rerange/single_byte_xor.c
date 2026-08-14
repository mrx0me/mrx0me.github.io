#include <stdio.h>
#include <stdint.h>

int main(void){
  char *pcVar1;
  uint8_t abStack_148 [128];
  uint8_t local_c8 [128];
  uint8_t local_48 [32];
  int local_18;
  
  local_48[0] = 8;
  local_48[1] = 0x3f;
  local_48[2] = 8;
  local_48[3] = 0x3b;
  local_48[4] = 0x34;
  local_48[5] = 0x3d;
  local_48[6] = 0x3f;
  local_48[7] = 0x21;
  local_48[8] = 0x29;
  local_48[9] = 0x6b;
  local_48[10] = 0x34;
  local_48[0xb] = 0x3d;
  local_48[0xc] = 0x36;
  local_48[0xd] = 0x69;
  local_48[0xe] = 0x38;
  local_48[0xf] = 0x23;
  local_48[0x10] = 0x6d;
  local_48[0x11] = 0x69;
  local_48[0x12] = 5;
  local_48[0x13] = 0x6a;
  local_48[0x14] = 0x38;
  local_48[0x15] = 0x3c;
  local_48[0x16] = 0x2f;
  local_48[0x17] = 0x29;
  local_48[0x18] = 0x39;
  local_48[0x19] = 0x6e;
  local_48[0x1a] = 0x2e;
  local_48[0x1b] = 0x6a;
  local_48[0x1c] = 0x28;
  local_48[0x1d] = 5;
  local_48[0x1e] = 0x3e;
  local_48[0x1f] = 0x27;
  local_18 = 0x20;
  int local_28 = 0xc3 ^ 0xa6 ^0x3f;
  for (int local_c = 0; local_c8[local_c] != 0; local_c = local_c + 1) {
      if ((local_c8[local_c] == 10) || (local_c8[local_c] == 0xd)) {
        local_c8[local_c] = 0;
        break;
      }
    }
   for (int local_14 = 0; local_c8[local_14] != 0; local_14 = local_14 + 1) {}
   for (int local_c = 0; local_c < local_18; local_c = local_c + 1) {
      abStack_148[local_c] = (uint8_t)local_28 ^ local_48[local_c];
      printf("%c", abStack_148[local_c]);
    }
    abStack_148[local_18] = 0;
    return 0;
}
