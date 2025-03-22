/* Copyright 2016 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"
#include "keycode.h"

// MIDI aliases
#define MIDI_TONE_MIN       MIDI_NOTE_C_0
#define MIDI_TONE_MAX       MIDI_NOTE_B_5
#define MIDI_OCTAVE_MIN     MIDI_OCTAVE_N2
#define MIDI_OCTAVE_MAX     MIDI_OCTAVE_7
#define MIDI_TRANSPOSE_MIN  MIDI_TRANSPOSE_N6
#define MIDI_TRANSPOSE_MAX  MIDI_TRANSPOSE_6
#define MIDI_VELOCITY_MIN   MIDI_VELOCITY_0
#define MIDI_VELOCITY_MAX   MIDI_VELOCITY_10
#define MIDI_CHANNEL_MIN    MIDI_CHANNEL_1
#define MIDI_CHANNEL_MAX    MIDI_CHANNEL_16

#ifdef MIDI_ENABLE

void midi_task(void);

typedef union {
    uint32_t raw;
    struct {
        uint8_t octave : 4;
        int8_t  transpose : 4;
        uint8_t velocity : 7;
        uint8_t channel : 4;
        uint8_t modulation_interval : 4;
    };
} MIDIConfig;

extern MIDIConfig midi_config;

void midi_init(void);
bool midi_event_handler(KeyboardEvent event, bool is_input_velocity, uint8_t input_velocity);

#        define MIDI_INVALID_NOTE 0xFF
#        define MIDI_TONE_COUNT (MIDI_TONE_MAX - MIDI_TONE_MIN + 1)

uint8_t midi_compute_note(uint16_t keycode);

#endif // MIDI_ENABLE
