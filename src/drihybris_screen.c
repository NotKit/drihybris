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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "drihybris_priv.h"
#include <misync.h>
#include <misyncshm.h>
#include <randrstr.h>

int
drihybris_pixmap_from_buffer(PixmapPtr *ppixmap, ScreenPtr screen,
                    CARD16 width, CARD16 height, CARD16 stride, CARD8 depth, CARD8 bpp,
                    int numInts, int *ints, int numFds, int *fds)
{
    drihybris_screen_priv_ptr   ds = drihybris_screen_priv(screen);
    drihybris_screen_info_ptr   info = ds->info;
    PixmapPtr                   pixmap;

    if (!info || !info->pixmap_from_buffer)
        return BadImplementation;

    pixmap = (*info->pixmap_from_buffer) (screen, width, height, stride, depth, bpp,
                                      numInts, ints, numFds, fds);
    if (!pixmap)
        return BadAlloc;

    *ppixmap = pixmap;
    return Success;
}

int
drihybris_fd_from_pixmap(int *pfd, PixmapPtr pixmap, CARD16 *stride, CARD32 *size)
{
    ScreenPtr                   screen = pixmap->drawable.pScreen;
    drihybris_screen_priv_ptr   ds = drihybris_screen_priv(screen);
    drihybris_screen_info_ptr   info = ds->info;
    int                         fd;

    if (!info || !info->fd_from_pixmap)
        return BadImplementation;

    fd = (*info->fd_from_pixmap)(screen, pixmap, stride, size);
    if (fd < 0)
        return BadAlloc;
    *pfd = fd;
    return Success;
}

