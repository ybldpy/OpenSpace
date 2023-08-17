#include "testmodule.h"
#include <openspace/scripting/scriptengine.h>
#include <openspace/engine/globals.h>
#include <openspace/camera/camera.h>
#include <openspace/camera/camerapose.h>
#include <openspace/navigation/navigationhandler.h>
#include <openspace/scene/scenegraphnode.h>





namespace testmodule_config {
    constexpr openspace::properties::Property::PropertyInfo enabledInfo = {
        "Enabled",
        "Enabled",
        "Test property",
        openspace::properties::Property::Visibility::Always
    };
    constexpr openspace::properties::Property::PropertyInfo xInfo = {
        "xaris",
        "xaris",
        "Test property",
        openspace::properties::Property::Visibility::Always
    };

    constexpr openspace::properties::Property::PropertyInfo yInfo = {
        "yaris",
        "yaris",
        "yest property",
        openspace::properties::Property::Visibility::Always
    };

    constexpr openspace::properties::Property::PropertyInfo zInfo = {
        "zaris",
        "zaris",
        "zest property",
        openspace::properties::Property::Visibility::Always
    };
    

}

static float var = 1.0f;
void callBack() {
    printf("++++++++++++++++++++++++++call back-----------------");
    openspace::Camera* camera = openspace::global::navigationHandler->camera();
    camera->setPose(openspace::CameraPose(camera->positionVec3()+camera->viewDirectionWorldSpace()*glm::dvec3(56.f), camera->rotationQuaternion()+glm::dquat(var++,0.0f,0.0f,0.0f)));
}
namespace openspace {

    
    
    TestModule::TestModule():OpenSpaceModule(Name),_enabled(testmodule_config::enabledInfo),_x(testmodule_config::xInfo,1.0f,0.0f,100.0f,0.1f),_y(testmodule_config::yInfo, 1.0f, 0.0f, 100.0f, 0.1f),_z(testmodule_config::zInfo, 1.0f, 0.0f, 100.0f, 0.1f){
        addProperty(this->_enabled);
        addProperty(this->_x);
        addProperty(this->_y);
        addProperty(this->_z);
        //_enabled.onChange(callBack);
        _enabled.onChange([this] {
            if (_enabled) {
                stopOritNavigator();
            }
            });
        _x.onChange([this] {
            if (!_enabled) { return; }
            openspace::Camera* camera = openspace::global::navigationHandler->camera();
            camera->setPositionVec3(camera->positionVec3() + glm::dvec3(100000.f, 0.0f, 0.f));
            });
        _y.onChange([this] {
            if (!_enabled) { return; }
            openspace::Camera* camera = openspace::global::navigationHandler->camera();
            camera->setPositionVec3(camera->positionVec3() + glm::dvec3(0.0f,100000.f, 0.f));
            });
        _z.onChange([this] {
            if (!_enabled) { return; }
            openspace::Camera* camera = openspace::global::navigationHandler->camera();
            camera->setPositionVec3(camera->positionVec3() + glm::dvec3(0.0f, 0.f, 100000.f));
            });
    };

    void TestModule::stopOritNavigator() {
        global::navigationHandler->orbitalNavigator().updateOnCameraInteraction();
        global::navigationHandler->orbitalNavigator().resetVelocities();
    }


}
