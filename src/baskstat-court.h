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

#ifndef _BASKSTAT_COURT_H
#define _BASKSTAT_COURT_H

#include "baskstat-player.h"

#include <gtk/gtk.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

#define BASKSTAT_TYPE_COURT            (baskstat_court_get_type ())
#define BASKSTAT_COURT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASKSTAT_TYPE_COURT, BaskstatCourt))
#define BASKSTAT_IS_COURT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASKSTAT_TYPE_COURT))
#define BASKSTAT_COURT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASKSTAT_TYPE_COURT, BaskstatCourtClass))
#define BASKSTAT_IS_COURT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASKSTAT_TYPE_COURT))
#define BASKSTAT_COURT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BASKSTAT_TYPE_COURT, BaskstatCourtClass))

typedef struct _BaskstatCourt    BaskstatCourt;
typedef struct _BaskstatCourtClass BaskstatCourtClass;

struct _BaskstatCourt {
    GtkDrawingArea parent;

    /* instance members */
    GList *basket_object_list;
    BaskstatPlayer *current_player;
    GtkLabel *current_player_widget;
    gint basket_points;
    GtkWidget *basket_points_widget;
    GtkWidget *basket_points_label;
};

struct _BaskstatCourtClass {
    GtkDrawingAreaClass parent_class;

    /* class members */
};

GType baskstat_court_get_type (void);

/*
 * Method definitions.
 */

GtkWidget * baskstat_court_new ();
void baskstat_court_set_current_player (BaskstatCourt *court, BaskstatPlayer *p);
GtkWidget * baskstat_court_current_player_widget (BaskstatCourt *court);
GtkWidget * baskstat_court_basket_points_widget (BaskstatCourt *court);
void baskstat_court_remove_last (BaskstatCourt *court);

JsonNode * baskstat_court_serialize (BaskstatCourt *court);
void baskstat_court_deserialize (BaskstatCourt *court, JsonNode *node);

#endif
