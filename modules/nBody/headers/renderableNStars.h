#ifndef __OPENSPACE_MODULE_NBODY_STARS__
#define __OPENSPACE_MODULE_NBODY_STARS__


#include <openspace/rendering/renderable.h>
#include <openspace/properties/optionproperty.h>
#include <openspace/properties/stringproperty.h>
#include <openspace/properties/list/stringlistproperty.h>
#include <openspace/properties/scalar/boolproperty.h>
#include <openspace/properties/scalar/floatproperty.h>
#include <openspace/properties/scalar/intproperty.h>
#include <openspace/properties/vector/vec2property.h>
#include <openspace/properties/vector/ivec2property.h>
#include <modules/nBody/headers/asyncstarreader.h>


#include <ghoul/opengl/bufferbinding.h>
#include <ghoul/opengl/ghoul_gl.h>
#include <ghoul/opengl/uniformcache.h>

namespace ghoul::filesystem { class File; }
namespace ghoul::opengl {
    class ProgramObject;
    class Texture;
}



namespace openspace {

    namespace documentation { struct Documentation; }


    class RenderableNStars :public Renderable {

    public:
        explicit RenderableNStars(const ghoul::Dictionary& dictionary);
        ~RenderableNStars() = default;
        void initializeGL() override;
        void deinitializeGL() override;
        bool isReady() const override;
        void render(const RenderData& data, RendererTasks& rendererTask) override;
        void update(const UpdateData& data) override;
        //static documentation::Documentation Documentation();

    private:

        

        struct {
            int width;
            int height;
        }positionTextureSize;

        static const int POS_SIZE = 3;
        static const int MASS_SIZE = 2;
        static const int VEL_SIZE = 3;
        bool nextFrame = false;
        bool init = false;
        bool loadFail = false;
        openspace::properties::StringProperty filePath;
        GLuint vbo;
        GLuint vao;
        GLuint startPositionTexture;
        GLuint targetPositionTexuture;
        GLuint currentPositionTexture;
        GLuint colorTexture;
//        GLint deltaTimeLocation;
        GLuint workGroupX;
        GLuint workGroupY;
//        GLint speedLocation;
        GLint displacementLocation;
        GLint maxIndexLocation;
        GLint maxMassLocation;
        GLint minMassLocation;
        struct {
            GLint widthLocation;
            GLint heightLocation;
        }positionTextureSizeLocation;
        GLint kpcLocation;
        GLint tranformationLocation;
        bool nextFileReady = false;
        // temp texture for storing updated position
        // after updating, assign it to position texture
        //GLuint tempPositionTexture;
        int numStars;
        const int& localSizeX = 10;
        const int& localSizeY = 10;
        float maxMass;
        float minMass;
        int numColors;
        // default 30 seconds
        int interval = 60;
        double pastSecondsInInterval;
        int intervalIndex;
        std::vector<float> colors;
        const int startPositionTextureBindingPoint = 0;
        const int targetPosTextureBindingPoint = startPositionTextureBindingPoint+1;
        const int currentPositionTextureBindingPoint = targetPosTextureBindingPoint + 1;
        const int colorTextureBindingPoint = currentPositionTextureBindingPoint+1;
        std::unique_ptr<ghoul::opengl::ProgramObject> program;
        std::unique_ptr<ghoul::opengl::ProgramObject> computeProgram;
        std::shared_ptr<AsyncStarReader> starReader;
        void loadStars(const StarData& starData);
        void loadPositionTextures(const std::vector<float>& startStars, const std::vector<float>& endStars,const int& width,const int&height);
        //bool readDataFromFile(const std::filesystem::path& filePath);
        //void updateLocation(Time time,Time previous);
        void calcWorkGroup(const int& numPixels,const int& localSizeX,const int& localSizeY);
        void calcPostitionTextureSize(const int& numStars);
        void initColorMap(const std::filesystem::path& filePath);
        
        
        
    };
}



#endif
