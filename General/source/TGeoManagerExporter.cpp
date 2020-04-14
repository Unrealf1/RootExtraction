//
// Created by fedor on 9/29/19.
//

#include "TGeoManagerExporter.hpp"

TGeoManagerExporter::TGeoManagerExporter(TGeoManager *geoManager)
:geoManager(geoManager){}

void TGeoManagerExporter::Write(std::ostream &os) {
    if (!prepared) {
        Prepare();
        prepared = true;
    }

    JSONWriter wr(os);
    wr.BeginBlock();

    wr.AddProperty("type", group_type);

    writeStyles(wr);

    writeTemplates(wr);

    writeProperties(wr);

    writeChildren(wr);

    wr.EndBlock();
    wr.Flush();
}

void TGeoManagerExporter::writeProperties(JSONWriter& wr) const {
    wr.BeginBlock(k_properties_name);

    wr.BeginBlock("rotation");

    wr.AddProperty("order", "ZXY");

    // End of block "rotation"
    wr.EndBlock();

    // End of block "properties"
    wr.EndBlock();
}

void TGeoManagerExporter::writeTemplates(JSONWriter& wr) const{
    wr.BeginBlock(k_templates_name);

    writeTemplateVolumes(wr);
    writeTemplateNodes(wr);

    wr.EndBlock();
    //End of "templates" block
}

void TGeoManagerExporter::writeTemplateVolumes(JSONWriter &wr) const {
    for (auto volume : volumes) {
        if (volume == nullptr) {
            continue;
        }
        wr.BeginBlock(volume->GetName());

        writeShape(wr, volume->GetShape());

        auto material = volume->GetMaterial();
        if (material != nullptr) {
            wr.BeginBlock("properties");
            std::string material_entry = getMaterialEntry(material);
            wr.AddProperty("@style", {material_entry});
        }

        if (material != nullptr) {
            wr.EndBlock();
            //End of properties
        }

        wr.EndBlock();
        //End of volume block
    }
}

void TGeoManagerExporter::writeTemplateNodes(JSONWriter &wr) const {
    // Every node is going to templates for now
    for (auto&& node : nodes) {
        wr.BeginBlock(node->GetName());
        writeMatrix(wr, node->GetMatrix());

        if (node->GetNodes() != nullptr && node->GetNodes()->GetSize() > 0) {
            wr.AddProperty("type", group_type);
            wr.BeginBlock(k_children_name);
            for (auto obj : *(node->GetNodes())) {
                auto nd = dynamic_cast<TGeoNode*> (obj);
                writeChildTemplateNode(wr, nd);
            }
            wr.EndBlock();
            // End of children block
        } else {
            wr.AddProperty("type", proxy_type);
            wr.AddProperty(k_template_field, node->GetVolume()->GetName());
        }

        wr.EndBlock();
        // End of node block
    }        
}

void TGeoManagerExporter::writeChildTemplateNode(JSONWriter &wr, TGeoNode* node) const {
    wr.BeginBlock(node->GetName());

    wr.AddProperty("type", proxy_type);

    wr.AddProperty(k_template_field, node->GetName());

    wr.EndBlock();
    // End of node block
}

void TGeoManagerExporter::writeShape(JSONWriter &wr, TGeoShape *shape) const {
    if (shape == nullptr) {
        return;
    }
    if (shape->TestShapeBit(TGeoCompositeShape::EShapeType::kGeoComb)) {
        writeComposite(wr, dynamic_cast<TGeoCompositeShape*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoCone)) {
        writeCone(wr, dynamic_cast<TGeoCone*>(shape));
    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoTube)) {
        writeTube(wr, dynamic_cast<TGeoTube*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoSph)) {
        writeSphere(wr, dynamic_cast<TGeoSphere*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBox)) {
        writeBox(wr, dynamic_cast<TGeoBBox*>(shape));

    } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBad) ||
              shape->TestShapeBit(TGeoShape::EShapeType::kGeoNoShape)) {
        wr.AddProperty("type", "ErrorType");
    } else {
        wr.AddProperty("type", "Unknown");
    }
}

