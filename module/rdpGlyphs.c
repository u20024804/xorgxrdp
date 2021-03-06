/*
Copyright 2012-2017 Jay Sorg

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

glyph (font) calls

*/

#if defined(HAVE_CONFIG_H)
#include "config_ac.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this should be before all X11 .h files */
#include <xorg-server.h>
#include <xorgVersion.h>

/* all driver need this */
#include <xf86.h>
#include <xf86_OSproc.h>

#include <picture.h>
#include <glyphstr.h>

#include "rdp.h"
#include "rdpGlyphs.h"
#include "rdpDraw.h"
#include "rdpMisc.h"
#include "rdpReg.h"

/******************************************************************************/
#define LOG_LEVEL 1
#define LLOGLN(_level, _args) \
    do { if (_level < LOG_LEVEL) { ErrorF _args ; ErrorF("\n"); } } while (0)

/******************************************************************************/
int
rdpGlyphDeleteRdpText(struct rdp_text *rtext)
{
    int index;

    if (rtext == NULL)
    {
        return 0;
    }
    for (index = 0; index < rtext->num_chars; index++)
    {
        if (rtext->chars[index] != NULL)
        {
            free(rtext->chars[index]->data);
            free(rtext->chars[index]);
        }
    }
    rdpRegionDestroy(rtext->reg);
    rdpGlyphDeleteRdpText(rtext->next);
    free(rtext);
    return 0;
}

/******************************************************************************/
static void
rdpGlyphsOrg(PictureScreenPtr ps, rdpPtr dev,
             CARD8 op, PicturePtr pSrc, PicturePtr pDst,
             PictFormatPtr maskFormat,
             INT16 xSrc, INT16 ySrc, int nlists, GlyphListPtr lists,
             GlyphPtr *glyphs)
{
    ps->Glyphs = dev->Glyphs;
    ps->Glyphs(op, pSrc, pDst, maskFormat, xSrc, ySrc,
               nlists, lists, glyphs);
    ps->Glyphs = rdpGlyphs;
}

/******************************************************************************/
void
rdpGlyphs(CARD8 op, PicturePtr pSrc, PicturePtr pDst,
          PictFormatPtr maskFormat,
          INT16 xSrc, INT16 ySrc, int nlists, GlyphListPtr lists,
          GlyphPtr *glyphs)
{
    ScreenPtr pScreen;
    rdpPtr dev;
    PictureScreenPtr ps;

    LLOGLN(10, ("rdpGlyphs:"));
    pScreen = pDst->pDrawable->pScreen;
    dev = rdpGetDevFromScreen(pScreen);
    ps = GetPictureScreen(pScreen);
    rdpGlyphsOrg(ps, dev, op, pSrc, pDst, maskFormat, xSrc, ySrc,
                 nlists, lists, glyphs);
}
