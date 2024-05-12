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

void AsyncStarReader::readStarsWithoutInterpolation(const std::string& filePathTemplate,const int& beginIndex,const int& endIndex) {
    std::thread(&AsyncStarReader::readStarsStream,this,filePathTemplate, beginIndex, endIndex).detach();
}

void AsyncStarReader::readStarsStream(const std::string filePathTemplate, const int beginIndex, const int endIndex) {

    int index = beginIndex;
    while (index <= endIndex)
    {
        if (isBufferFull()) {
            wait();
            continue;
        }
        std::vector<float>& stars = starBuffers[bufferIndex];
        int numStars = read(fmt::format("E:/star/data/bstream/snap_{}.bin",index), stars);
        if (numStars <= 0) {
            continue;
        }
        bufferStarNums[bufferIndex] = numStars;
        index++;
        bufferIndex = (bufferIndex + 1) % bufferSize;
        bufferIncrease();
    }
}

void AsyncStarReader::wait() {
    std::unique_lock<std::mutex> l(waitNotifyMutex);
    waitNotifyVariable.wait(l);
}

bool AsyncStarReader::isBufferFull() {
    std::lock_guard l(mutex);
    return size == bufferSize;
}

void AsyncStarReader::bufferIncrease() {
    std::lock_guard l(mutex);
    size++;
}

int AsyncStarReader::read(const std::filesystem::path& path,std::vector<float>& stars) {
    std::ifstream in(path,std::ifstream::binary);
    if (!in.good()) {
        return 0;
    }
    int totalValues;
    int perValueCount;
    in.read(reinterpret_cast<char*>(&totalValues), sizeof(int32_t));
    in.read(reinterpret_cast<char*>(&perValueCount), sizeof(int32_t));
    if(stars.size() < totalValues){
        stars.resize(totalValues);
    }
    in.read(reinterpret_cast<char*>(stars.data()), sizeof(float_t) * totalValues);
    return totalValues / perValueCount;
}


void AsyncStarReader::readFinish() {
    std::lock_guard l(mutex);
    if (size <= 0) {
        size = 0;
    }
    else {
        --size;
    }
    //std::unique_lock<std::mutex> l(waitNotifyMutex);
    waitNotifyVariable.notify_all();
    readIndex = (readIndex + 1) % bufferSize;
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


StarData AsyncStarReader::getStarsNxtTimeFrame() {
    if (size <= 0) {
        return { starBuffers[0],starBuffers[0],0 };
    }
    return { starBuffers[readIndex],starBuffers[0],bufferStarNums[readIndex] };
}

StarData AsyncStarReader::getStarData() {
    return { startStars,endStars,numStars };
}
