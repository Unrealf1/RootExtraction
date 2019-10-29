//
// Created by fedor on 9/29/19.
//

#ifndef ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
#define ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP

#include "JSON/JSONWriter.hpp"
#include <iostream>
#include <ostream>
#include <queue>
#include <sstream>
#include <TColor.h>
#include <TGeoBBox.h>
#include <TGeoBoolNode.h>
#include <TGeoCompositeShape.h>
#include <TGeoCone.h>
#include <TGeoManager.h>
#include <TGeometry.h>
#include <TGeoNode.h>
#include <TGeoSphere.h>
#include <TGeoTube.h>
#include <TObject.h>
#include <TROOT.h>
#include <unordered_set>
#include <unordered_map>

class TGeoManagerExporter {
public:
    explicit TGeoManagerExporter(TGeoManager* geoManager);

    void Write(std::ostream& os);
private:
    bool prepared = false;
    std::unordered_set<TGeoVolume*> volumes;
    std::unordered_map<TGeoNode*, uint32_t> nodes;
    TGeoManager* geoManager;
    static const std::string box_type;
    static const std::string proxy_type;
    static const std::string composite_type;
    static const std::string tube_type;

    void Prepare();

    void writeTemplates(JSONWriter& wr) const;
    void writeTemplateVolumes(JSONWriter& wr) const;
    void writeTemplateNodes(JSONWriter& wr) const;
    void writeChildTemplateNode(JSONWriter& wr, TGeoNode* node) const;

    void writeShape(JSONWriter& wr, TGeoShape* shape) const;
    void writeComposite(JSONWriter& wr, TGeoCompositeShape* composite) const;
    void writeTube(JSONWriter& wr, TGeoTube* tube) const;
    void writeBox(JSONWriter& wr, TGeoBBox* shape) const;
    void writeCone(JSONWriter& wr, TGeoCone* shape) const;
    void writeSphere(JSONWriter& wr, TGeoSphere* sphere) const;
    void writeBoxPosition(JSONWriter& wr, TGeoBBox* box) const;

    void writeChildren(JSONWriter& wr) const;
    void writeStyles(JSONWriter& wr) const;
    std::string stringFromColor(TColor* color) const;
    void DFSAddNodes(JSONWriter& wr, TGeoNode* node) const;
    void writeMaterial(JSONWriter& wr, TGeoMaterial* material) const;
    void writeMatrix(JSONWriter& wr, TGeoMatrix* matrix) const;
    void WriteRotation(JSONWriter& wr, TGeoRotation* matrix) const;
    void writeTranslation(JSONWriter& wr, TGeoTranslation* matrix) const;
    void writeScale(JSONWriter& wr, TGeoScale* matrix) const;

    //ClassDef(TGeoManagerExporter, 2223);
};

#endif //ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
