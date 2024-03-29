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

#include "baskstat-window.h"
#include "baskstat-player.h"
#include "baskstat-court.h"
#include "baskstat-team.h"
#include "baskstat-window.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <glib/gi18n.h>

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


G_DEFINE_TYPE (BaskstatWindow, baskstat_window, GTK_TYPE_WINDOW);

static void
baskstat_window_init (BaskstatWindow *self)
{
}

static void
baskstat_window_class_init (BaskstatWindowClass *klass)
{
}

static gboolean
new_match (GtkWidget *widget, BaskstatWindow *window)
{
    printf ("NEW MATCH");
    return FALSE;
}

static gboolean
open_dialog (GtkWidget *widget, BaskstatWindow *window)
{
    GtkWidget *dialog;
    JsonParser *parser;
    JsonNode *root;
    GError *error;

    dialog = gtk_file_chooser_dialog_new (_("Open match"),
            GTK_WINDOW (window),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
            NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        parser = json_parser_new ();
        error = NULL;
        json_parser_load_from_file (parser, filename, &error);
        if (error)
        {
            g_print ("Unable to parse `%s': %s\n", filename, error->message);
            g_error_free (error);
            g_object_unref (parser);
            gtk_widget_destroy (dialog);
            g_free (filename);
            return FALSE;
        }

        root = json_parser_get_root (parser);
        baskstat_window_deserialize (window, root);

        g_object_unref (parser);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);

    return FALSE;
    return FALSE;
}

static gboolean
save_dialog (GtkWidget *widget, BaskstatWindow *window)
{
    GtkWidget *dialog;
    JsonNode *root;

    dialog = gtk_file_chooser_dialog_new (_("Save as"),
            GTK_WINDOW (window),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
            NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        FILE *f;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

        f = fopen (filename, "w");
        baskstat_window_serialize (window, f);
        fclose (f);

        g_free (filename);
    }

    gtk_widget_destroy (dialog);

    return FALSE;
}

static gboolean
close_win_cb (GtkWidget *widget, BaskstatWindow *window)
{
    gtk_widget_destroy (widget);
    return FALSE;
}

static gboolean
report_cb (GtkWidget *widget, cairo_t *cr, BaskstatWindow *window)
{
    gint width, height;

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    baskstat_window_simple_report (window, cr, width, height);
    return FALSE;
}

static gboolean
make_report (GtkWidget *widget, BaskstatWindow *window)
{
    cairo_surface_t *surface = NULL;
    cairo_t *cr;

    GtkWidget *w;
    GtkWidget *darea;

    w = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (w, 600, 700);
    darea = gtk_drawing_area_new ();
    g_signal_connect (G_OBJECT (darea), "draw", G_CALLBACK (report_cb), window);

    gtk_container_add (GTK_CONTAINER (w), darea);
    g_signal_connect (w, "destroy", G_CALLBACK (close_win_cb), NULL);
    gtk_widget_show_all (w);
}

static gboolean
close_cb (GtkWidget *widget, BaskstatWindow *window)
{
    gtk_main_quit ();
    return FALSE;
}

static gboolean
undo_cb (GtkWidget *widget, BaskstatWindow *window)
{
    baskstat_court_remove_last (BASKSTAT_COURT (window->basket_court));
    return FALSE;
}

static gboolean
about_cb (GtkWidget *widget, BaskstatWindow *window)
{
    const gchar *authors[] = {"Daniel Garcia <danigm@wadobo.com>", NULL};
    const gchar *artists[] = {"Daniel Garcia <danigm@wadobo.com>", NULL};

    gtk_show_about_dialog (GTK_WINDOW (window),
            "name", _("BaskStat"),
            "version", VERSION,
            "authors", authors,
            "artists", artists,
            "website", "http://github.com/danigm/baskstat",
            "translator-credits", _("translator-credits"),
            NULL);

    return FALSE;
}

static const GtkActionEntry baskstat_menu_entries [] = {
    /* Toplevel */

    { "File", NULL, N_("_File") },
    { "Edit", NULL, N_("_Edit") },
    { "Help", NULL, N_("Help") },

    /* File menu */

    { "NewFile", GTK_STOCK_NEW, N_("_New"), "<control>N",
      N_("Create a new file"),
      G_CALLBACK (new_match) },

    { "OpenFile", GTK_STOCK_OPEN, N_("_Open"), "<control>O",
      N_("Open a file"),
      G_CALLBACK (open_dialog) },

    { "SaveFile", GTK_STOCK_SAVE, N_("_Save"), "<control>S",
      N_("Save current match"),
      G_CALLBACK (save_dialog) },

    { "Report", GTK_STOCK_SAVE, N_("_Report"), "<control>R",
      N_("Make a current match report"),
      G_CALLBACK (make_report) },

    { "Quit", GTK_STOCK_QUIT, N_("_Quit"), "<control>Q",
      N_("Quit"),
      G_CALLBACK (close_cb) },

    /* edit menu */
    { "Delete", GTK_STOCK_UNDO, N_("_Delete"), "<control>Z",
      N_("Delete last basket"),
      G_CALLBACK (undo_cb) },
    { "Delete2", GTK_STOCK_UNDO, N_("_Delete"), "Delete",
      N_("Delete last basket"),
      G_CALLBACK (undo_cb) },

    /* Help menu */
    { "About", GTK_STOCK_ABOUT, N_("About"), "",
      N_("About"),
      G_CALLBACK (about_cb) },
};

