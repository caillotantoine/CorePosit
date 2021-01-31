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

#include "posit.h"
#include "decode_posit.h"
#include "quire.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    //Posit pA = 0b0000110111011101;
    //Posit pA = 0b0101010010000000; // 35.9872 encodable en 36.0
    Posit pA = 0b0000111110010011; // 0.000012
    //Posit pA = 0b0000001011101010; //1.2e-11
    //Posit pA = 0b0000000000000100; // 1.2e-29 encodé en 1.262177448353619e-29
    //Posit pA = 0b1111111111111100; // -1.2e-29 encodé en -1.262177448353619e-29
    //Posit pA = 0b0111100100000110; // 70254592
    //Posit pA = 0b0111111111111001; // 1.2379400392853803e+27
    PositDecod pAdec = decodePosit(pA);
    Posit pB = encodePosit(pAdec);
    Quire qA = posit2quire(pA);
    Posit pC = quire2posit(qA);
    PositDecod pCdec = decodePosit(pA);
    printf("Sign \t\t%c\n", pAdec.sign == 0?'+':'-');
    printf("regime \t\t%d\n", pAdec.regime);
    printf("exponent \t%d\n", pAdec.exp);
    printf("fraction \t%x\n", pAdec.frac);
    printf("Value: \t\t%.5e\n", PositToFloat(pA));
    printf("Sign \t\t%c\n", pCdec.sign == 0?'+':'-');
    printf("regime \t\t%d\n", pCdec.regime);
    printf("exponent \t%d\n", pCdec.exp);
    printf("fraction \t%x\n", pCdec.frac);
    printf("Value: \t\t%.5e\n", PositToFloat(pC));
    
    return 0;
}
