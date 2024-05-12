#include <modules/nBody/headers/renderableNStars.h>

#include <openspace/engine/globals.h>
#include <openspace/engine/openspaceengine.h>
#include <openspace/engine/windowdelegate.h>
#include <openspace/rendering/renderengine.h>
#include <openspace/documentation/documentation.h>
#include <openspace/documentation/verifier.h>
#include <openspace/util/distanceconversion.h>
#include <openspace/util/updatestructures.h>
#include <ghoul/fmt.h>
#include <ghoul/filesystem/filesystem.h>
#include <ghoul/io/texture/texturereader.h>
#include <ghoul/misc/templatefactory.h>
#include <ghoul/opengl/programobject.h>
#include <ghoul/opengl/texture.h>
#include <ghoul/opengl/textureunit.h>
#include <ghoul/io/texture/texturereader.h>
#include <openspace/util/timemanager.h>

namespace {

    constexpr std::string_view _loggerCat = "RenderableNStars";

    constexpr openspace::properties::Property::PropertyInfo filePathInfo = {
        "File",
        "File Path",
        "The path to the file with data for the stars to be rendered",
        openspace::properties::Property::Visibility::User
    };

    std::string filePaths[] = {
            "E:/star/data/nbin/snap_0.bin",
            "E:/star/data/nnin/snap_1.bin",
            "E:/star/data/nbin/snap_2.bin",
            "E:/star/data/nbin/snap_3.bin",
            "E:/star/data/nbin/snap_4.bin"
    };

    constexpr std::string_view filePathTemplate = "E:/star/data/nbin/snap_{}.bin";

}


namespace openspace {


   RenderableNStars::RenderableNStars(const ghoul::Dictionary& dict):Renderable(dict),filePath(filePathInfo),pastSecondsInInterval(0),intervalIndex(0){
       
   }


   void RenderableNStars::loadStars(const StarData& starData) {
       numStars = starData.numStars;
       calcWorkGroup(numStars, localSizeX, localSizeY);
       calcPostitionTextureSize(numStars);
       loadPositionTextures(starData.startStarsData, starData.endStarsData,positionTextureSize.width,positionTextureSize.height);
   }

