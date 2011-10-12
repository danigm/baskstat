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

#ifndef _BASKSTAT_PLAYER_H
#define _BASKSTAT_PLAYER_H

#include <glib-object.h>
#include <json-glib/json-glib.h>

typedef struct _BaskstatTeam BaskstatTeam;

#define BASKSTAT_TYPE_PLAYER            (baskstat_player_get_type ())
#define BASKSTAT_PLAYER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASKSTAT_TYPE_PLAYER, BaskstatPlayer))
#define BASKSTAT_IS_PLAYER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASKSTAT_TYPE_PLAYER))
#define BASKSTAT_PLAYER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASKSTAT_TYPE_PLAYER, BaskstatPlayerClass))
#define BASKSTAT_IS_PLAYER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASKSTAT_TYPE_PLAYER))
#define BASKSTAT_PLAYER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BASKSTAT_TYPE_PLAYER, BaskstatPlayerClass))

typedef struct _BaskstatPlayer    BaskstatPlayer;
typedef struct _BaskstatPlayerClass BaskstatPlayerClass;

struct _BaskstatPlayer {
    GObject parent;

    /* instance members */
    gint number;
    gchar *name;
    gboolean playing;
    gint points;
    gint rebounds;
    gint steals;
    gint assists;
    gint turnover;
    gint foults;
    BaskstatTeam *team;
};

struct _BaskstatPlayerClass {
    GObjectClass parent_class;

    /* class members */
};

GType baskstat_player_get_type (void);

/*
 * Method definitions.
 */

GObject * baskstat_player_new ();

#endif
