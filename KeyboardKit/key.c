/*
 * key.c
 */
#include "key.h"
#include "keyboard_def.h"

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
