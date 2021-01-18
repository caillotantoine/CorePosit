//
//  main.c
//  CorePosit
//
//  Created by CAILLOT Antoine on 17/01/2021.
//


// online posit converter https://posithub.org/widget/plookup
// Posit standard reference https://posithub.org/docs/posit_standard.pdf
// Posit - Float comparison http://www.johngustafson.net/pdfs/BeatingFloatingPoint.pdf


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
    uint8_t to_review = 15;
    uint16_t frac_mask = 0;
    uint8_t exp_mask = 0, i;
    uint16_t r0 = 0, regime = 0;
    Posit16Decod out;
    
    
    // SIGN
    out.sign = (p & (1 << to_review)) >> to_review;
    to_review -= 1; // we read 1 bit
    
    
    // REGIME
    r0 = (p & (1 << to_review)) >> to_review; // first regime bit
    for(; !(((p & (1 << (to_review - regime))) >> (to_review - regime)) ^ r0) && regime < to_review; regime++); // find the first diffent bit
    to_review -= regime; // we read the bits of the regime
    out.regime = r0 == 0 ? -regime: regime - 1; // converting the regime to a mathematical value
    
    
    //  EXPONENT
    out.es = es; // Get the Exponent size (default is 1 for 16 bits)
    
    for(i = es ;i > 0; i--) exp_mask = (exp_mask << 1) + 1; // From the exponent size, create the mask to get the exponent
    out.exp = (p >> (to_review - es)) & exp_mask; // extracting the exponent
    to_review-=es; // we read es bits
    
    
    // FRAC
    for(;to_review > 0; to_review--) frac_mask = (frac_mask << 1) + 1; // create a mask with the remaining bits
    out.frac = p & frac_mask;

    return out;
}