static GtkWidget *
generate_menu (BaskstatWindow *window)
{
    GtkWidget *menu;
    GtkUIManager *manager;
    GError *error = NULL;
    GtkActionGroup *menu_action_group = NULL;
    GtkAccelGroup *accel = NULL;

    manager = gtk_ui_manager_new ();
    gtk_ui_manager_add_ui_from_file (manager, DATA_DIR "/ui/baskstat-menu-ui.xml", &error);
    if (error != NULL)
    {
        g_warning (_("Could not merge baskstat-menu-ui.xml: %s"), error->message);
        g_error_free (error);
    }

    menu_action_group = gtk_action_group_new ("MenuActions");
    gtk_action_group_set_translation_domain (menu_action_group, NULL);
    gtk_action_group_add_actions (menu_action_group, baskstat_menu_entries,
                        G_N_ELEMENTS (baskstat_menu_entries), window);

    gtk_ui_manager_insert_action_group (manager, menu_action_group, 0);

    menu = gtk_ui_manager_get_widget (manager, "/menubar");

    accel = gtk_ui_manager_get_accel_group (manager);
    gtk_window_add_accel_group (GTK_WINDOW (window), accel);

    return menu;
}

GtkWidget *
baskstat_window_new ()
{
    GtkWidget *obj;
    BaskstatWindow *window;
    obj = g_object_new (BASKSTAT_TYPE_WINDOW, NULL);
    window = BASKSTAT_WINDOW (obj);

    // declaration
    gint i;
    GtkWidget *menu;
    GtkWidget *layout;
    GtkWidget *central_layout;
    GtkWidget *aspect_frame;

    GtkWidget *local_players_widget;
    GtkWidget *visit_players_widget;

    GtkWidget *local_players_playing;
    GtkWidget *visit_players_playing;

    GtkWidget *current_player_widget;
    GtkWidget *points_widget;
    GtkWidget *timer;

    menu = generate_menu (window);
    gtk_window_set_title (GTK_WINDOW (window), _("BaskStat - Basketball match stats"));
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    // basket court creation
    window->basket_court = baskstat_court_new ();
    aspect_frame = gtk_aspect_frame_new ("", 0.5, 0.5, 0, TRUE);
    gtk_container_add (GTK_CONTAINER (aspect_frame), window->basket_court);


    // teams creation
    window->local = BASKSTAT_TEAM (baskstat_team_new (BASKSTAT_COURT (window->basket_court), _("local"), TRUE));
    window->visit = BASKSTAT_TEAM (baskstat_team_new (BASKSTAT_COURT (window->basket_court), _("visit"), FALSE));
    for (i = 0; i < 12; i++) {
        BaskstatPlayer *p = BASKSTAT_PLAYER (baskstat_player_new ());
        p->number = i + 4;
        baskstat_team_add_player (window->local, p);

        p = BASKSTAT_PLAYER (baskstat_player_new ());
        p->number = i + 4;
        baskstat_team_add_player (window->visit, p);

        baskstat_court_set_current_player (BASKSTAT_COURT (window->basket_court), p);
    }

    local_players_widget = baskstat_team_player_widget_new (window->local);
    local_players_playing = baskstat_team_playing_new (window->local);

    visit_players_widget = baskstat_team_player_widget_new (window->visit);
    visit_players_playing = baskstat_team_playing_new (window->visit);

    current_player_widget = baskstat_court_current_player_widget (BASKSTAT_COURT (window->basket_court));
    points_widget = baskstat_court_basket_points_widget (BASKSTAT_COURT (window->basket_court));

    timer = GTK_WIDGET (BASKSTAT_COURT (window->basket_court)->timer);
    gtk_widget_set_halign (timer, GTK_ALIGN_CENTER);

    // central layout
    central_layout = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (central_layout), timer, 1, 0, 5, 1);
    gtk_grid_attach_next_to (GTK_GRID (central_layout), aspect_frame, timer, GTK_POS_BOTTOM, 4, 1);

    gtk_grid_attach_next_to (GTK_GRID (central_layout), current_player_widget, aspect_frame, GTK_POS_BOTTOM, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (central_layout), points_widget, current_player_widget, GTK_POS_RIGHT, 2, 1);

    gtk_grid_attach_next_to (GTK_GRID (central_layout), window->local->name_widget, current_player_widget, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (central_layout), window->local->color_widget, window->local->name_widget, GTK_POS_RIGHT, 1, 1);

    gtk_grid_attach_next_to (GTK_GRID (central_layout), window->visit->name_widget, points_widget, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (central_layout), window->visit->color_widget, window->visit->name_widget, GTK_POS_RIGHT, 1, 1);

    gtk_grid_attach_next_to (GTK_GRID (central_layout), window->local->score_widget, window->local->name_widget, GTK_POS_BOTTOM, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (central_layout), window->visit->score_widget, window->visit->name_widget, GTK_POS_BOTTOM, 2, 1);

    gtk_grid_attach_next_to (GTK_GRID (central_layout), local_players_playing, window->local->score_widget, GTK_POS_BOTTOM, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (central_layout), visit_players_playing, window->visit->score_widget, GTK_POS_BOTTOM, 2, 1);

    // layout
    layout = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (layout), menu, 0, 0, 5, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), local_players_widget, menu, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), central_layout, local_players_widget, GTK_POS_RIGHT, 5, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), visit_players_widget, central_layout, GTK_POS_RIGHT, 1, 1);

    gtk_container_add (GTK_CONTAINER (window), layout);

    gtk_widget_show_all (GTK_WIDGET (window));

    return GTK_WIDGET (window);
}

