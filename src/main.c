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


#include "config.h"
#include "baskstat-player.h"
#include "baskstat-court.h"

#include <gtk/gtk.h>
#include <stdlib.h>

#include <glib/gi18n.h>

enum
{
  COLUMN_PLAYING,
  COLUMN_NUMBER,
  COLUMN_POINTS,
  COLUMN_FOULTS,
  COLUMN_PLAYER,
  COLUMN_LIST,
  NUM_COLUMNS
};

// global basket object list
GList *basket_object_list = NULL;
GList *local_player_list = NULL;
GList *visit_player_list = NULL;
GtkWidget *basket_court = NULL;

void
baskstat_change_current_player (GtkToggleButton *button,
                                GList *player_list)
{
    BaskstatPlayer *p = NULL;
    GList *l;
    gchar pnumber[3];
    const gchar *blabel;

    if (gtk_toggle_button_get_active (button)) {
        blabel = gtk_button_get_label (GTK_BUTTON (button));
        for (l = player_list; l; l = l->next) {
            p = (BaskstatPlayer*)l->data;
            snprintf (pnumber, 3, "%d\n", p->number);
            if (!strcmp (pnumber, blabel)) {
                baskstat_court_set_current_player (BASKSTAT_COURT (basket_court), p);
            }
        }
    }
}

static void
playing_toggled (GtkCellRendererToggle *cell,
                 gchar                 *path_str,
                 gpointer               data)
{
    GtkTreeModel *model = (GtkTreeModel *)data;
    GtkTreeIter  iter;
    GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
    gboolean playing;
    BaskstatPlayer *player;
    GList *l, *list;
    gint nplaying = 0;

    /* get toggled iter */
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_model_get (model, &iter, COLUMN_PLAYING, &playing, -1);
    gtk_tree_model_get (model, &iter, COLUMN_PLAYER, &player, -1);
    gtk_tree_model_get (model, &iter, COLUMN_LIST, &list, -1);

    for (l = list; l; l = l->next) {
        if (((BaskstatPlayer*)(l->data))->playing) {
            nplaying++;
        }
    }

    /* do something with the value */
    playing ^= 1;
    if (nplaying < 5 || !playing) {
        /* set new value */
        player->playing = playing;
        gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_PLAYING, playing, -1);
    }

    /* clean up */
    gtk_tree_path_free (path);
}

GtkWidget *
baskstat_player_widget_new (GList *player_list)
{
    GtkWidget *widget;
    GtkListStore *model;
    GtkTreeIter iter;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    BaskstatPlayer *p;
    GList *l;

    model = gtk_list_store_new (NUM_COLUMNS,
            G_TYPE_BOOLEAN,
            G_TYPE_UINT,
            G_TYPE_UINT,
            G_TYPE_UINT,
            G_TYPE_POINTER,
            G_TYPE_POINTER);

    /* add data to the list store */
    for (l = player_list; l; l = l->next) {
        p = (BaskstatPlayer*)(l->data);
        gtk_list_store_append (model, &iter);
        gtk_list_store_set (model, &iter,
                COLUMN_PLAYING, p->playing,
                COLUMN_NUMBER, p->number,
                COLUMN_POINTS, p->points,
                COLUMN_FOULTS, p->foults,
                COLUMN_PLAYER, p,
                COLUMN_LIST, player_list,
                -1);
    }

    widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
    g_object_unref (model);

    renderer = gtk_cell_renderer_toggle_new ();
    g_signal_connect (renderer, "toggled", G_CALLBACK (playing_toggled), model);

    column = gtk_tree_view_column_new_with_attributes ("",
            renderer,
            "active", COLUMN_PLAYING,
            NULL);

    /* set this column to a fixed sizing (of 50 pixels) */
    gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column),
            GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_fixed_width (GTK_TREE_VIEW_COLUMN (column), 50);
    gtk_tree_view_append_column (GTK_TREE_VIEW (widget), column);

    gtk_tree_view_columns_autosize (GTK_TREE_VIEW (widget));
    gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (widget), TRUE);

    /* column for numbers */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes ("",
            renderer,
            "text",
            COLUMN_NUMBER,
            NULL);
    gtk_tree_view_column_set_sort_column_id (column, COLUMN_NUMBER);
    gtk_tree_view_append_column (GTK_TREE_VIEW (widget), column);

    /* column for points */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes ("",
            renderer,
            "text",
            COLUMN_POINTS,
            NULL);
    gtk_tree_view_column_set_sort_column_id (column, COLUMN_POINTS);
    gtk_tree_view_append_column (GTK_TREE_VIEW (widget), column);

    /* column for foults */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes ("",
            renderer,
            "text",
            COLUMN_FOULTS,
            NULL);
    gtk_tree_view_column_set_sort_column_id (column, COLUMN_FOULTS);
    gtk_tree_view_append_column (GTK_TREE_VIEW (widget), column);

    return widget;
}

