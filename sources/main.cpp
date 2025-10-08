// work through learnopengl to the end of starting stuff
// bench game
// look up the 3d collison stuff-ahh
// do concept artt
// plan game out
// make the simple walking sim (go from point to point when you get to point spawn in new world)
// make audio and music for it, add that
// upload to websats
// yay you did your first dream

#define DEBUG

#include <functional>

#ifndef DEBUG
#include <sys/resource.h>
#endif

#include "../headers/system.hxx"
#include "../headers/audio.hxx"
#include "../headers/editor.hxx"

extern const unsigned int window_width = 640;
extern const unsigned int window_height = 420;
unsigned int current_window_width = window_width;
unsigned int current_window_height = window_height;
glm::vec2 window_multiple = glm::vec2(1.0, 1.0);
bool window_resize = false;
extern float pixel_scale;
extern float ui_pixel_scale;
const double tick_time = 0.01;
const unsigned int frameUpdateLimit = 60;
unsigned int frameUpdateCount = 0;
extern const glm::vec3 spawnLocation = glm::vec3(0, 0, 3.0);
const std::string START_LEVEL_PATH = "./levels/menu.l";

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void window_focus_callback(GLFWwindow *window, int focused);
void processInput(GLFWwindow *window);
void processDebugInput(GLFWwindow *window);

double delta_time = 0.0,
       lastTime = 0.0, currentTime = 0.0;

double debugSaveTimer = 0.0;
const double debugSaveCountdown = 5.0;
templevel editor_level;
int debug_texture_offset = 0;
bool debug_editing_texture = false, debug_create_obj_called = false, debug_delete_obj_called = false;

glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraXZFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraRight = glm::vec3(0.0);
glm::quat cameraRotation = glm::quat(glm::vec3(0.0));
level_object player_object;
const double CAMERA_SPEED = 5.0;

double pitch = 0.0, yaw = -90.0;
const float mouse_sensitivity = 0.001f;
double cameraFloor = 0.0;
double jump_velocity = 0.0;
bool onGround = false;
glm::vec2 mousePos = glm::vec2(0.0);
bool mouseClicked = false, mousePressed = false;

bool cursorHeld = false;

bool debugMode = false;
bool mobileMode = false;

GLFWwindow *window;

std::function<void()> loop;
void main_loop(void *mLoopArg)
{
    loop();
}

game mainGame;
texturegroup allTextures;

