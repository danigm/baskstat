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

#include "baskstat-team.h"

#include "baskstat-player.h"
#include "baskstat-court.h"

G_DEFINE_TYPE (BaskstatTeam, baskstat_team, G_TYPE_OBJECT);

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

static gboolean
baskstat_team_iter_by_player (BaskstatTeam *team, BaskstatPlayer *p, GtkTreeIter *iter)
{
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (team->player_widget));
    BaskstatPlayer *bp = NULL;
    gboolean found = FALSE;

    // finding the player in model
    gtk_tree_model_get_iter_first (model, iter);
    gtk_tree_model_get (model, iter, COLUMN_PLAYER, &bp, -1);
    if (bp->number == p->number)
        found = TRUE;
    while (!found && gtk_tree_model_iter_next (model, iter)) {
        gtk_tree_model_get (model, iter, COLUMN_PLAYER, &bp, -1);
        if (bp->number == p->number) {
            found = TRUE;
            break;
        }
    }

    return found;
}

static void
baskstat_team_change_current_player (GtkButton *button,
                                     BaskstatTeam *team)
{
    BaskstatPlayer *p = NULL;
    GList *l;
    gchar pnumber[3];
    const gchar *blabel;

    blabel = gtk_button_get_label (GTK_BUTTON (button));
    for (l = team->players; l; l = l->next) {
        p = (BaskstatPlayer*)l->data;
        snprintf (pnumber, 3, "%d\n", p->number);
        if (!strcmp (pnumber, blabel)) {
            baskstat_court_set_current_player (BASKSTAT_COURT (team->court), p);
        }
    }
}


static GtkWidget *
baskstat_team_playing (BaskstatTeam *team)
{
    GtkWidget *widget;
    GtkWidget *button = NULL, *prev = NULL;
    GList *l;
    gchar text[3] = {0};
    BaskstatPlayer *p;

    if (team->playing) {
        for (l = gtk_container_get_children (GTK_CONTAINER (team->playing)); l; l = l->next) {
            gtk_container_remove (GTK_CONTAINER (team->playing), GTK_WIDGET (l->data));
        }
        widget = team->playing;
    } else {
        widget = gtk_grid_new ();
    }

    for (l = team->players; l; l = l->next) {
        p = (BaskstatPlayer*)l->data;
        if (!p->playing)
            continue;

        snprintf (text, 3, "%d\n", p->number);
        button = gtk_button_new_with_label (text);
        if (!prev) {
            gtk_grid_attach (GTK_GRID (widget), button, 0, 0, 1, 1);
        } else {
            gtk_grid_attach_next_to (GTK_GRID (widget), button, prev, GTK_POS_RIGHT, 1, 1);
        }

        g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (baskstat_team_change_current_player), team);
        prev = button;
    }

    gtk_widget_show_all (widget);

    return widget;
}

static void
reload_playing (GtkTreeModel *tree_model,
                GtkTreePath  *path,
                GtkTreeIter  *iter,
                BaskstatTeam *team)
{
    baskstat_team_playing (team);
}

static void
cell_edited (GtkCellRendererText *cell,
             const gchar         *path_string,
             const gchar         *new_text,
             gpointer             data)
{
    GtkTreeModel *model = (GtkTreeModel *)data;
    GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter iter;
    GValue value;
    BaskstatPlayer *p;

    gtk_tree_model_get_iter (model, &iter, path);

    gtk_tree_model_get (model, &iter, COLUMN_PLAYER, &p, -1);
    p->number = atoi (new_text);

    gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_NUMBER,
            p->number, -1);

    gtk_tree_path_free (path);
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

static GtkWidget *
baskstat_team_player_widget (GList *player_list)
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
    g_object_set (renderer, "editable", TRUE, NULL);
    g_signal_connect (renderer, "edited",
            G_CALLBACK (cell_edited), model);
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

GObject *
baskstat_team_new (BaskstatCourt *court, gchar *name)
{
    GObject *obj;
    BaskstatTeam *team = NULL;

    obj = g_object_new (BASKSTAT_TYPE_TEAM, NULL);
    team = BASKSTAT_TEAM (obj);
    team->playing = NULL;
    team->court = court;
    team->team_score = 0;
    team->score_widget = gtk_label_new ("");
    team->name_widget = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (team->name_widget), name);
    gtk_label_set_markup (GTK_LABEL (team->score_widget), "<span font=\"40\">0</span>");
    return obj;
}

static void
baskstat_team_init (BaskstatTeam *self)
{
}

static void
baskstat_team_class_init (BaskstatTeamClass *klass)
{
}

void
baskstat_team_add_player (BaskstatTeam *team, BaskstatPlayer *p)
{
    team->players = g_list_append (team->players, p);
    p->team = team;
}

GtkWidget *
baskstat_team_player_widget_new (BaskstatTeam *team)
{
    GtkWidget *widget = baskstat_team_player_widget (team->players);
    team->player_widget = widget;
    return widget;
}

GtkWidget *
baskstat_team_playing_new (BaskstatTeam *team)
{
    GtkWidget *widget;
    widget = baskstat_team_playing (team);
    g_signal_connect (G_OBJECT (gtk_tree_view_get_model (GTK_TREE_VIEW (team->player_widget))),
                      "row-changed", G_CALLBACK (reload_playing), team);
    team->playing = widget;
    return widget;
}

void
baskstat_team_new_basket (BaskstatPlayer *p, gint score)
{
    char newtext[50];
    GtkTreeIter iter;
    BaskstatTeam *team = p->team;
    GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (team->player_widget));

    team->team_score += score;
    p->points += score;

    baskstat_team_iter_by_player (team, p, &iter);
    gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_POINTS, p->points, -1);

    g_snprintf (newtext, 50, "<span font=\"40\">%d</span>", team->team_score);
    gtk_label_set_markup (GTK_LABEL (team->score_widget), newtext);
}

const gchar *
baskstat_team_name (BaskstatTeam *team)
{
    return gtk_entry_get_text (GTK_ENTRY (team->name_widget));
}

void
baskstat_team_serialize (BaskstatTeam *team, FILE *file)
{
    char buffer[255];
    GList *l;
    BaskstatPlayer *p;

    snprintf (buffer, 255, "\n\"team\": \n {");
    fwrite (buffer, sizeof (char), strlen (buffer), file);

    snprintf (buffer, 255, "\n\"name\": \"%s\",", baskstat_team_name (team));
    fwrite (buffer, sizeof (char), strlen (buffer), file);

    snprintf (buffer, 255, "\n\"players\": [");
    fwrite (buffer, sizeof (char), strlen (buffer), file);

    for (l = team->players; l; l = l->next) {
        p = BASKSTAT_PLAYER (l->data);
        snprintf (buffer, 255, "\n{");

        fwrite (buffer, sizeof (char), strlen (buffer), file);
        snprintf (buffer, 255, "\n\"number\": %d, \"points\": %d", p->number, p->points);
        fwrite (buffer, sizeof (char), strlen (buffer), file);

        if (l->next)
            snprintf (buffer, 255, "},");
        else
            snprintf (buffer, 255, "}");
        fwrite (buffer, sizeof (char), strlen (buffer), file);
    }
    snprintf (buffer, 255, "\n]");
    fwrite (buffer, sizeof (char), strlen (buffer), file);

    snprintf (buffer, 255, "\n}");
    fwrite (buffer, sizeof (char), strlen (buffer), file);
}

void
baskstat_team_deserialize (BaskstatTeam *team, JsonNode *node)
{
}
