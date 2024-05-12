#include <modules/nBody/nBodyModule.h>
#include <modules/nBody/headers/renderableNStars.h>
#include <openspace/documentation/documentation.h>
#include <openspace/rendering/renderable.h>
#include <openspace/scripting/lualibrary.h>
#include <openspace/util/factorymanager.h>
#include <openspace/rendering/dashboarditem.h>
#include <modules/nBody/headers/dashboarditemcameraposition.h>




namespace openspace {


    NBodyModule::NBodyModule() :OpenSpaceModule(Name) {
        ghoul::TemplateFactory<Renderable>* fRenderable =
            FactoryManager::ref().factory<Renderable>();
        ghoul_assert(fRenderable, "No renderable factory existed");
        fRenderable->registerClass<RenderableNStars>("RenderableNStars");

        ghoul::TemplateFactory<DashboardItem>* fDashboard =
            FactoryManager::ref().factory<DashboardItem>();

        fDashboard->registerClass<DashboardItemCameraPosition>("DashboardItemCameraPosition");


    }

    



    




}
