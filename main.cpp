#include <cstdio>
#include <iostream>
#include <vector>
#include <Windows.h>

#include "openfx/ofxCore.h"

#include "host.h"
#include "imageIo.h"

using fn_ofxGetNumberOfPlugins = int (*)();
using fn_ofxGetPlugin = OfxPlugin* (*)(int);

std::vector<OfxPlugin*> GetPlugins(const char *libraryPath);
void RenderImageWithOfx(const char *path);
void ShowImage();

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
    OfxStatus status;

    const auto plugins = GetPlugins(path);
    for (const auto plugin : plugins) {
        printf("%s: %s\n", plugin->pluginApi, plugin->pluginIdentifier);
    }

    const auto plugin = plugins[0];
    plugin->setHost(ofxHost.OfxHandle());

    status = plugin->mainEntry(kOfxActionDescribe, nullptr, nullptr, nullptr);
    if (status != kOfxStatOK) {
        printf("Describe finished with %d\n", status);
        return;
    }

    status = plugin->mainEntry(kOfxActionLoad, nullptr, nullptr, nullptr);
    if (status != kOfxStatOK) {
        printf("Load finished with %d\n", status);
        return;
    }
}


std::vector<OfxPlugin*> GetPlugins(const char *libraryPath)
{
    // We're leaking this handle.
    const HMODULE ofxDll = LoadLibraryA(libraryPath);
    assert(ofxDll != nullptr);

    const auto ofxGetNumberOfPlugins = reinterpret_cast<fn_ofxGetNumberOfPlugins>(GetProcAddress(ofxDll, "OfxGetNumberOfPlugins"));
    assert(ofxGetNumberOfPlugins != nullptr);
    const auto ofxGetPlugin = reinterpret_cast<fn_ofxGetPlugin>(GetProcAddress(ofxDll, "OfxGetPlugin"));
    assert(ofxGetPlugin != nullptr);

    const int numPlugins = ofxGetNumberOfPlugins();
    std::vector<OfxPlugin*> result(numPlugins);

    for (int i = 0; i < numPlugins; i++) {
        result[i] = ofxGetPlugin(i);
    }

    return result;
}

void ShowImage()
{
    auto img = image::Image::load("./starry_night.jpg");
    printf("Width: %d x Height %d\n", img.width(), img.height());

    for (int y = 0; y < 512; y++) {
        for (int x = 0; x < 512; x++) {
            img.at(x, y)->b = 0;
        }
    }

    img.show_blocking();
}