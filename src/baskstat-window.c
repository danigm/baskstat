#include "baskstat-window.h"
#include "baskstat-player.h"
#include "baskstat-court.h"
#include "baskstat-team.h"
#include "baskstat-window.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <glib/gi18n.h>

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

    dialog = gtk_file_chooser_dialog_new (_("Open match"),
            GTK_WINDOW (window),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
            NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        printf ("FILE: %s\n", filename);
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

    dialog = gtk_file_chooser_dialog_new (_("Save as"),
            GTK_WINDOW (window),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
            NULL);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;

        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
        printf ("FILE: %s\n", filename);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);

    return FALSE;
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
    printf ("ABOUT\n");
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

    GtkWidget *local_players_widget;
    GtkWidget *visit_players_widget;

    GtkWidget *local_players_playing;
    GtkWidget *visit_players_playing;

    BaskstatTeam *local = NULL, *visit = NULL;

    menu = generate_menu (window);
    gtk_window_set_title (GTK_WINDOW (window), _("BaskStat - Basketball match stats"));
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    // basket court creation
    window->basket_court = baskstat_court_new ();

    // teams creation
    local = BASKSTAT_TEAM (baskstat_team_new (BASKSTAT_COURT (window->basket_court), _("local")));
    visit = BASKSTAT_TEAM (baskstat_team_new (BASKSTAT_COURT (window->basket_court), _("visit")));
    for (i = 0; i < 12; i++) {
        BaskstatPlayer *p = BASKSTAT_PLAYER (baskstat_player_new ());
        p->number = i + 4;
        baskstat_team_add_player (local, p);

        p = BASKSTAT_PLAYER (baskstat_player_new ());
        p->number = i + 4;
        baskstat_team_add_player (visit, p);

        baskstat_court_set_current_player (BASKSTAT_COURT (window->basket_court), p);
    }

    local_players_widget = baskstat_team_player_widget_new (local);
    local_players_playing = baskstat_team_playing_new (local);

    visit_players_widget = baskstat_team_player_widget_new (visit);
    visit_players_playing = baskstat_team_playing_new (visit);

    // central layout
    central_layout = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (central_layout), window->basket_court, 0, 0, 2, 1);
    gtk_grid_attach (GTK_GRID (central_layout), baskstat_court_current_player_widget (BASKSTAT_COURT (window->basket_court)), 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), baskstat_court_basket_points_widget (BASKSTAT_COURT (window->basket_court)), 1, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), local->name_widget, 0, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), visit->name_widget, 1, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), local->score_widget, 0, 3, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), visit->score_widget, 1, 3, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), local_players_playing, 0, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), visit_players_playing, 1, 4, 1, 1);

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

JsonNode *
baskstat_window_serialize (BaskstatWindow *window)
{
    return NULL;
}

void
baskstat_window_deserialize (BaskstatWindow *window, JsonNode *node)
{
}