int main()
{
    if (!glfwInit())
        return -1;

#ifndef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    window = glfwCreateWindow(window_width, window_height, "game window", nullptr, nullptr);

    glfwMakeContextCurrent(window);

#ifndef __EMSCRIPTEN__
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
#endif

#ifndef __EMSCRIPTEN__
    shader shader_main("./shaders/texture.vs", "./shaders/texture-color.fs");
    shader shader_ui("./shaders/texture.vs", "./shaders/texture-ui.fs");
#else
    shader shader_main("./shaders/texture-emscripten.vs", "./shaders/texture-emscripten-color.fs");
    shader shader_ui("./shaders/texture-emscripten.vs", "./shaders/texture-emscripten-ui.fs");
#endif

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE); // need to re-do vertices for models to make this work properly
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    graphics_init();

    shader_main.use();

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

    glm::mat4 proj = glm::perspective(glm::radians(65.0f), (float)window_width / (float)window_height, 0.1f, 1000.0f);
    shader_main.setMat4("projection", proj);

    shader_ui.use();

    float window_aspect = static_cast<float>(window_width) / static_cast<float>(window_height);
    glm::mat4 orth_proj = glm::ortho(0.0f, window_aspect, 0.0f, 1.0f, -1000.0f, 1000.0f);
    shader_ui.setMat4("projection", orth_proj);

    allTextures.addTexture(0, "./img/playbutton.png"); // mobile (mouse-based) graphics/movement. Done! Start designing a game if you don't want to be broke! :) Also, windows build is broken
    allTextures.addTexture(1, "./img/mobilebutton.png");
    allTextures.addTexture(2, "./img/mobile-movement.png");
    allTextures.addTexture(3, "./img/mobile-sight.png");
    allTextures.addTexture(4, "./img/fullscreen.png");
    allTextures.addTexture(5, "./img/tree.png");
    allTextures.addTexture(6, "./img/tree-2.png");
    allTextures.addTexture(7, "./img/tree-3.png");
    allTextures.addTexture(8, "./img/leaves.png");
    allTextures.addTexture(9, "./img/dirt.png");

    // aabb plcol = makeAABB(glm::vec3(0.0), glm::vec3(0.5, 1.75, 0.5));
    player_object.visual = model_primitive(MODEL_CUBE, true, false);
    player_object.Put(spawnLocation);
    player_object.SetScale(glm::vec3(0.5, 1.75, 0.5));
    player_object.type = OBJ_SOLID;

    bool testbool = false;

    mainGame.setup_level("./levels/menu.l");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    model_primitive mobileVisual(MODEL_QUAD);
    mobileVisual.Image(1);
    mobileVisual.Put(-32.0 / ui_pixel_scale * window_aspect, 48.0 / ui_pixel_scale, 0.0);
    mobileVisual.Scale(32.0 / ui_pixel_scale, 24.0 / ui_pixel_scale, 1.0);
    glm::vec2 windowVec2 = glm::vec2((float)window_width, (float)window_height);
    aabb2d ui_collider({glm::vec2(32.0, 48.0) * (windowVec2 / ui_pixel_scale), glm::vec2(32.0 * (float)window_height / ui_pixel_scale, 24.0 * (float)window_height / ui_pixel_scale)});

    ui_object mobileButton = ui_object({mobileVisual, ui_collider, 0, glm::vec2(32.0, 48.0), glm::vec2(32.0, 24.0)});

    model_primitive mobileEyeVisual(MODEL_QUAD);
    mobileEyeVisual.Image(3);
    mobileEyeVisual.Put(-52.0 / ui_pixel_scale * window_aspect, 16.0 / ui_pixel_scale, 0.0);
    mobileEyeVisual.Scale(32.0 / ui_pixel_scale, 32.0 / ui_pixel_scale, 1.0);
    aabb2d m_eye_collider({glm::vec2(52.0, 16.0) * (windowVec2 / ui_pixel_scale), glm::vec2(32.0 * (float)window_height / ui_pixel_scale, 32.0 * (float)window_height / ui_pixel_scale)});

    ui_object mobileEyeObject = ui_object({mobileEyeVisual, m_eye_collider, 0, glm::vec2(52.0, 16.0), glm::vec2(32.0, 32.0)});

    model_primitive mobileWalkVisual(MODEL_QUAD);
    mobileWalkVisual.Image(2);
    mobileWalkVisual.Put(-12.0 / ui_pixel_scale * window_aspect, 16.0 / ui_pixel_scale, 0.0);
    mobileWalkVisual.Scale(32.0 / ui_pixel_scale, 32.0 / ui_pixel_scale, 1.0);
    aabb2d m_walk_collider({glm::vec2(12.0, 16.0) * (windowVec2 / ui_pixel_scale), glm::vec2(32.0 * (float)window_height / ui_pixel_scale, 32.0 * (float)window_height / ui_pixel_scale)});

    ui_object mobileWalkObject = ui_object({mobileWalkVisual, m_walk_collider, 0, glm::vec2(12.0, 16.0), glm::vec2(32.0, 32.0)});

    model_primitive fullscreenVisual(MODEL_QUAD);
    fullscreenVisual.Image(4);
    fullscreenVisual.Put(-8.0 / ui_pixel_scale * window_aspect, 52.0 / ui_pixel_scale, 0.0);
    fullscreenVisual.Scale(16.0 / ui_pixel_scale, 16.0 / ui_pixel_scale, 1.0);
    aabb2d fs_ui_collider({glm::vec2(8.0, 52.0) * (windowVec2 / ui_pixel_scale), glm::vec2(16.0 * (float)window_height / ui_pixel_scale, 16.0 * (float)window_height / ui_pixel_scale)});

    ui_object fullscreenButton = ui_object({fullscreenVisual, fs_ui_collider, 0, glm::vec2(8.0, 52.0), glm::vec2(32.0, 24.0)});

    double frame_accumulation = 0.0;

    loop = [&]
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (!debugMode)
            glClearColor(0.0, 0.0, 0.0, 1.0);
        else
            glClearColor(0.2, 0.2, 1.0, 1.0);

        frame_accumulation += delta_time;

        lastTime = currentTime;
        currentTime = glfwGetTime();
        delta_time = currentTime - lastTime;

        mobileButton.updateSize(window_resize);
        fullscreenButton.updateSize(window_resize);
        if (mobileMode)
        {
            mobileEyeObject.updateSize(window_resize);
            mobileWalkObject.updateSize(window_resize);
            if (colliding(mobileEyeObject.collider, mousePos) && mousePressed)
            {
                glm::vec2 mobileLookVector = glm::normalize(mousePos - mobileEyeObject.collider.pos) * 10.0f;
                pitch += mobileLookVector.y * mouse_sensitivity;
                yaw += mobileLookVector.x * mouse_sensitivity;
            }

            if (pitch > 1.5f)
                pitch = 1.5f;
            if (pitch < -1.5f)
                pitch = -1.5f;

            glm::quat qPitch = glm::angleAxis(static_cast<float>(-pitch), glm::vec3(1.0, 0.0, 0.0));
            glm::quat qYaw = glm::angleAxis(static_cast<float>(yaw), glm::vec3(0.0, 1.0, 0.0));
            glm::quat nqYaw = glm::angleAxis(static_cast<float>(-yaw), glm::vec3(0.0, 1.0, 0.0));

            cameraRotation = glm::normalize(qPitch * qYaw);

            glm::quat lookDirection = glm::angleAxis(static_cast<float>(yaw), glm::vec3(0.0, -1.0, 0.0));
            lookDirection *= glm::angleAxis(static_cast<float>(pitch), glm::vec3(1.0, 0.0, 0.0));
            cameraFront = glm::normalize(glm::rotate(lookDirection, glm::vec3(0.0, 0.0, -1.0)));

            glm::quat swappedRot = glm::normalize(glm::quat(nqYaw));
            cameraXZFront = glm::rotate(swappedRot, glm::vec3(0.0, 0.0, -1.0));
            cameraRight = glm::cross(cameraXZFront, up);

            if (colliding(mobileWalkObject.collider, mousePos) && mousePressed)
            {
                glm::vec2 mobileMoveVector = mousePos - mobileWalkObject.collider.pos;
                mobileMoveVector = glm::normalize(mobileMoveVector) * static_cast<float>(CAMERA_SPEED);
                glm::vec3 mobileMoveDir = glm::vec3(0.0);
                mobileMoveDir += cameraXZFront * mobileMoveVector.y;
                mobileMoveDir += cameraRight * mobileMoveVector.x;
                player_object.velocity = glm::vec3(mobileMoveDir.x, player_object.velocity.y, mobileMoveDir.z);
            }
        }

        while (frame_accumulation >= tick_time)
        {
            if (!debugMode)
                processInput(window);
            if (debugMode)
            {
                processDebugInput(window);
            }

            mainGame.update_level(tick_time, player_object, cameraFront, onGround, debugMode);
            if (debugMode)
            {
                player_object.visual.Move(player_object.velocity * static_cast<float>(tick_time));
                editor_level.updateObj(player_object.visual.getPos(), player_object.visual.getLastPosition(),
                                       debug_texture_offset, debug_editing_texture, debug_create_obj_called, debug_delete_obj_called, tick_time);
            }

            frame_accumulation -= tick_time;
        }
        double alpha_time = frame_accumulation / tick_time;

        glm::vec3 interpolatedCameraPos = (player_object.visual.getPos() * static_cast<float>(alpha_time)) +
                                          (player_object.visual.getLastPosition() * static_cast<float>(1.0 - alpha_time));

        glm::mat4 mCamRotation = glm::mat4(1.0);
        mCamRotation = glm::mat4_cast(cameraRotation);

        glm::mat4 mCamTranslate = glm::mat4(1.0);
        mCamTranslate = glm::translate(mCamTranslate, -interpolatedCameraPos - glm::vec3(0.0, 0.5, 0.0));

        view = mCamRotation * mCamTranslate;
        shader_main.setMat4("view", view);
        shader_main.setVec3("playerPos", interpolatedCameraPos + glm::vec3(0.0, 0.5, 0.0));
        *player_object.visual.refLastPos() = player_object.visual.getPos();

        glm::mat4 ortho_view = glm::lookAt(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0), up);
        shader_ui.setMat4("view", ortho_view);

        if (debugMode)
        {
            if (frameUpdateCount >= frameUpdateLimit)
            {
                editor_level.updateFile();
                frameUpdateCount = 0;
            }
            frameUpdateCount += 1;

            shader_main.setDouble("lightDistanceDetail", 5000.0);
            shader_main.setDouble("lightRadius", 5000.0);
            shader_main.setDouble("lightRingsCount", 5000.0);

            editor_level.drawCollider(shader_main, pixel_scale, alpha_time);
        }

        // inter-update here
        mainGame.draw_level(shader_main, shader_ui, debugMode, alpha_time);

        if (mobileMode)
        {
            mobileEyeObject.visual.draw(shader_ui, ui_pixel_scale, alpha_time);
            mobileWalkObject.visual.draw(shader_ui, ui_pixel_scale, alpha_time);
        }

        if (mainGame.current_level_path == START_LEVEL_PATH)
        {
            mobileButton.visual.draw(shader_ui, ui_pixel_scale, alpha_time);
            fullscreenButton.visual.draw(shader_ui, ui_pixel_scale, alpha_time);
            if (colliding(mobileButton.collider, mousePos) && !mousePressed)
            {
                mobileButton.visual.SetColor(1.5, 1.5, 1.5, 1.0);
            }
            else if (colliding(mobileButton.collider, mousePos) && mousePressed)
            {
                mobileButton.visual.SetColor(0.75, 0.75, 0.75, 1.0);
            }
            else
            {
                mobileButton.visual.SetColor(1.0, 1.0, 1.0, 1.0);
            }
            if (colliding(mobileButton.collider, mousePos) && mouseClicked)
            {
                mobileButton.visual.SetColor(0.5, 0.5, 0.5, 1.0);
                mobileMode = !mobileMode;
            }

            static bool isFullScreen = false;
            fullscreenButton.visual.SetColor(1.0, 1.0, 1.0, 1.0);
            if (colliding(fullscreenButton.collider, mousePos) && mouseClicked && !isFullScreen)
            {
                fullscreenButton.visual.SetColor(0.5, 0.5, 0.5, 1.0);
                int monitorSizeX;
                int monitorSizeY;
                glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &monitorSizeX, &monitorSizeY);
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, monitorSizeX, monitorSizeY, GLFW_DONT_CARE);
                isFullScreen = true;
            }
            else if (colliding(fullscreenButton.collider, mousePos) && mouseClicked && isFullScreen)
            {
                fullscreenButton.visual.SetColor(0.5, 0.5, 0.5, 1.0);
                glfwSetWindowMonitor(window, NULL, 0, 0, current_window_width, current_window_height, GLFW_DONT_CARE);
                isFullScreen = false;
            }

