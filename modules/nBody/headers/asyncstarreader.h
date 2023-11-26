#pragma once


#include <ghoul/filesystem/filesystem.h>


struct StarData {
    std::vector<float>& startStarsData;
    std::vector<float>& endStarsData;
    int numStars;
};

class AsyncStarReader {
public:
    void readStars(const std::filesystem::path& filePath, bool& finishFlag, bool& resultFlag);
    // this function is sync for convenience
    bool readStars(const std::filesystem::path& filePath);
    StarData getStarData();


private:
    int numStars;
    std::vector<float> startStars;
    std::vector<float> endStars;


};

