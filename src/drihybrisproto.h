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

#ifndef _DRIHYBRIS_PROTO_H_
#define _DRIHYBRIS_PROTO_H_

#define DRIHYBRIS_NAME			"DRIHYBRIS"
#define DRIHYBRIS_MAJOR			1
#define DRIHYBRIS_MINOR			0

#define DRIHYBRISNumberErrors		0
#define DRIHYBRISNumberEvents		0

#define X_DRIHYBRISQueryVersion		0
#define X_DRIHYBRISPixmapFromBuffer		1
#define X_DRIHYBRISBufferFromPixmap		2

#define DRIHYBRISNumberRequests		3

typedef struct {
    CARD8   reqType;
    CARD8   drihybrisReqType;
    CARD16  length B16;
    CARD32  majorVersion B32;
    CARD32  minorVersion B32;
} xDRIHYBRISQueryVersionReq;
#define sz_xDRIHYBRISQueryVersionReq   12

typedef struct {
    BYTE    type;   /* X_Reply */
    BYTE    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  majorVersion B32;
    CARD32  minorVersion B32;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
} xDRIHYBRISQueryVersionReply;
#define sz_xDRIHYBRISQueryVersionReply	32

typedef struct {
    CARD8   reqType;
    CARD8   drihybrisReqType;
    CARD16  length B16;
    CARD32  pixmap B32;
    CARD32  drawable B32;
    CARD32  size B32;
    CARD16  width B16;
    CARD16  height B16;
    CARD16  stride B16;
    CARD8   depth;
    CARD8   bpp;
    CARD16  numInts B16;
    CARD16  numFds B16;
} xDRIHYBRISPixmapFromBufferReq;

#define sz_xDRIHYBRISPixmapFromBufferReq     28

typedef struct {
    CARD8   reqType;
    CARD8   drihybrisReqType;
    CARD16  length B16;
    CARD32  pixmap B32;
} xDRIHYBRISBufferFromPixmapReq;
#define sz_xDRIHYBRISBufferFromPixmapReq     8

typedef struct {
    BYTE    type;   /* X_Reply */
    CARD8   nfd;    /* Number of file descriptors returned (1) */
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD32  size B32;
    CARD16  width B16;
    CARD16  height B16;
    CARD16  stride B16;
    CARD8   depth;
    CARD8   bpp;
    CARD32  pad20 B32;
    CARD32  pad24 B32;
    CARD32  pad28 B32;
} xDRIHYBRISBufferFromPixmapReply;
#define sz_xDRIHYBRISBufferFromPixmapReply   32

#endif
