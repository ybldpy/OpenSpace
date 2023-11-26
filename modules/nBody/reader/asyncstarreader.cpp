#include <modules/nBody/headers/asyncstarreader.h>
#include <fstream>
#include <future>

void AsyncStarReader::readStars(const std::filesystem::path& filePath,bool& finishFlag, bool& failFlag) {

    auto readFunc = [this](const std::filesystem::path& filePath,bool& finishFlag, bool& failFlag) {
        bool success = readStars(filePath);
        finishFlag = true;
        failFlag = !success;
    };
    std::thread(readFunc,filePath,std::ref(finishFlag), std::ref(failFlag)).detach();
}

bool AsyncStarReader::readStars(const std::filesystem::path& filePath) {


    std::ifstream in(filePath, std::ifstream::binary);
    if (!in.good()) {
        return false;
    }
    int totalValues;
    int perValueCount;
    in.read(reinterpret_cast<char*>(&totalValues), sizeof(int32_t));
    in.read(reinterpret_cast<char*>(&perValueCount), sizeof(int32_t));
    startStars.clear();
    endStars.clear();
    std::vector<float> data(totalValues);
    in.read(reinterpret_cast<char*>(data.data()), sizeof(float_t) * totalValues);
    int idx = 0;
    for (int i = 0;i < data.size();i += perValueCount) {
        auto posBegin = data.begin() + i;
        auto posEnd = posBegin + 4;
        auto nextPosEnd = posEnd + 4;
        startStars.insert(startStars.end(), posBegin, posEnd);
        endStars.insert(endStars.end(), posEnd, nextPosEnd);
    }
    numStars = totalValues / perValueCount;
    in.close();
    return true;

}


StarData AsyncStarReader::getStarData() {
    return { startStars,endStars,numStars };
}