#ifdef __EMSCRIPTEN__
            if (isFullScreen)
            {
                std::cout << " yay\n";
            }
#endif
        }

        window_resize = false;

        glfwSwapBuffers((GLFWwindow *)window);
        glfwPollEvents();
    };

#ifdef __EMSCRIPTEN__
    void *mloop_arg = window;
    emscripten_set_main_loop_arg(main_loop, mloop_arg, 0, true);
#else
    while (!glfwWindowShouldClose(window))
    {
        main_loop(window);
    }
#endif

    mainGame.getCurrentLevel()->deleteOctree();
    editor_level.removeTemp();
    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    current_window_width = width;
    current_window_height = height;
    window_resize = true;
    window_multiple = glm::vec2(static_cast<double>(width) / static_cast<double>(window_width), (static_cast<double>(height) / static_cast<double>(window_height)));
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    mousePos = glm::vec2(xpos, current_window_height - ypos);

    if (mobileMode)
        return;

    float xoffset = xpos - 0.0;
    float yoffset = 0.0 - ypos;

    xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    if (cursorHeld)
    {
        yaw += xoffset;
        pitch += yoffset;
    }

    if (pitch > 1.5f)
        pitch = 1.5f;
    if (pitch < -1.5f)
        pitch = -1.5f;

    glm::quat qPitch = glm::angleAxis(static_cast<float>(-pitch), glm::vec3(1.0, 0.0, 0.0));
    glm::quat qYaw = glm::angleAxis(static_cast<float>(yaw), glm::vec3(0.0, 1.0, 0.0));
    glm::quat nqYaw = glm::angleAxis(static_cast<float>(-yaw), glm::vec3(0.0, 1.0, 0.0));

    cameraRotation = glm::normalize(qPitch * qYaw);

    glm::quat lookDirection = glm::angleAxis(static_cast<float>(yaw), glm::vec3(0.0, -1.0, 0.0)); // this took so long to figure out you have no idea :)
    lookDirection *= glm::angleAxis(static_cast<float>(pitch), glm::vec3(1.0, 0.0, 0.0));
    cameraFront = glm::normalize(glm::rotate(lookDirection, glm::vec3(0.0, 0.0, -1.0)));

    glm::quat swappedRot = glm::normalize(glm::quat(nqYaw));
    cameraXZFront = glm::rotate(swappedRot, glm::vec3(0.0, 0.0, -1.0));
    cameraRight = glm::cross(cameraXZFront, up);

    if (cursorHeld)
        glfwSetCursorPos(window, 0, 0);
}

