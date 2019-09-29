//
// Created by fedor on 9/29/19.
//

#include "TGeoManagerExporter.hpp"
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
    //Step one: write all volumes to templates
    wr.AddProperty("templates");
    wr.BeginBlock();
    for (auto&& volume : volumes) {

    }
    wr.EndBlock();


    wr.EndBlock();
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
ClassImp(TGeoManagerExporter);
