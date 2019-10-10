#include "test_main.hpp"

int main(int argc, char** argv) {
    TGeoManager::Import("~/Programming/npm/root_tests/geofile_full.root");
    if (gGeoManager == nullptr) {
        std::cerr << "Can't find TGeoManager in file" << std::endl;
        return 0;
    }
    TGeoManagerExporter exp(gGeoManager);
    std::ofstream myformat("myformat.json");
    exp.Write(myformat);
    gGeoManager->Export("test.gdml", "kekname");

    gGeoManager->GetTopVolume()->SetVisibility(true);
    gGeoManager->Draw();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
