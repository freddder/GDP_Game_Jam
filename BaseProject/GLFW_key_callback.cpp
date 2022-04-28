#include "globalThings.h"

#include <sstream>
#include <iostream>


// Turns off the: warning C4005: 'APIENTRY': macro redefinition
#pragma warning( disable: 4005)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
void UseMessageBox(void)
{

    // If you want to do some Win32 calls:
    // Note the "L" which is "wide" or Unicode encoding
    //MessageBox(NULL, L"Moved Camera", L"FYI:", MB_OK);
    int response = MessageBox(NULL, L"Moved Camera", L"FYI:", MB_YESNOCANCEL | MB_ICONWARNING);
    switch (response)
    {
    case IDYES:
        std::cout << "You chose YES" << std::endl;
        break;
    case IDNO:
        std::cout << "You chose NO" << std::endl;
        break;
    case IDCANCEL:
        std::cout << "You chose CANCEL" << std::endl;
        break;
    }

    return;
}

/*static*/ void GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        useFlyCamera = !useFlyCamera;
    }

    //if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    //{
    //    ::g_pFlyCamera->MoveForward_Z(+g_pFlyCamera->movementSpeed);
    //}

    float cameraSpeed = 1.0f;
    float objectMovementSpeed = 1.0f;
    float lightMovementSpeed = 1.0f;

    bool bShiftDown = false;
    bool bControlDown = false;
    bool bAltDown = false;

        // Use bitwise mask to filter out just the shift
    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
    {
        // Shift is down and maybe other things, too
        bShiftDown = true;
    }
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        // Shift is down and maybe other things, too
        bControlDown = true;
    }
    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
    {
        // Shift is down and maybe other things, too
        bAltDown = true;
    }
       // If JUST the ALT is down, move the "selected" light

    return;
}
