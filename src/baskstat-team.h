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

#ifndef _BASKSTAT_TEAM_H
#define _BASKSTAT_TEAM_H

#include <gtk/gtk.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

typedef struct _BaskstatPlayer BaskstatPlayer;
typedef struct _BaskstatCourt BaskstatCourt;

#define BASKSTAT_TYPE_TEAM            (baskstat_team_get_type ())
#define BASKSTAT_TEAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASKSTAT_TYPE_TEAM, BaskstatTeam))
#define BASKSTAT_IS_TEAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASKSTAT_TYPE_TEAM))
#define BASKSTAT_TEAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASKSTAT_TYPE_TEAM, BaskstatTeamClass))
#define BASKSTAT_IS_TEAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASKSTAT_TYPE_TEAM))
#define BASKSTAT_TEAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BASKSTAT_TYPE_TEAM, BaskstatTeamClass))

typedef struct _BaskstatTeam    BaskstatTeam;
typedef struct _BaskstatTeamClass BaskstatTeamClass;

struct _BaskstatTeam {
    GObject parent;

    /* instance members */
    GList *players;
    GtkWidget *playing;
    GtkWidget *player_widget;
    GtkWidget *score_widget;
    GtkWidget *name_widget;
    BaskstatCourt *court;
    gint team_score;
    gboolean local;
};

struct _BaskstatTeamClass {
    GObjectClass parent_class;

    /* class members */
};

GType baskstat_team_get_type (void);

/*
 * Method definitions.
 */

GObject * baskstat_team_new (BaskstatCourt *court, gchar *name, gboolean local);
void baskstat_team_add_player (BaskstatTeam *team, BaskstatPlayer *p);
GtkWidget * baskstat_team_player_widget_new (BaskstatTeam *team);
GtkWidget * baskstat_team_playing_new (BaskstatTeam *team);
void baskstat_team_new_basket (BaskstatPlayer *p, gint score);
const gchar * baskstat_team_name (BaskstatTeam *team);

void baskstat_team_serialize (BaskstatTeam *team, FILE *file);
void baskstat_team_deserialize (BaskstatTeam *team, JsonObject *obj);

#endif
