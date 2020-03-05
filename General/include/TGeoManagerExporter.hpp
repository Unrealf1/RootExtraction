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
#include <unordered_set>
#include <unordered_map>

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

class TGeoManagerExporter {
public:
    explicit TGeoManagerExporter(TGeoManager* geoManager);

    // A function to write TGeoManager to a given stream in JSON format
    void Write(std::ostream& os);
private:
    bool prepared = false;
    std::unordered_map<TGeoVolume*, std::string> volumes;
    std::unordered_set<TGeoNode*> nodes;
    std::unordered_set<TGeoMaterial*> materials;

    TGeoManager* geoManager;
    
    static const std::string box_type;
    static const std::string proxy_type;
    static const std::string composite_type;
    static const std::string tube_type;
    
    static const std::string k_templates_name;
    static const std::string k_children_name; 

    // A function where all of the actual extraction of data occurs
    void Prepare();

    void writeTemplates(JSONWriter& wr) const;
    void writeTemplateVolumes(JSONWriter& wr) const;
    void writeTemplateNodes(JSONWriter& wr) const;
    void writeChildTemplateNode(JSONWriter& wr, TGeoNode* node) const;
    void writeVolumeTemplateNode(JSONWriter &wr, TGeoVolume* volume) const;

    void writeShape(JSONWriter& wr, TGeoShape* shape) const;
    void writeComposite(JSONWriter& wr, TGeoCompositeShape* composite) const;
    void writeTube(JSONWriter& wr, TGeoTube* tube) const;
    void writeBox(JSONWriter& wr, TGeoBBox* shape) const;
    void writeCone(JSONWriter& wr, TGeoCone* shape) const;
    void writeSphere(JSONWriter& wr, TGeoSphere* sphere) const;
    void writeBoxPosition(JSONWriter& wr, TGeoBBox* box) const;

    void writeChildren(JSONWriter& wr) const;
    void writeNode(JSONWriter &wr, TGeoNode* node) const;

    void writeStyles(JSONWriter& wr) const;
    std::string stringFromColor(TColor* color) const;
    void writeStylesMaterial(JSONWriter& wr, TGeoMaterial* material) const;
    void writeMatrix(JSONWriter& wr, TGeoMatrix* matrix) const;
    void writeGeneralMatrix(JSONWriter& wr, TGeoGenTrans* matrix) const;
    void writeCombination(JSONWriter& wr, TGeoCombiTrans* matrix) const;
    void writeRotation(JSONWriter& wr, TGeoRotation* matrix) const;
    void writeTranslation(JSONWriter& wr, TGeoTranslation* matrix) const;
    void writeScale(JSONWriter& wr, TGeoScale* matrix) const;
    void writeRotationBlock(JSONWriter& wr, double phi, double theta, double psi) const;
    void writeTranslationBlock(JSONWriter& wr, double x, double y, double z) const;
    void writeScaleBlock(JSONWriter& wr, double x, double y, double z) const;

    // return string "material[material_name]" where material_name is replaced by the name of material
    std::string getMaterialEntry(TGeoMaterial* material) const;
    double DegreeToRad(double angle) const;

    void writeProperties(JSONWriter& wr) const;
    //ClassDef(TGeoManagerExporter, 2223);
};

#endif //ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
