
namespace {
    [[codegen::luawrap]] void nextPlanet() {
        openspace::global::moduleEngine->module<openspace::ImGUIModule>()->guiTravelComopnent().nextPlanet();
    }
}