void TGeoManagerExporter::writeComposite(JSONWriter &wr, TGeoCompositeShape *composite) const {
    wr.AddProperty("type", composite_type);
    auto bool_node = composite->GetBoolNode();

    wr.BeginBlock("first");
    writeMatrix(wr, bool_node->GetLeftMatrix());
    writeShape(wr, bool_node->GetLeftShape());
    wr.EndBlock();

    wr.BeginBlock("second");
    writeMatrix(wr, bool_node->GetRightMatrix());
    writeShape(wr, bool_node->GetRightShape());
    wr.EndBlock();

    std::string operator_type;
    switch (bool_node->GetBooleanOperator()) {
        case TGeoBoolNode::EGeoBoolType::kGeoIntersection:
            operator_type = "INTERSECT";
            break;
        case TGeoBoolNode::EGeoBoolType::kGeoSubtraction:
            operator_type = "SUBTRACT";
            break;
        case TGeoBoolNode::EGeoBoolType::kGeoUnion:
            operator_type = "UNION";
    }
    wr.AddProperty("compositeType", operator_type);
}

void TGeoManagerExporter::writeTube(JSONWriter &wr, TGeoTube *tube) const {
    wr.AddProperty("type", tube_type);

    if (tube->GetRmin() != 0.0) {
        wr.AddProperty("innerRadius", tube->GetRmin());
    }
    if (tube->GetRmax() != 0.0) {
        wr.AddProperty("radius", tube->GetRmax());
    }
    if (tube->GetDz() != 0.0) {
        wr.AddProperty("height", tube->GetDz() * 2.0);
    }
}

void TGeoManagerExporter::writeSphere(JSONWriter &wr, TGeoSphere *sphere) const {
    wr.AddProperty("type", "sphere");
    if (sphere->GetRmin() != 0.0) {
        wr.AddProperty("rMin", sphere->GetRmin());
    }
    if (sphere->GetRmax() != 0.0) {
        wr.AddProperty("rMax", sphere->GetRmax());
    }
}

void TGeoManagerExporter::writeCone(JSONWriter& wr, TGeoCone* cone) const {
    wr.AddProperty("type", "3d.cone");
    if (cone->GetDz() != 0.0) {
        wr.AddProperty("length", 2.0 * cone->GetDz());
    }
    if (cone->GetRmin1() != 0.0) {
        wr.AddProperty("rMin1", cone->GetRmin1());
    }
    if (cone->GetRmax1() != 0.0) {
        wr.AddProperty("rMiax", cone->GetRmax1());
    }
    if (cone->GetRmin2() != 0.0) {
        wr.AddProperty("rMin2", cone->GetRmin2());
    }
    if (cone->GetRmax2() != 0.0) {
        wr.AddProperty("rMax2", cone->GetRmax2());
    }

}

void TGeoManagerExporter::writeBox(JSONWriter &wr, TGeoBBox *box) const {
    wr.AddProperty("type", box_type);

    //auto origin = box->GetOrigin();

    writeBoxPosition(wr, box);
}

void TGeoManagerExporter::writeBoxPosition(JSONWriter &wr, TGeoBBox* box) const {

    wr.AddProperty("xSize", box->GetDX() * 2.0);
    wr.AddProperty("ySize", box->GetDY() * 2.0);
    wr.AddProperty("zSize", box->GetDZ() * 2.0);
}

void TGeoManagerExporter::writeChildren(JSONWriter &wr) const {
    auto topVolume = geoManager->GetMasterVolume();
    if (topVolume == nullptr) {
        return;
    }

    wr.BeginBlock(k_children_name);

    writeNode(wr, gGeoManager->GetTopNode());

    auto nds = topVolume->GetNodes();
    if (nds != nullptr && nds->GetSize() > 0) {
        std::cout << "SIZE OF CHILDREN " << nds->GetSize() << std::endl;
        
        for (auto obj : *(nds)) {
            auto nd = dynamic_cast<TGeoNode*> (obj);
            writeNode(wr, nd);
        }
        
    } else {
        std::cout << "NO CHILDREN AT ALL!" << std::endl;
    }
    wr.EndBlock();
    // End of block "children"
}

