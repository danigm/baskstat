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
#include "baskstat-team.h"

#include <gtk/gtk.h>
#include <stdlib.h>

#include <glib/gi18n.h>

// global basket object list
GList *basket_object_list = NULL;
GList *local_player_list = NULL;
GList *visit_player_list = NULL;
GtkWidget *basket_court = NULL;

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
    GtkWidget *central_layout;

    GtkWidget *local_players_widget;
    GtkWidget *visit_players_widget;

    GtkWidget *local_players_playing;
    GtkWidget *visit_players_playing;

    BaskstatTeam *local = NULL, *visit = NULL;

    // code
    gtk_init (&argc, &argv);

    // main window creation
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), _("BaskStat - Basketball match stats"));
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    // basket court creation
    basket_court = baskstat_court_new ();

    // teams creation
    local = BASKSTAT_TEAM (baskstat_team_new (BASKSTAT_COURT (basket_court), _("local")));
    visit = BASKSTAT_TEAM (baskstat_team_new (BASKSTAT_COURT (basket_court), _("visit")));
    for (i = 0; i < 12; i++) {
        BaskstatPlayer *p = BASKSTAT_PLAYER (baskstat_player_new ());
        p->number = i + 4;
        baskstat_team_add_player (local, p);

        p = BASKSTAT_PLAYER (baskstat_player_new ());
        p->number = i + 4;
        baskstat_team_add_player (visit, p);

        baskstat_court_set_current_player (BASKSTAT_COURT (basket_court), p);
    }

    local_player_list = local->players;
    visit_player_list = visit->players;

    local_players_widget = baskstat_team_player_widget_new (local);
    local_players_playing = baskstat_team_playing_new (local);

    visit_players_widget = baskstat_team_player_widget_new (visit);
    visit_players_playing = baskstat_team_playing_new (visit);

    // central layout
    central_layout = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (central_layout), basket_court, 0, 0, 2, 1);
    gtk_grid_attach (GTK_GRID (central_layout), baskstat_court_current_player_widget (BASKSTAT_COURT (basket_court)), 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), baskstat_court_basket_points_widget (BASKSTAT_COURT (basket_court)), 1, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), local->name_widget, 0, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), visit->name_widget, 1, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), local->score_widget, 0, 3, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), visit->score_widget, 1, 3, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), local_players_playing, 0, 4, 1, 1);
    gtk_grid_attach (GTK_GRID (central_layout), visit_players_playing, 1, 4, 1, 1);

    // layout
    layout = gtk_grid_new ();
    gtk_grid_attach (GTK_GRID (layout), local_players_widget, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), central_layout, local_players_widget, GTK_POS_RIGHT, 5, 1);
    gtk_grid_attach_next_to (GTK_GRID (layout), visit_players_widget, central_layout, GTK_POS_RIGHT, 1, 1);

    gtk_container_add (GTK_CONTAINER (window), layout);

    gtk_widget_show_all (window);

    gtk_main();

    return 0;
}
