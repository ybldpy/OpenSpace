
#include <modules/imgui/imguimodule.h>
#include <modules/imgui/include/imgui_include.h>
#include <modules/imgui/include/guitravelcomponent.h>
#include <openspace/engine/globals.h>
#include <openspace/rendering/renderengine.h>
#include <openspace/scene/scenegraphnode.h>
#include <openspace/scene/scene.h>
#include <openspace/camera/camera.h>
#include <openspace/navigation/navigationhandler.h>
#include <openspace/navigation/orbitalnavigator.h>
#include <openspace/engine/moduleengine.h>
#include <queue>
#include <openspace/util/collisionhelper.h>
#include <openspace/util/screenlog.h>




#include "guitravelcomponent_lua.inl"
#include"guitravelcomponent_lua_codegen.cpp"

namespace guitravelcomponent{

    std::string postfix = "_TestNode";
    std::string nodeName(const int& index);
    int printCount = 0;
    bool DistanceCompartor::operator()(const openspace::SceneGraphNode* node1, const openspace::SceneGraphNode* node2) {
        const openspace::SceneGraphNode* currentFocusNode = openspace::global::navigationHandler->orbitalNavigator().anchorNode();
        return glm::distance(node1->worldPosition(), currentFocusNode->worldPosition()) < glm::distance(node2->worldPosition(), currentFocusNode->worldPosition());
    }

    std::string vectorToString(const std::vector<std::string>& strings) {

        std::string t = "{";
        for (const std::string& s : strings) {
            t += s+", ";
        }
        return t+"}";

    }

    openspace::SceneGraphNode* findClosestParentNode(const glm::dvec3& pos) {

        openspace::SceneGraphNode* closest = nullptr;
        double minDistance = DBL_MAX;
        double minSphere = DBL_MAX;
        const std::vector<openspace::SceneGraphNode*>& nodes = openspace::global::renderEngine->scene()->allSceneGraphNodes();
        double distance;
        
        for (openspace::SceneGraphNode* node : nodes) {
            if (node->identifier().find(postfix) == std::string::npos) {
                distance = glm::distance(node->worldPosition(), pos);
                if (distance < minDistance && distance <= node->boundingSphere()&&node->boundingSphere()<minSphere) {
                    minDistance = distance;
                    closest = node;
                    minSphere = node->boundingSphere();
                }
                
            }
        }
        return closest;


    }

    std::string addNodeScript(const int& index, const glm::dvec3& pos) {

        openspace::SceneGraphNode* closest = findClosestParentNode(pos);
        glm::dvec3 relativePosition(pos - closest->worldPosition());
        std::string script = "{Identifier = '" + nodeName(index) + "',Parent = '" + closest->identifier() + "',BoundingSphere = " + ghoul::to_string(20.0f) + ",ReachFactor = 1.5,Transform = {Translation = {Type = 'StaticTranslation', Position = " + ghoul::to_string(relativePosition) + "}}}";
        printf("closest: %s\n\n\n\n", closest->identifier().c_str());

        return script;
    }

    

    const ImVec2 windowSize = ImVec2(500, 500);
    
    std::string nodeName(const int& index) {
        printf("sssssss %s", std::format("{}\n\n\n\n", ghoul::to_string(index) + postfix).c_str());
        return std::to_string(index) + postfix;
    }


