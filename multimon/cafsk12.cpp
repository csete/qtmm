#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "filter.h"
#include "cafsk12.h"




CAfsk12::CAfsk12(QObject *parent) :
    QObject(parent)
{
    state = (demod_state *) malloc(sizeof(demod_state));
    reset();
}

CAfsk12::~CAfsk12()
{
    free(state);
}

/*! \brief Reset the decoder. */
void CAfsk12::reset()
{
    float f;
    int i;

    hdlc_init(state);
    memset(&state->l1.afsk12, 0, sizeof(state->l1.afsk12));
    for (f = 0, i = 0; i < CORRLEN; i++) {
        corr_mark_i[i] = cos(f);
        corr_mark_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_MARK/FREQ_SAMP;
    }
    for (f = 0, i = 0; i < CORRLEN; i++) {
        corr_space_i[i] = cos(f);
        corr_space_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_SPACE/FREQ_SAMP;
    }

}


void CAfsk12::demod(float *buffer, int length)
{
    float f;
    unsigned char curbit;

    if (state->l1.afsk12.subsamp) {
        int numfill = SUBSAMP - state->l1.afsk12.subsamp;
        if (length < numfill) {
            state->l1.afsk12.subsamp += length;
            return;
        }
        buffer += numfill;
        length -= numfill;
        state->l1.afsk12.subsamp = 0;
    }
    for (; length >= SUBSAMP; length -= SUBSAMP, buffer += SUBSAMP) {
        f = fsqr(mac(buffer, corr_mark_i, CORRLEN)) +
            fsqr(mac(buffer, corr_mark_q, CORRLEN)) -
            fsqr(mac(buffer, corr_space_i, CORRLEN)) -
            fsqr(mac(buffer, corr_space_q, CORRLEN));
        state->l1.afsk12.dcd_shreg <<= 1;
        state->l1.afsk12.dcd_shreg |= (f > 0);
        verbprintf(10, "%c", '0'+(state->l1.afsk12.dcd_shreg & 1));
        /*
         * check if transition
         */
        if ((state->l1.afsk12.dcd_shreg ^ (state->l1.afsk12.dcd_shreg >> 1)) & 1) {
            if (state->l1.afsk12.sphase < (0x8000u-(SPHASEINC/2)))
                state->l1.afsk12.sphase += SPHASEINC/8;
            else
                state->l1.afsk12.sphase -= SPHASEINC/8;
        }
        state->l1.afsk12.sphase += SPHASEINC;
        if (state->l1.afsk12.sphase >= 0x10000u) {
            state->l1.afsk12.sphase &= 0xffffu;
            state->l1.afsk12.lasts <<= 1;
            state->l1.afsk12.lasts |= state->l1.afsk12.dcd_shreg & 1;
            curbit = (state->l1.afsk12.lasts ^
                  (state->l1.afsk12.lasts >> 1) ^ 1) & 1;
            verbprintf(9, " %c ", '0'+curbit);
            hdlc_rxbit(state, curbit);
        }
    }
    state->l1.afsk12.subsamp = length;
}

/** HDLC functions **/

void CAfsk12::hdlc_init(struct demod_state *s)
{
    memset(&s->l2.hdlc, 0, sizeof(s->l2.hdlc));
}


void CAfsk12::hdlc_rxbit(struct demod_state *s, int bit)
{
    s->l2.hdlc.rxbitstream <<= 1;
    s->l2.hdlc.rxbitstream |= !!bit;
    if ((s->l2.hdlc.rxbitstream & 0xff) == 0x7e) {
        if (s->l2.hdlc.rxstate && (s->l2.hdlc.rxptr - s->l2.hdlc.rxbuf) > 2)
            /** ax25_disp_packet(s, s->l2.hdlc.rxbuf, s->l2.hdlc.rxptr - s->l2.hdlc.rxbuf); **/
        s->l2.hdlc.rxstate = 1;
        s->l2.hdlc.rxptr = s->l2.hdlc.rxbuf;
        s->l2.hdlc.rxbitbuf = 0x80;
        return;
    }
    if ((s->l2.hdlc.rxbitstream & 0x7f) == 0x7f) {
        s->l2.hdlc.rxstate = 0;
        return;
    }
    if (!s->l2.hdlc.rxstate)
        return;
    if ((s->l2.hdlc.rxbitstream & 0x3f) == 0x3e) /* stuffed bit */
        return;
    if (s->l2.hdlc.rxbitstream & 1)
        s->l2.hdlc.rxbitbuf |= 0x100;
    if (s->l2.hdlc.rxbitbuf & 1) {
        if (s->l2.hdlc.rxptr >= s->l2.hdlc.rxbuf+sizeof(s->l2.hdlc.rxbuf)) {
            s->l2.hdlc.rxstate = 0;
            verbprintf(1, "Error: packet size too large\n");
            return;
        }
        *s->l2.hdlc.rxptr++ = s->l2.hdlc.rxbitbuf >> 1;
        s->l2.hdlc.rxbitbuf = 0x80;
        return;
    }
    s->l2.hdlc.rxbitbuf >>= 1;
}


static int verbose_level = 2;

void CAfsk12::verbprintf(int verb_level, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    if (verb_level <= verbose_level) {
        vfprintf(stdout, fmt, args);
        fflush(stdout);
    }
    va_end(args);
}
