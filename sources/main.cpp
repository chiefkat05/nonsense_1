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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);

double delta_time = 0.0,
       lastTime = 0.0, currentTime = 0.0;
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraXZFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraRight = glm::vec3(0.0);
glm::vec3 cameraDirection = glm::vec3(0.0);
glm::vec3 cameraVelocity = glm::vec3(0.0);
const double CAMERA_SPEED = 400.0;

double pitch = 0.0, yaw = -90.0;
double cameraFloor = 0.0;
double jump_velocity = 4.0;
bool onGround = false;

double mouseX, mouseY;

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
    graphics_init();

    shader_main.use();

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

    glm::mat4 proj = glm::perspective(glm::radians(65.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);
    shader_main.setMat4("projection", proj);

    glfwSetCursorPosCallback(window, mouse_callback);

    // cube c1, c2, c3, c4, floorcube;

    texture t1, t2, t3, t4;
    t1.Set(0, "./img/brick.png");
    t2.Set(1, "./img/cargo.png");
    t3.Set(2, "./img/dirt.png");
    t4.Set(3, "./img/sign.png");
    // c1.Image(0);
    // c2.Image(1);
    // c3.Image(2);
    // c4.Image(1);
    // floorcube.Image(0);
    // floorcube.Put(0.0, -1.0, 0.0);
    // floorcube.Scale(40.0, 1.0, 40.0);
    // std::vector<cube *> worldcubes;
    // worldcubes.push_back(&floorcube);
    // worldcubes.push_back(&c1);
    // worldcubes.push_back(&c2);
    // worldcubes.push_back(&c3);
    // worldcubes.push_back(&c4);

    // aabb floorcubecol = makeAABB(floorcube.getPos(), floorcube.getScale());
    // aabb c1col = makeAABB(c1.getPos(), c1.getScale());
    // aabb c2col = makeAABB(c2.getPos(), c2.getScale());
    // aabb c3col = makeAABB(c3.getPos(), c3.getScale());
    // aabb c4col = makeAABB(c4.getPos(), c4.getScale());
    aabb plcol = makeAABB(glm::vec3(0.0), glm::vec3(0.5, 1.75, 0.5));

    // std::vector<aabb *> worldboxes;
    // worldboxes.push_back(&floorcubecol);
    // worldboxes.push_back(&c1col);
    // worldboxes.push_back(&c2col);
    // worldboxes.push_back(&c3col);
    // worldboxes.push_back(&c4col);

    glfwFocusWindow(window);

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

    cube c1;
    c1.Put(0.0, -1.0, 0.0);
    c1.Scale(4.0, 0.5, 4.0);
    c1.Image(2);

    loop = [&]
    {
        if (ma_sound_at_end(&music))
        {
            ma_sound_seek_to_pcm_frame(&music, 0);
        }

        lastTime = currentTime;
        currentTime = glfwGetTime();
        delta_time = currentTime - lastTime;

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.4, 0.6, 0.9, 1.0);

        mainGame.update(shader_main, delta_time, cameraPos, cameraVelocity, plcol, onGround);
        // std::cout << cameraPos.z << " vs " << c1col.pos.z << " and " << plcol.pos.z << " huh\n";

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
        shader_main.setMat4("view", view);

        // c2.Put(0.0, 0.5 + static_cast<float>(std::sin(glfwGetTime())), -4.0);

        // c3.Put(4.0, 1.0, 2.0);
        // c3.Scale(0.5, 0.5, 0.5);
        // c3.Rotate(0.0, static_cast<float>(glfwGetTime()), 0.0);

        // c4.Put(7.0 + static_cast<float>(std::sin(glfwGetTime())), 1.5 + static_cast<float>(std::sin(glfwGetTime())), 7.0);
        // c4.Scale(4.0 + static_cast<float>(std::sin(glfwGetTime()) * 2.0), 4.0 + static_cast<float>(std::sin(glfwGetTime()) * 2.0), 2.0);

        // c1.draw(shader_main);
        // c2.draw(shader_main);
        // c3.draw(shader_main, TRANSFORM_ROTATION_FIRST);
        // c4.draw(shader_main);
        // floorcube.draw(shader_main);

        glfwSwapBuffers((GLFWwindow *)window);
        glfwPollEvents();
    };

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

    const float sensitivity = 0.06f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    cameraDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection.y = sin(glm::radians(pitch));
    cameraDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraDirection);
    cameraRight = glm::normalize(glm::cross(cameraFront, up));

    cameraDirection = glm::dvec3(0.0);
    cameraDirection.x = std::cos(glm::radians(yaw));
    cameraDirection.z = std::sin(glm::radians(yaw));
    cameraXZFront = glm::normalize(cameraDirection);

    mouseX = xpos;
    mouseY = ypos;

    glfwSetCursorPos(window, 0, 0);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwFocusWindow(window);
    }
}

double stepTimer = 5.0;
void processInput(GLFWwindow *window)
{
    if (stepTimer < 0.0 && onGround)
    {
        stepTimer = 5.0;
        ma_sound_start(&stepsfx);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraVelocity += cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
        stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraVelocity -= cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
        stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraVelocity -= cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
        stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraVelocity += cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
        stepTimer -= 15.0 * delta_time;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround)
    {
        cameraVelocity.y = jump_velocity;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // if (mouseY > (window_height - (window_height * 0.25)) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    // {
    //     cameraPos -= cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
    // if (mouseY < (window_height * 0.25) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    // {
    //     cameraPos += cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
    // if (mouseX > (window_width - (window_width * 0.25)) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    // {
    //     cameraPos += cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
    // if (mouseX < (window_width * 0.25) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    // {
    //     cameraPos -= cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
}