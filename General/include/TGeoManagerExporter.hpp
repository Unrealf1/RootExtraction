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
#include "JSON/JSONWriter.hpp"
#include "TGeoBBox.h"
#include <iostream>
#include "TColor.h"
#include <TROOT.h>
#include <sstream>
#include <TGeoNode.h>
#include <TObject.h>

class TGeoManagerExporter {
public:
    explicit TGeoManagerExporter(TGeoManager* geoManager);

    void Write(std::ostream& os);
private:
    bool prepared = false;
    std::unordered_set<TGeoVolume*> volumes;
    TGeoManager* geoManager;

    void Prepare();

    void writeTemplates(JSONWriter& wr) const;
    void writeChildren(JSONWriter& wr) const;
    void writeStyles(JSONWriter& wr) const;
    std::string stringFromColor(TColor* color) const;
    void DFSAddNodes(JSONWriter& wr, TGeoNode* node) const;
    void WriteRotation(JSONWriter& wr, TGeoRotation* matrix) const;
    void WriteTranslation(JSONWriter& wr, TGeoTranslation* matrix) const;
    void WriteScale(JSONWriter& wr, TGeoScale* matrix) const;

    //ClassDef(TGeoManagerExporter, 1);
};


#endif //ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
