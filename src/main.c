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

#include <gtk/gtk.h>
#include <stdlib.h>

#include <glib/gi18n.h>


int
main (int argc, char **argv)
{
#ifdef ENABLE_NLS
    /* Initialize the i18n stuff */
    bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);
#endif

    // code
    gtk_init (&argc, &argv);

    // main window creation
    GtkWidget *window = baskstat_window_new ();
    gtk_main();

    return 0;
}
