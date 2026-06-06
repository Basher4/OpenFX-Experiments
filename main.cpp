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

std::vector<OfxPlugin*> GetPlugins(const char* libraryPath);
OfxResult<void> RenderImageWithOfx(const char* path, image::Image& srcImg);
void ShowImage(image::Image& img);

OfxResult<void> PluginMain(
    const OfxPlugin* plugin, const char* action, const char* msg,
    const void* handle = nullptr, OfxPropertySetHandle inArgs = nullptr, OfxPropertySetHandle outArgs = nullptr);

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "App requires an argument with a path to an OFX plugin DLL\n";
        return 1;
    }

    auto img = image::Image::load("./starry_night.jpg");
    if (!RenderImageWithOfx(argv[1], img)) {
        std::cerr << "Failed to render image\n";
        return 1;
    }

    ShowImage(img);
    return 0;
}

OfxResult<void> RenderImageWithOfx(const char* path, image::Image& srcImg)
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

    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxActionLoad, "Loading plugin"));
    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxActionDescribe, "Describing plugin", effect.OfxHandle()));

    // Describe in general context.
    inArgs.SetString(kOfxImageEffectPropContext, 0, kOfxImageEffectContextGeneral);
    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxImageEffectActionDescribeInContext,
        "ImageEffect describing plugin in context", effect.OfxHandle(), inArgs.OfxHandle()));
    std::print("Plugin description in general context, including parameter set:\n");
    effect.DebugPrint();
    inArgs.Clear();

    // TODO: Create instance
    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxActionCreateInstance, "Creating instance", effect.OfxHandle()));

    // Render one frame of the plugin.
    auto srcClip = *effect.GetClip(kOfxImageEffectSimpleSourceClipName);
    auto outClip = *effect.GetClip(kOfxImageEffectOutputClipName);
    auto srcRef = srcImg.as_ofx_ref();
    srcClip->SetImageRef(&srcRef);
    outClip->SetImageRef(&srcRef);
    // nb. Modify the image in place. Probably not always valid but oh hey.

    // TODO: For basic plugin - set `scale` parameter to something


    inArgs.SetDouble(kOfxPropTime, 0, 0.0);
    inArgs.SetN(kOfxImageEffectPropRenderWindow, { 0, 0, (int)srcImg.width() / 2, (int)srcImg.height() });
    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxImageEffectActionRender,
        "ImageEffect render", effect.OfxHandle(), inArgs.OfxHandle()));

    // TODO: Destroy instance
    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxActionDestroyInstance, "Destroying instance", effect.OfxHandle()));

    // Cleanup.
    OFX_RETURN_IF_ERROR(PluginMain(plugin, kOfxActionUnload, "Unloading plugin"));

    return {};
}

std::vector<OfxPlugin*> GetPlugins(const char* libraryPath)
{
    // We're leaking this handle.
#if defined(WIN32) || defined(WIN64)
    const HMODULE ofxDll = LoadLibraryA(libraryPath);
    assert(ofxDll != nullptr);
    const FARPROC getNumPluginsPtr = GetProcAddress(ofxDll, "OfxGetNumberOfPlugins");
    const FARPROC getPluginPtr = GetProcAddress(ofxDll, "OfxGetPlugin");
#elif defined(__linux__)
    void* ofxDll = dlopen(libraryPath, RTLD_LAZY);
    assert(ofxDll != nullptr);
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

OfxResult<void> PluginMain(
    const OfxPlugin* plugin, const char* action, const char* msg,
    const void* handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs)
{
    std::print("{}...\n", msg);
    OfxStatus status = plugin->mainEntry(action, handle, inArgs, outArgs);
    if (status != kOfxStatOK) {
        std::print("Failed with {}\n", status);
        return std::unexpected { status };
    }

    std::print("OK\n\n");
    return {};
}

void ShowImage(image::Image& img)
{
    std::print("Width: {} x Height {}\n", img.width(), img.height());
    img.show_blocking();
}