GtkWidget *
baskstat_player_playing_new (GList *player_list, GtkWidget *w)
{
    GtkWidget *widget;
    GtkWidget *radio = NULL, *prev = NULL;
    GList *l;
    gchar text[3] = {0};
    BaskstatPlayer *p;

    if (w) {
        for (l = gtk_container_get_children (GTK_CONTAINER (w)); l; l = l->next) {
            gtk_container_remove (GTK_CONTAINER (w), GTK_WIDGET (l->data));
        }
        widget = w;
    } else {
        widget = gtk_grid_new ();
    }

    for (l = player_list; l; l = l->next) {
        p = (BaskstatPlayer*)l->data;
        if (!p->playing)
            continue;

        snprintf (text, 3, "%d\n", p->number);
        if (!prev) {
            radio = gtk_radio_button_new_with_label (NULL, text);
            gtk_grid_attach (GTK_GRID (widget), radio, 0, 0, 1, 1);
        } else {
            radio = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (prev), text);
            gtk_grid_attach_next_to (GTK_GRID (widget), radio, prev, GTK_POS_RIGHT, 1, 1);
        }

        g_signal_connect (G_OBJECT (radio), "toggled", G_CALLBACK (baskstat_change_current_player), player_list);
        prev = radio;
    }
    gtk_widget_show_all (widget);

    return widget;
}

void
reload_local_playing (GtkTreeModel *tree_model,
                      GtkTreePath  *path,
                      GtkTreeIter  *iter,
                      GtkWidget    *player_playing)
{
    baskstat_player_playing_new (local_player_list, player_playing);
}

void
reload_visit_playing (GtkTreeModel *tree_model,
                      GtkTreePath  *path,
                      GtkTreeIter  *iter,
                      GtkWidget    *player_playing)
{
    baskstat_player_playing_new (visit_player_list, player_playing);
}

int
main (int argc, char **argv)
{
#ifdef ENABLE_NLS
    /* Initialize the i18n stuff */
    bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

    // declaration
    gint i;
    GtkWidget *window;
    GtkWidget *layout;

    GtkWidget *local_players_widget;
    GtkWidget *visit_players_widget;

    GtkWidget *local_players_playing;
    GtkWidget *visit_players_playing;

    // code
    gtk_init (&argc, &argv);

    // main window creation
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), _("BaskStat - Basketball match stats"));
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    // basket court creation
    basket_court = baskstat_court_new ();

    // player widgets creation
    for (i = 0; i < 12; i++) {
        BaskstatPlayer *p = baskstat_player_new ();
        p->number = i + 4;
        local_player_list = g_list_append (local_player_list, p);

        p = baskstat_player_new ();
        p->number = i + 4;
        visit_player_list = g_list_append (visit_player_list, p);
    }

    local_players_widget = baskstat_player_widget_new (local_player_list);
    local_players_playing = baskstat_player_playing_new (local_player_list, NULL);
    g_signal_connect (G_OBJECT (gtk_tree_view_get_model (GTK_TREE_VIEW (local_players_widget))),
                      "row-changed", G_CALLBACK (reload_local_playing), local_players_playing);

    visit_players_widget = baskstat_player_widget_new (visit_player_list);
    visit_players_playing = baskstat_player_playing_new (visit_player_list, NULL);
    g_signal_connect (G_OBJECT (gtk_tree_view_get_model (GTK_TREE_VIEW (visit_players_widget))),
                      "row-changed", G_CALLBACK (reload_visit_playing), visit_players_playing);

    // layout
    layout = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (layout), local_players_widget, 0, 0, 1, 2);
    gtk_grid_attach_next_to (GTK_GRID (layout), basket_court, local_players_widget, GTK_POS_RIGHT, 5, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), visit_players_widget, basket_court, GTK_POS_RIGHT, 1, 2);

    gtk_grid_attach_next_to (GTK_GRID (layout), local_players_playing, basket_court, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), visit_players_playing, local_players_playing, GTK_POS_RIGHT, 1, 1);

    gtk_container_add (GTK_CONTAINER (window), layout);

    gtk_widget_show_all (window);

    gtk_main();

    return 0;
}
