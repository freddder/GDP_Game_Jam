#include "GLCommon.h" 
#include "globalThings.h"

#include <sstream>
#include <iostream>



// Handle async IO (keyboard, joystick, mouse, etc.)


// This is so the "fly camera" won't pay attention to the mouse if it's 
// not directly over the window. 
bool g_MouseIsInsideWindow = false;



void handleAsyncKeyboard(GLFWwindow* pWindow, double deltaTime)
{
    float cameraMoveSpeed = ::g_pFlyCamera->movementSpeed;

    float objectMovementSpeed = 0.1f;
    float lightMovementSpeed = 10.0f;

    if (cGFLWKeyboardModifiers::areAllModsUp(pWindow))
    {
        if (useFlyCamera)
        {
            // Use "fly" camera (keyboard for movement, mouse for aim)
            if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
            {
                ::g_pFlyCamera->MoveForward_Z(+cameraMoveSpeed);
            }
            if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)	// "backwards"
            {
                ::g_pFlyCamera->MoveForward_Z(-cameraMoveSpeed);
            }
            if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)	// "left"
            {
                ::g_pFlyCamera->MoveLeftRight_X(-cameraMoveSpeed);
            }
            if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)	// "right"
            {
                ::g_pFlyCamera->MoveLeftRight_X(+cameraMoveSpeed);
            }
            if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS)	// "up"
            {
                ::g_pFlyCamera->MoveUpDown_Y(-cameraMoveSpeed);
            }
            if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS)	// "down"
            {
                ::g_pFlyCamera->MoveUpDown_Y(+cameraMoveSpeed);
            }
        }

        //if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_RELEASE) // if w isnt pressed
        //{
        //    std::cout << "W released" << std::endl;
        //}

        std::stringstream strTitle;
        // std::cout << 
        glm::vec3 cameraEye = ::g_pFlyCamera->getEye();
        strTitle << "Camera: "
            << cameraEye.x << ", "
            << cameraEye.y << ", "
            << cameraEye.z; //<< std::endl;

        //::g_TitleText = strTitle.str();

    }//if ( cGFLWKeyboardModifiers::areAllModsUp(pWindow) )


//    // Basic camera controls (if NONE of the control keys are pressed)
//    if ( cGFLWKeyboardModifiers::areAllModsUp(pWindow) )
//    {
//        if ( glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS ) { ::g_cameraEye.x -= cameraMoveSpeed; } // Go left
//        if ( glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS ) { ::g_cameraEye.x += cameraMoveSpeed; } // Go right
//        if ( glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS ) { ::g_cameraEye.z += cameraMoveSpeed; }// Go forward 
//        if ( glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS ) { ::g_cameraEye.z -= cameraMoveSpeed; }// Go backwards
//        if ( glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS ) { ::g_cameraEye.y -= cameraMoveSpeed; }// Go "Down"
//        if ( glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS ) { ::g_cameraEye.y += cameraMoveSpeed; }// Go "Up"
//
//        std::stringstream strTitle;
//        // std::cout << 
//        strTitle << "Camera: "
//            << ::g_cameraEye.x << ", "
//            << ::g_cameraEye.y << ", "
//            << ::g_cameraEye.z; //<< std::endl;
//
//        ::g_TitleText = strTitle.str();
//
//    }//if ( areAllModsUp(window) )...



    // If JUST the shift is down, move the "selected" object
    //if (cGFLWKeyboardModifiers::isModifierDown(pWindow, true, false, false))
    //{
        if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS ||
            glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS ||
            glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS ||
            glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
            {
                if(glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    g_Player->Run(Directions::Up);
                else
                    g_Player->Walk(Directions::Up);
            }
            else if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    g_Player->Run(Directions::Down);
                else
                    g_Player->Walk(Directions::Down);
            }
            else if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    g_Player->Run(Directions::Left);
                else
                    g_Player->Walk(Directions::Left);
            }
            else if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    g_Player->Run(Directions::Right);
                else
                    g_Player->Walk(Directions::Right);
            }
        }
        else
        {
            if (glfwGetKey(pWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                g_Player->Run(Directions::NONE);
            else
                g_Player->Walk(Directions::NONE);
            //g_Player->Walk(Directions::NONE);
        }
    //}//if ( cGFLWKeyboardModifiers::...


    // If JUST the ALT is down, move the "selected" light
    if (cGFLWKeyboardModifiers::isModifierDown(pWindow, false, false, true))
    {
        
    }//if ( cGFLWKeyboardModifiers::...

    return;
}


// We call these every frame
void handleAsyncMouse(GLFWwindow* window, double deltaTime)
{

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    ::g_pFlyCamera->setMouseXY(x, y);

    const float MOUSE_SENSITIVITY = 2.0f;


    // Mouse left (primary?) button pressed? 
    // AND the mouse is inside the window...
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        && ::g_MouseIsInsideWindow)
    {
        // Mouse button is down so turn the camera
        ::g_pFlyCamera->Yaw_LeftRight(::g_pFlyCamera->getDeltaMouseX() * MOUSE_SENSITIVITY, deltaTime);

        ::g_pFlyCamera->Pitch_UpDown(-::g_pFlyCamera->getDeltaMouseY() * MOUSE_SENSITIVITY, deltaTime);

    }

    // Adjust the mouse speed
    if (::g_MouseIsInsideWindow)
    {
        const float MOUSE_WHEEL_SENSITIVITY = 0.1f;

        // Adjust the movement speed based on the wheel position
        ::g_pFlyCamera->movementSpeed -= (::g_pFlyCamera->getMouseWheel() * MOUSE_WHEEL_SENSITIVITY);

        // Clear the mouse wheel delta (or it will increase constantly)
        ::g_pFlyCamera->clearMouseWheelValue();


        if (::g_pFlyCamera->movementSpeed <= 0.0f)
        {
            ::g_pFlyCamera->movementSpeed = 0.0f;
        }
    }


    return;
}

void GLFW_cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        std::cout << "Mouse cursor is over the window" << std::endl;
        ::g_MouseIsInsideWindow = true;
    }
    else
    {
        std::cout << "Mouse cursor is no longer over the window" << std::endl;
        ::g_MouseIsInsideWindow = false;
    }
    return;
}

// Called when the mouse scroll wheel is moved
void GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float mouseScrollWheelSensitivity = 0.1f;

    ::g_pFlyCamera->setMouseWheelDelta(yoffset * mouseScrollWheelSensitivity);

    return;
}

void GLFW_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

//#ifdef YO_NERDS_WE_USING_WINDOWS_CONTEXT_MENUS_IN_THIS_THANG
//    // Right button is pop-up
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
//    {
//        ShowWindowsContextMenu(window, button, action, mods);
//    }
//#endif

    return;
}


void GLFW_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    return;
}