void TGeoManagerExporter::writeNode(JSONWriter &wr, TGeoNode* node) const {
    if(node == nullptr) {
        return;
    }

    wr.BeginBlock(node->GetName());
    if (node->GetVolume()) {
        wr.AddProperty("type", proxy_type);
        wr.AddProperty(k_template_field, node->GetVolume()->GetName());
    } else {
        wr.AddProperty("type", "ERROR");
    }

    auto matrix = node->GetMatrix();
    if (matrix != nullptr) {
        writeMatrix(wr, matrix);
    }

    wr.EndBlock();
    //End of node block
}

void TGeoManagerExporter::writeStyles(JSONWriter &wr) const {
    wr.BeginBlock(k_styles_name);

    for (auto material : materials) {
        writeStylesMaterial(wr, material);
    }

    wr.EndBlock();
    //End of "styleSheet"
}

void TGeoManagerExporter::writeStylesMaterial(JSONWriter &wr, TGeoMaterial *material) const {
    if (material != nullptr) {

        wr.BeginBlock(getMaterialEntry(material));

        wr.BeginBlock("material");

        TColor* color = gROOT->GetColor(material->GetDefaultColor());
        wr.AddProperty("color", stringFromColor(color));

        auto opacity = (int64_t)material->GetTransparency();
        if (opacity != 0) {
            wr.AddProperty("opacity", (int64_t)material->GetTransparency());
        }

        // End of "material"
        wr.EndBlock();
        

        wr.BeginBlock("gdml");

        wr.AddProperty("material", material->GetName());

        // End of "gdml"
        wr.EndBlock();


        //End of material[]
        wr.EndBlock();
    }
}

std::string TGeoManagerExporter::stringFromColor(TColor *color) const {
    uint16_t red = color->GetRed() * 255;
    uint16_t green = color->GetGreen() * 255;
    uint16_t blue = color->GetBlue() * 255;

    std::stringstream sstr;
    sstr << "0x" << std::hex << std::setfill('0') << std::setw(3) << red << green << blue;

    return sstr.str();
}

void TGeoManagerExporter::Prepare() {
    //BFS over all TGeoVolumes and TGeoNodes starting from MasterVolume

    std::queue<TGeoVolume*> volume_que;
    volume_que.push(geoManager->GetMasterVolume());

    while(!volume_que.empty()) {
        TGeoVolume* current_volume = volume_que.front();
        volume_que.pop();
        volumes.insert(current_volume);
        materials.insert(current_volume->GetMaterial());

        TObjArray* current_nodes = current_volume->GetNodes();
        if (current_nodes == nullptr) {
            continue;
        }
        for (int i = 0; i < current_nodes->GetSize(); ++i) {
            auto node = (TGeoNode*)current_nodes->At(i);
            if (node == nullptr) {
                continue;
            }
            nodes.insert(node);
            if (volumes.find(node->GetVolume()) == volumes.end()) {
                volume_que.push(node->GetVolume());
            }
        }

    }

    std::cout << "Found " << volumes.size() << " unique volumes\n"
    << "Found " << nodes.size() << " unique nodes\n"
    << "Found " << materials.size() << " unique materials" << std::endl;
}

void TGeoManagerExporter::writeMatrix(JSONWriter &wr, TGeoMatrix* matrix) const {
    if (matrix == nullptr) {
        return;
    }

    if (matrix->IsGeneral()) {
        writeGeneralMatrix(wr, (TGeoGenTrans*) matrix);
    } else if (matrix->IsCombi()) {
        writeCombination(wr, (TGeoCombiTrans*) matrix);   
    } else if (matrix->IsRotation()) {
        writeRotation(wr, (TGeoRotation*) matrix);
    } else if (matrix->IsTranslation() && typeid(*matrix) == typeid(TGeoTranslation)) {
        writeTranslation(wr, (TGeoTranslation*)matrix);
    } else if (matrix->IsScale()) {
        writeScale(wr, (TGeoScale*) matrix);
    } else {
        std::cerr << "TGeoManagerExporter::writeMatrix - unknown matrix type" << std::endl;
    }
}

