//
// Created by fedor on 9/29/19.
//

#ifndef ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
#define ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP

#include <queue>
#include <unordered_set>
#include <ostream>
#include <TGeoManager.h>
#include "TGeometry.h"
//#include "TGeoManager.h"
#include "JSON/JSONWriter.hpp"

class TGeoManagerExporter {
public:
    explicit TGeoManagerExporter(TGeoManager* geoManager);

    void Write(std::ostream& os);
private:
    bool prepared = false;
    std::unordered_set<TGeoVolume*> volumes;
    TGeoManager* geoManager;

    void Prepare();
    //ClassDef(TGeoManagerExporter, 1);
};


#endif //ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
