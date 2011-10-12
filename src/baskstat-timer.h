
#ifndef _BASKSTAT_TIMER_H
#define _BASKSTAT_TIMER_H

#include <gtk/gtk.h>
#include <glib-object.h>

#define BASKSTAT_TYPE_TIMER            (baskstat_timer_get_type ())
#define BASKSTAT_TIMER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BASKSTAT_TYPE_TIMER, BaskstatTimer))
#define BASKSTAT_IS_TIMER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BASKSTAT_TYPE_TIMER))
#define BASKSTAT_TIMER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BASKSTAT_TYPE_TIMER, BaskstatTimerClass))
#define BASKSTAT_IS_TIMER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BASKSTAT_TYPE_TIMER))
#define BASKSTAT_TIMER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BASKSTAT_TYPE_TIMER, BaskstatTimerClass))

typedef struct _BaskstatTimer    BaskstatTimer;
typedef struct _BaskstatTimerClass BaskstatTimerClass;

enum { Q1, Q2, Q3, Q4, QOVERTIME, QEND };

struct _BaskstatTimer {
    GtkGrid parent;

    /* instance members */
    gint quarter;
    gdouble time;
    GTimer *timer;
    gboolean stop;

    GtkWidget *play_button;
    GtkWidget *quarter_combo;
    GtkWidget *time_entry;
};

struct _BaskstatTimerClass {
    GtkGridClass parent_class;

    /* class members */
};

GType baskstat_timer_get_type (void);

/*
 * Method definitions.
 */

GtkWidget * baskstat_timer_new ();
gdouble baskstat_timer_current_time (BaskstatTimer *timer);
void baskstat_timer_update (BaskstatTimer *timer);
void baskstat_timer_reset (BaskstatTimer *timer);
void baskstat_timer_set_quarter (BaskstatTimer *timer, gint quarter);

#endif
