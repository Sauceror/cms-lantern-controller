/*
 * Version.c
 *
 *  Created on: Sep 12, 2023
 *      Author: lq
 */
#include <GetVer.h>
#include "stdio.h"


const uint32_t FW_BUILD = 2502191840;     // yyMMddHHmm

const uint16_t FW_VERSION = 0x0101;        // 0130, max size = 4

int GetVer(char *buf)
{
#ifdef DEBUG
    const char *dr = "Debug";
#else
    const char *dr = "Release";
#endif
   return sprintf(buf, "%s Version:%04x Build:%lu", dr, FW_VERSION, FW_BUILD);
}
