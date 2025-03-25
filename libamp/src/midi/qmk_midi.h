#pragma once

#include "keyboard_conf.h"
#ifdef MIDI_ENABLE
#include "stdint.h"
#include "keyboard_def.h"
/** \brief MIDI Class Driver Event Packet.
 *
 *  Type define for a USB MIDI event packet, used to encapsulate sent and received MIDI messages from a USB MIDI interface.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint8_t  Event; /**< MIDI event type, constructed with the \ref MIDI_EVENT() macro. */
    
	uint8_t  Data1; /**< First byte of data in the MIDI event. */
	uint8_t  Data2; /**< Second byte of data in the MIDI event. */
	uint8_t  Data3; /**< Third byte of data in the MIDI event. */
} __PACKED MIDIEventPacket;

#    include "midi.h"
extern MIDIDevice midi_device;
void              setup_midi(void);
void              send_midi_packet(MIDIEventPacket* event);
bool              recv_midi_packet(MIDIEventPacket* const event);
#endif
