//
//  decode_posit.h
//  CorePosit
//
//  Created by CAILLOT Antoine on 31/01/2021.
//

#ifndef decode_posit_h
#define decode_posit_h

#include <stdio.h>
#include <strings.h>

#include "posit.h"

PositDecod decodePosit(Posit p);
float PositToFloat(Posit p);
Quire posit2quire(Posit p);
Posit encodePosit(PositDecod p);
Posit quire2posit(Quire q);

#endif /* decode_posit_h */
