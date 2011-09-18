
#ifndef _BASKSTAT_PLAYER_H
#define _BASKSTAT_PLAYER_H

#include <glib-object.h>

#define BASKSTAT_TYPE_PLAYER            (baskstat_player_get_type ())
#define BASKSTAT_PLAYER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASKSTAT_TYPE_PLAYER, BaskstatPlayer))
#define BASKSTAT_IS_PLAYER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASKSTAT_TYPE_PLAYER))
#define BASKSTAT_PLAYER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASKSTAT_TYPE_PLAYER, BaskstatPlayerClass))
#define BASKSTAT_IS_PLAYER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASKSTAT_TYPE_PLAYER))
#define BASKSTAT_PLAYER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BASKSTAT_TYPE_PLAYER, BaskstatPlayerClass))

typedef struct _BaskstatPlayer    BaskstatPlayer;
typedef struct _BaskstatPlayerClass BaskstatPlayerClass;

struct _BaskstatPlayer {
    GObject parent;

    /* instance members */
    gint number;
    gboolean playing;
    gint points;
    gint rebounds;
    gint steals;
    gint assists;
    gint turnover;
    gint foults;
};

struct _BaskstatPlayerClass {
    GObjectClass parent_class;

    /* class members */
};

GType baskstat_player_get_type (void);

/*
 * Method definitions.
 */

GObject * baskstat_player_new ();

#endif
