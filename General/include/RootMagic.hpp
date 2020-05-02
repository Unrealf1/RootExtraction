//
// Created by fedor on 5/3/20.
//

#ifndef ROOTEXTRACTION_ROOTMAGIC_HPP
#define ROOTEXTRACTION_ROOTMAGIC_HPP

#include <TMath.h>
#include "Rtypes.h"
#include "ParsedShape.hpp"

typedef  std::map<TString, Bool_t>   NameList;

struct StructLst {
    NameList fLst;
};

Rotation GetXYZangles(const Double_t * rotationMatrix)
{
    Rotation lxyz;
    Double_t a, b, c;
    Double_t rad = 180.0 / TMath::ACos(-1.0);
    const Double_t *r = rotationMatrix;
    Double_t cosb = TMath::Sqrt(r[0] * r[0] + r[1] * r[1]);
    if (cosb > 0.00001) {
        a = TMath::ATan2(r[5], r[8]) * rad;
        b = TMath::ATan2(-r[2], cosb) * rad;
        c = TMath::ATan2(r[1], r[0]) * rad;
    } else {
        a = TMath::ATan2(-r[7], r[4]) * rad;
        b = TMath::ATan2(-r[2], cosb) * rad;
        c = 0;
    }
    lxyz.x = a;
    lxyz.y = b;
    lxyz.z = c;
    return lxyz;
}



#endif //ROOTEXTRACTION_ROOTMAGIC_HPP
