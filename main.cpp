#include <expected>
#include <iostream>
#include <print>
#include <string>
#include <vector>

#if defined(WIN32) || defined(WIN64)
#include <Windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#endif

#include "openfx/ofxCore.h"

#include "host.h"
#include "imageEffect.h"
#include "imageIo.h"
#include "openfx/ofxImageEffect.h"

using fn_ofxGetNumberOfPlugins = int (*)();
using fn_ofxGetPlugin = OfxPlugin* (*)(int);

std::vector<OfxPlugin*> GetPlugins(const char *libraryPath);
void RenderImageWithOfx(const char *path);
void ShowImage();

std::expected<void, OfxStatus> PluginMain(
    const OfxPlugin* plugin, const char *action, const char *msg,
    const void *handle = nullptr, OfxPropertySetHandle inArgs = nullptr, OfxPropertySetHandle outArgs = nullptr
);

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "App requires an argument with a path to an OFX plugin DLL\n";
        return 1;
    }

    RenderImageWithOfx(argv[1]);
    ShowImage();
    return 0;
}

void RenderImageWithOfx(const char *path)
{
    Host ofxHost;
    ImageEffect effect;
    PropertySet inArgs;

    const auto plugins = GetPlugins(path);
    for (const auto plugin : plugins) {
        std::print("{}: {}\n", plugin->pluginApi, plugin->pluginIdentifier);
    }

    const auto plugin = plugins[0];
    plugin->setHost(ofxHost.OfxHandle());

    if (!PluginMain(plugin, kOfxActionLoad,     "Loading plugin")) { return; }
    if (!PluginMain(plugin, kOfxActionDescribe, "Describing plugin", effect.OfxHandle())) { return; }

    // Describe in general context.
    inArgs.SetString(kOfxImageEffectPropContext, 0, kOfxImageEffectContextGeneral);
    if (!PluginMain(plugin, kOfxImageEffectActionDescribeInContext,
                    "ImageEffect describing plugin in context", effect.OfxHandle(), inArgs.OfxHandle())) {
        return;
    }
    std::print("Plugin description in general context:\n");
    effect.DebugPrint();
    inArgs.Clear();

    // Render one frame of the plugin.

    if (!PluginMain(plugin, kOfxActionUnload,   "Unloading plugin")) { return; }
}

std::vector<OfxPlugin*> GetPlugins(const char *libraryPath)
{
    // We're leaking this handle.
#if defined(WIN32) || defined(WIN64)
    const HMODULE ofxDll = LoadLibraryA(libraryPath); assert(ofxDll != nullptr);
    const FARPROC getNumPluginsPtr = GetProcAddress(ofxDll, "OfxGetNumberOfPlugins");
    const FARPROC getPluginPtr = GetProcAddress(ofxDll, "OfxGetPlugin");
#elif defined(__linux__)
    void* ofxDll = dlopen(libraryPath, RTLD_LAZY); assert(ofxDll != nullptr);
    void* getNumPluginsPtr = dlsym(ofxDll, "OfxGetNumberOfPlugins");
    void* getPluginPtr = dlsym(ofxDll, "OfxGetPlugin");
#endif

    assert(getNumPluginsPtr != nullptr);
    const auto ofxGetNumberOfPlugins = reinterpret_cast<fn_ofxGetNumberOfPlugins>(getNumPluginsPtr);

    assert(getPluginPtr != nullptr);
    const auto ofxGetPlugin = reinterpret_cast<fn_ofxGetPlugin>(getPluginPtr);

    const int numPlugins = ofxGetNumberOfPlugins();
    std::vector<OfxPlugin*> result(numPlugins);

    for (int i = 0; i < numPlugins; i++) {
        result[i] = ofxGetPlugin(i);
    }

    return result;
}

std::expected<void, OfxStatus> PluginMain(
    const OfxPlugin* plugin, const char *action, const char *msg,
    const void *handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs
)
{
    std::print("{}...\n", msg);
    OfxStatus status = plugin->mainEntry(action, handle, inArgs, outArgs);
    if (status != kOfxStatOK) {
        std::print("Failed with {}\n", status);
        return std::unexpected{status};
    }

    std::print("OK\n\n");
    return {};
}

void ShowImage()
{
    auto img = image::Image::load("./starry_night.jpg");
    std::print("Width: {} x Height {}\n", img.width(), img.height());

    img.show_blocking();
}