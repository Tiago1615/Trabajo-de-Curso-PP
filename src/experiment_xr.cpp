// ------------------------------------------------------------
// EXPERIMENTO OPENXR (NO PARTE DEL BUILD FINAL)
// ------------------------------------------------------------

#include <iostream>
#include <cstring>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include <GLFW/glfw3native.h>

#include <X11/Xlib.h>
#include <GL/glx.h>

#define XR_USE_PLATFORM_XLIB
#define XR_USE_GRAPHICS_API_OPENGL
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

using namespace std;

// ------------------------------------------------------------
GLFWwindow* initOpenGL()
{
    if (!glfwInit())
        throw runtime_error("GLFW init failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenXR Test", nullptr, nullptr);
    if (!window)
        throw runtime_error("GLFW window failed");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw runtime_error("GLAD failed");

    cout << "OpenGL ready: " << glGetString(GL_VERSION) << endl;
    return window;
}

// ------------------------------------------------------------
XrInstance createXRInstance()
{
    const char* extensions[] = {
        XR_KHR_OPENGL_ENABLE_EXTENSION_NAME
    };

    XrInstanceCreateInfo ci{XR_TYPE_INSTANCE_CREATE_INFO};
    strcpy(ci.applicationInfo.applicationName, "TrabajoCursoPP_XR");
    ci.applicationInfo.applicationVersion = 1;
    strcpy(ci.applicationInfo.engineName, "CustomEngine");
    ci.applicationInfo.engineVersion = 1;
    ci.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    ci.enabledExtensionCount = 1;
    ci.enabledExtensionNames = extensions;

    XrInstance instance;
    if (xrCreateInstance(&ci, &instance) != XR_SUCCESS)
        throw runtime_error("xrCreateInstance failed");

    cout << "OpenXR instance created\n";
    return instance;
}

// ------------------------------------------------------------
XrSystemId getXRSystem(XrInstance instance)
{
    XrSystemGetInfo info{XR_TYPE_SYSTEM_GET_INFO};
    info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrSystemId systemId;
    if (xrGetSystem(instance, &info, &systemId) != XR_SUCCESS)
        throw runtime_error("xrGetSystem failed");

    return systemId;
}

// ------------------------------------------------------------
XrSession createXRSession(XrInstance instance, XrSystemId systemId, GLFWwindow* window)
{
    XrGraphicsBindingOpenGLXlibKHR glBinding{
        XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR
    };
    glBinding.xDisplay = glfwGetX11Display();
    glBinding.glxDrawable = glfwGetGLXWindow(window);
    glBinding.glxContext = glfwGetGLXContext(window);

    XrSessionCreateInfo sci{XR_TYPE_SESSION_CREATE_INFO};
    sci.systemId = systemId;
    sci.next = &glBinding;

    XrSession session;
    if (xrCreateSession(instance, &sci, &session) != XR_SUCCESS)
        throw runtime_error("xrCreateSession failed");

    cout << "OpenXR session created (OpenGL)\n";
    return session;
}

// ------------------------------------------------------------
int main()
{
    try
    {
        GLFWwindow* window = initOpenGL();
        XrInstance instance = createXRInstance();
        XrSystemId systemId = getXRSystem(instance);
        XrSession session = createXRSession(instance, systemId, window);

        cout << "Paso A completado correctamente\n";

        xrDestroySession(session);
        xrDestroyInstance(instance);
        glfwTerminate();
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return -1;
    }

    return 0;
}
