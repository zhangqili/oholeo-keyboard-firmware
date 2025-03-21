/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef PACKET_H
#define PACKET_H

#include "stdint.h"
#include "keyboard_def.h"
#include "dynamic_key.h"
#include "storage.h"

enum {
  PACKET_DATA_ADVANCED_KEY = 0x00,
  PACKET_DATA_RGB_SWITCH = 0x01,
  PACKET_DATA_RGB_CONFIG = 0x02,
  PACKET_DATA_KEYMAP = 0x03,
  PACKET_DATA_DYNAMIC_KEY = 0x04
};

typedef struct __PacketBase
{
  uint8_t code;
  uint8_t buf[];
} __PACKED PacketBase;

typedef struct __PacketCommand
{
  uint8_t code;
} __PACKED PacketCommand;

typedef struct __PacketData
{
  uint8_t code;
  uint8_t type;
} __PACKED PacketData;

typedef struct __PacketAdvancedKey
{
  uint8_t code;
  uint8_t type;
  uint16_t index;
  AdvancedKeyConfigurationNormalized data;
} PacketAdvancedKey;

typedef struct __PacketRGBSwitch
{
  uint8_t code;
  uint8_t type;
  uint8_t state;
} __PACKED PacketRGBSwitch;

typedef struct __PacketRGBConfig
{
  uint8_t code;
  uint8_t type;
  uint16_t index;
  uint8_t mode;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  float speed;
} __PACKED PacketRGBConfig;

typedef struct __PacketRGBConfigs
{
  uint8_t code;
  uint8_t type;
  uint8_t length;
  struct
  {
    uint16_t index;
    uint8_t mode;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    float speed;
  } __PACKED data[];
} __PACKED PacketRGBConfigs;


typedef struct __PacketKeymap
{
  uint8_t code;
  uint8_t type;
  uint8_t layer;
  uint16_t start;
  uint8_t length;
  uint16_t keymap[];
} __PACKED PacketKeymap;

typedef struct __PacketDynamicKey
{
  uint8_t code;
  uint8_t type;
  uint8_t index;
  uint8_t reserved;
  uint8_t dynamic_key[];
} __PACKED PacketDynamicKey;

typedef struct __PacketDebug
{
  uint8_t code;
  uint8_t length;
  struct
  {
    uint16_t index;
    uint8_t state;
    float raw;
    float value;
  } __PACKED data[];
} __PACKED PacketDebug;

#endif //PACKET_H
