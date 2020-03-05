//
// Created by fedor on 10/18/19.
//

#include "test_main.hpp"

TEST(General, SimpleGeom) {
    TGeoManager* manager = new TGeoManager("simple1", "Simple geometry");
    TGeoMaterial* matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMaterial* matAl = new TGeoMaterial("Al", 26.98,13,2.7);
    TGeoMedium* Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume* R = manager->MakeBox("R", Vacuum, 25., 25., 5.);
    R->SetVisibility(true);
    manager->SetTopVolume(R);
    TGeoManagerExporter exp(manager);
    std::ofstream myformat("simple1.json", std::ios_base::trunc);
    exp.Write(myformat);
    myformat.close();
    std::ifstream mf("simple1.json");
    ASSERT_TRUE(checkBalance(mf));
    gGeoManager->Export("simple1.gdml", "TEST 1");
}

TEST(General, GeofileFull) {
    TGeoManager::Import("~/Programming/npm/root_tests/geofile_full.root");

    ASSERT_NE(gGeoManager, nullptr);

    TGeoManagerExporter exp(gGeoManager);
    std::ofstream myformat("myformat.json", std::ios_base::trunc);
    exp.Write(myformat);
    myformat.close();

    std::ifstream mf("myformat.json");
    ASSERT_TRUE(checkBalance(mf));
    gGeoManager->Export("myformat.gdml", "somename");

    gGeoManager->GetTopVolume()->SetVisibility(true);
    gGeoManager->Draw();
}
