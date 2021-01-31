//
//  decode_posit.c
//  CorePosit
//
//  Created by CAILLOT Antoine on 31/01/2021.
//

#include "decode_posit.h"

#include <stdio.h>
#include <strings.h>

#include "posit.h"
#include "quire.h"

// +001110001110101  1.2e-3
// -110001110001011 -1.2e-3

PositDecod decodePosit(Posit p) {
    uint8_t to_review = P_SIZE-1, r0 = 0, regime = 0;
    PositDecod out;
    Posit a = p;
    
    // SIGN
    out.sign = (a & (1 << to_review)) >> to_review;
    to_review -= 1; // we read 1 bit
    
    if(out.sign)
        a = (~a) + 1;
    
    // REGIME
    r0 = (a & (1 << to_review)) >> to_review; // first regime bit
    regime = __builtin_clz((r0?~a:a) << ((sizeof(int)*8) - to_review - 1)); // find the first diffent bit
    to_review -= regime; // we read the bits of the regime
    out.regime = r0 == 0 ? -regime: regime - 1; // converting the regime to a mathematical value
    
    
    //  EXPONENT
    if(to_review <= P_ES)
    {
        out.exp = a & ((1 << to_review) - 1);
        out.frac = 0;
        return out;
    }
    out.exp = (a >> (to_review - P_ES)) & ((1 << P_ES) - 1); // extracting the exponent
    to_review-=P_ES; // we read es bits
    
    
    // FRAC
    out.frac = a & ((1 << to_review) - 1);
    
#if P_SIZE > 16
    out.frac <<= (32-to_review);
#else
    out.frac <<= (16-to_review);
#endif

    return out;
}

Posit encodePosit(PositDecod p)
{
    Posit out = 0;
    int remainingSpace = 0;
    if(p.regime < 0)
    {
        out = 1 << (P_SIZE+p.regime-2);
        remainingSpace = P_SIZE+p.regime-2;
    }
    else
    {
        out = ((1 << (p.regime+1)) - 1) << (P_SIZE-p.regime-2);
        remainingSpace = P_SIZE-p.regime-3;
    }
        
    if(P_SIZE-p.regime-3-P_ES >= 0)
    {
        out |= (p.exp & (1 << P_ES) - 1) << (remainingSpace-P_ES);
        remainingSpace -= P_ES;
    }
    else
    {
        out |= (p.exp & (1 << remainingSpace) - 1);
        return out;
    }
    
    
#if P_SIZE > 16
    uint32_t frac = p.frac >> (32-remainingSpace);
#else
    uint16_t frac = p.frac >> (16-remainingSpace);
#endif
    
    out |= frac;
    
    if(p.sign)
        out = ~out + 0x1;
    return out;
}

float PositToFloat(Posit p) {
    typedef union {
        uint32_t bin;
        float f;
    } BinFloat;
    
    BinFloat out;
    PositDecod decodedPosit;
    decodedPosit = decodePosit(p);
    uint8_t rShift = 1, i, exp = 0;
    
    out.bin = 0;
    for(i=0; i<P_ES; i++)
        rShift *= 2;
    
    exp = decodedPosit.exp + (decodedPosit.regime * rShift) + 127;
    
    out.bin = decodedPosit.sign << 31;
    out.bin |= exp << 23;
    
#if P_SIZE > 16
    out.bin |= decodedPosit.frac >> (32-23);
#else
    out.bin |= decodedPosit.frac << (23-16);
#endif
    
    return out.f;
}

Quire posit2quire(Posit p)
{
    PositDecod decodedP = decodePosit(p);
    Quire out;
    unsigned int nq = ((P_SIZE * P_SIZE) >> 2) - (P_SIZE >> 1);
    int i, rShift = 1, exp;
    
    QZERO(&out);
    
#if P_SIZE > 16
    uint32_t frac = decodedP.frac;
    out.reg[0] = (uint8_t) (frac & 0xff);
    out.reg[1] = (uint8_t) ((frac >> 8) & 0xff);
    out.reg[2] = (uint8_t) ((frac >> 16) & 0xff);
    out.reg[3] = (uint8_t) ((frac >> 24) & 0xff);
    out.reg[4] = 0x01;

    QSL(&out, nq-32);
#else
    uint16_t frac = decodedP.frac;
    out.reg[0] = (uint8_t) (frac & 0xff);
    out.reg[1] = (uint8_t) ((frac >> 8) & 0xff);
    out.reg[2] = 0x01;
    QSL(&out, nq-16);
#endif
    
    for(i=0; i<P_ES; i++)
        rShift *= 2;
    exp = decodedP.exp + (decodedP.regime * rShift);
    
    if(exp > 0)
        QSL(&out, exp);
    else if (exp < 0)
        QSR(&out, -exp);
    
    if(decodedP.sign)
        QNEG(&out);
    
    return out;
}

// 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 11001001 10000000 00000000 00000000 00000000
//                                                                                  56       48       40       32       24       16       8
//                                                                                I F
//                                                                                0 -------- -------- -------- -------- -------- -------- --------
//                                                                                0 00000000 01111111 11122222 22222333 33333334 44444444 45555555
//                                                                                0 12345678 90123456 78901234 56789012 34567890 12345678 90123456

// 2^-17+2^-18+2^-21+2^-24+2^-25 = 0.00001201033

Posit quire2posit(Quire q)
{
    Quire Qcopy = q;
    PositDecod decodedP;
    int i, chunk = -1, bit = -1, rShift = 1, exp = 0;
    unsigned int nq = ((P_SIZE * P_SIZE) >> 2) - (P_SIZE >> 1);
    const unsigned int I_MSB = (nq*2)/8-1;
#if P_SIZE > 16
    uint32_t frac;
#else
    uint16_t frac;
#endif
    decodedP.sign = 0;
    decodedP.exp = 0;
    decodedP.regime = 0;
    decodedP.frac = 0;
    if(Qcopy.reg[Q_SIZE-1] & 0b10000000)
    {
        QNEG(&Qcopy);
        decodedP.sign = 1;
    }
    
    for(i = I_MSB; i > 0; i--)
    {
        if(Qcopy.reg[i] > 0)
            break;
    }
    chunk = i;
    for(i = 7; i>0; i--)
    {
        if(Qcopy.reg[chunk] & 1<<i)
            break;
    }
    bit = i;
    
    for(i=0; i<P_ES; i++)
        rShift *= 2;
    
    exp = 8 * chunk + bit - nq;
    decodedP.regime = exp / rShift;
    if(exp % rShift < 0)
    {
        decodedP.regime --;
    }
    decodedP.exp = exp - (decodedP.regime * rShift);
    
#if P_SIZE > 16
    QSR(&Qcopy, 8 * chunk + bit - 32);
    frac = (((uint32_t) Qcopy.reg[3]) << 24) | (((uint32_t) Qcopy.reg[2]) << 16) | (((uint32_t) Qcopy.reg[1]) << 8) | ((uint16_t) Qcopy.reg[0]);
#else
    QSR(&Qcopy, 8 * chunk + bit - 16);
    frac = (((uint16_t) Qcopy.reg[1]) << 8) | ((uint16_t) Qcopy.reg[0]);
#endif
    
    decodedP.frac = frac;
    
    return encodePosit(decodedP);
}