    int testCount = 0;


    


    



}
namespace openspace::gui{

GuiTravelComponent::GuiTravelComponent() :GuiComponent("PlanetTravel", "PlanetTravel"){ };


void GuiTravelComponent::render() {
    ImGui::SetNextWindowCollapsed(_isCollapsed);
    bool v = _isEnabled;
    ImGui::SetNextWindowSize(guitravelcomponent::windowSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin(guiName().c_str(), &v, ImGuiWindowFlags_AlwaysAutoResize);
    _isEnabled = v;
    _isCollapsed = ImGui::IsWindowCollapsed();
    const std::vector<SceneGraphNode*>& nodes = global::renderEngine->scene()->allSceneGraphNodes();
    
    std::vector<std::string> nodesName;
    
    std::map<std::string, bool> tselectableState = selectableState;
    selectableState.clear();
    for (SceneGraphNode* node : nodes) {
        nodesName.push_back(node->identifier());
        if (tselectableState.find(node->identifier()) == tselectableState.end()) {
            tselectableState[node->identifier()] = false;
        }
        
    }
    Camera* c = global::renderEngine->scene()->camera();
    if (guitravelcomponent::testCount == 10) {
        
        //c->setPose({ c->getCameraPose().position,glm::quatLookAt(global::renderEngine->scene()->sceneGraphNode("Sun")->worldPosition(),c->lookUpVectorCameraSpace())});
        guitravelcomponent::testCount = 0;
    }
    else {
        guitravelcomponent::testCount++;
    }
    std::sort(nodesName.begin(), nodesName.end(), [](const std::string& l, const std::string& r) ->bool {return l < r;});

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

    ImGui::Columns(2);

    if (ImGui::BeginListBox("", ImVec2(200, 200))){

        for (std::string& nodeName : nodesName) {
            if(ImGui::Selectable(nodeName.c_str(), &tselectableState[nodeName])) {
                for(std::pair<const std::string,bool>& entry:tselectableState){
                    entry.second = false;
                }
                tselectableState[nodeName] = true;
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Add node")) {

        for (auto& entry : tselectableState) {
            if (entry.second) {
                addNewPathNode(entry.first);
                entry.second = false;
                break;
            }
        }
    }

    selectableState = tselectableState;
    ImGui::NextColumn();

    ImGui::InputDouble("Position X", &x);
    ImGui::InputDouble("Position Y", &y);
    ImGui::InputDouble("Position Z", &z);
    if (ImGui::Button("Add position")) {
        addNewPathNode(glm::dvec3(x, y, z));
    }
    if (ImGui::InputDouble("Speed", &speed)) {
        customerSpeed = true;
    }
    if (ImGui::Button("Reset speed")) {
        customerSpeed = false;
    }
    ImGui::EndColumns();


    if (ImGui::BeginListBox("List", ImVec2(250,250))) {
        std::lock_guard l(getQueueMutex());
        std::vector<std::string> deletedNode;
        const Scene* scene = global::renderEngine->scene();
        for (std::string& nodeIdentifier : nodesQueue) {
            SceneGraphNode* graphNode = scene->sceneGraphNode(nodeIdentifier);
            if (!graphNode) {
                deletedNode.push_back(nodeIdentifier);
            }
            else {
                std::string text = nodeIdentifier + " Position: " + ghoul::to_string(graphNode->worldPosition());
                ImGui::Text(text.c_str());
            }
        }

        for (std::string& node : deletedNode) {
            nodesQueue.erase(std::find(nodesQueue.begin(),nodesQueue.end(),node));
        }
        ImGui::EndListBox();
    }
    ImGui::NewLine();
    const bool pressed = ImGui::Button("Start path");
    if (pressed) {
        std::lock_guard queueLock(getQueueMutex());
        if (!nodesQueue.empty()) {
            initTravel();
            nextPlanet();
        }
    }
    ImGui::End();


}

bool GuiTravelComponent::hasCustomerSpeed() {
    return customerSpeed;
}

double GuiTravelComponent::getCustomerSpeed() {
    return speed;
}

std::mutex& GuiTravelComponent::getQueueMutex() {
    return queueMutex;
}
std::mutex& GuiTravelComponent::getNodesMutex() {
    return nodesMutex;
}

void GuiTravelComponent::initTravel() {
    std::lock_guard l(getNodesMutex());
    nodesToTravel = nodesQueue;
    nodesQueue.clear();
}

void GuiTravelComponent::addNewPathNode(const glm::dvec3& worldPos) {
    printf("ssssss %s\n\n\n\n%s\n\n\n", ghoul::to_string(global::navigationHandler->orbitalNavigator().anchorNode()->worldPosition()).c_str(), ghoul::to_string(global::renderEngine->scene()->camera()->positionVec3()).c_str());
    int index = nodeIndex++;
    std::string t = "{Identifier = '" + std::to_string(index) + guitravelcomponent::postfix+"',BoundingSphere = 1000000.0, ReachFactor = 1.5,Transform = {Translation = {Type = 'StaticTranslation', Position = " + ghoul::to_string(worldPos) + "}}}";
    std::string t1 = "{Identifier = '" + std::to_string(index) + guitravelcomponent::postfix + "',Transform = {Translation = {Type = 'StaticTranslation', Position = " + ghoul::to_string(worldPos) + "}}}";
    global::scriptEngine->queueScript("openspace.addSceneGraphNode(" + t + ")", scripting::ScriptEngine::RemoteScripting::No, [index, this](ghoul::Dictionary data) {
        const std::string nodeName = guitravelcomponent::nodeName(index);
        while (!global::renderEngine->scene()->sceneGraphNode(nodeName)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        this->addNewPathNode(nodeName);
        });
}

void GuiTravelComponent::addNewPathNode(const std::string& node) {
    std::lock_guard l(getQueueMutex());
    nodesQueue.push_back(node);
}

bool GuiTravelComponent::hasNext() {
    return !nodesToTravel.empty();
}

std::string GuiTravelComponent::next() {
    std::vector<std::string>::iterator first = nodesToTravel.begin();
    std::string firstNode = nodesToTravel[0];
    nodesToTravel.erase(first);
    return firstNode;
}

void GuiTravelComponent::nextPlanet() {
    std::lock_guard l(getNodesMutex());
    while (hasNext())
    {
        std::string nodeIdentifier = next();
        if (global::renderEngine->scene()->sceneGraphNode(nodeIdentifier))
        {
            global::scriptEngine->queueScript("openspace.pathnavigation.flyTo(\"" + nodeIdentifier + "\")", scripting::ScriptEngine::RemoteScripting::Yes);
            break;
        }
    }
}



scripting::LuaLibrary GuiTravelComponent::luaLibrary() {

    return {
        "travelplanets",
        {
            codegen::lua::NextPlanet
}

    };
}












}
