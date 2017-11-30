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
#include <xorg/xf86Module.h>
#include "drihybris_priv.h"

static int drihybris_request;
DevPrivateKeyRec drihybris_screen_private_key;

static int drihybris_screen_generation;

static Bool
drihybris_close_screen(ScreenPtr screen)
{
    drihybris_screen_priv_ptr screen_priv = drihybris_screen_priv(screen);

    unwrap(screen_priv, screen, CloseScreen);

    free(screen_priv);
    return (*screen->CloseScreen) (screen);
}

Bool
drihybris_screen_init(ScreenPtr screen, drihybris_screen_info_ptr info)
{
    drihybris_screen_generation = serverGeneration;

    if (!dixRegisterPrivateKey(&drihybris_screen_private_key, PRIVATE_SCREEN, 0))
        return FALSE;

    if (!drihybris_screen_priv(screen)) {
        drihybris_screen_priv_ptr screen_priv = calloc(1, sizeof (drihybris_screen_priv_rec));
        if (!screen_priv)
            return FALSE;

        wrap(screen_priv, screen, CloseScreen, drihybris_close_screen);

        screen_priv->info = info;

        dixSetPrivate(&screen->devPrivates, &drihybris_screen_private_key, screen_priv);
    }

    return TRUE;
}

static XF86ModuleVersionInfo VersRec = {
    "drihybris",
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    1, 0, 0, /* version */
    ABI_CLASS_ANSIC,            /* Only need the ansic layer */
    ABI_ANSIC_VERSION,
    MOD_CLASS_NONE,
    {0, 0, 0, 0}                /* signature, to be patched into the file by a tool */
};

_X_EXPORT XF86ModuleData drihybrisModuleData = { &VersRec, NULL, NULL };

void
drihybris_extension_init(void)
{
    ExtensionEntry *extension;
    int i;

    extension = AddExtension(DRIHYBRIS_NAME, DRIHYBRISNumberEvents, DRIHYBRISNumberErrors,
                             proc_drihybris_dispatch, sproc_drihybris_dispatch,
                             NULL, StandardMinorOpcode);
    if (!extension)
        goto bail;

    drihybris_request = extension->base;

    for (i = 0; i < screenInfo.numScreens; i++) {
        if (!drihybris_screen_init(screenInfo.screens[i], NULL))
            goto bail;
    }
    return;

bail:
    FatalError("Cannot initialize DRIHYBRIS extension");
}
