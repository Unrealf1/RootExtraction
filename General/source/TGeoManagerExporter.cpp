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

    writeStyles(wr);

    writeTemplates(wr);

    writeChildren(wr);

    wr.EndBlock();
}

void TGeoManagerExporter::writeTemplates(JSONWriter& wr) const{
    wr.BeginBlock("templates");
    for (auto&& volume : volumes) {
        wr.BeginBlock(volume->GetName());

        writeShape(wr, volume->GetShape());

        auto material = volume->GetMaterial();
        if (material != nullptr) {
            wr.BeginBlock("properties");
            writeMaterial(wr, material);
        }

        if (material != nullptr) {
            wr.EndBlock();
            //End of properties
        }

        wr.EndBlock();
        //End of volume block
    }
    wr.EndBlock();
    //End of "templates"
}

void TGeoManagerExporter::writeMaterial(JSONWriter &wr, TGeoMaterial *material) const {
    if (material != nullptr) {

        wr.BeginBlock("Material");

        wr.AddProperty("name", material->GetName());

        TColor* color = gROOT->GetColor(material->GetDefaultColor());
        wr.AddProperty("color", stringFromColor(color));

        auto opacity = (int64_t)material->GetTransparency();
        if (opacity != 0) {
            wr.AddProperty("opacity", (int64_t)material->GetTransparency());
        }

        wr.EndBlock();
        //End of material
    }
}

void TGeoManagerExporter::writeShape(JSONWriter &wr, TGeoShape *shape) const {
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

    if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBox)) {
        writeBoxPosition(wr, dynamic_cast<TGeoBBox*>(shape));
    }
}

void TGeoManagerExporter::writeComposite(JSONWriter &wr, TGeoCompositeShape *composite) const {
    wr.AddProperty("Type", composite_type);
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
            operator_type = "intersection";
            break;
        case TGeoBoolNode::EGeoBoolType::kGeoSubtraction:
            operator_type = "subtraction";
            break;
        case TGeoBoolNode::EGeoBoolType::kGeoUnion:
            operator_type = "union";
    }
    wr.AddProperty("compositeType", operator_type);
}

