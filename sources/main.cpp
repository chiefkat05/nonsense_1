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

#define MINIAUDIO_IMPLEMENTATION
#include "../deps/miniaudio.h"

#include "../headers/system.hxx"

const unsigned int window_width = 640;
const unsigned int window_height = 420;
const double tick_time = 0.01;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void window_focus_callback(GLFWwindow *window, int focused);
void processInput(GLFWwindow *window);

double delta_time = 0.0,
       lastTime = 0.0, currentTime = 0.0;
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraOffset = glm::vec3(0.0, 0.5, 0.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraXZFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraRight = glm::vec3(0.0);
glm::quat cameraRotation = glm::quat(glm::vec3(0.0));
glm::vec3 cameraVelocity = glm::vec3(0.0);
const double CAMERA_SPEED = 4.0;

double pitch = 0.0, yaw = -90.0;
double cameraFloor = 0.0;
double jump_velocity = 4.0;
bool onGround = false;

bool cursorHeld = false;

GLFWwindow *window;

std::function<void()> loop;
void main_loop(void *mLoopArg)
{
    loop();
}
ma_result result;
ma_engine engine;
ma_sound music, landsfx, stepsfx;

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
    shader shader_main("./shaders/texture.vs", "./shaders/texture.fs");
#else
    shader shader_main("./shaders/texture-emscripten.vs", "./shaders/texture-emscripten.fs");
#endif

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);
    graphics_init();

    shader_main.use();

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

    glm::mat4 proj = glm::perspective(glm::radians(65.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
    shader_main.setMat4("projection", proj);

    glfwSetCursorPosCallback(window, mouse_callback);

    allTextures.addTexture(0, "./img/brick.png");
    allTextures.addTexture(1, "./img/cargo.png");
    allTextures.addTexture(2, "./img/dirt.png");
    allTextures.addTexture(3, "./img/sign.png");
    aabb plcol = makeAABB(glm::vec3(0.0), glm::vec3(0.5, 1.75, 0.5));

    bool testbool = false;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    {
        return -1;
    }

    result = ma_sound_init_from_file(&engine, "./snd/weary.mp3", 0, NULL, NULL, &music);
    if (result != MA_SUCCESS)
    {
        return -1;
    }
    result = ma_sound_init_from_file(&engine, "./snd/land.wav", 0, NULL, NULL, &landsfx);
    if (result != MA_SUCCESS)
    {
        return -1;
    }
    result = ma_sound_init_from_file(&engine, "./snd/step.wav", 0, NULL, NULL, &stepsfx);
    if (result != MA_SUCCESS)
    {
        return -1;
    }
    ma_sound_start(&music);

    mainGame.setup_level("./levels/spawn.l");
    mainGame.goto_level(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);

    double frame_accumulation = 0.0;
    glm::vec3 prevCameraPos = cameraPos;

    loop = [&]
    {
        if (ma_sound_at_end(&music))
        {
            ma_sound_seek_to_pcm_frame(&music, 0);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.4, 0.6, 0.9, 1.0);

        frame_accumulation += delta_time;

        lastTime = currentTime;
        currentTime = glfwGetTime();
        delta_time = currentTime - lastTime;

        while (frame_accumulation >= tick_time)
        {
            prevCameraPos = cameraPos;

            processInput(window);

            mainGame.update(tick_time, cameraPos, cameraVelocity, plcol, onGround); // player movement happens here (no moving objects yet)
            frame_accumulation -= tick_time;
        }
        double alpha_time = frame_accumulation / tick_time;

        glm::vec3 interpolatedCameraPos = (cameraPos * static_cast<float>(alpha_time)) + (prevCameraPos * static_cast<float>(1.0 - alpha_time));
        cameraOffset = interpolatedCameraPos + glm::vec3(0.0, 0.5, 0.0);

        // cameraDirection = glm::slerp(cameraDirection, glm::quat(glm::vec3(newDir)), alpha_time);
        // cameraFront = cameraDirection;
        // view = glm::lookAt(cameraOffset, cameraOffset + cameraFront, up);
        glm::mat4 mCamRotation = glm::mat4(1.0);
        mCamRotation = glm::mat4_cast(cameraRotation);

        glm::mat4 mCamTranslate = glm::mat4(1.0);
        mCamTranslate = glm::translate(mCamTranslate, -cameraPos); // not right I think it needs to be cameraFront?

        view = mCamRotation * mCamTranslate;
        shader_main.setMat4("view", view);

        // inter-update here
        mainGame.draw(shader_main, alpha_time);

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

    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    float xoffset = xpos - 0.0;
    float yoffset = 0.0 - ypos;

    const float sensitivity = 0.001f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (cursorHeld)
    {
        yaw += xoffset;
        pitch += yoffset;
    }

    if (pitch > 1.5f)
        pitch = 1.5f;
    if (pitch < -1.5f)
        pitch = -1.5f;

    // cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // cameraDirection.y = sin(glm::radians(pitch));
    // cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    glm::quat qPitch = glm::angleAxis(static_cast<float>(-pitch), glm::vec3(1.0, 0.0, 0.0)); // now the movement needs to be effected by the
    glm::quat qYaw = glm::angleAxis(static_cast<float>(yaw), glm::vec3(0.0, 1.0, 0.0));

    cameraRotation = qPitch * qYaw;
    cameraRotation = glm::normalize(cameraRotation);
    // cameraFront = glm::normalize(cameraDirection);
    // cameraRight = glm::normalize(glm::cross(cameraFront, up));

    // cameraXZFront = glm::vec3(0.0); // problem: the commented-out code in the input function works, but it has the issue where you slow down when looking up. Please figure out and fix!
    // cameraXZFront.x = std::cos(cameraRotation.w);
    // cameraXZFront.z = std::sin(cameraRotation.x + cameraRotation.y + cameraRotation.z);
    // cameraDirection = glm::dvec3(0.0);
    // cameraDirection.x = std::cos(glm::radians(yaw));
    // cameraDirection.z = std::sin(glm::radians(yaw));
    // cameraXZFront = glm::normalize(cameraDirection);

    glfwSetCursorPos(window, 0, 0);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwFocusWindow(window);
        cursorHeld = true;
    }
}

// double stepTimer = 5.0;
void processInput(GLFWwindow *window)
{
    // if (stepTimer < 0.0 && onGround)
    // {
    //     stepTimer = 5.0;
    //     ma_sound_start(&stepsfx);
    // }

    glm::quat swappedRot = glm::quat(cameraRotation); // I think this could be done better but that's all I got the patience for this time
    swappedRot.w *= -1;
    swappedRot.x = 0.0;
    swappedRot.z = 0.0;
    swappedRot = glm::normalize(swappedRot);
    cameraXZFront = glm::rotate(swappedRot, cameraFront);
    cameraRight = glm::cross(cameraXZFront, up);

    glm::vec3 moveDir = glm::vec3(0.0);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        moveDir += cameraXZFront * static_cast<float>(CAMERA_SPEED);
        // stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        moveDir += -cameraXZFront * static_cast<float>(CAMERA_SPEED);
        // stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        moveDir += -cameraRight * static_cast<float>(CAMERA_SPEED);
        // stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        moveDir += cameraRight * static_cast<float>(CAMERA_SPEED);
        // stepTimer -= 15.0 * delta_time;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround)
    {
        cameraVelocity.y = jump_velocity;
    }
    cameraVelocity = glm::vec3(moveDir.x, cameraVelocity.y, moveDir.z);

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
