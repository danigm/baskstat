
#include "baskstat-player.h"

G_DEFINE_TYPE (BaskstatPlayer, baskstat_player, G_TYPE_OBJECT);

GObject *
baskstat_player_new ()
{
    GObject *obj;
    BaskstatPlayer *p;

    obj = g_object_new (BASKSTAT_TYPE_PLAYER, NULL);
    p = BASKSTAT_PLAYER (obj);

    p->number = 4;
    p->playing = FALSE;
    p->points = 0;
    p->rebounds = 0;
    p->steals = 0;
    p->assists = 0;
    p->turnover = 0;
    p->foults = 0;

    return obj;
}

static void
baskstat_player_init (BaskstatPlayer *self)
{
}

static void
baskstat_player_class_init (BaskstatPlayerClass *klass)
{
}
