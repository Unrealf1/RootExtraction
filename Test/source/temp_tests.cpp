//
// Created by fedor on 10/18/19.
//

#include "test_main.hpp"
#include <TGeoManager.h>

TEST(General, SimpleGeom) {
    TGeoManager* manager = new TGeoManager("simple1", "Simple geometry");
    TGeoMaterial* matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMaterial* matAl = new TGeoMaterial("Al", 26.98,13,2.7);
    TGeoMedium* Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume* R = manager->MakeBox("R", Vacuum, 25., 25., 5.);
    R->SetVisibility(true);
    manager->SetTopVolume(R);
    TGeoManagerExporter exp(manager);
    std::ofstream myformat("test1.json");
    exp.Write(myformat);
    gGeoManager->Export("test1.gdml", "TEST 1");
}

TEST(General, GeofileFull) {
    TGeoManager::Import("~/Programming/npm/root_tests/geofile_full.root");

    ASSERT_NE(gGeoManager, nullptr);

    auto volumes = gGeoManager->GetListOfVolumes();
    auto nodes = gGeoManager->GetListOfNodes();

    TGeoManagerExporter exp(gGeoManager);
    std::ofstream myformat("myformat.json");
    exp.Write(myformat);
    gGeoManager->Export("test.gdml", "somename");

    gGeoManager->GetTopVolume()->SetVisibility(true);
    gGeoManager->Draw();
}

