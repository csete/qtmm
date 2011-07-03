#ifndef CAFSK12_H
#define CAFSK12_H

#include <QObject>

extern const float costabf[0x400];
#define COS(x) costabf[(((x)>>6)&0x3ffu)]
#define SIN(x) COS((x)+0xc000)

/*
 * Standard TCM3105 clock frequency: 4.4336MHz
 * Mark frequency: 2200 Hz
 * Space frequency: 1200 Hz
 */
#define FREQ_MARK  1200
#define FREQ_SPACE 2200
#define FREQ_SAMP  22050
#define BAUD       1200
#define SUBSAMP    2

#define CORRLEN ((int)(FREQ_SAMP/BAUD))
#define SPHASEINC (0x10000u*BAUD*SUBSAMP/FREQ_SAMP)


struct demod_state {
    const struct demod_param *dem_par;
    union {
        struct l2_state_hdlc {
            unsigned char rxbuf[512];
            unsigned char *rxptr;
            unsigned int rxstate;
            unsigned int rxbitstream;
            unsigned int rxbitbuf;
        } hdlc;

        struct l2_state_pocsag {
            unsigned long rx_data;
            struct l2_pocsag_rx {
                unsigned char rx_sync;
                unsigned char rx_word;
                unsigned char rx_bit;
                char func;
                unsigned long adr;
                unsigned char buffer[128];
                unsigned int numnibbles;
            } rx[2];
        } pocsag;
    } l2;
    union {
        struct l1_state_poc5 {
            unsigned int dcd_shreg;
            unsigned int sphase;
            unsigned int subsamp;
        } poc5;

        struct l1_state_poc12 {
            unsigned int dcd_shreg;
            unsigned int sphase;
            unsigned int subsamp;
        } poc12;

        struct l1_state_poc24 {
            unsigned int dcd_shreg;
            unsigned int sphase;
        } poc24;

        struct l1_state_afsk12 {
            unsigned int dcd_shreg;
            unsigned int sphase;
            unsigned int lasts;
            unsigned int subsamp;
        } afsk12;

        struct l1_state_afsk24 {
            unsigned int dcd_shreg;
            unsigned int sphase;
            unsigned int lasts;
        } afsk24;

        struct l1_state_hapn48 {
            unsigned int shreg;
            unsigned int sphase;
            float lvllo, lvlhi;
        } hapn48;

        struct l1_state_fsk96 {
            unsigned int dcd_shreg;
            unsigned int sphase;
            unsigned int descram;
        } fsk96;

        struct l1_state_dtmf {
            unsigned int ph[8];
            float energy[4];
            float tenergy[4][16];
            int blkcount;
            int lastch;
        } dtmf;

        struct l1_state_zvei {
            unsigned int ph[16];
            float energy[4];
            float tenergy[4][32];
            int blkcount;
            int lastch;
        } zvei;

        struct l1_state_scope {
            int datalen;
            int dispnum;
            float data[512];
        } scope;
    } l1;
};


struct demod_param {
    const char *name;
    unsigned int samplerate;
    unsigned int overlap;
    //void (*init)(struct demod_state *s);
    //void (*demod)(struct demod_state *s, float *buffer, int length);
};


class CAfsk12 : public QObject
{
    Q_OBJECT
public:
    explicit CAfsk12(QObject *parent = 0);
    ~CAfsk12();

    void demod(float *buffer, int length);
    void reset();

signals:

public slots:

private:
    float corr_mark_i[CORRLEN];
    float corr_mark_q[CORRLEN];
    float corr_space_i[CORRLEN];
    float corr_space_q[CORRLEN];

    struct demod_state *state;

    /* HDLC functions */
    void hdlc_init(struct demod_state *s);
    void hdlc_rxbit(struct demod_state *s, int bit);
    void verbprintf(int verb_level, const char *fmt, ...);
};

#endif // CAFSK12_H
