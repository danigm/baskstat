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
#include "baskstat-team.h"

#include <stdlib.h>
#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>

#include <glib/gi18n.h>

#define WFACTOR 30

G_DEFINE_TYPE (BaskstatCourt, baskstat_court, GTK_TYPE_DRAWING_AREA);

typedef struct {
    gchar *path;
    gfloat x;
    gfloat y;
    BaskstatPlayer *player;
} BasketObject;

static BasketObject *
baskstat_new_basket (BaskstatPlayer *p, gint points)
{
    BasketObject *basket = malloc (sizeof (BasketObject));
    switch (points) {
        case 0:
            basket->path = DATA_DIR "/fail.svg";
            break;
        case 1:
            basket->path = DATA_DIR "/success1.svg";
            break;
        case 2:
            basket->path = DATA_DIR "/success2.svg";
            break;
        case 3:
            basket->path = DATA_DIR "/success3.svg";
            break;
        default:
            basket->path = DATA_DIR "/success2.svg";
            break;
    }

    basket->x = 0;
    basket->y = 0;
    basket->player = p;

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

static void
baskstat_court_change_points_n (BaskstatCourt *court, gint n)
{
    gchar label[5];

    g_snprintf (label, 5, "%d", n);
    gtk_label_set_text (GTK_LABEL (court->basket_points_label), label);
    court->basket_points = n;
}

static gboolean
add_basket (GtkWidget       *widget,
            GdkEventButton  *event,
            gpointer        user_data)
{
    BasketObject *new_basket = NULL;
    BaskstatCourt *this = BASKSTAT_COURT (widget);
    gint width, height;

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    if (event->button == 1) {
        if (event->state & GDK_SHIFT_MASK) {
            baskstat_court_change_points_n (this, 3);
        } else if (event->state & GDK_CONTROL_MASK) {
            baskstat_court_change_points_n (this, 1);
        }
        new_basket = baskstat_new_basket (this->current_player, this->basket_points);
        baskstat_team_new_basket (this->current_player, this->basket_points);
    } else if (event->button == 3) {
        new_basket = baskstat_new_basket (this->current_player, 0);
    }

    // setting default 2 points
    baskstat_court_change_points_n (this, 2);

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

static void
baskstat_court_change_points (GtkButton *button,
                              BaskstatCourt *court)
{
    const gchar *blabel;

    blabel = gtk_button_get_label (GTK_BUTTON (button));
    gtk_label_set_text (GTK_LABEL (court->basket_points_label), blabel);
    court->basket_points = atoi (blabel);
}

static GtkWidget *
basket_points_widget (BaskstatCourt *court)
{
    GtkWidget *layout;
    GtkWidget *button1, *button2, *button3;
    GtkWidget *label;

    court->basket_points_label = gtk_label_new ("2");
    label = gtk_label_new (_("Points:"));

    layout = gtk_grid_new ();
    button1 = gtk_button_new_with_label ("1");
    button2 = gtk_button_new_with_label ("2");
    button3 = gtk_button_new_with_label ("3");

    gtk_grid_attach (GTK_GRID (layout), button1, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), button2, button1, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), button3, button2, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), label, button3, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), court->basket_points_label, label, GTK_POS_RIGHT, 1, 1);

    g_signal_connect (G_OBJECT (button1), "clicked", G_CALLBACK (baskstat_court_change_points), court);
    g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (baskstat_court_change_points), court);
    g_signal_connect (G_OBJECT (button3), "clicked", G_CALLBACK (baskstat_court_change_points), court);

    gtk_widget_show_all (layout);
    return layout;
}

GtkWidget *
baskstat_court_new ()
{
    GtkWidget *obj;
    BaskstatCourt *court;

    obj = g_object_new (BASKSTAT_TYPE_COURT, NULL);
    court = BASKSTAT_COURT (obj);
    gtk_widget_set_size_request (obj, 800, 400);
    gtk_widget_add_events (obj, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    g_signal_connect (G_OBJECT (obj), "draw", G_CALLBACK (draw_callback), NULL);
    g_signal_connect (G_OBJECT (obj), "button-press-event", G_CALLBACK (add_basket), NULL);

    court->current_player_widget = GTK_LABEL (gtk_label_new (_("Current player")));
    court->basket_points = 2;
    court->basket_points_widget = basket_points_widget (court);
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
    gchar text[200];
    court->current_player = p;

    g_snprintf (text, 200, "%s: %d", baskstat_team_name (p->team), p->number);
    gtk_label_set_text (court->current_player_widget, text);
}

GtkWidget *
baskstat_court_current_player_widget (BaskstatCourt *court)
{
    return GTK_WIDGET (court->current_player_widget);
}

GtkWidget *
baskstat_court_basket_points_widget (BaskstatCourt *court)
{
    return GTK_WIDGET (court->basket_points_widget);
}
