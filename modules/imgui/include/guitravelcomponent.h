#pragma once

#include <modules/imgui/include/guicomponent.h>

#include <openspace/scene/scenegraphnode.h>
#include <queue>
#include <map>


namespace guitravelcomponent {

    struct DistanceCompartor {
        bool operator()(const openspace::SceneGraphNode* node1, const openspace::SceneGraphNode* node2);
    };

}

namespace openspace::gui {

    class GuiTravelComponent :public GuiComponent {

    public:
        GuiTravelComponent();

        void render() override;
        
        std::string next();
        bool hasNext();
        static scripting::LuaLibrary luaLibrary();
        
        std::mutex& getQueueMutex();
        std::mutex& getNodesMutex();
        void nextPlanet();
        bool hasCustomerSpeed();
        double getCustomerSpeed();
        
       

    private:

        void addNewPathNode(glm::dvec3 worldPos);
        void addNewPathNode(std::string nodeIdentifier);
        void initTravel();
        double x = 0, y = 0, z = 0;
        int nodeIndex = 0;
        bool customerSpeed = false;
        double speed = 1;
        mutable std::mutex queueMutex;
        mutable std::mutex nodesMutex;
        std::vector<std::string> nodesQueue;
        std::vector<std::string> nodesToTravel;
        std::map<std::string, bool> selectableState;

        
    };

}
