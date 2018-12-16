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
#include <unistd.h>
#include <xace.h>

static int
proc_drihybris_query_version(ClientPtr client)
{
    REQUEST(xDRIHYBRISQueryVersionReq);
    xDRIHYBRISQueryVersionReply rep = {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .majorVersion = 1,
        .minorVersion = 0
    };

    REQUEST_SIZE_MATCH(xDRIHYBRISQueryVersionReq);
    (void) stuff;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.majorVersion);
        swapl(&rep.minorVersion);
    }
    WriteToClient(client, sizeof(rep), &rep);
    return Success;
}

static int
proc_drihybris_pixmap_from_buffer(ClientPtr client)
{
    REQUEST(xDRIHYBRISPixmapFromBufferReq);
    int *fds, *ints;
    DrawablePtr drawable;
    PixmapPtr pixmap;
    int rc;

    REQUEST_AT_LEAST_SIZE(xDRIHYBRISPixmapFromBufferReq);
    REQUEST_FIXED_SIZE(xDRIHYBRISPixmapFromBufferReq, stuff->numInts * sizeof(int));

    SetReqFds(client, stuff->numFds);

    LEGAL_NEW_RESOURCE(stuff->pixmap, client);
    rc = dixLookupDrawable(&drawable, stuff->drawable, client, M_ANY, DixGetAttrAccess);
    if (rc != Success) {
        client->errorValue = stuff->drawable;
        return rc;
    }

    if (!stuff->width || !stuff->height) {
        client->errorValue = 0;
        return BadValue;
    }

    if (stuff->width > 32767 || stuff->height > 32767)
        return BadAlloc;

    if (stuff->depth != 1) {
        DepthPtr depth = drawable->pScreen->allowedDepths;
        int i;
        for (i = 0; i < drawable->pScreen->numDepths; i++, depth++)
            if (depth->depth == stuff->depth)
                break;
        if (i == drawable->pScreen->numDepths) {
            client->errorValue = stuff->depth;
            return BadValue;
        }
    }

    ints = malloc(stuff->numInts * sizeof(int));
    memcpy(ints, &stuff[1], stuff->numInts * sizeof(int));

    fds = malloc(stuff->numFds * sizeof(int));
    for (int i = 0; i < stuff->numFds; i++) {
        fds[i] = ReadFdFromClient(client);
        if (fds[i] < 0)
            return BadValue;
    }

    rc = drihybris_pixmap_from_buffer(&pixmap,
                             drawable->pScreen,
                             stuff->width, stuff->height,
                             stuff->stride, stuff->depth,
                             stuff->bpp,
                             stuff->numInts, ints,
                             stuff->numFds, fds);
    free(ints);
    free(fds);

    if (rc != Success)
        return rc;

    pixmap->drawable.id = stuff->pixmap;

    /* security creation/labeling check */
    rc = XaceHook(XACE_RESOURCE_ACCESS, client, stuff->pixmap, RT_PIXMAP,
                  pixmap, RT_NONE, NULL, DixCreateAccess);

    if (rc != Success) {
        (*drawable->pScreen->DestroyPixmap) (pixmap);
        return rc;
    }
    if (!AddResource(stuff->pixmap, RT_PIXMAP, (void *) pixmap))
        return BadAlloc;

    return Success;
}

