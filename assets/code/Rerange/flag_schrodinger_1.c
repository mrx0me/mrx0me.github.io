#include <stdio.h>
#include <stdint.h>

int ks(int *param_1)

{
  *param_1 = *param_1 * 0x19660d + 0x3c6ef35f;
  return *param_1 >> 0x18;
}

void flag(void){
  uint8_t bVar1;
  int uVar2;
  long long lVar3;
  uint8_t *pbVar4;
  int local_8c;
  uint8_t local_88 [32];
  uint8_t local_21;
  unsigned long long local_20;
  int ct []= { 0x0d, 0xea, 0x74, 0x35, 0x4c, 0xb2, 0xa8, 0xf9, 0xdc, 0x79, 0x22, 0xdf, 0x44, 0xd4, 0x28, 0xcd, 0x77, 0xf0, 0xb7, 0x8f, 0x58, 0x9a, 0xcd, 0x25, 0x3c, 0xa0, 0xdc, 0x69, 0x89, 0x79, 0x96, 0x71 }; 
  
  local_8c = 0x13579bdf;
  for (local_20 = 0; local_20 < 0x20; local_20 = local_20 + 1) {
    local_21 = (char)local_20 * '\x1f' + 7;
    bVar1 = ct[local_20];
    uVar2 = ks(&local_8c);
    local_88[local_20] = uVar2 ^ bVar1 ^ local_21;
  }
  puts("FLAG:");
  puts((char *)local_88);
  return;
}

int main(){
    flag();
    return 0;
}