double TGeoManagerExporter::DegreeToRad(double angle) const {
    constexpr double Pi = 3.14159265;
    return angle * Pi / 180;
}

void TGeoManagerExporter::writeRotationBlock(JSONWriter& wr, double phi, double theta, double psi) const {
    wr.BeginBlock("rotation");

    wr.AddProperty("z", DegreeToRad(phi));
    wr.AddProperty("x", DegreeToRad(theta));
    wr.AddProperty("y", DegreeToRad(psi));
    wr.EndBlock();
}

void TGeoManagerExporter::writeTranslationBlock(JSONWriter& wr, double x, double y, double z) const {
    wr.BeginBlock("position");

    wr.AddProperty("x", x);
    wr.AddProperty("y", y);
    wr.AddProperty("z", z);

    wr.EndBlock();
}

void TGeoManagerExporter::writeScaleBlock(JSONWriter& wr, double x, double y, double z) const {
    wr.BeginBlock("scale");

    wr.AddProperty("x", x);
    wr.AddProperty("y", y);
    wr.AddProperty("z", z);

    wr.EndBlock();
}

void TGeoManagerExporter::writeGeneralMatrix(JSONWriter& wr, TGeoGenTrans* matrix) const {
    writeCombination(wr, matrix);
    const double* scale = matrix->GetScale();
    if (scale == nullptr) {
        return;
    }
    writeScaleBlock(wr, scale[0], scale[1], scale[2]);
}

void TGeoManagerExporter::writeCombination(JSONWriter& wr, TGeoCombiTrans* matrix) const {
    TGeoRotation* rotation = matrix->GetRotation();
    if (rotation != nullptr) {
        writeRotation(wr, rotation);
    }
    
    const double* translation = matrix->GetTranslation();
    if (translation != nullptr) {
        writeTranslationBlock(wr, translation[0], translation[1], translation[2]);
    } 
}

void TGeoManagerExporter::writeRotation(JSONWriter& wr, TGeoRotation* matrix) const{
    double phi = 0.0;
    double theta = 0.0;
    double psi = 0.0;
    matrix->GetAngles(phi, theta, psi);
    writeRotationBlock(wr, phi, theta, psi);    
}

void TGeoManagerExporter::writeTranslation(JSONWriter& wr, TGeoTranslation* matrix) const{
    const double* translation = matrix->GetTranslation();
    writeTranslationBlock(wr, translation[0], translation[1], translation[2]);
}

void TGeoManagerExporter::writeScale(JSONWriter& wr, TGeoScale* matrix) const {
    const double* scale = matrix->GetScale();
    writeScaleBlock(wr, scale[0], scale[1], scale[2]);
}

std::string TGeoManagerExporter::getMaterialEntry(TGeoMaterial* material) const {
    return "material[" + std::string(material->GetName()) + "]";
}


const char* const TGeoManagerExporter::box_type = "3d.box";
const char* const TGeoManagerExporter::proxy_type = "3d.proxy";
const char* const TGeoManagerExporter::composite_type = "3d.composite";
const char* const TGeoManagerExporter::tube_type = "3d.tube";
const char* const TGeoManagerExporter::group_type = "group.3d";

const char* const TGeoManagerExporter::k_templates_name = "prototypes";
const char* const TGeoManagerExporter::k_children_name = "children";
const char* const TGeoManagerExporter::k_styles_name = "styleSheet";
const char* const TGeoManagerExporter::k_properties_name = "properties";
const char* const TGeoManagerExporter::k_template_field = "templateName";
//ClassImp(TGeoManagerExporter);
