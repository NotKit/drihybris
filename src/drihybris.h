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

#ifndef _DRIHYBRIS_H_
#define _DRIHYBRIS_H_

#define DRIHYBRIS
#ifdef DRIHYBRIS

#include <X11/extensions/drihybrisproto.h>
#include <randrstr.h>

#define DRIHYBRIS_SCREEN_INFO_VERSION        1

typedef int (*drihybris_open_proc)(ScreenPtr screen,
                              RRProviderPtr provider,
                              int *fd);

typedef int (*drihybris_open_client_proc)(ClientPtr client,
                                     ScreenPtr screen,
                                     RRProviderPtr provider,
                                     int *fd);

typedef PixmapPtr (*drihybris_pixmap_from_buffer_proc) (ScreenPtr screen,
                                               CARD16 width,
                                               CARD16 height,
                                               CARD16 stride,
                                               CARD8 depth,
                                               CARD8 bpp,
                                               int numInts, int *ints,
                                               int numFds, int *fds);

typedef int (*drihybris_fd_from_pixmap_proc) (ScreenPtr screen,
                                         PixmapPtr pixmap,
                                         CARD16 *stride,
                                         CARD32 *size);

typedef struct drihybris_screen_info {
    uint32_t                    version;

    drihybris_open_proc                open;
    drihybris_pixmap_from_buffer_proc  pixmap_from_buffer;
    drihybris_fd_from_pixmap_proc      fd_from_pixmap;

    /* Version 1 */
    drihybris_open_client_proc         open_client;

} drihybris_screen_info_rec, *drihybris_screen_info_ptr;

extern _X_EXPORT Bool
drihybris_screen_init(ScreenPtr screen, drihybris_screen_info_ptr info);

extern _X_EXPORT int
drihybris_send_open_reply(ClientPtr client, int fd);

extern _X_EXPORT void drihybris_extension_init(void);

#endif

#endif /* _DRIHYBRIS_H_ */
