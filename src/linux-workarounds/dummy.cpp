#ifdef LINUX
	#include <cstdio>
	#include "TypeDef.h"

	void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) { 
	  printf("not implemented %s\n", __func__);
	}

	void detachInterrupt(uint8_t interruptNum) {
	  printf("not implemented %s\n", __func__);
	}

	_BIN BIN;
	MockDebugSerial Serial;

	#ifdef NOWIRINGIPI
                void digitalWrite(RADIOLIB_PIN_TYPE pin, RADIOLIB_PIN_STATUS value) {
                }
                RADIOLIB_PIN_STATUS digitalRead(RADIOLIB_PIN_TYPE pin) {
                  return 0;
                }
                void pinMode(RADIOLIB_PIN_TYPE pin, RADIOLIB_PIN_MODE mode) {
                }
        #endif

#endif
