#pragma once


#include <ghoul/filesystem/filesystem.h>
#include <queue>
#include <condition_variable>


struct StarData {
    std::vector<float>& startStarsData;
    std::vector<float>& endStarsData;
    int numStars;
};

class AsyncStarReader {
public:
    //AsyncStarReader(const int& streamBuffer);
    void readStars(const std::filesystem::path& filePath, bool& finishFlag, bool& resultFlag);
    // this function is sync for convenience
    bool readStars(const std::filesystem::path& filePath);
    StarData getStarData();
    //void readStarsFromStream(const std::filesystem::path& fileTemplate);
    void readStarsWithoutInterpolation(const std::string& filePathTemplate, const int& beginIndex, const int& endIndex);
    StarData getStarsNxtTimeFrame();
    void readFinish();
private:
    void wait();
    void bufferIncrease();
    bool isBufferFull();
    void readStarsStream(const std::string filePathTemplate, const int beginIndex, const int endIndex);
    int read(const std::filesystem::path& filePath,std::vector<float>& stars);
    int numStars;
    std::vector<float> startStars;
    std::vector<float> endStars;
    const int& streamBuffer = 1;
    static const int bufferSize = 10;
    std::vector<float> starBuffers[bufferSize];
    int bufferStarNums[bufferSize];
    std::mutex mutex;
    int readIndex = 0;
    int bufferIndex = 0;
    int size = 0;
    std::mutex waitNotifyMutex;
    std::condition_variable waitNotifyVariable;

};