   void RenderableNStars::initColorMap(const std::filesystem::path& filePath) {

       std::ifstream in(filePath);
       if (!in.good()) {
           numColors = -1;
           return;
       }
       in >> minMass;
       in >> maxMass;
       in >> numColors;

       std::string line;
       while (std::getline(in, line)) {
           std::istringstream strStream(line);
           float value;
           while (strStream >> value) {
               colors.push_back(value);
           }
       }
       glBindTexture(GL_TEXTURE_1D, colorTexture);
       glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, numColors, 0, GL_RGBA, GL_FLOAT, colors.data());
       glBindTexture(GL_TEXTURE_1D, 0);
   }

   void RenderableNStars::initializeGL() {
       program = ghoul::opengl::ProgramObject::Build("NBodyStars",
           absPath("${MODULE_NBODY}/shaders/vs.glsl"),
           absPath("${MODULE_NBODY}/shaders/fs.glsl"),
           absPath("${MODULE_NBODY}/shaders/ges.glsl"));
       

       computeProgram = ghoul::opengl::ProgramObject::BuildComputeShader("NBodyStars",
           absPath("${MODULE_NBODY}/shaders/compute.glsl"));
       glGenBuffers(1, &vbo);
       glGenVertexArrays(1, &vao);
       glGenTextures(1, &startPositionTexture);
       glGenTextures(1, &targetPositionTexuture);
       glGenTextures(1, &currentPositionTexture);
       glGenTextures(1, &colorTexture);

       spriteTexture = nullptr;
       spriteTexture = ghoul::io::TextureReader::ref().loadTexture("D:/OpenSpace/OpenSpace/sync/http/stars_textures/1/halo.png",
           2);
       if (spriteTexture) {
           LDEBUG(
               fmt::format("Loaded texture from {}", absPath("D:/OpenSpace/OpenSpace/sync/http/stars_textures/1/halo.png"))
           );
           spriteTexture->bind();
           spriteTexture->uploadTexture();
           spriteTexture->setFilter(ghoul::opengl::Texture::FilterMode::AnisotropicMipMap);
           // unbind
           glBindTexture(GL_TEXTURE_2D, 0);
       }


       computeShaderUniforms.displacment = computeProgram->uniformLocation("displacement");
       vsUniforms.positionTextureWidth = program->uniformLocation("positionTextureWidth");
       fsUniforms.minMass = program->uniformLocation("minMass");
       fsUniforms.maxMass = program->uniformLocation("maxMass");
       vsUniforms.kpc = program->uniformLocation("kpc");
       vsUniforms.transformation = program->uniformLocation("transformation");
       vsUniforms.modelViewMat = program->uniformLocation("modelViewMat");
       fsUniforms.spriteTexture = program->uniformLocation("spriteTexture");
       initColorMap("E:/star/data/color.cmap");
       starReader->readStarsWithoutInterpolation(std::string(filePathTemplate), 0, 199);

   }

   void RenderableNStars::deinitializeGL() {
       if (vbo != 0) {
           glDeleteBuffers(1, &vbo);
       }
       
   }
   void RenderableNStars::updateWithoutInterpolation(const UpdateData& data) {

       if (frameIndex % maxFrameIndex == 0) {
           StarData starData = starReader->getStarsNxtTimeFrame();
           if (starData.numStars <= 0) { return; }
           calcPostitionTextureSize(starData.numStars);
           glBindTexture(GL_TEXTURE_2D, currentPositionTexture);
           if (!init) {
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
               init = true;
           }
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1620, 1620, 0, GL_RGBA, GL_FLOAT, starData.startStarsData.data());
           glBindTexture(GL_TEXTURE_2D, 0);
           numStars = starData.numStars;
           starReader->readFinish();
           frameIndex++;
       }
       else {
           frameIndex = (frameIndex+1)%maxFrameIndex;
       }
   }

   bool RenderableNStars::isReady() const{ return program&&computeProgram; }
   void RenderableNStars::update(const UpdateData& data) {

       if (false) {
           if (data.time.j2000Seconds() - data.previousFrameTime.j2000Seconds() <= 1e-5) { return; }
           updateWithoutInterpolation(data);
           return;
       }
       

       if (loadFail) { ghoul_assert(false, "Load file failed"); }

       if (!init) {
           bool success = starReader->readStars(fmt::format(filePathTemplate, 0));
           ghoul_assert(success, "Load file failed");
           loadFail = !success;
           loadStars(starReader->getStarData());
           init = true;
           // read next stars in async way
           starReader->readStars(std::filesystem::path(fmt::format(filePathTemplate, 1)), nextFileReady, loadFail);
       }
       double deltaTime = data.time.j2000Seconds() - data.previousFrameTime.j2000Seconds();
       if (deltaTime < 0) { return; }
       pastSecondsInInterval = pastSecondsInInterval + deltaTime;
       if (pastSecondsInInterval > interval)
       {
           if (nextFileReady && intervalIndex < 100) {
               loadStars(starReader->getStarData());
               intervalIndex += 1;
               nextFileReady = false;
               pastSecondsInInterval = 0;
               starReader->readStars(std::filesystem::path(fmt::format(filePathTemplate, intervalIndex + 1)), nextFileReady, loadFail);
           }
           else {
               // no need to compute new location
               return;
           }
       }
       
       float displacement = std::min(pastSecondsInInterval / interval, (double)1);
       //LDEBUG(fmt::format("{}", ghoul::to_string(displacement)));
       computeProgram->activate();
       computeProgram->setUniform(computeShaderUniforms.displacment, displacement);
       glBindImageTexture(startPositionTextureBindingPoint, startPositionTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
       glBindImageTexture(targetPosTextureBindingPoint, targetPositionTexuture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
       glBindImageTexture(currentPositionTextureBindingPoint, currentPositionTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
       glDispatchCompute(workGroupX, workGroupY, 1);
       glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   }

   void RenderableNStars::loadPositionTextures(const std::vector<float>& startStars,const std::vector<float>& endStars,const int& width,const int& height) {
       
       //glActiveTexture(GL_TEXTURE0);
       glBindTexture(GL_TEXTURE_2D, startPositionTexture);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA32F, width,height, 0, GL_RGBA, GL_FLOAT, startStars.data());
       glBindTexture(GL_TEXTURE_2D, currentPositionTexture);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
       //glActiveTexture(GL_TEXTURE1);
       glBindTexture(GL_TEXTURE_2D, targetPositionTexuture);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F,width,height, 0, GL_RGBA, GL_FLOAT,endStars.data());
       glBindTexture(GL_TEXTURE_2D, 0);
       

   }

   void RenderableNStars::render(const RenderData& data, RendererTasks& rendererTask) {
       if (loadFail) { return; }
       
       glBindVertexArray(vao);
       glBindBuffer(GL_ARRAY_BUFFER, vbo);
       glVertexAttribPointer(0, 1, GL_INT, GL_FALSE, 1 * sizeof(int32_t), 0);
       glEnableVertexAttribArray(0);
       glBindBuffer(GL_ARRAY_BUFFER, 0);
       program->activate();
       program->setUniform(vsUniforms.kpc, (float)3e12);
       glm::dmat4 model = calcModelTransform(data);
       glm::dmat4 view = data.camera.combinedViewMatrix();
       glm::dmat4 projection = data.camera.projectionMatrix();
       ghoul::opengl::TextureUnit unit;
       spriteTexture->bind();
       unit.activate();
       glm::dmat4 viewModel = view * model;
       program->setUniform(fsUniforms.spriteTexture, unit);
       program->setUniform(vsUniforms.transformation, projection*viewModel);
       //program->setUniform(vsUniforms.modelViewMat, viewModel);
       program->setUniform(vsUniforms.positionTextureWidth, positionTextureSize.width);
       program->setUniform(fsUniforms.minMass, minMass);
       program->setUniform(fsUniforms.maxMass, maxMass);
       //glEnable(GL_POINT_SPRITE);
       glEnable(GL_PROGRAM_POINT_SIZE);
       //program->setUniform(positionTextureSizeLocation.heightLocation, positionTextureSize.height);
       glBindImageTexture(currentPositionTextureBindingPoint, currentPositionTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
       glBindImageTexture(colorTextureBindingPoint, colorTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
       glEnable(GL_DEPTH_TEST);
      
       //glPointSize(6);
       glDrawArrays(GL_POINTS, 0, numStars);
       glBindVertexArray(0);
       //glDisable(GL_POINT_SPRITE);
       program->deactivate();
       unit.deactivate();
       

   }

   /*bool RenderableNStars::readDataFromFile(const std::filesystem::path& filePath) {
       std::ifstream instream(filePath, std::ifstream::binary);

       if (!instream.good()) {
           return false;
       }


       
       int totalValues;
       int perValueCount;
       instream.read(reinterpret_cast<char*>(&totalValues), sizeof(int32_t));
       instream.read(reinterpret_cast<char*>(&perValueCount), sizeof(int32_t));
       
       std::vector<float> data(totalValues);
       instream.read(reinterpret_cast<char*>(data.data()), sizeof(float_t) * totalValues);
       int idx = 0;
       for (int i = 0;i < data.size();i += perValueCount) {
           auto posBegin = data.begin() + i;
           auto posEnd = posBegin + POS_SIZE + 1;
           auto nextPosEnd = posEnd + POS_SIZE + 1;
           starsData.pos.insert(starsData.pos.end(), posBegin,posEnd);
           starsData.nextPos.insert(starsData.nextPos.end(), posEnd, nextPosEnd);
       }
       numStars = totalValues / perValueCount;
       instream.close();
       return true;
   }*/


   /*void RenderableNStars::updateLocation(Time time,Time previous) {


       
   }*/


   void RenderableNStars::calcWorkGroup(const int& numPixels,const int& localSizeX,const int& localSizeY) {
       GLuint numWorkGroup = 1;
       while ((numWorkGroup++) * localSizeX *  localSizeY < numPixels) {}
       int x = 1;
       int y = 1;
       bool turn = true;
       while (x * y < numWorkGroup) {
           if (turn)
           {
               ++x;
               turn = false;
           }
           else {
               ++y;
               turn = true;
           }
       }
       workGroupX = x;
       workGroupY = y;
   }


   void RenderableNStars::calcPostitionTextureSize(const int& numStars) {
       int x = 1;
       int y = 1;
       bool turn = true;
       while (x * y < numStars) {
           if (turn)
           {
               ++x;
               turn = false;
           }
           else {
               ++y;
               turn = true;
           }
       }                                                                                                                                                      
       positionTextureSize.width = x;
       positionTextureSize.height = y;
   }


   



}
