#ifndef COMMAND_H_
#define COMMAND_H_

#include"stdint.h"
#include"keyboard.h"

extern const uint8_t command_advanced_key_mapping[ADVANCED_KEY_NUM];

void command_prase(uint8_t *buf,uint8_t len);

#endif