// free RAM check for debugging. SRAM for ATmega328p = 2048Kb.
int availableMemory() {
    int size = 8*1024;
    byte *buf;
    while ((buf = (byte *) malloc(--size)) == NULL);
        free(buf);
    return size;
}

int freeRam () {  
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