void TGeoManagerExporter::writeTube(JSONWriter &wr, TGeoTube *tube) const {
    wr.AddProperty("type", tube_type);

    if (tube->GetRmin() != 0.0) {
        wr.AddProperty("rMin", tube->GetRmin());
    }
    if (tube->GetRmax() != 0.0) {
        wr.AddProperty("rMax", tube->GetRmax());
    }
    if (tube->GetDz() != 0.0) {
        wr.AddProperty("length", tube->GetDz() * 2.0);
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
    wr.AddProperty("type", "cone");
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
    wr.AddProperty("Type", box_type);

    auto origin = box->GetOrigin();
    if (!(origin[0] == 0 && origin[1] == 0 && origin[2] == 0)) {
        wr.BeginBlock("Position");
        wr.AddProperty("x", origin[0]);
        wr.AddProperty("y", origin[1]);
        wr.AddProperty("z", origin[2]);
        wr.EndBlock();
    }
}

void TGeoManagerExporter::writeBoxPosition(JSONWriter &wr, TGeoBBox* box) const {
    std::vector<double> size;

    size.push_back(box->GetDX() * 2.0);
    size.push_back(box->GetDY() * 2.0);
    size.push_back(box->GetDZ() * 2.0);

    if (!(size[0] == 0 && size[1] == 0 && size[2] == 0)) {
        wr.BeginBlock("Size");
        wr.AddProperty("x", size[0]);
        wr.AddProperty("y", size[1]);
        wr.AddProperty("z", size[2]);
        wr.EndBlock();
    }
}

void TGeoManagerExporter::writeChildren(JSONWriter &wr) const {
    wr.BeginBlock("children");

    std::queue<TGeoNode*> nodes;
    for (TObject* obj : *(geoManager->GetTopVolume()->GetNodes())) {
        nodes.push(dynamic_cast<TGeoNode*>(obj));
    }

    while(!nodes.empty()) {
        TGeoNode* current = nodes.front();
        nodes.pop();
        DFSAddNodes(wr, current);
    }

    wr.EndBlock();
    //End of "children"
}

void TGeoManagerExporter::writeStyles(JSONWriter &wr) const {
    std::cerr << "TGeoManagerExporter::WriteStyles is not implemented yet!" << std::endl;
    return;
    wr.BeginBlock("styles");

    wr.BeginBlock("FakeName");
    wr.AddProperty("FakeProperty", "FakeValue");
    wr.EndBlock();

    wr.EndBlock();
    //End of "styles"
}

void TGeoManagerExporter::Prepare() {
    std::queue<TGeoVolume*> volume_que;
    volume_que.push(geoManager->GetMasterVolume());

    while(!volume_que.empty()) {
        TGeoVolume* current_volume = volume_que.front();
        volume_que.pop();
        volumes.insert(current_volume);

        TObjArray* nodes = current_volume->GetNodes();
        if (nodes == nullptr) {
            continue;
        }
        for (int i = 0; i < nodes->GetSize(); ++i) {
            auto node = (TGeoNode*)nodes->At(i);
            if (node == nullptr) {
                continue;
            }
            if (volumes.find(node->GetVolume()) == volumes.end()) {
                volume_que.push(node->GetVolume());
            }
        }

    }

}

std::string TGeoManagerExporter::stringFromColor(TColor *color) const {
    uint16_t red = color->GetRed() * 255;
    uint16_t green = color->GetGreen() * 255;
    uint16_t blue = color->GetBlue() * 255;

    std::stringstream sstr;
    sstr << '#' << std::hex << std::setfill('0') << std::setw(3) << red << green << blue;

    return sstr.str();
}

void TGeoManagerExporter::DFSAddNodes(JSONWriter& wr, TGeoNode* node) const {
    if (node == nullptr) {
        return;
    }
    wr.BeginBlock(node->GetName());

    wr.AddProperty("type", proxy_type);

    wr.AddProperty("templateName", node->GetVolume()->GetName());

    writeMatrix(wr, node->GetMatrix());

    if (node->GetNodes() != nullptr && node->GetNodes()->GetSize() > 0) {
        wr.BeginBlock("children");
        for (auto obj : *(node->GetNodes())) {
            auto nd = dynamic_cast<TGeoNode*> (obj);
            DFSAddNodes(wr, nd);
        }
        wr.EndBlock();
    }
    wr.EndBlock();
}

void TGeoManagerExporter::writeMatrix(JSONWriter &wr, TGeoMatrix *matrix) const {
    if (matrix->IsGeneral() || matrix->IsCombi() || matrix->IsRotation()) {
        WriteRotation(wr, (TGeoRotation*)matrix);
    }

    if (matrix->IsGeneral() || matrix->IsCombi() || matrix->IsTranslation()) {
        writeTranslation(wr, (TGeoTranslation *) matrix);
    }

    if (matrix->IsGeneral() || matrix->IsScale()) {
        writeScale(wr, (TGeoScale *) matrix);
    }
}

void TGeoManagerExporter::WriteRotation(JSONWriter& wr, TGeoRotation* matrix) const{
    double phi = 0.0;
    double theta = 0.0;
    double psi = 0.0;
    matrix->GetAngles(phi, theta, psi);

    wr.BeginBlock("rotation");

    wr.AddProperty("phi", phi);
    wr.AddProperty("theta", theta);
    wr.AddProperty("psi", psi);
    wr.EndBlock();
}

void TGeoManagerExporter::writeTranslation(JSONWriter& wr, TGeoTranslation* matrix) const{
    const double* translation = matrix->GetTranslation();

    wr.BeginBlock("translation");

    wr.AddProperty("x", translation[0]);
    wr.AddProperty("y", translation[1]);
    wr.AddProperty("z", translation[2]);

    wr.EndBlock();
}

void TGeoManagerExporter::writeScale(JSONWriter& wr, TGeoScale* matrix) const {
    const double* scale = matrix->GetScale();
    wr.BeginBlock("scale");

    wr.AddProperty("x", scale[0]);
    wr.AddProperty("y", scale[1]);
    wr.AddProperty("z", scale[2]);

    wr.EndBlock();
}


const std::string TGeoManagerExporter::box_type = "hep.dataforge.vis.spatial.Box";
const std::string TGeoManagerExporter::proxy_type = "hep.dataforge.vis.spatial.Proxy";
const std::string TGeoManagerExporter::composite_type = "hep.dataforge.vis.spatial.Composite";
const std::string TGeoManagerExporter::tube_type = "hep.dataforge.vis.spatial.Tube";

//ClassImp(TGeoManagerExporter);
