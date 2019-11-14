//
// Created by fedor on 3/18/19.
//

#ifndef FORMALLANGUAGES_TEST_MAIN_HPP
#define FORMALLANGUAGES_TEST_MAIN_HPP

#include "gtest/gtest.h"
#include "TGeoManagerExporter.hpp"
#include "TGeoManager.h"
#include "TGeometry.h"
#include "TFile.h"
#include <fstream>
#include <iostream>
#include <ios>

class NullBuffer : public std::streambuf {
    int overflow(int c) override;
};
bool checkBalance(std::istream& str);

#endif //FORMALLANGUAGES_TEST_MAIN_HPP
