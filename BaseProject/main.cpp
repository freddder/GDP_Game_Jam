#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLCommon.h"

#include "globalThings.h"
#include "cFBO.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>      // "string stream" just like iostream and fstream
#include <vector>       // "smart array"
#include <fstream>      // C++ file I-O library (look like any other stream)
#include "cMapLoader.h"
#include "cAssimpMesh.h"

cMesh* pSkyBox;
GLint matModel_Location;
GLint matModelInverseTranspose_Location;

std::vector<cAssimpMesh*> assimpMeshes;

// Function signature for DrawObject()
void DrawObject(
    cMesh* pCurrentMesh,
    glm::mat4 matModel,
    GLint matModel_Location,
    GLint matModelInverseTranspose_Location,
    GLuint program,
    cVAOManager* pVAOManager);

void DrawScene(GLuint program, bool drawSkybox);

void PopulateOffsets(GLuint program);


int main(int argc, char** argv)
{
    GLFWwindow* pWindow;

    GLuint program = 0;     // 0 means "no shader program"

    GLint mvp_location = -1;        // Because glGetAttribLocation() returns -1 on error

    g_TitleText = "Definitely Not Pokemon Black/White 3";

    glfwSetErrorCallback(GLFW_error_callback);

    if (!glfwInit())
    {
        return -1;
        //exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    pWindow = glfwCreateWindow(1200, 640, "OpenGL is great!", NULL, NULL);

    if (!pWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(pWindow, GLFW_key_callback);
    // NEW: 
    glfwSetCursorEnterCallback(pWindow, GLFW_cursor_enter_callback);
    glfwSetCursorPosCallback(pWindow, GLFW_cursor_position_callback);
    glfwSetScrollCallback(pWindow, GLFW_scroll_callback);
    glfwSetMouseButtonCallback(pWindow, GLFW_mouse_button_callback);
    glfwSetWindowSizeCallback(pWindow, GLFW_window_size_callback);

    glfwMakeContextCurrent(pWindow);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);


    // See how many active uniform variables (registers) I can have
    GLint max_uniform_location = 0;
    GLint* p_max_uniform_location = NULL;
    p_max_uniform_location = &max_uniform_location;
    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, p_max_uniform_location);

    // You'll actually see something like this:
//    GLint max_uniform_location = 0;
//    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &max_uniform_location);

    std::cout << "GL_MAX_UNIFORM_LOCATIONS: " << max_uniform_location << std::endl;

    // Create global things
    ::g_StartUp(pWindow);

    //::g_pFlyCamera->setEye(glm::vec3(10.0f, 0.0f, 30.0f));
    ::g_pFlyCamera->setEye(glm::vec3(0.0f, 15.0f, 0.0f));



    cShaderManager::cShader vertShader;
    vertShader.fileName = "assets/shaders/vertShader_01.glsl";

    cShaderManager::cShader fragShader;
    fragShader.fileName = "assets/shaders/fragShader_01.glsl";

    if (::g_pShaderManager->createProgramFromFile("Shader#1", vertShader, fragShader))
    {
        std::cout << "Shader compiled OK" << std::endl;
        // 
        // Set the "program" variable to the one the Shader Manager used...
        program = ::g_pShaderManager->getIDFromFriendlyName("Shader#1");
    }
    else
    {
        std::cout << "Error making shader program: " << std::endl;
        std::cout << ::g_pShaderManager->getLastError() << std::endl;
    }


    // Select the shader program we want to use
    // (Note we only have one shader program at this point)
    glUseProgram(program);


    // ************************************************
    // Now, I'm going to "load up" all the uniform locations
    // (This was to show how a map could be used)
    cShaderManager::cShaderProgram* pShaderProc = ::g_pShaderManager->pGetShaderProgramFromFriendlyName("Shader#1");

    int theUniformIDLoc = -1;
    theUniformIDLoc = glGetUniformLocation(program, "matModel");
    pShaderProc->mapUniformName_to_UniformLocation["matModel"] = theUniformIDLoc;

    // Or...
    pShaderProc->mapUniformName_to_UniformLocation["matModel"] = glGetUniformLocation(program, "matModel");
    pShaderProc->mapUniformName_to_UniformLocation["matView"] = glGetUniformLocation(program, "matView");
    pShaderProc->mapUniformName_to_UniformLocation["matProjection"] = glGetUniformLocation(program, "matProjection");
    pShaderProc->mapUniformName_to_UniformLocation["matModelInverseTranspose"] = glGetUniformLocation(program, "matModelInverseTranspose");
    pShaderProc->mapUniformName_to_UniformLocation["wholeObjectSpecularColour"] = glGetUniformLocation(program, "wholeObjectSpecularColour");
    pShaderProc->mapUniformName_to_UniformLocation["theLights[0].position"] = glGetUniformLocation(program, "wholeObjectSpecularColour");
    // ... and so on..
    // ************************************************

    //GLint mvp_location = -1;
    mvp_location = glGetUniformLocation(program, "MVP");

    // Get "uniform locations" (aka the registers these are in)
    matModel_Location = glGetUniformLocation(program, "matModel");
    GLint matProjection_Location = glGetUniformLocation(program, "matProjection");
    matModelInverseTranspose_Location = glGetUniformLocation(program, "matModelInverseTranspose");

    ::g_pTheLights->theLights[0].position = glm::vec4(-10.f, 10.f, 10.f, 1.f);
    ::g_pTheLights->theLights[0].atten.y = 0.0001f;
    ::g_pTheLights->theLights[0].atten.z = 0.00001f;
    ::g_pTheLights->theLights[0].param1.x = 2.0f;    // directional
    ::g_pTheLights->theLights[0].direction = -(g_pTheLights->theLights[0].position);//glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ::g_pTheLights->theLights[0].diffuse = glm::vec4(0.9922f, 0.9843f, 0.8275f, 1.0f);
    ::g_pTheLights->TurnOnLight(0);

      // Get the uniform locations of the light shader values
    ::g_pTheLights->SetUpUniformLocations(program);

    ::g_pVAOManager->setFilePath("assets/models/");

    std::string demoDecorationMapName = "DemoDec.bmp";
    std::string demoCollisionMapName = "DemoCol.bmp";
    std::string mapErrorString;

    if (argc == 3)
    {
        demoDecorationMapName = argv[1];
        demoCollisionMapName = argv[2];
    }

    if (!g_MapLoader->LoadMap(demoDecorationMapName, demoCollisionMapName, mapErrorString))
    {
        std::cout << mapErrorString << std::endl;
        return 0;
    }

    PopulateOffsets(program);

    std::vector<std::string> vecModelsToLoad;
    vecModelsToLoad.push_back("ISO_Shphere_flat_3div_xyz_n_rgba_uv.ply");
    vecModelsToLoad.push_back("ISO_Shphere_flat_4div_xyz_n_rgba_uv.ply");
    vecModelsToLoad.push_back("Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply");
    vecModelsToLoad.push_back("SpriteHolder.ply");

    unsigned int totalVerticesLoaded = 0;
    unsigned int totalTrianglesLoaded = 0;
    std::vector<glm::vec4> oneInstanceVector;
    oneInstanceVector.push_back(glm::vec4(0.f, 0.f, 0.f, 1.f));
    for (std::vector<std::string>::iterator itModel = vecModelsToLoad.begin(); itModel != vecModelsToLoad.end(); itModel++)
    {
        sModelDrawInfo theModel;
        std::string modelName = *itModel;
        std::cout << "Loading " << modelName << "...";
        if (!::g_pVAOManager->LoadModelIntoVAO(modelName, theModel, program, oneInstanceVector))
        {
            std::cout << "didn't work because: " << std::endl;
            std::cout << ::g_pVAOManager->getLastError(true) << std::endl;
        }
        std::cout << std::endl;
    }//for (std::vector<std::string>::iterator itModel

    glm::vec3 dummyVec3;
    glm::vec3 directionalLightPositionVec3 = g_pTheLights->theLights[0].diffuse;
    glm::vec3 directionalLightRGBVec3 = g_pTheLights->theLights[0].diffuse;

    cModelAnimation* dayNightCycle = new cModelAnimation(directionalLightPositionVec3, dummyVec3, directionalLightRGBVec3);
    dayNightCycle->AddPositionKeyFrame(KeyFrameVec3(0.f, glm::vec3(13.f, 10.f, 15.f)));
    dayNightCycle->AddPositionKeyFrame(KeyFrameVec3(100.f, glm::vec3(0.f, 10.f, 15.f)));
    dayNightCycle->AddPositionKeyFrame(KeyFrameVec3(200.f, glm::vec3(-13.f, 10.f, 15.f)));
    dayNightCycle->AddScaleKeyFrame(KeyFrameVec3(0.f, glm::vec3(0.98f, 0.73f, 0.45f)));
    dayNightCycle->AddScaleKeyFrame(KeyFrameVec3(100.f, glm::vec3(0.9f, 0.9f, 0.9f)));
    dayNightCycle->AddScaleKeyFrame(KeyFrameVec3(200.f, glm::vec3(0.55f, 0.48f, 0.87f)));
    dayNightCycle->speed = 5.f;
    dayNightCycle->repeat = true;
    g_AnimationSystem->AddModelAnimation(dayNightCycle);


    cMesh* pSprite = new cMesh();
    pSprite->meshName = "SpriteHolder.ply";
    pSprite->orientationXYZ.x = glm::radians(315.f);
    pSprite->scale = glm::vec3(2.f);
    pSprite->textureNames[0] = "Nate_Idle_Down.bmp";
    pSprite->textureRatios[0] = 1.f;
    pSprite->discardTexture = true;
    pSprite->discardColor = glm::vec4(1.f, 0.f, 1.f, 1.f);
    ::g_vec_pMeshes.push_back(pSprite);

    g_MapLoader->PlacePlayer(pSprite->positionXYZ);
    g_Player = new cPlayer(pSprite);
    
   
    GLint glMaxCombinedTextureImageUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &glMaxCombinedTextureImageUnits);
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = " << glMaxCombinedTextureImageUnits << std::endl;

    GLint glMaxVertexTextureImageUnits = 0;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &glMaxVertexTextureImageUnits);
    std::cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << glMaxVertexTextureImageUnits << std::endl;

    GLint glMaxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTextureSize);
    std::cout << "GL_MAX_TEXTURE_SIZE = " << glMaxTextureSize << std::endl;

    // Load the textures
    ::g_pTextureManager->SetBasePath("assets/textures");
    ::g_pTextureManager->Create2DTextureFromBMPFile("BrightColouredUVMap.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Idle_Down.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Down_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Down_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Idle_Up.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Up_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Up_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Idle_Left.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Left_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Left_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Idle_Right.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Right_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Walk_Right_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Down_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Down_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Down_Middle.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Up_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Up_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Up_Middle.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Left_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Left_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Left_Middle.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Right_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Right_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Nate_Run_Right_Middle.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Grass_Tile_1-1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_T.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_TR.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_TRC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_TL.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_TLC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_B.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_BR.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_BRC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_BL.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_BLC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_M.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_R.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Path_Tile_L.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Tree_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_T.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_L.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_R.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_B.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_BR.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_BL.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_BRC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_BLC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_TL.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_TR.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_TRC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Edge_Tile_TLC.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Water_Tile_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Water_Tile_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Water_Tile_3.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Wall_Tile_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Stair_Tile_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Weed_1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Weed_2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("h1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("h2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("shadow.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("window.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("yane_001_lm1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("h_kage_lm1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("h_mado_lm2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("kabe_001_lm1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("kabe_002_lm1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("kazami001_lm1.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("c5ch.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("c5ch2.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("c5ch3.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("h_kage.bmp", true);


    // Add a skybox texture
    std::string errorTextString;
    ::g_pTextureManager->SetBasePath("assets/textures/cubemaps");
    if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("TropicalSunnyDay",
        "TropicalSunnyDayRight2048.bmp",    /* posX_fileName */
        "TropicalSunnyDayLeft2048.bmp",     /*negX_fileName */
        "TropicalSunnyDayDown2048.bmp",     /*posY_fileName*/
        "TropicalSunnyDayUp2048.bmp",       /*negY_fileName*/
        "TropicalSunnyDayBack2048.bmp",     /*posZ_fileName*/
        "TropicalSunnyDayFront2048.bmp",    /*negZ_fileName*/
        true, errorTextString))
    {
        std::cout << "Didn't load because: " << errorTextString << std::endl;
    }
    else
    {
        std::cout << "Loaded the sunny day cube texture OK" << std::endl;
    }
    if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("Space01",
        "SpaceBox_right1_posX.bmp",     /* posX_fileName */
        "SpaceBox_left2_negX.bmp",      /*negX_fileName */
        "SpaceBox_top3_posY.bmp",       /*posY_fileName*/
        "SpaceBox_bottom4_negY.bmp",    /*negY_fileName*/
        "SpaceBox_front5_posZ.bmp",     /*posZ_fileName*/
        "SpaceBox_back6_negZ.bmp",      /*negZ_fileName*/
        true, errorTextString))
    {
        std::cout << "Didn't load because: " << errorTextString << std::endl;
    }
    else
    {
        std::cout << "Loaded the sunny day cube texture OK" << std::endl;
    }
    std::cout << "I've loaded these textures:" << std::endl;
    std::vector<std::string> vecTexturesLoaded;
    ::g_pTextureManager->GetLoadedTextureList(vecTexturesLoaded);

    for (std::vector<std::string>::iterator itTexName = vecTexturesLoaded.begin();
        itTexName != vecTexturesLoaded.end(); itTexName++)
    {
        std::cout << "\t" << *itTexName << std::endl;
    }


    // Create a skybox object (a sphere with inverted normals that moves with the camera eye)
    pSkyBox = new cMesh();
    pSkyBox->meshName = "Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply";
    pSkyBox->scale = glm::vec3(::g_pFlyCamera->nearPlane * 1000.0f);
    pSkyBox->positionXYZ = ::g_pFlyCamera->getEye();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.WantCaptureMouse = false;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(pWindow, true);
    ImGui_ImplOpenGL3_Init("#version 420");

    //***************** Depth map ********************

    const unsigned int SHADOW_WIDTH = 3048, SHADOW_HEIGHT = 3048;

    cFBO* depthFBO = new cFBO();
    std::string errorString;
    if (!depthFBO->init(SHADOW_WIDTH, SHADOW_HEIGHT, errorString))
        std::cout << "FBO error: " << errorString << std::endl;

    //************************************************

    float cameraDistance = 17.f;
    float cameraAngle = glm::radians(35.f);

    const double MAX_DELTA_TIME = 0.1;  // 100 ms
    double previousTime = glfwGetTime();

    bool dayNightCycleOn = true;

    while (!glfwWindowShouldClose(pWindow))
    {
        float ratio;
        int width, height;
        glm::mat4 matModel;             // used to be "m"; Sometimes it's called "world"
        glm::mat4 matProjection;        // used to be "p";
        glm::mat4 matView;              // used to be "v";

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - previousTime;
        deltaTime = (deltaTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : deltaTime);
        previousTime = currentTime;

        g_AnimationSystem->Process(deltaTime);
        
        if (dayNightCycleOn)
        {
            g_pTheLights->theLights[0].diffuse.r = directionalLightRGBVec3.x;
            g_pTheLights->theLights[0].diffuse.g = directionalLightRGBVec3.y;
            g_pTheLights->theLights[0].diffuse.b = directionalLightRGBVec3.z;
            g_pTheLights->theLights[0].position.x = directionalLightPositionVec3.x;
            g_pTheLights->theLights[0].position.y = directionalLightPositionVec3.y;
            g_pTheLights->theLights[0].position.z = directionalLightPositionVec3.z;
        }

        glfwGetFramebufferSize(pWindow, &width, &height);
        ratio = width / (float)height;

        //*******************************************************
        g_pTheLights->theLights[0].direction = -(g_pTheLights->theLights[0].position);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO->ID);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::vec3 eyeForLight = glm::vec3(g_pTheLights->theLights[0].position) + pSprite->positionXYZ; // "eye" is 100 units away from the quad
        glm::vec3 atForLight = pSprite->positionXYZ;//glm::vec3(0.f, 0.f, 0.f);    // "at" the quad
        matView = glm::lookAt(eyeForLight, atForLight, glm::vec3(0.0f, 1.0f, 0.0f));

        float near_plane = 1.0f, far_plane = 100.f;
        glm::mat4 lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);

        glm::mat4 lightSpaceMatrix = lightProjection * matView;
                                                          
        glUniformMatrix4fv(pShaderProc->getUniformID_From_Name("matView"), 1, GL_FALSE, glm::value_ptr(matView));
        glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(lightProjection));

        glCullFace(GL_FRONT);
        DrawScene(program, false);
        glCullFace(GL_BACK);

        GLuint lightSpaceMatrix_LocId = glGetUniformLocation(program, "matLightSpace");
        glUniformMatrix4fv(lightSpaceMatrix_LocId, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        //*******************************************************

        GLuint depthTextureID = depthFBO->depthTexture_ID;
        GLuint textureNumber = 20;
        glActiveTexture(textureNumber + GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTextureID);

        GLint depthTexture_LocID = glGetUniformLocation(program, "depthMap");
        glUniform1i(depthTexture_LocID, textureNumber);

        // Start of final pass

        // Turn on the depth buffer
        glEnable(GL_DEPTH);         // Turns on the depth buffer
        glEnable(GL_DEPTH_TEST);    // Check if the pixel is already closer

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform1i(depthTexture_LocID, textureNumber);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // *******************************************************
        // Screen is cleared and we are ready to draw the scene...
        // *******************************************************

        // Update the title text
        glfwSetWindowTitle(pWindow, ::g_TitleText.c_str());

        // Copy the light information into the shader to draw the scene
        ::g_pTheLights->CopyLightInfoToShader();

        ::g_pFlyCamera->Update(deltaTime);

        matProjection = glm::perspective(
            ::g_pFlyCamera->FOV,
            ratio,
            ::g_pFlyCamera->nearPlane,      // Near plane (as large as possible)
            ::g_pFlyCamera->farPlane);      // Far plane (as small as possible)

        glm::vec3 cameraEye = ::g_pFlyCamera->getEye();
        glm::vec3 cameraAt = ::g_pFlyCamera->getAtInWorldSpace();
        glm::vec3 cameraUp = ::g_pFlyCamera->getUpVector();

        if (!useFlyCamera)
        {
            cameraEye.x = g_Player->GetPosition().x;
            cameraEye.y = 1 + g_Player->GetPosition().y + (glm::sin(cameraAngle) * cameraDistance);
            cameraEye.z = g_Player->GetPosition().z + (glm::cos(cameraAngle) * cameraDistance);

            cameraAt = g_Player->GetPosition();
        }

        matView = glm::mat4(1.0f);
        matView = glm::lookAt(cameraEye,   // "eye"
            cameraAt,    // "at"
            cameraUp);

        glUniformMatrix4fv(pShaderProc->getUniformID_From_Name("matView"), 1, GL_FALSE, glm::value_ptr(matView));
        glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(matProjection));

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        DrawScene(program, true);
        // End of final pass

        //float* position[3];
        //position[0] = &g_pTheLights->theLights[0].position.x;
        //position[1] = &g_pTheLights->theLights[0].position.y;
        //position[2] = &g_pTheLights->theLights[0].position.z;

        //float* direction[3];
        //direction[0] = &g_pTheLights->theLights[0].direction.x;
        //direction[1] = &g_pTheLights->theLights[0].direction.y;
        //direction[2] = &g_pTheLights->theLights[0].direction.z;

        //float* colors[3];
        //colors[0] = &g_pTheLights->theLights[0].diffuse.r;
        //colors[1] = &g_pTheLights->theLights[0].diffuse.g;
        //colors[2] = &g_pTheLights->theLights[0].diffuse.b;

        //ImGui::Begin("Lights");
        //ImGui::ColorEdit3("Color", *colors);
        //ImGui::DragFloat3("Position", *position);
        //ImGui::Checkbox("Day & Night cycle", &dayNightCycleOn);
        //ImGui::DragFloat("Cycle speed", &dayNightCycle->speed);
        //ImGui::Image((void*)(intptr_t)depthFBO->depthTexture_ID, ImVec2(200, 200));
        //ImGui::End();

        //float* playerPosition[3];
        //playerPosition[0] = &pSprite->positionXYZ.x;
        //playerPosition[1] = &pSprite->positionXYZ.y;
        //playerPosition[2] = &pSprite->positionXYZ.z;

        //float* playerRotation[3];
        //playerRotation[0] = &pSprite->orientationXYZ.x;
        //playerRotation[1] = &pSprite->orientationXYZ.y;
        //playerRotation[2] = &pSprite->orientationXYZ.z;

        //ImGui::Begin("Player");
        //ImGui::DragFloat3("Position", *playerPosition);
        //ImGui::DragFloat3("Orientation", *playerRotation, 0.785, 0, glm::radians(360.f));
        //ImGui::End();

        //ImGui::Begin("Camera");
        //ImGui::DragFloat("Distance", &cameraDistance);
        //ImGui::DragFloat("Angle", &cameraAngle, 0.087f, 0.f, glm::radians(90.f) - 0.1f);
        //ImGui::End();

        //float* houseposition[3];
        //houseposition[0] = &house.position.x;
        //houseposition[1] = &house.position.y;
        //houseposition[2] = &house.position.z;

        //ImGui::Begin("House");
        //ImGui::DragFloat3("Position", *houseposition);
        //ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // "Present" what we've drawn.
        glfwSwapBuffers(pWindow);        // Show what we've drawn

        // Process any events that have happened
        glfwPollEvents();

        // Handle OUR keyboard, mouse stuff
        handleAsyncKeyboard(pWindow, deltaTime);

        if (!io.WantCaptureMouse)
        {
            handleAsyncMouse(pWindow, deltaTime);
        }

    }//while (!glfwWindowShouldClose(window))

    delete pSkyBox;

    // All done, so delete things...
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    ::g_ShutDown(pWindow);

    glfwDestroyWindow(pWindow);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void DrawScene(GLuint program, bool drawSkybox)
{
    GLint bIsSkyBox_LocID = glGetUniformLocation(program, "bIsSkyBox");

    if (drawSkybox)
    {        
        glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_TRUE);

        glDisable(GL_DEPTH_TEST);
        // Move the "skybox object" with the camera
        pSkyBox->positionXYZ = ::g_pFlyCamera->getEye();
        DrawObject(
            pSkyBox, glm::mat4(1.0f),
            matModel_Location, matModelInverseTranspose_Location,
            program, ::g_pVAOManager);
    }

    glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    // *********************************************************************

    for (unsigned int index = 0; index != ::g_vec_pMeshes.size(); index++)
    {
        // So the code is a little easier...
        cMesh* pCurrentMesh = ::g_vec_pMeshes[index];

        //matModel = glm::mat4(1.0f);

        DrawObject(pCurrentMesh,
            glm::mat4(1.0f),
            matModel_Location,
            matModelInverseTranspose_Location,
            program,
            ::g_pVAOManager);
    }

    for (unsigned int i = 0; i < assimpMeshes.size(); i++)
    {
        assimpMeshes[i]->DrawObject(program);
    }
}

void PopulateOffsets(GLuint program)
{
    // LOAD STUFF HERE 4HEAD
    sModelDrawInfo uh;

    std::vector<glm::vec4> grassOffsets;
    g_MapLoader->Load_Grass_M_Offsets(grassOffsets);

    std::vector<glm::vec4> path_M_offsets;
    g_MapLoader->Load_Path_M_Offsets(path_M_offsets);

    std::vector<glm::vec4> path_L_offsets;
    g_MapLoader->Load_Path_L_Offsets(path_L_offsets);

    std::vector<glm::vec4> path_TL_offsets;
    g_MapLoader->Load_Path_TL_Offsets(path_TL_offsets);

    std::vector<glm::vec4> path_BL_offsets;
    g_MapLoader->Load_Path_BL_Offsets(path_BL_offsets);

    std::vector<glm::vec4> path_R_offsets;
    g_MapLoader->Load_Path_R_Offsets(path_R_offsets);

    std::vector<glm::vec4> path_BR_offsets;
    g_MapLoader->Load_Path_BR_Offsets(path_BR_offsets);

    std::vector<glm::vec4> path_TR_offsets;
    g_MapLoader->Load_Path_TR_Offsets(path_TR_offsets);

    std::vector<glm::vec4> path_T_offsets;
    g_MapLoader->Load_Path_T_Offsets(path_T_offsets);

    std::vector<glm::vec4> path_B_offsets;
    g_MapLoader->Load_Path_B_Offsets(path_B_offsets);

    std::vector<glm::vec4> path_TLC_offsets;
    g_MapLoader->Load_Path_TLC_Offsets(path_TLC_offsets);

    std::vector<glm::vec4> path_BLC_offsets;
    g_MapLoader->Load_Path_BLC_Offsets(path_BLC_offsets);

    std::vector<glm::vec4> path_TRC_offsets;
    g_MapLoader->Load_Path_TRC_Offsets(path_TRC_offsets);

    std::vector<glm::vec4> path_BRC_offsets;
    g_MapLoader->Load_Path_BRC_Offsets(path_BRC_offsets);

    std::vector<glm::vec4> trees_offsets;
    g_MapLoader->Load_Trees_Offsets(trees_offsets);

    std::vector<glm::vec4> edge_T_offsets;
    g_MapLoader->Load_Edge_T_Offsets(edge_T_offsets);

    std::vector<glm::vec4> edge_L_offsets;
    g_MapLoader->Load_Edge_L_Offsets(edge_L_offsets);

    std::vector<glm::vec4> edge_R_offsets;
    g_MapLoader->Load_Edge_R_Offsets(edge_R_offsets);

    std::vector<glm::vec4> edge_B_offsets;
    g_MapLoader->Load_Edge_B_Offsets(edge_B_offsets);

    std::vector<glm::vec4> edge_BL_offsets;
    g_MapLoader->Load_Edge_BL_Offsets(edge_BL_offsets);

    std::vector<glm::vec4> edge_BR_offsets;
    g_MapLoader->Load_Edge_BR_Offsets(edge_BR_offsets);

    std::vector<glm::vec4> edge_TR_offsets;
    g_MapLoader->Load_Edge_TR_Offsets(edge_TR_offsets);

    std::vector<glm::vec4> edge_TL_offsets;
    g_MapLoader->Load_Edge_TL_Offsets(edge_TL_offsets);

    std::vector<glm::vec4> edge_TLC_offsets;
    g_MapLoader->Load_Edge_TLC_Offsets(edge_TLC_offsets);

    std::vector<glm::vec4> edge_TRC_offsets;
    g_MapLoader->Load_Edge_TRC_Offsets(edge_TRC_offsets);

    std::vector<glm::vec4> edge_BRC_offsets;
    g_MapLoader->Load_Edge_BRC_Offsets(edge_BRC_offsets);

    std::vector<glm::vec4> edge_BLC_offsets;
    g_MapLoader->Load_Edge_BLC_Offsets(edge_BLC_offsets);

    std::vector<glm::vec4> water_offsets;
    g_MapLoader->Load_Water_Offsets(water_offsets);

    std::vector<glm::vec4> stair_R_offsets;
    g_MapLoader->Load_Stair_R_Offsets(stair_R_offsets);

    std::vector<glm::vec4> stair_L_offsets;
    g_MapLoader->Load_Stair_L_Offsets(stair_L_offsets);

    std::vector<glm::vec4> stair_U_offsets;
    g_MapLoader->Load_Stair_U_Offsets(stair_U_offsets);

    std::vector<glm::vec4> stair_D_offsets;
    g_MapLoader->Load_Stair_D_Offsets(stair_D_offsets);

    std::vector<glm::vec4> weed_offsets;
    g_MapLoader->Load_Weed_Offsets(weed_offsets, grassOffsets, 0.1f);

    std::vector<glm::vec4> house_offsets;
    g_MapLoader->Load_Structure_Offsets(house_offsets, "house");

    std::vector<glm::vec4> lab_offsets;
    g_MapLoader->Load_Structure_Offsets(lab_offsets, "lab");

    std::vector<glm::vec4> church_offsets;
    g_MapLoader->Load_Structure_Offsets(church_offsets, "church");

    cMesh* pGrassTile = new cMesh();
    pGrassTile->meshName = "Tile_Quad_Grass.ply";
    pGrassTile->textureNames[0] = "Grass_Tile_1-1.bmp";
    pGrassTile->textureRatios[0] = 1.f;
    if (grassOffsets.size() != 0)
    {
        pGrassTile->isInstanced = true;
        pGrassTile->instancedNumber = grassOffsets.size();
        ::g_vec_pMeshes.push_back(pGrassTile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Grass.ply", uh, program, grassOffsets);
    }

    cMesh* pPath_M_Tile = new cMesh();
    pPath_M_Tile->meshName = "Tile_Quad_Path_M.ply";
    pPath_M_Tile->textureNames[0] = "Path_Tile_M.bmp";
    pPath_M_Tile->textureRatios[0] = 1.f;
    if (path_M_offsets.size() != 0)
    {
        pPath_M_Tile->isInstanced = true;
        pPath_M_Tile->instancedNumber = path_M_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_M_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_M.ply", uh, program, path_M_offsets);
    }

    cMesh* pPath_L_Tile = new cMesh();
    pPath_L_Tile->meshName = "Tile_Quad_Path_L.ply";
    pPath_L_Tile->textureNames[0] = "Path_Tile_L.bmp";
    pPath_L_Tile->textureRatios[0] = 1.f;
    if (path_L_offsets.size() != 0)
    {
        pPath_L_Tile->isInstanced = true;
        pPath_L_Tile->instancedNumber = path_L_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_L_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_L.ply", uh, program, path_L_offsets);
    }

    cMesh* pPath_TL_Tile = new cMesh();
    pPath_TL_Tile->meshName = "Tile_Quad_Path_TL.ply";
    pPath_TL_Tile->textureNames[0] = "Path_Tile_TL.bmp";
    pPath_TL_Tile->textureRatios[0] = 1.f;
    if (path_TL_offsets.size() != 0)
    {
        pPath_TL_Tile->isInstanced = true;
        pPath_TL_Tile->instancedNumber = path_TL_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_TL_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_TL.ply", uh, program, path_TL_offsets);
    }

    cMesh* pPath_BL_Tile = new cMesh();
    pPath_BL_Tile->meshName = "Tile_Quad_Path_BL.ply";
    pPath_BL_Tile->textureNames[0] = "Path_Tile_BL.bmp";
    pPath_BL_Tile->textureRatios[0] = 1.f;
    if (path_BL_offsets.size() != 0)
    {
        pPath_BL_Tile->isInstanced = true;
        pPath_BL_Tile->instancedNumber = path_BL_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_BL_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_BL.ply", uh, program, path_BL_offsets);
    }

    cMesh* pPath_R_Tile = new cMesh();
    pPath_R_Tile->meshName = "Tile_Quad_Path_R.ply";
    pPath_R_Tile->textureNames[0] = "Path_Tile_R.bmp";
    pPath_R_Tile->textureRatios[0] = 1.f;
    if (path_R_offsets.size() != 0)
    {
        pPath_R_Tile->isInstanced = true;
        pPath_R_Tile->instancedNumber = path_R_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_R_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_R.ply", uh, program, path_R_offsets);
    }

    cMesh* pPath_BR_Tile = new cMesh();
    pPath_BR_Tile->meshName = "Tile_Quad_Path_BR.ply";
    pPath_BR_Tile->textureNames[0] = "Path_Tile_BR.bmp";
    pPath_BR_Tile->textureRatios[0] = 1.f;
    if (path_BR_offsets.size() != 0)
    {
        pPath_BR_Tile->isInstanced = true;
        pPath_BR_Tile->instancedNumber = path_BR_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_BR_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_BR.ply", uh, program, path_BR_offsets);
    }

    cMesh* pPath_TR_Tile = new cMesh();
    pPath_TR_Tile->meshName = "Tile_Quad_Path_TR.ply";
    pPath_TR_Tile->textureNames[0] = "Path_Tile_TR.bmp";
    pPath_TR_Tile->textureRatios[0] = 1.f;
    if (path_TR_offsets.size() != 0)
    {
        pPath_TR_Tile->isInstanced = true;
        pPath_TR_Tile->instancedNumber = path_TR_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_TR_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_TR.ply", uh, program, path_TR_offsets);
    }

    cMesh* pPath_T_Tile = new cMesh();
    pPath_T_Tile->meshName = "Tile_Quad_Path_T.ply";
    pPath_T_Tile->textureNames[0] = "Path_Tile_T.bmp";
    pPath_T_Tile->textureRatios[0] = 1.f;
    if (path_T_offsets.size() != 0)
    {
        pPath_T_Tile->isInstanced = true;
        pPath_T_Tile->instancedNumber = path_T_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_T_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_T.ply", uh, program, path_T_offsets);
    }

    cMesh* pPath_B_Tile = new cMesh();
    pPath_B_Tile->meshName = "Tile_Quad_Path_B.ply";
    pPath_B_Tile->textureNames[0] = "Path_Tile_B.bmp";
    pPath_B_Tile->textureRatios[0] = 1.f;
    if (path_B_offsets.size() != 0)
    {
        pPath_B_Tile->isInstanced = true;
        pPath_B_Tile->instancedNumber = path_B_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_B_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_B.ply", uh, program, path_B_offsets);
    }

    cMesh* pPath_TLC_Tile = new cMesh();
    pPath_TLC_Tile->meshName = "Tile_Quad_Path_TLC.ply";
    pPath_TLC_Tile->textureNames[0] = "Path_Tile_TLC.bmp";
    pPath_TLC_Tile->textureRatios[0] = 1.f;
    if (path_TLC_offsets.size() != 0)
    {
        pPath_TLC_Tile->isInstanced = true;
        pPath_TLC_Tile->instancedNumber = path_TLC_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_TLC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_TLC.ply", uh, program, path_TLC_offsets);
    }

    cMesh* pPath_BLC_Tile = new cMesh();
    pPath_BLC_Tile->meshName = "Tile_Quad_Path_BLC.ply";
    pPath_BLC_Tile->textureNames[0] = "Path_Tile_BLC.bmp";
    pPath_BLC_Tile->textureRatios[0] = 1.f;
    if (path_BLC_offsets.size() != 0)
    {
        pPath_BLC_Tile->isInstanced = true;
        pPath_BLC_Tile->instancedNumber = path_BLC_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_BLC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_BLC.ply", uh, program, path_BLC_offsets);
    }

    cMesh* pPath_BRC_Tile = new cMesh();
    pPath_BRC_Tile->meshName = "Tile_Quad_Path_BRC.ply";
    pPath_BRC_Tile->textureNames[0] = "Path_Tile_BRC.bmp";
    pPath_BRC_Tile->textureRatios[0] = 1.f;
    if (path_BRC_offsets.size() != 0)
    {
        pPath_BRC_Tile->isInstanced = true;
        pPath_BRC_Tile->instancedNumber = path_BRC_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_BRC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_BRC.ply", uh, program, path_BRC_offsets);
    }

    cMesh* pPath_TRC_Tile = new cMesh();
    pPath_TRC_Tile->meshName = "Tile_Quad_Path_TRC.ply";
    pPath_TRC_Tile->textureNames[0] = "Path_Tile_TRC.bmp";
    pPath_TRC_Tile->textureRatios[0] = 1.f;
    if (path_TRC_offsets.size() != 0)
    {
        pPath_TRC_Tile->isInstanced = true;
        pPath_TRC_Tile->instancedNumber = path_TRC_offsets.size();
        ::g_vec_pMeshes.push_back(pPath_TRC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Path_TRC.ply", uh, program, path_TRC_offsets);
    }

    cMesh* pTree1 = new cMesh();
    pTree1->meshName = "Tree_Quad_1.ply";
    pTree1->textureNames[0] = "Tree_1.bmp";
    pTree1->textureRatios[0] = 1.f;
    pTree1->discardBlack = true;
    if (trees_offsets.size() != 0)
    {
        pTree1->isInstanced = true;
        pTree1->instancedNumber = trees_offsets.size();
        ::g_vec_pMeshes.push_back(pTree1);
        g_pVAOManager->LoadModelIntoVAO("Tree_Quad_1.ply", uh, program, trees_offsets);
    }

    cMesh* pTree2 = new cMesh();
    pTree2->meshName = "Tree_Quad_2.ply";
    pTree2->textureNames[0] = "Tree_1.bmp";
    pTree2->textureRatios[0] = 1.f;
    pTree2->discardBlack = true;
    if (trees_offsets.size() != 0)
    {
        pTree2->isInstanced = true;
        pTree2->instancedNumber = trees_offsets.size();
        ::g_vec_pMeshes.push_back(pTree2);
        g_pVAOManager->LoadModelIntoVAO("Tree_Quad_2.ply", uh, program, trees_offsets);
    }

    cMesh* pEdge_T_Tile = new cMesh();
    pEdge_T_Tile->meshName = "Tile_Quad_Edge_T.ply";
    pEdge_T_Tile->textureNames[0] = "Edge_Tile_T.bmp";
    pEdge_T_Tile->textureRatios[0] = 1.f;
    pEdge_T_Tile->discardBlack = true;
    if (edge_T_offsets.size() != 0)
    {
        pEdge_T_Tile->isInstanced = true;
        pEdge_T_Tile->instancedNumber = edge_T_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_T_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_T.ply", uh, program, edge_T_offsets);
    }

    cMesh* pEdge_L_Tile = new cMesh();
    pEdge_L_Tile->meshName = "Tile_Quad_Edge_L.ply";
    pEdge_L_Tile->textureNames[0] = "Edge_Tile_L.bmp";
    pEdge_L_Tile->textureRatios[0] = 1.f;
    pEdge_L_Tile->discardBlack = true;
    if (edge_L_offsets.size() != 0)
    {
        pEdge_L_Tile->isInstanced = true;
        pEdge_L_Tile->instancedNumber = edge_L_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_L_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_L.ply", uh, program, edge_L_offsets);
    }

    cMesh* pEdge_R_Tile = new cMesh();
    pEdge_R_Tile->meshName = "Tile_Quad_Edge_R.ply";
    pEdge_R_Tile->textureNames[0] = "Edge_Tile_R.bmp";
    pEdge_R_Tile->textureRatios[0] = 1.f;
    pEdge_R_Tile->discardBlack = true;
    if (edge_R_offsets.size() != 0)
    {
        pEdge_R_Tile->isInstanced = true;
        pEdge_R_Tile->instancedNumber = edge_R_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_R_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_R.ply", uh, program, edge_R_offsets);
    }

    cMesh* pEdge_B_Tile = new cMesh();
    pEdge_B_Tile->meshName = "Tile_Quad_Edge_B.ply";
    pEdge_B_Tile->textureNames[0] = "Edge_Tile_B.bmp";
    pEdge_B_Tile->textureRatios[0] = 1.f;
    pEdge_B_Tile->discardBlack = true;
    if (edge_B_offsets.size() != 0)
    {
        pEdge_B_Tile->isInstanced = true;
        pEdge_B_Tile->instancedNumber = edge_B_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_B_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_B.ply", uh, program, edge_B_offsets);
    }

    cMesh* pEdge_BL_Tile = new cMesh();
    pEdge_BL_Tile->meshName = "Tile_Quad_Edge_BL.ply";
    pEdge_BL_Tile->textureNames[0] = "Edge_Tile_BL.bmp";
    pEdge_BL_Tile->textureRatios[0] = 1.f;
    pEdge_BL_Tile->discardBlack = true;
    if (edge_BL_offsets.size() != 0)
    {
        pEdge_BL_Tile->isInstanced = true;
        pEdge_BL_Tile->instancedNumber = edge_BL_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_BL_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_BL.ply", uh, program, edge_BL_offsets);
    }

    cMesh* pEdge_BR_Tile = new cMesh();
    pEdge_BR_Tile->meshName = "Tile_Quad_Edge_BR.ply";
    pEdge_BR_Tile->textureNames[0] = "Edge_Tile_BR.bmp";
    pEdge_BR_Tile->textureRatios[0] = 1.f;
    pEdge_BR_Tile->discardBlack = true;
    if (edge_BR_offsets.size() != 0)
    {
        pEdge_BR_Tile->isInstanced = true;
        pEdge_BR_Tile->instancedNumber = edge_BR_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_BR_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_BR.ply", uh, program, edge_BR_offsets);
    }

    cMesh* pEdge_TR_Tile = new cMesh();
    pEdge_TR_Tile->meshName = "Tile_Quad_Edge_TR.ply";
    pEdge_TR_Tile->textureNames[0] = "Edge_Tile_TR.bmp";
    pEdge_TR_Tile->textureRatios[0] = 1.f;
    pEdge_TR_Tile->discardBlack = true;
    if (edge_TR_offsets.size() != 0)
    {
        pEdge_TR_Tile->isInstanced = true;
        pEdge_TR_Tile->instancedNumber = edge_TR_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_TR_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_TR.ply", uh, program, edge_TR_offsets);
    }

    cMesh* pEdge_TL_Tile = new cMesh();
    pEdge_TL_Tile->meshName = "Tile_Quad_Edge_TL.ply";
    pEdge_TL_Tile->textureNames[0] = "Edge_Tile_TL.bmp";
    pEdge_TL_Tile->textureRatios[0] = 1.f;
    pEdge_TL_Tile->discardBlack = true;
    if (edge_TL_offsets.size() != 0)
    {
        pEdge_TL_Tile->isInstanced = true;
        pEdge_TL_Tile->instancedNumber = edge_TL_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_TL_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_TL.ply", uh, program, edge_TL_offsets);
    }

    cMesh* pEdge_TLC_Tile = new cMesh();
    pEdge_TLC_Tile->meshName = "Tile_Quad_Edge_TLC.ply";
    pEdge_TLC_Tile->textureNames[0] = "Edge_Tile_TLC.bmp";
    pEdge_TLC_Tile->textureRatios[0] = 1.f;
    pEdge_TLC_Tile->discardBlack = true;
    if (edge_TLC_offsets.size() != 0)
    {
        pEdge_TLC_Tile->isInstanced = true;
        pEdge_TLC_Tile->instancedNumber = edge_TLC_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_TLC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_TLC.ply", uh, program, edge_TLC_offsets);
    }

    cMesh* pEdge_TRC_Tile = new cMesh();
    pEdge_TRC_Tile->meshName = "Tile_Quad_Edge_TRC.ply";
    pEdge_TRC_Tile->textureNames[0] = "Edge_Tile_TRC.bmp";
    pEdge_TRC_Tile->textureRatios[0] = 1.f;
    pEdge_TRC_Tile->discardBlack = true;
    if (edge_TRC_offsets.size() != 0)
    {
        pEdge_TRC_Tile->isInstanced = true;
        pEdge_TRC_Tile->instancedNumber = edge_TRC_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_TRC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_TRC.ply", uh, program, edge_TRC_offsets);
    }

    cMesh* pEdge_BRC_Tile = new cMesh();
    pEdge_BRC_Tile->meshName = "Tile_Quad_Edge_BRC.ply";
    pEdge_BRC_Tile->textureNames[0] = "Edge_Tile_BRC.bmp";
    pEdge_BRC_Tile->textureRatios[0] = 1.f;
    pEdge_BRC_Tile->discardBlack = true;
    if (edge_BRC_offsets.size() != 0)
    {
        pEdge_BRC_Tile->isInstanced = true;
        pEdge_BRC_Tile->instancedNumber = edge_BRC_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_BRC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_BRC.ply", uh, program, edge_BRC_offsets);
    }

    cMesh* pEdge_BLC_Tile = new cMesh();
    pEdge_BLC_Tile->meshName = "Tile_Quad_Edge_BLC.ply";
    pEdge_BLC_Tile->textureNames[0] = "Edge_Tile_BLC.bmp";
    pEdge_BLC_Tile->textureRatios[0] = 1.f;
    pEdge_BLC_Tile->discardBlack = true;
    if (edge_BLC_offsets.size() != 0)
    {
        pEdge_BLC_Tile->isInstanced = true;
        pEdge_BLC_Tile->instancedNumber = edge_BLC_offsets.size();
        ::g_vec_pMeshes.push_back(pEdge_BLC_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Edge_BLC.ply", uh, program, edge_BLC_offsets);
    }

    cMesh* pWater_Tile = new cMesh();
    pWater_Tile->meshName = "Tile_Quad_Water.ply";
    pWater_Tile->positionXYZ.y = 0.5;
    pWater_Tile->textureNames[0] = "Water_Tile_1.bmp";
    pWater_Tile->textureRatios[0] = 1.f;
    pWater_Tile->discardTexture = true;
    pWater_Tile->discardColor = glm::vec4(0.f, 1.f, 1.f, 1.f);
    if (water_offsets.size() != 0)
    {
        pWater_Tile->isInstanced = true;
        pWater_Tile->instancedNumber = water_offsets.size();
        ::g_vec_pMeshes.push_back(pWater_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Water.ply", uh, program, water_offsets);
    }

    if (edge_B_offsets.size() != 0 || edge_TLC_offsets.size() != 0 || edge_TRC_offsets.size() != 0)
    {
        std::vector<glm::vec4> wall_B_offsets;
        g_MapLoader->Load_Wall_B_Offsets(wall_B_offsets, edge_B_offsets, edge_TLC_offsets, edge_TRC_offsets);

        cMesh* pWall_B_Tile = new cMesh();
        pWall_B_Tile->meshName = "Tile_Quad_Wall_B.ply";
        pWall_B_Tile->textureNames[0] = "Wall_Tile_1.bmp";
        pWall_B_Tile->textureRatios[0] = 1.f;
        if (wall_B_offsets.size() != 0)
        {
            pWall_B_Tile->isInstanced = true;
            pWall_B_Tile->instancedNumber = wall_B_offsets.size();
            ::g_vec_pMeshes.push_back(pWall_B_Tile);
            g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Wall_B.ply", uh, program, wall_B_offsets);
        }
    }

    if (edge_R_offsets.size() != 0 || edge_TLC_offsets.size() != 0)
    {
        std::vector<glm::vec4> wall_R_offsets;
        g_MapLoader->Load_Wall_R_Offsets(wall_R_offsets, edge_R_offsets, edge_TLC_offsets);

        cMesh* pWall_R_Tile = new cMesh();
        pWall_R_Tile->meshName = "Tile_Quad_Wall_R.ply";
        pWall_R_Tile->textureNames[0] = "Wall_Tile_1.bmp";
        pWall_R_Tile->textureRatios[0] = 1.f;
        if (wall_R_offsets.size() != 0)
        {
            pWall_R_Tile->isInstanced = true;
            pWall_R_Tile->instancedNumber = wall_R_offsets.size();
            ::g_vec_pMeshes.push_back(pWall_R_Tile);
            g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Wall_R.ply", uh, program, wall_R_offsets);
        }
    }

    if (edge_L_offsets.size() != 0 || edge_TRC_offsets.size() != 0)
    {
        std::vector<glm::vec4> wall_L_offsets;
        g_MapLoader->Load_Wall_L_Offsets(wall_L_offsets, edge_L_offsets, edge_TRC_offsets);

        cMesh* pWall_L_Tile = new cMesh();
        pWall_L_Tile->meshName = "Tile_Quad_Wall_L.ply";
        pWall_L_Tile->textureNames[0] = "Wall_Tile_1.bmp";
        pWall_L_Tile->textureRatios[0] = 1.f;
        if (wall_L_offsets.size() != 0)
        {
            pWall_L_Tile->isInstanced = true;
            pWall_L_Tile->instancedNumber = wall_L_offsets.size();
            ::g_vec_pMeshes.push_back(pWall_L_Tile);
            g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Wall_L.ply", uh, program, wall_L_offsets);
        }
    }

    if (edge_BL_offsets.size() != 0)
    {
        std::vector<glm::vec4> wall_BL_offsets;
        g_MapLoader->Load_Wall_BL_Offsets(wall_BL_offsets, edge_BL_offsets);

        cMesh* pWall_BL_Tile = new cMesh();
        pWall_BL_Tile->meshName = "Tile_Corner_BL.ply";
        pWall_BL_Tile->textureNames[0] = "Wall_Tile_1.bmp";
        pWall_BL_Tile->textureRatios[0] = 1.f;
        if (wall_BL_offsets.size() != 0)
        {
            pWall_BL_Tile->isInstanced = true;
            pWall_BL_Tile->instancedNumber = wall_BL_offsets.size();
            ::g_vec_pMeshes.push_back(pWall_BL_Tile);
            g_pVAOManager->LoadModelIntoVAO("Tile_Corner_BL.ply", uh, program, wall_BL_offsets);
        }
    }

    if (edge_BR_offsets.size() != 0)
    {
        std::vector<glm::vec4> wall_BR_offsets;
        g_MapLoader->Load_Wall_BR_Offsets(wall_BR_offsets, edge_BR_offsets);

        cMesh* pWall_BR_Tile = new cMesh();
        pWall_BR_Tile->meshName = "Tile_Corner_BR.ply";
        pWall_BR_Tile->textureNames[0] = "Wall_Tile_1.bmp";
        pWall_BR_Tile->textureRatios[0] = 1.f;
        if (wall_BR_offsets.size() != 0)
        {
            pWall_BR_Tile->isInstanced = true;
            pWall_BR_Tile->instancedNumber = wall_BR_offsets.size();
            ::g_vec_pMeshes.push_back(pWall_BR_Tile);
            g_pVAOManager->LoadModelIntoVAO("Tile_Corner_BR.ply", uh, program, wall_BR_offsets);
        }
    }

    cMesh* pStair_R_Tile = new cMesh();
    pStair_R_Tile->meshName = "Tile_Stair_R.ply";
    pStair_R_Tile->textureNames[0] = "Stair_Tile_1.bmp";
    pStair_R_Tile->textureRatios[0] = 1.f;
    if (stair_R_offsets.size() != 0)
    {
        pStair_R_Tile->isInstanced = true;
        pStair_R_Tile->instancedNumber = stair_R_offsets.size();
        ::g_vec_pMeshes.push_back(pStair_R_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Stair_R.ply", uh, program, stair_R_offsets);
    }

    cMesh* pStair_L_Tile = new cMesh();
    pStair_L_Tile->meshName = "Tile_Stair_L.ply";
    pStair_L_Tile->textureNames[0] = "Stair_Tile_1.bmp";
    pStair_L_Tile->textureRatios[0] = 1.f;
    if (stair_L_offsets.size() != 0)
    {
        pStair_L_Tile->isInstanced = true;
        pStair_L_Tile->instancedNumber = stair_L_offsets.size();
        ::g_vec_pMeshes.push_back(pStair_L_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Stair_L.ply", uh, program, stair_L_offsets);
    }

    cMesh* pStair_U_Tile = new cMesh();
    pStair_U_Tile->meshName = "Tile_Stair_U.ply";
    pStair_U_Tile->textureNames[0] = "Stair_Tile_1.bmp";
    pStair_U_Tile->textureRatios[0] = 1.f;
    if (stair_U_offsets.size() != 0)
    {
        pStair_U_Tile->isInstanced = true;
        pStair_U_Tile->instancedNumber = stair_U_offsets.size();
        ::g_vec_pMeshes.push_back(pStair_U_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Stair_U.ply", uh, program, stair_U_offsets);
    }

    cMesh* pStair_D_Tile = new cMesh();
    pStair_D_Tile->meshName = "Tile_Stair_D.ply";
    pStair_D_Tile->textureNames[0] = "Stair_Tile_1.bmp";
    pStair_D_Tile->textureRatios[0] = 1.f;
    if (stair_D_offsets.size() != 0)
    {
        pStair_D_Tile->isInstanced = true;
        pStair_D_Tile->instancedNumber = stair_D_offsets.size();
        ::g_vec_pMeshes.push_back(pStair_D_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Stair_D.ply", uh, program, stair_D_offsets);
    }

    cMesh* pWeed_Tile = new cMesh();
    pWeed_Tile->meshName = "Tile_Quad_Weed.ply";
    pWeed_Tile->textureNames[0] = "Weed_1.bmp";
    pWeed_Tile->textureRatios[0] = 1.f;
    pWeed_Tile->discardBlack = true;
    if (weed_offsets.size() != 0)
    {
        pWeed_Tile->isInstanced = true;
        pWeed_Tile->instancedNumber = weed_offsets.size();
        ::g_vec_pMeshes.push_back(pWeed_Tile);
        g_pVAOManager->LoadModelIntoVAO("Tile_Quad_Weed.ply", uh, program, weed_offsets);
    }

    cAssimpMesh* house = new cAssimpMesh();
    house->position.y = -0.1f;
    house->scale = glm::vec3(0.07f);
    house->LoadModel("Mistralton City House.obj");
    if (house_offsets.size() != 0)
    {
        house->isInstanced = true;
        house->instancedNumber = house_offsets.size();
        house->LoadModelIntoVAO(program, house_offsets);
        assimpMeshes.push_back(house);
    }

    cAssimpMesh* lab = new cAssimpMesh();
    lab->position.y = -0.1f;
    lab->scale = glm::vec3(0.07f);
    lab->LoadModel("Juniper Pokemon Lab.obj");
    if (lab_offsets.size() != 0)
    {
        lab->isInstanced = true;
        lab->instancedNumber = lab_offsets.size();
        lab->LoadModelIntoVAO(program, lab_offsets);
        assimpMeshes.push_back(lab);
    }

    cAssimpMesh* church = new cAssimpMesh();
    church->position.y = -0.1f;
    church->scale = glm::vec3(0.07f);
    church->LoadModel("Driftveil City Building.obj");
    if (church_offsets.size() != 0)
    {
        church->isInstanced = true;
        church->instancedNumber = church_offsets.size();
        church->LoadModelIntoVAO(program, church_offsets);
        assimpMeshes.push_back(church);
    }

    cSpriteAnimation* waterAnimation = new cSpriteAnimation(pWater_Tile->textureNames[0]);
    waterAnimation->repeat = true;
    waterAnimation->AddKeyFrame(KeyFrameString(0.5f, "Water_Tile_3.bmp"));
    waterAnimation->AddKeyFrame(KeyFrameString(1.f, "Water_Tile_1.bmp"));
    g_AnimationSystem->AddSpriteAnimation(waterAnimation);

    cSpriteAnimation* weedAnimation = new cSpriteAnimation(pWeed_Tile->textureNames[0]);
    weedAnimation->repeat = true;
    weedAnimation->AddKeyFrame(KeyFrameString(0.15f, "Weed_2.bmp"));
    weedAnimation->AddKeyFrame(KeyFrameString(0.3f, "Weed_1.bmp"));
    g_AnimationSystem->AddSpriteAnimation(weedAnimation);
}