void
baskstat_window_serialize (BaskstatWindow *window, FILE *file)
{
    char buffer[255];
    snprintf (buffer, 255, "{\n\"match\": {");
    fwrite (buffer, sizeof (char), strlen (buffer), file);
    baskstat_court_serialize (BASKSTAT_COURT (window->basket_court), file);

    snprintf (buffer, 255, "\n\"local\": {");
    fwrite (buffer, sizeof (char), strlen (buffer), file);
    baskstat_team_serialize (window->local, file);
    snprintf (buffer, 255, "\n},");
    fwrite (buffer, sizeof (char), strlen (buffer), file);

    snprintf (buffer, 255, "\n\"visit\": {");
    fwrite (buffer, sizeof (char), strlen (buffer), file);
    baskstat_team_serialize (window->visit, file);
    snprintf (buffer, 255, "\n}");
    fwrite (buffer, sizeof (char), strlen (buffer), file);

    snprintf (buffer, 255, "}\n}");
    fwrite (buffer, sizeof (char), strlen (buffer), file);
}

void
baskstat_window_deserialize (BaskstatWindow *window, JsonNode *node)
{
    JsonNode *n = NULL;
    JsonObject *o = NULL;

    JsonObject *obj = json_node_get_object (node);
    obj = json_object_get_object_member (obj, "match");

    o = json_object_get_object_member (obj, "local");
    // deserializing local team
    o = json_object_get_object_member (o, "team");
    baskstat_team_deserialize (window->local, o);

    o = json_object_get_object_member (obj, "visit");
    // deserializing visit team
    o = json_object_get_object_member (o, "team");
    baskstat_team_deserialize (window->visit, o);

    o = json_object_get_object_member (obj, "court");
    // deserializing court
    baskstat_court_deserialize (window, o);

    baskstat_court_set_current_player (BASKSTAT_COURT (window->basket_court),
                                       BASKSTAT_PLAYER (window->local->players->data));
}

BaskstatPlayer *
baskstat_window_get_player (BaskstatWindow *window, const gchar *team_name, gint player_number)
{
    BaskstatPlayer *p = NULL;
    GList *players = NULL, *l = NULL;
    if (!strcmp (team_name, baskstat_team_name (window->local))) {
        players = window->local->players;
    } else if (!strcmp (team_name, baskstat_team_name (window->visit))) {
        players = window->visit->players;
    }

    if (!players)
        return p;

    for (l = players; l; l = l->next) {
        p = BASKSTAT_PLAYER (l->data);
        if (player_number == p->number) {
            return p;
        }
    }

    return NULL;
}

void
baskstat_window_simple_report (BaskstatWindow *window, cairo_t *cr, gint width, gint height)
{
    int w;
    int h;
    PangoLayout *layout;
    gchar text[250];

    cairo_set_source_rgba (cr, 1, 1, 1, 1);
    cairo_rectangle (cr, 0, 0, width, height);

    cairo_fill (cr);

    cairo_set_source_rgba (cr, 0, 0, 0, 1);

    layout = pango_cairo_create_layout (cr);
    g_snprintf (text, 250, "<b><u>Report - %s vs %s</u></b>", baskstat_team_name (window->local), baskstat_team_name (window->visit));
    pango_layout_set_markup (layout, text, -1);
    pango_layout_get_size (layout, &w, &h);
    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);

    cairo_translate (cr, 10, 10);
    pango_cairo_show_layout (cr, layout);
    cairo_translate (cr, -10, -10);
}
