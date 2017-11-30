/*
 * Copyright © 2013 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef _DRIHYBRISPRIV_H_
#define _DRIHYBRISPRIV_H_

#include <X11/X.h>
#include "scrnintstr.h"
#include "misc.h"
#include "list.h"
#include "windowstr.h"
#include "dixstruct.h"
#include <randrstr.h>
#include "drihybris.h"

extern DevPrivateKeyRec drihybris_screen_private_key;

typedef struct drihybris_screen_priv {
    CloseScreenProcPtr          CloseScreen;
    ConfigNotifyProcPtr         ConfigNotify;
    DestroyWindowProcPtr        DestroyWindow;

    drihybris_screen_info_ptr        info;
} drihybris_screen_priv_rec, *drihybris_screen_priv_ptr;

#define wrap(priv,real,mem,func) {\
    priv->mem = real->mem; \
    real->mem = func; \
}

#define unwrap(priv,real,mem) {\
    real->mem = priv->mem; \
}

static inline drihybris_screen_priv_ptr
drihybris_screen_priv(ScreenPtr screen)
{
    return (drihybris_screen_priv_ptr)dixLookupPrivate(&(screen)->devPrivates, &drihybris_screen_private_key);
}

int
proc_drihybris_dispatch(ClientPtr client);

int
sproc_drihybris_dispatch(ClientPtr client);

/* DDX interface */

int
drihybris_open(ClientPtr client, ScreenPtr screen, RRProviderPtr provider, int *fd);

int
drihybris_pixmap_from_buffer(PixmapPtr *ppixmap, ScreenPtr screen,
                    CARD16 width, CARD16 height, CARD16 stride, CARD8 depth, CARD8 bpp,
                    int numInts, int *ints, int numFds, int *fds);

int
drihybris_fd_from_pixmap(int *pfd, PixmapPtr pixmap, CARD16 *stride, CARD32 *size);

#endif /* _DRIHYBRISPRIV_H_ */
