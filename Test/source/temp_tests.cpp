//
// Created by fedor on 10/18/19.
//

#include "test_main.hpp"

TEST(General, SimpleGeom) {
    TGeoManager* manager = new TGeoManager("simple1", "Simple geometry");
    TGeoMaterial* matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
    TGeoMedium* Vacuum = new TGeoMedium("Vacuum",1, matVacuum);
    TGeoVolume* R = manager->MakeBox("R", Vacuum, 25., 25., 5.);
    R->SetVisibility(true);
    manager->SetTopVolume(R);
    TGeoManagerExporter exp(manager);
    std::ofstream myformat("simple1.json", std::ios_base::trunc);
    exp.write(myformat);
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
    exp.write(myformat);
    myformat.close();

    std::ifstream mf("myformat.json");
    ASSERT_TRUE(checkBalance(mf));
    gGeoManager->Export("myformat.gdml", "somename");

    gGeoManager->GetTopVolume()->SetVisibility(true);
    gGeoManager->Draw();
}

TEST(General, MediumGeom) {
    const std::string name = "medium1";

    TGeoManager::Import(("~/Programming/npm/RootExtraction/Examples/" + name + ".root").c_str());

    ASSERT_NE(gGeoManager, nullptr);

    TGeoManagerExporter exp(gGeoManager);
    std::ofstream myformat(name + ".json", std::ios_base::trunc);
    exp.write(myformat);
    myformat.close();

    std::ifstream mf(name + ".json");
    ASSERT_TRUE(checkBalance(mf));
    gGeoManager->Export((name + ".gdml").c_str(), "somename");

    gGeoManager->GetTopVolume()->SetVisibility(true);
}

TEST(General, Tank) {
    const std::string name = "tank";

    TGeoManager::Import(("~/Programming/npm/RootExtraction/Examples/" + name + ".root").c_str());

    ASSERT_NE(gGeoManager, nullptr);

    TGeoManagerExporter exp(gGeoManager);
    std::ofstream myformat(name + ".json", std::ios_base::trunc);
    exp.write(myformat);
    myformat.close();

    std::ifstream mf(name + ".json");
    ASSERT_TRUE(checkBalance(mf));
    gGeoManager->Export((name + ".gdml").c_str(), "somename");

    gGeoManager->GetTopVolume()->SetVisibility(true);
}
