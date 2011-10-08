/**
 * Copyright (C) 2011 Daniel Garcia <danigm@wadobo.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify
 * it under the terms of the GNU General Public License as published
 * by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "baskstat-player.h"

G_DEFINE_TYPE (BaskstatPlayer, baskstat_player, G_TYPE_OBJECT);

GObject *
baskstat_player_new ()
{
    GObject *obj;
    BaskstatPlayer *p;

    obj = g_object_new (BASKSTAT_TYPE_PLAYER, NULL);
    p = BASKSTAT_PLAYER (obj);

    p->number = 4;
    p->playing = FALSE;
    p->points = 0;
    p->rebounds = 0;
    p->steals = 0;
    p->assists = 0;
    p->turnover = 0;
    p->foults = 0;

    return obj;
}

static void
baskstat_player_init (BaskstatPlayer *self)
{
}

static void
baskstat_player_class_init (BaskstatPlayerClass *klass)
{
}

JsonNode *
baskstat_player_serialize (BaskstatPlayer *player)
{
    return NULL;
}

void
baskstat_player_deserialize (BaskstatPlayer *player, JsonNode *node)
{
}
