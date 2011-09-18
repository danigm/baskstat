
#ifndef _BASKSTAT_COURT_H
#define _BASKSTAT_COURT_H

#include "baskstat-player.h"

#include <gtk/gtk.h>
#include <glib-object.h>

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

#endif
