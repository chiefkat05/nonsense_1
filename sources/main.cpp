// work through learnopengl to the end of starting stuff
// bench game
// look up the 3d collison stuff-ahh
// do concept artt
// plan game out
// make the simple walking sim (go from point to point when you get to point spawn in new world)
// make audio and music for it, add that
// upload to websats
// yay you did your first dream

#include <functional>
#include "../headers/graphics_backend.hxx"

const unsigned int window_width = 640;
const unsigned int window_height = 420;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

double delta_time = 0.0, lastTime = 0.0, currentTime = 0.0;
glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraXZFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
glm::vec3 cameraRight = glm::vec3(0.0);
glm::vec3 cameraDirection = glm::vec3(0.0);
const double CAMERA_SPEED = 4.0;

double pitch = 0.0, yaw = -90.0;
double cameraFloor = 0.0;
double jump_velocity = 4.0;
double cameraVelocityY = 0.0, cameraGravity = -9.81;
double cameraVelocityX = 0.0, cameraVelocityZ = 0.0;

double mouseX, mouseY;

GLFWwindow *window;

std::function<void()> loop;
void main_loop(void *mLoopArg)
{
    loop();
}

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

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 20.0f);
    shader_main.setMat4("projection", proj);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    cube c1, c2, c3, c4, floorcube; // define some default cubes (position 0-0-0 with size 1-1-1)

    texture t1, t2, t3;           // define some empty textures
    t1.Set(0, "./img/brick.png"); // give the textures an id and an image
    t2.Set(1, "./img/cargo.png");
    t3.Set(2, "./img/dirt.png");
    c1.Image(0); // set the cubes' texture id to the id of the textures
    c2.Image(1);
    c3.Image(2);
    c4.Image(1);
    floorcube.Image(0);

    glfwFocusWindow(window);

    loop = [&] // main loop
    {
        lastTime = currentTime; // delta_time setup (the thing that makes stuff move smoothly)
        currentTime = glfwGetTime();
        delta_time = currentTime - lastTime;

        if (cameraPos.y < -25.0)
        {
            cameraPos = glm::vec3(0.0, 25.0, -3.0);
        }
        if (cameraPos.y > cameraFloor || cameraPos.x < -20.0 || cameraPos.x > 20.0 || cameraPos.z < -20.0 || cameraPos.z > 20.0) // gravity code
        {
            cameraVelocityY += cameraGravity * delta_time;
        }
        else if (cameraPos.y < cameraFloor && cameraPos.y > cameraFloor - 5.0 &&
                 cameraPos.x >= -20.0 && cameraPos.x <= 20.0 && cameraPos.z >= -20.0 && cameraPos.z <= 20.0)
        {
            cameraPos.y = cameraFloor;
            cameraVelocityY = 0.0;
        }
        cameraPos.y += cameraVelocityY * delta_time;
        cameraPos.x += cameraVelocityX * delta_time;
        cameraPos.z += cameraVelocityZ * delta_time;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // reset the background every frame so it don't look funny
        glClearColor(0.4, 0.6, 0.9, 1.0);

        processInput(window); // movement

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
        shader_main.setMat4("view", view);

        c2.Put(0.0, 0.5 + static_cast<float>(std::sin(glfwGetTime())), -4.0); // cube 2 transformations

        c3.Put(4.0, 1.0, 2.0); // cube 3 transformation
        c3.Scale(0.5, 0.5, 0.5);
        c3.Rotate(0.0, static_cast<float>(glfwGetTime()), 0.0);

        c4.Put(7.0 + static_cast<float>(std::sin(glfwGetTime())), 1.5 + static_cast<float>(std::sin(glfwGetTime())), 7.0); // cube 4 transformation
        c4.Scale(4.0 + static_cast<float>(std::sin(glfwGetTime()) * 2.0), 4.0 + static_cast<float>(std::sin(glfwGetTime()) * 2.0), 2.0);

        floorcube.Put(0.0, -1.0, 0.0); // floor cube transformation
        floorcube.Scale(40.0, 1.0, 40.0);

        c1.draw(shader_main); // draw cubes
        c2.draw(shader_main);
        c3.draw(shader_main, TRANSFORM_ROTATION_FIRST);
        c4.draw(shader_main);
        floorcube.draw(shader_main);

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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && cameraPos.y == cameraFloor)
    {
        cameraVelocityY = jump_velocity;
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

    if (mouseY > (window_height - (window_height * 0.25)) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        cameraPos -= cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (mouseY < (window_height * 0.25) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        cameraPos += cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (mouseX > (window_width - (window_width * 0.25)) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        cameraPos += cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    }
    if (mouseX < (window_width * 0.25) && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        cameraPos -= cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    // {
    //     // cameraPos += cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    //     cameraVelocityX = 0.0;
    //     cameraVelocityZ = 0.0;
    //     std::cout << "forward, release key_callback\n";
    // }
    // if (key == GLFW_KEY_W && action == GLFW_PRESS)
    // {
    //     // cameraPos += cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    //     cameraVelocityX = cameraXZFront.x * static_cast<float>(CAMERA_SPEED);
    //     cameraVelocityZ = cameraXZFront.z * static_cast<float>(CAMERA_SPEED);
    //     std::cout << "forward, key_callback\n";
    // }
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    // {
    //     cameraPos -= cameraXZFront * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    // {
    //     cameraPos -= cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    // {
    //     cameraPos += cameraRight * static_cast<float>(CAMERA_SPEED * delta_time);
    // }
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && cameraPos.y == cameraFloor)
    // {
    //     cameraVelocityY = jump_velocity;
    // }

    // if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    // {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // }
    // if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    // {
    //     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // }

    // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    // {
    //     glfwSetWindowShouldClose(window, true);
    // }
}