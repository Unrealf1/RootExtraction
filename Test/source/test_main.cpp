#include "test_main.hpp"

int main(int argc, char** argv) {
    TGeoManager::Import("../root_tests/geofile_full.root");
    if (gGeoManager == nullptr) {
        std::cerr << "Can't find TGeoManager in file" << std::endl;
        return 0;
    }
    TGeoManagerExporter exp(gGeoManager);
    exp.Write(std::cout);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
