/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "key.h"

void key_update(Key* key,bool state)
{
    if ((!(key->state)) && state)
    {
        if (key->key_cb[KEY_EVENT_DOWN])
            key->key_cb[KEY_EVENT_DOWN](key);
    }
    if ((key->state) && (!state))
    {
        if (key->key_cb[KEY_EVENT_UP])
            key->key_cb[KEY_EVENT_UP](key);
    }
    key->state = state;
}

void key_attach(Key* key, KEY_EVENT e, key_cb_t cb)
{
    key->key_cb[e] = cb;
}
