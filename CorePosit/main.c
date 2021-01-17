//
//  main.c
//  CorePosit
//
//  Created by CAILLOT Antoine on 17/01/2021.
//

#include <stdio.h>

typedef uint16_t Posit16;
typedef struct {
    uint8_t sign:1,
            exp:7;
    int8_t regime;
    uint16_t frac;
    int8_t es;
} Posit16Decod;

Posit16Decod decodeP16(Posit16 p, int8_t es);



int main(int argc, const char * argv[]) {
    // insert code here...
    Posit16 pA = 0b0000110111011101;
    Posit16Decod pAdec = decodeP16(pA, 3);
    printf("Sign \t\t%c\n", pAdec.sign == 0?'+':'-');
    printf("regime \t\t%d\n", pAdec.regime);
    printf("exponent \t%d\n", pAdec.exp);
    printf("fraction \t%x\n", pAdec.frac);
    return 0;
}

Posit16Decod decodeP16(Posit16 p, int8_t es) {
    uint8_t sign;
    int8_t regime;
    uint8_t to_review = 15;
    uint16_t frac_mask = 0, frac = 0;
    uint8_t exp_mask = 0, i;
    
    for(i = es ;i > 0; i--)
        exp_mask = (exp_mask << 1) + 1;
    
    Posit16Decod out;
    
    sign = (p & (1 << to_review)) >> to_review;
    to_review -= 1;
    
    const uint16_t r0 = (p & (1 << to_review)) >> to_review;
    for(regime = 0; !(((p & (1 << (to_review - regime))) >> (to_review - regime)) ^ r0) && regime < to_review; regime++);
    to_review -= regime;
    regime = r0 == 0 ? -regime: regime - 1;
    
    uint8_t exp = (p >> (to_review - es)) & exp_mask;
    to_review-=3;
    
    for(;to_review > 0; to_review--)
        frac_mask = (frac_mask << 1) + 1;
    frac = p & frac_mask;

    out.sign = sign;
    out.exp = exp;
    out.regime = regime;
    out.frac = frac;
    out.es = es;
    return out;
}

