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
#include <strings.h>

typedef uint16_t Posit16;
typedef struct {
    uint8_t sign:1,
            exp:7;
    int8_t regime;
    uint16_t frac;
    int8_t es;
    uint8_t fs;
} Posit16Decod;

Posit16Decod decodeP16(Posit16 p, int8_t es);
float P16toFloat(Posit16 p, int8_t es);


int main(int argc, const char * argv[]) {
    // insert code here...
    //Posit16 pA = 0b0000110111011101;
    //Posit16 pA = 0b0101010010000000; // 35.9872 encodable en 36.0
    //Posit16 pA = 0b0000111110010011; // 0.000012
    //Posit16 pA = 0b0000001011101010; //1.2e-11
    //Posit16 pA = 0b0000000000000100; // 1.2e-29 encodé en 1.262177448353619e-29
    Posit16 pA = 0b1111111111111100; // -1.2e-29 encodé en -1.262177448353619e-29
    Posit16Decod pAdec = decodeP16(pA, 3);
    printf("Sign \t\t%c\n", pAdec.sign == 0?'+':'-');
    printf("regime \t\t%d\n", pAdec.regime);
    printf("exponent \t%d\n", pAdec.exp);
    printf("fraction \t%x\n", pAdec.frac);
    printf("Value: \t\t%.5e\n", P16toFloat(pA, 3));
    
    return 0;
}

// +001110001110101  1.2e-3
// -110001110001011 -1.2e-3

Posit16Decod decodeP16(Posit16 p, int8_t es) {
    uint8_t to_review = 15, r0 = 0, regime = 0;
    Posit16Decod out;
    Posit16 a = p;
    
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
    out.es = es; // Get the Exponent size (default is 1 for 16 bits)
    if(to_review <= es)
    {
        out.exp = a & ((1 << to_review) - 1);
        out.frac = 0;
        out.fs = 0;
        return out;
    }
    out.exp = (a >> (to_review - es)) & ((1 << es) - 1); // extracting the exponent
    to_review-=es; // we read es bits
    
    
    // FRAC
    out.frac = a & ((1 << to_review) - 1);
    out.fs = to_review;

    return out;
}

float P16toFloat(Posit16 p, int8_t es) {
    typedef union {
        uint32_t bin;
        float f;
    } BinFloat;
    
    BinFloat out;
    Posit16Decod decodedPosit;
    decodedPosit = decodeP16(p, es);
    uint8_t rShift = 1, i, exp = 0;
    
    out.bin = 0;
    for(i=0; i<es; i++)
        rShift *= 2;
    
    exp = decodedPosit.exp + (decodedPosit.regime * rShift) + 127;
    
    out.bin = decodedPosit.sign << 31;
    out.bin |= exp << 23;
    out.bin |= decodedPosit.frac << (23 - decodedPosit.fs);
    
    return out.f;
}
