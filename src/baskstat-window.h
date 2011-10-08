
#ifndef _BASKSTAT_WINDOW_H
#define _BASKSTAT_WINDOW_H

#include <gtk/gtk.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

#define BASKSTAT_TYPE_WINDOW            (baskstat_window_get_type ())
#define BASKSTAT_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASKSTAT_TYPE_WINDOW, BaskstatWindow))
#define BASKSTAT_IS_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASKSTAT_TYPE_WINDOW))
#define BASKSTAT_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASKSTAT_TYPE_WINDOW, BaskstatWindowClass))
#define BASKSTAT_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASKSTAT_TYPE_WINDOW))
#define BASKSTAT_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BASKSTAT_TYPE_WINDOW, BaskstatWindowClass))

typedef struct _BaskstatWindow    BaskstatWindow;
typedef struct _BaskstatWindowClass BaskstatWindowClass;

typedef struct _BaskstatTeam BaskstatTeam;
typedef struct _BaskstatPlayer BaskstatPlayer;

struct _BaskstatWindow {
    GtkWindow parent;

    /* instance members */
    GtkWidget *basket_court;
    BaskstatTeam *local;
    BaskstatTeam *visit;
};

struct _BaskstatWindowClass {
    GtkWindowClass parent_class;

    /* class members */
};

GType baskstat_window_get_type (void);

/*
 * Method definitions.
 */

GtkWidget * baskstat_window_new ();
BaskstatPlayer * baskstat_window_get_player (BaskstatWindow *window, const gchar *team_name, gint player_number);
void baskstat_window_serialize (BaskstatWindow *window, FILE *file);
void baskstat_window_deserialize (BaskstatWindow *window, JsonNode *node);

#endif
