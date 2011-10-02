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

#include "baskstat-court.h"

#include <stdlib.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

#define WFACTOR 30

G_DEFINE_TYPE (BaskstatCourt, baskstat_court, GTK_TYPE_DRAWING_AREA);

typedef struct {
    gchar *path;
    gfloat x;
    gfloat y;
    BaskstatPlayer *player;
} BasketObject;

static BasketObject *
baskstat_new_basket ()
{
    BasketObject *basket = malloc (sizeof (BasketObject));
    basket->path = DATA_DIR "/success.svg";
    basket->x = 0;
    basket->y = 0;
    basket->player = NULL;

    return basket;
}

static gboolean
draw_svg (const gchar *path, cairo_t *cr, gint width, gint x, gint y)
{
    GError *error = NULL;
    RsvgHandle *rsvg_handle = NULL;
    RsvgDimensionData rsvg_dimension_data;
    gint w;
    gfloat factor;

    rsvg_handle = rsvg_handle_new_from_file (path, &error);
    if (!rsvg_handle)
    {
        g_print ("Couldn't load %s\n", path);
        return FALSE;
    }

    if (error != NULL)
    {
        g_print ("%s\n", error->message);
        g_error_free (error);
        return FALSE;
    } else {
        rsvg_handle_get_dimensions (rsvg_handle, &rsvg_dimension_data);
        w = rsvg_dimension_data.width;

        factor = (float)width / (float)w;

        if (x > 0)
            cairo_translate (cr, x - width / 2.0, y - width / 2.0);
        cairo_scale (cr, factor, factor);

        rsvg_handle_render_cairo (rsvg_handle, cr);

        cairo_scale (cr, 1/factor, 1/factor);
        if (x > 0)
            cairo_translate (cr, -(x - width / 2.0), -(y - width / 2.0));

        g_object_unref (rsvg_handle);
    }
    return TRUE;
}

static gboolean
add_basket (GtkWidget       *widget,
            GdkEventButton  *event,
            gpointer        user_data)
{
    BasketObject *new_basket = baskstat_new_basket ();
    BaskstatCourt *this = BASKSTAT_COURT (widget);
    gint width, height;

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    if (event->button == 1) {
        new_basket->path = DATA_DIR "/success.svg";
    } else if (event->button == 3) {
        new_basket->path = DATA_DIR "/fail.svg";
    }

    new_basket->x = event->x / (float)width;
    new_basket->y = event->y / (float)width;
    new_basket->player = this->current_player;
    this->basket_object_list = g_list_append (this->basket_object_list, new_basket);

    gtk_widget_queue_draw_area (widget, 0, 0, width, height);
    return FALSE;
}

static gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    guint width, height;
    GdkRGBA color;
    GList *obj_list;
    BasketObject *obj;
    PangoLayout *layout;
    gchar player_text[100] = {0};
    GdkColor white = {0, 65535, 65535, 65535};
    BaskstatCourt *this = BASKSTAT_COURT (widget);

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    gtk_style_context_get_color (gtk_widget_get_style_context (widget),
                                 0, &color);
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_fill (cr);

    draw_svg (DATA_DIR "/court.svg", cr, width, -1, -1);

    gdk_cairo_set_source_color (cr, &white);

    for (obj_list = this->basket_object_list; obj_list; obj_list = obj_list->next) {
        gint objx, objy;
        gint w, h;
        gdouble factor;
        gdouble text_width = (width / 2.0) / (float)WFACTOR;
        gdouble text_height;

        obj = (BasketObject*)(obj_list->data);
        objx = obj->x * width;
        objy = obj->y * width;

        draw_svg (obj->path, cr, width / WFACTOR, objx, objy);

        layout = pango_cairo_create_layout (cr);
        if (obj->player) {
            snprintf (player_text, 100, "<b>%d</b>", obj->player->number);
            pango_layout_set_markup (layout, player_text, -1);
            pango_layout_get_size (layout, &w, &h);
            factor = text_width / (float)(w / PANGO_SCALE);
            text_height = (h / PANGO_SCALE) * factor;

            cairo_translate (cr, objx - text_width / 2.0, objy - text_height / 2.0);
            cairo_scale (cr, factor, factor);
            pango_cairo_show_layout (cr, layout);
            cairo_scale (cr, 1/factor, 1/factor);
            cairo_translate (cr, -(objx - text_width / 2.0), -(objy - text_height / 2.0));
        }
    }

    return FALSE;
}

GtkWidget *
baskstat_court_new ()
{
    GtkWidget *obj;
    obj = g_object_new (BASKSTAT_TYPE_COURT, NULL);
    gtk_widget_set_size_request (obj, 800, 500);
    gtk_widget_add_events (obj, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    g_signal_connect (G_OBJECT (obj), "draw", G_CALLBACK (draw_callback), NULL);
    g_signal_connect (G_OBJECT (obj), "button-press-event", G_CALLBACK (add_basket), NULL);
    return obj;
}

static void
baskstat_court_init (BaskstatCourt *self)
{
}

static void
baskstat_court_class_init (BaskstatCourtClass *klass)
{
}

void
baskstat_court_set_current_player (BaskstatCourt *court, BaskstatPlayer *p)
{
    court->current_player = p;
}
