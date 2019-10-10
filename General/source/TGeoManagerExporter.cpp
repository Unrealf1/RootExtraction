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
    wr.AddProperty("templates");
    wr.BeginBlock();
    for (auto&& volume : volumes) {
        wr.AddProperty(volume->GetName());
        wr.BeginBlock();

        std::vector<std::pair<std::string, std::string>> additional_properties;
        std::vector<double> position;
        std::vector<double> size;

        std::string type;
        auto shape = volume->GetShape();
        if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBox)) {
            type = "box";
            auto box = dynamic_cast<TGeoBBox*>(shape);
            position.push_back(box->GetOrigin()[0]);
            position.push_back(box->GetOrigin()[1]);
            position.push_back(box->GetOrigin()[2]);

            size.push_back(box->GetDX() * 2.0);
            size.push_back(box->GetDY() * 2.0);
            size.push_back(box->GetDZ() * 2.0);
        }

        if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoCone)) {
            type += "cone";
        }
        if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoTube)) {
            type += "tube";
        }
        if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBad) ||
            shape->TestShapeBit(TGeoShape::EShapeType::kGeoNoShape)) {
            type += "ErrorType";
        }

        if (type.empty()) {
            type += "Unknown";
        }

        wr.AddProperty("Type", type);
        auto material = volume->GetMaterial();
        if (material != nullptr) {
            wr.AddProperty("Material");
            wr.BeginBlock();

            wr.AddProperty("name", material->GetName());

            TColor* color = gROOT->GetColor(material->GetDefaultColor());
            wr.AddProperty("color", stringFromColor(color));

            wr.AddProperty("opacity", (int64_t)material->GetTransparency());

            wr.EndBlock();
            //End of material
        }
        if (!position.empty() && !(position[0] == 0 && position[1] == 0 && position[2] == 0)) {
            wr.AddProperty("Position");
            wr.BeginBlock();
            wr.AddProperty("x", position[0]);
            wr.AddProperty("y", position[1]);
            wr.AddProperty("z", position[2]);
            wr.EndBlock();
        }

        if (!size.empty() && !(size[0] == 0 && size[1] == 0 && size[2] == 0)) {
            wr.AddProperty("Size");
            wr.BeginBlock();
            wr.AddProperty("x", size[0]);
            wr.AddProperty("y", size[1]);
            wr.AddProperty("z", size[2]);
            wr.EndBlock();
        }

        wr.EndBlock();
    }
    wr.EndBlock();
    //End of "templates"
}

void TGeoManagerExporter::writeChildren(JSONWriter &wr) const {
    wr.AddProperty("children");
    wr.BeginBlock();

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
    wr.AddProperty("styles");
    wr.BeginBlock();

    wr.AddProperty("FakeName");
    wr.BeginBlock();
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
    wr.AddProperty(node->GetName());
    wr.BeginBlock();

    wr.AddProperty("type", "proxy");

    wr.AddProperty("templateName", node->GetVolume()->GetName());

    auto matrix = node->GetMatrix();
    if (matrix->IsGeneral() || matrix->IsCombi() || matrix->IsRotation()) {
        WriteRotation(wr, (TGeoRotation*)matrix);
    }

    if (matrix->IsGeneral() || matrix->IsCombi() || matrix->IsTranslation()) {
        WriteTranslation(wr, (TGeoTranslation*)matrix);
    }

    if (matrix->IsGeneral() || matrix->IsScale()) {
        WriteScale(wr, (TGeoScale*)matrix);
    }

    if (node->GetNodes() != nullptr && node->GetNodes()->GetSize() > 0) {
        wr.AddProperty("children");
        wr.BeginBlock();
        for (auto obj : *(node->GetNodes())) {
            auto nd = dynamic_cast<TGeoNode*> (obj);
            DFSAddNodes(wr, nd);
        }
        wr.EndBlock();
    }
    wr.EndBlock();
}

void TGeoManagerExporter::WriteRotation(JSONWriter& wr, TGeoRotation* matrix) const{
    double phi = 0.0;
    double theta = 0.0;
    double psi = 0.0;
    matrix->GetAngles(phi, theta, psi);

    wr.AddProperty("rotation");
    wr.BeginBlock();

    wr.AddProperty("phi", phi);
    wr.AddProperty("theta", theta);
    wr.AddProperty("psi", psi);
    wr.EndBlock();
}

void TGeoManagerExporter::WriteTranslation(JSONWriter& wr, TGeoTranslation* matrix) const{
    const double* translation = matrix->GetTranslation();

    wr.AddProperty("translation");
    wr.BeginBlock();

    wr.AddProperty("x", translation[0]);
    wr.AddProperty("y", translation[1]);
    wr.AddProperty("z", translation[2]);

    wr.EndBlock();
}

void TGeoManagerExporter::WriteScale(JSONWriter& wr, TGeoScale* matrix) const {
    const double* scale = matrix->GetScale();
    wr.AddProperty("scale");
    wr.BeginBlock();

    wr.AddProperty("x", scale[0]);
    wr.AddProperty("y", scale[1]);
    wr.AddProperty("z", scale[2]);

    wr.EndBlock();
}

//ClassImp(TGeoManagerExporter);
