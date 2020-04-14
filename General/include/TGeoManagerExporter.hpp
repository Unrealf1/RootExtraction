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
    std::unordered_set<TGeoVolume*> volumes;
    std::unordered_set<TGeoNode*> nodes;
    std::unordered_set<TGeoMaterial*> materials;

    TGeoManager* geoManager;

    static const char *const box_type;
    static const char *const proxy_type;
    static const char *const composite_type;
    static const char *const tube_type;
    static const char *const group_type;

    static const char *const k_templates_name;
    static const char *const k_children_name;
    static const char *const k_styles_name;
    static const char *const k_properties_name;
    static const char *const k_template_field;

    // A function where all of the actual extraction of the data occurs
    void Prepare();

    // These functions write "prototypes" section
    void writeTemplates(JSONWriter& wr) const;
    void writeTemplateVolumes(JSONWriter& wr) const;
    void writeTemplateNodes(JSONWriter& wr) const;
    void writeChildTemplateNode(JSONWriter& wr, TGeoNode* node) const;

    // A function that writes top level "properties" section
    void writeProperties(JSONWriter& wr) const;

    // These two functions write top level "children" section
    void writeChildren(JSONWriter& wr) const;
    void writeNode(JSONWriter &wr, TGeoNode* node) const;

    // Functions for top level "styleSheet" section
    void writeStyles(JSONWriter& wr) const;
    std::string stringFromColor(TColor* color) const;
    void writeStylesMaterial(JSONWriter& wr, TGeoMaterial* material) const;

    // Functions to identify and serialize various shapes
    void writeShape(JSONWriter& wr, TGeoShape* shape) const;
    void writeComposite(JSONWriter& wr, TGeoCompositeShape* composite) const;
    void writeTube(JSONWriter& wr, TGeoTube* tube) const;
    void writeBox(JSONWriter& wr, TGeoBBox* shape) const;
    void writeCone(JSONWriter& wr, TGeoCone* shape) const;
    void writeSphere(JSONWriter& wr, TGeoSphere* sphere) const;
    void writeBoxPosition(JSONWriter& wr, TGeoBBox* box) const;

    // Set of functions to identify and serialize various matrices
    void writeMatrix(JSONWriter& wr, TGeoMatrix* matrix) const;
    void writeGeneralMatrix(JSONWriter& wr, TGeoGenTrans* matrix) const;
    void writeCombination(JSONWriter& wr, TGeoCombiTrans* matrix) const;
    void writeRotation(JSONWriter& wr, TGeoRotation* matrix) const;
    void writeTranslation(JSONWriter& wr, TGeoTranslation* matrix) const;
    void writeScale(JSONWriter& wr, TGeoScale* matrix) const;
    void writeRotationBlock(JSONWriter& wr, double phi, double theta, double psi) const;
    void writeTranslationBlock(JSONWriter& wr, double x, double y, double z) const;
    void writeScaleBlock(JSONWriter& wr, double x, double y, double z) const;

    // Utility functions
    // returns string "material[material_name]" where material_name is replaced by the name of material
    std::string getMaterialEntry(TGeoMaterial* material) const;
    double DegreeToRad(double angle) const;

    //ClassDef(TGeoManagerExporter, 2223);
};

#endif //ROOTEXTRACTION_TGEOMANAGEREXPORTER_HPP