static int
proc_drihybris_buffer_from_pixmap(ClientPtr client)
{
    REQUEST(xDRIHYBRISBufferFromPixmapReq);
    xDRIHYBRISBufferFromPixmapReply rep = {
        .type = X_Reply,
        .nfd = 1,
        .sequenceNumber = client->sequence,
        .length = 0,
    };
    int rc;
    int fd;
    PixmapPtr pixmap;
    int *fds, *ints;
    int numInts, numFds, repSize;
    char *repFull;

    REQUEST_SIZE_MATCH(xDRIHYBRISBufferFromPixmapReq);
    rc = dixLookupResourceByType((void **) &pixmap, stuff->pixmap, RT_PIXMAP,
                                 client, DixWriteAccess);
    if (rc != Success) {
        client->errorValue = stuff->pixmap;
        return rc;
    }

    rep.width = pixmap->drawable.width;
    rep.height = pixmap->drawable.height;
    rep.depth = pixmap->drawable.depth;
    rep.bpp = pixmap->drawable.bitsPerPixel;

    rc = drihybris_buffer_from_pixmap(pixmap, &rep.stride,
                                      &numInts, &ints, &numFds, &fds);
    if (rc != Success)
        return rc;

    rep.numInts = (unsigned short)numInts;
    rep.numFds = (unsigned short)numFds;
    rep.nfd = numFds;
    rep.length = numInts;

    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swaps(&rep.width);
        swaps(&rep.height);
        swaps(&rep.stride);
        swaps(&rep.numInts);
        swaps(&rep.numFds);
    }

    repSize = sizeof(rep) + numInts * sizeof(int);
    repFull = malloc(repSize);
    memcpy(repFull, &rep, sizeof(rep));
    memcpy((repFull + sizeof(rep)), ints, numInts * sizeof(int));

    for (int i = 0; i < numFds; i++) {
        if (WriteFdToClient(client, fds[i], TRUE) < 0) {
            close(fd);
            return BadAlloc;
        }
    }

    WriteToClient(client, repSize, (xDRIHYBRISBufferFromPixmapReply*)repFull);

    free(repFull);
    free(ints);
    free(fds);

    return Success;
}

int (*proc_drihybris_vector[DRIHYBRISNumberRequests]) (ClientPtr) = {
    proc_drihybris_query_version,            /* 0 */
    proc_drihybris_pixmap_from_buffer,       /* 1 */
    proc_drihybris_buffer_from_pixmap,       /* 2 */
};

int
proc_drihybris_dispatch(ClientPtr client)
{
    REQUEST(xReq);
    if (!client->local)
        return BadMatch;
    if (stuff->data >= DRIHYBRISNumberRequests || !proc_drihybris_vector[stuff->data])
        return BadRequest;
    return (*proc_drihybris_vector[stuff->data]) (client);
}

static int
sproc_drihybris_query_version(ClientPtr client)
{
    REQUEST(xDRIHYBRISQueryVersionReq);
    REQUEST_SIZE_MATCH(xDRIHYBRISQueryVersionReq);

    swaps(&stuff->length);
    swapl(&stuff->majorVersion);
    swapl(&stuff->minorVersion);
    return (*proc_drihybris_vector[stuff->drihybrisReqType]) (client);
}

static int
sproc_drihybris_pixmap_from_buffer(ClientPtr client)
{
    REQUEST(xDRIHYBRISPixmapFromBufferReq);
    REQUEST_AT_LEAST_SIZE(xDRIHYBRISPixmapFromBufferReq);

    swaps(&stuff->length);
    swapl(&stuff->pixmap);
    swapl(&stuff->drawable);
    swapl(&stuff->size);
    swaps(&stuff->width);
    swaps(&stuff->height);
    swaps(&stuff->stride);
    swaps(&stuff->numInts);
    swaps(&stuff->numFds);

    REQUEST_FIXED_SIZE(xDRIHYBRISPixmapFromBufferReq, stuff->numInts * sizeof(int));

    return (*proc_drihybris_vector[stuff->drihybrisReqType]) (client);
}

static int
sproc_drihybris_buffer_from_pixmap(ClientPtr client)
{
    REQUEST(xDRIHYBRISBufferFromPixmapReq);
    REQUEST_SIZE_MATCH(xDRIHYBRISBufferFromPixmapReq);

    swaps(&stuff->length);
    swapl(&stuff->pixmap);
    return (*proc_drihybris_vector[stuff->drihybrisReqType]) (client);
}

int (*sproc_drihybris_vector[DRIHYBRISNumberRequests]) (ClientPtr) = {
    sproc_drihybris_query_version,           /* 0 */
    sproc_drihybris_pixmap_from_buffer,      /* 1 */
    sproc_drihybris_buffer_from_pixmap,      /* 2 */
};

int
sproc_drihybris_dispatch(ClientPtr client)
{
    REQUEST(xReq);

    if (!client->local)
        return BadMatch;
    if (stuff->data >= DRIHYBRISNumberRequests || !sproc_drihybris_vector[stuff->data])
        return BadRequest;
    return (*sproc_drihybris_vector[stuff->data]) (client);
}