void processDebugInput(GLFWwindow *window)
{
    double camera_spd = CAMERA_SPEED;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera_spd *= 2.5;
    }
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
        std::cout << "Please hold the key to save level and exit debug mode... " << debugSaveTimer << " time remaining." << std::endl;
        debugSaveTimer -= 1.0 * tick_time;
    }
    else
    {
        debugSaveTimer = debugSaveCountdown;
    }
    if (debugSaveTimer <= 0.0)
    {
        std::cout << "\tLevel saved, debug mode is now off.\n";
        editor_level.saveTempToMain();
        editor_level.removeTemp();
        debugMode = false;
    }
    glm::vec3 moveDir = glm::vec3(0.0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveDir += cameraXZFront * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveDir += -cameraXZFront * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveDir += -cameraRight * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveDir += cameraRight * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        moveDir.y = camera_spd;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        moveDir.y = -camera_spd;
    }

    static bool textureKeyMinusHeld = false, textureKeyPlusHeld = false;
    if (!textureKeyMinusHeld && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && debug_editing_texture)
    {
        textureKeyMinusHeld = true;
        --debug_texture_offset;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        textureKeyMinusHeld = false;
    }
    if (!textureKeyPlusHeld && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && debug_editing_texture)
    {
        textureKeyPlusHeld = true;
        ++debug_texture_offset;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        textureKeyPlusHeld = false;
    }
    static bool addObjButtonHeld = false;
    debug_create_obj_called = false;
    if (!addObjButtonHeld && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        debug_create_obj_called = true;
        addObjButtonHeld = true;
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
    {
        addObjButtonHeld = false;
    }
    static bool deleteObjButtonHeld = false;
    debug_delete_obj_called = false;
    if (!deleteObjButtonHeld && glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
    {
        debug_delete_obj_called = true;
        deleteObjButtonHeld = true;
    }
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_RELEASE)
    {
        deleteObjButtonHeld = false;
    }

    player_object.velocity = moveDir;

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorHeld = true;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorHeld = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        cursorHeld = false;
    }

    static bool rightMouseHeld = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        rightMouseHeld = true;
    }
    if (rightMouseHeld && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
    {
        rightMouseHeld = false;
        editor_level.selectObj(player_object.visual.getPos(), cameraFront, debug_texture_offset);
    }
}
void processInput(GLFWwindow *window)
{
    double camera_spd = CAMERA_SPEED;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera_spd *= 1.45;
    }

    glm::vec3 moveDir = glm::vec3(0.0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveDir += cameraXZFront * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveDir += -cameraXZFront * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveDir += -cameraRight * static_cast<float>(camera_spd);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveDir += cameraRight * static_cast<float>(camera_spd);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround)
    {
        player_object.velocity.y = jump_velocity;
    }
    player_object.velocity = glm::vec3(moveDir.x, player_object.velocity.y, moveDir.z);

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorHeld = true;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorHeld = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        cursorHeld = false;
    }

    mouseClicked = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        mousePressed = true;
    }
    if (mousePressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
    {
        mousePressed = false;
        mouseClicked = true;
    }

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        std::cout << "Enabling debug mode for level: " << mainGame.current_level_path << std::endl;
        editor_level = templevel(mainGame.current_level_path);
        editor_level.copyMainToTemp();
        debugMode = true;
    }
}
void window_focus_callback(GLFWwindow *window, int focused)
{
    if (focused)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorHeld = true;
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorHeld = false;
    }
}
