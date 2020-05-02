//
// Created by fedor on 2/05/20.
//

#ifndef ROOTEXTRACTION_TGEOMANAGERPARSER_HPP
#define ROOTEXTRACTION_TGEOMANAGERPARSER_HPP

//#include "TGDMLWrite.h"
#include "ParsedTGeoManager.hpp"
#include "RootMagic.hpp"
#include <unordered_set>
#include <map>

class TGeoManagerParser {
public:
    static ParsedTGeoManager parse(TGeoManager* geoManager);
    static ParsedShape* parseShape(TGeoShape* shape);
private:
    static void parseVolumes(
            TGeoNode* node,
            std::unordered_map<std::string, ParsedVolume*> parsedVolumes,
            StructLst* fAccPatt,
            StructLst* fRejShape);

    static std::unordered_set<ParsedMaterial*> parseMaterials(TList* materials);
    static ParsedMaterial* parseMaterial(TGeoMaterial* material);

    static void UnsetTemporaryBits(TGeoManager* geoMng);
    static const UInt_t fgkProcBit    = BIT(14);
    static const UInt_t fgkProcBitVol = BIT(19);
};

#endif //ROOTEXTRACTION_TGEOMANAGERPARSER_HPP