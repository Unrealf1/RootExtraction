//
// Created by fedor on 9/29/19.
//

#include "TGeoManagerExporter.hpp"
#include "TGeoBBox.h"
#include <iostream>

TGeoManagerExporter::TGeoManagerExporter(TGeoManager *geoManager)
:geoManager(geoManager){}

void TGeoManagerExporter::Write(std::ostream &os) {
    if (!prepared) {
        Prepare();
        prepared = true;
    }

    JSONWriter wr(os);
    wr.BeginBlock();

    //Step one: write styles?
    writeStyles(wr);
    //Step two: write all volumes to templates
    writeTemplates(wr);
    //Step three: write all nodes
    writeChildren(wr);

    wr.EndBlock();
}

void TGeoManagerExporter::writeTemplates(JSONWriter& wr) {
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

            size.push_back(box->GetDX());
            size.push_back(box->GetDY());
            size.push_back(box->GetDZ());
        }

        if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoCone)) {
            type = "cone";
        } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoTube)) {
            type = "tube";
        } else if (shape->TestShapeBit(TGeoShape::EShapeType::kGeoBad) ||
            shape->TestShapeBit(TGeoShape::EShapeType::kGeoNoShape)) {
            type = "ErrorType";
        } else if (type.empty()) {
            type = "Unknown";
        }

        wr.AddProperty("Type", type);
        wr.AddProperty("Material", volume->GetMaterial()->GetName());
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

void TGeoManagerExporter::writeChildren(JSONWriter &wr) {
    wr.AddProperty("children");
    wr.BeginBlock();

    wr.AddProperty("FakeChild");
    wr.BeginBlock();
    wr.AddProperty("FakeProperty", "FakeValue");
    wr.EndBlock();

    wr.EndBlock();
    //End of "children"
}

void TGeoManagerExporter::writeStyles(JSONWriter &wr) {
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
//ClassImp(TGeoManagerExporter);
