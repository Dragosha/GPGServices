#define EXTENSION_NAME GPGServices
#define LIB_NAME "GPGServices"
#define MODULE_NAME "GPGservices"

#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_ANDROID)
#include <jni.h>
#include "gpg/gpg.h"
//#include "gpg/android_initialization.h"
//#include "gpg/android_platform_configuration.h"
//#include "gpg/builder.h"
//#include "gpg/game_services.h"

bool is_auth_in_progress_ = false;
std::unique_ptr<gpg::GameServices> game_services_;


static int Init(lua_State* L)
{

     // Game Services have not been initialized, create a new Game Services.
    gpg::AndroidPlatformConfiguration platform_configuration;
    gpg::GameServices::Builder::OnAuthActionStartedCallback started_callback;
    gpg::GameServices::Builder::OnAuthActionFinishedCallback finished_callback;

    game_services_ = gpg::GameServices::Builder()
                .SetDefaultOnLog(gpg::LogLevel::VERBOSE)
                .SetOnAuthActionStarted([started_callback](gpg::AuthOperation op) {
                    is_auth_in_progress_ = true;
                    if (started_callback != nullptr) started_callback(op);
                })
                .SetOnAuthActionFinished([finished_callback](gpg::AuthOperation op,
                                                             gpg::AuthStatus status) {
                    printf("Sign in finished with a result of %d \n", status);
                    is_auth_in_progress_ = false;
                    if (finished_callback != nullptr) finished_callback(op, status);
                })
                .Create(platform_configuration);
    return 1;
}

static const luaL_reg Module_methods[] =
{
    {"init", Init},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}
#else

static void LuaInit(lua_State* L) {}

#endif

dmExtension::Result AppInitializeGPGServices(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeGPGServices(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeGPGServices(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeGPGServices(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}


// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)
DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeGPGServices, AppFinalizeGPGServices, InitializeGPGServices, 0, 0, FinalizeGPGServices)
