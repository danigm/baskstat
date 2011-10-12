
#include "baskstat-timer.h"

G_DEFINE_TYPE (BaskstatTimer, baskstat_timer, GTK_TYPE_GRID);

static gboolean
quarter_changed (GtkComboBoxText *combo, BaskstatTimer *timer)
{
    baskstat_timer_reset (timer);
    timer->quarter = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
    return FALSE;
}

static gboolean
update_time_entry (BaskstatTimer *timer)
{
    gint min;
    gdouble sec, total, max, left;
    gchar newtext[20];
    GtkEntry *entry = GTK_ENTRY (timer->time_entry);

    // max is ten minutes
    max = timer->time;

    total = timer->timer ? g_timer_elapsed (timer->timer, NULL) : 0;
    left = max - total;
    if (left <= 0) {
        g_snprintf (newtext, 20, "%02d:%05.2f", 0, 0);
        g_timer_stop (timer->timer);
        timer->stop = TRUE;
        return FALSE;
    }

    min = left / 60;
    sec = left - (min * 60);

    g_snprintf (newtext, 20, "%02d:%05.2f", min, sec);

    gtk_entry_set_text (entry, newtext);

    if (timer->stop) {
        return FALSE;
    }

    return TRUE;
}

static gboolean
play_pause (GtkButton *button, BaskstatTimer *timer)
{
    GtkWidget *img;

    if (timer->stop) {
        img = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_BUTTON);
        gtk_button_set_image (GTK_BUTTON (timer->play_button), img);

        if (timer->timer == NULL) {
            timer->timer = g_timer_new ();
        } else {
            g_timer_continue (timer->timer);
        }

        timer->stop = FALSE;
        g_timeout_add (20, (GSourceFunc)update_time_entry, timer);
    } else {
        img = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
        gtk_button_set_image (GTK_BUTTON (timer->play_button), img);

        timer->stop = TRUE;
        g_timer_stop (timer->timer);
    }

    return FALSE;
}

GtkWidget *
baskstat_timer_new ()
{
    GtkWidget *obj;
    GtkWidget *img;
    obj = g_object_new (BASKSTAT_TYPE_TIMER, NULL);
    BaskstatTimer *timer = BASKSTAT_TIMER (obj);

    timer->quarter = Q1;
    timer->timer = NULL;
    timer->stop = TRUE;
    timer->time = 10 * 60;

    img = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
    timer->play_button = gtk_button_new ();
    gtk_button_set_image (GTK_BUTTON (timer->play_button), img);

    timer->quarter_combo = gtk_combo_box_text_new ();
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (timer->quarter_combo), "Q1", "Q1");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (timer->quarter_combo), "Q2", "Q2");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (timer->quarter_combo), "Q3", "Q3");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (timer->quarter_combo), "Q4", "Q4");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (timer->quarter_combo), "OT", "OT");
    gtk_combo_box_set_active (GTK_COMBO_BOX (timer->quarter_combo), 0);

    timer->time_entry = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY (timer->time_entry), "10:00");

    g_signal_connect (timer->play_button, "clicked", G_CALLBACK (play_pause), timer);
    g_signal_connect (timer->quarter_combo, "changed", G_CALLBACK (quarter_changed), timer);

    gtk_grid_attach (GTK_GRID (obj), timer->quarter_combo, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (obj), timer->play_button, timer->quarter_combo, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (obj), timer->time_entry, timer->play_button, GTK_POS_RIGHT, 1, 1);

    return obj;
}

static void
baskstat_timer_init (BaskstatTimer *self)
{
}

static void
baskstat_timer_finalize (GObject *object)
{
    BaskstatTimer *t = BASKSTAT_TIMER (object);

    if (t->timer) {
        g_timer_destroy (t->timer);
    }

    G_OBJECT_CLASS (baskstat_timer_parent_class)->finalize (object);
}

static void
baskstat_timer_class_init (BaskstatTimerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = baskstat_timer_finalize;
}

gdouble baskstat_timer_current_time (BaskstatTimer *timer)
{
    gdouble t = 10 * 60 - timer->time;
    if (timer->timer)
        return t + g_timer_elapsed (timer->timer, NULL);
    else
        return t;
}

void
baskstat_timer_update (BaskstatTimer *timer)
{
    update_time_entry (timer);
}

void
baskstat_timer_reset (BaskstatTimer *timer)
{
    GtkWidget *img;
    timer->time = 10 * 60;
    if (timer->timer)
        g_timer_destroy (timer->timer);
    timer->timer = NULL;
    baskstat_timer_update (timer);

    img = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image (GTK_BUTTON (timer->play_button), img);
}

void
baskstat_timer_set_quarter (BaskstatTimer *timer, gint quarter)
{
    gtk_combo_box_set_active (GTK_COMBO_BOX (timer->quarter_combo), quarter);
}
