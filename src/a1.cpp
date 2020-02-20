//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace glm;
using namespace std;

const char* getVertexShaderSource();

const char* getFragmentShaderSource();

int compileAndLinkShaders();

int createVertexArrayObject();

bool initContext();

GLFWwindow* window = NULL;

int main(int argc, char* argv[])
{
    if (!initContext()) return -1;

    // Black background
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkShaders();

    // We can set the shader once, since we have only one
    glUseProgram(shaderProgram);

    GLuint colourLocation = glGetUniformLocation(shaderProgram, "objectColour");

    // Camera parameters for view transform
    vec3 cameraPosition(0.6f, 1.0f, 20.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;
    bool  cameraFirstPerson = true; // press 1 or 2 to toggle this variable

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;

    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,            // field of view in degrees
        1024.0f / 768.0f,  // aspect ratio
        0.01f, 100.0f);   // near and far (near > 0)

    GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);



    // Define and upload geometry to the GPU here ...
    int vao = createVertexArrayObject();

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Other OpenGL states to set once before the Game Loop
    // Enable Backface culling
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST); // @TODO 1

    // Entering Game Loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        // Each frame, reset color of each pixel to glClearColor

        // Add the GL_DEPTH_BUFFER_BIT to glClear – TODO 1
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Draw geometry
        glBindVertexArray(vao);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo);

        GLuint worldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

        // Draw left leg
        mat4 legWorldMatrix = translate(mat4(1.0f), vec3(-1.0f, 0.5f, 0.0f)) * scale(mat4(1.0f), vec3(1.5f, 1.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &legWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw right leg
        legWorldMatrix = translate(mat4(1.0f), vec3(1.0f, 0.5f, 0.0f)) * scale(mat4(1.0f), vec3(1.5f, 1.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &legWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw lower body
        mat4 lowerBodyWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 4.0f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 6.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &lowerBodyWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw upper body
        mat4 upperBodyWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 7.87f, 0.0f)) * scale(mat4(1.0f), vec3(4.0f, 1.75f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &upperBodyWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw head
        mat4 headWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 9.62f, 0.0f)) * scale(mat4(1.0f), vec3(3.0f, 1.75f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &headWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw left eye
        mat4 eyeWorldMatrix = translate(mat4(1.0f), vec3(-0.7f, 9.85f, 1.0f)) * scale(mat4(1.0f), vec3(0.35f, 0.35f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &eyeWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw right eye
        eyeWorldMatrix = translate(mat4(1.0f), vec3(0.7f, 9.85f, 1.0f)) * scale(mat4(1.0f), vec3(0.35f, 0.35f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &eyeWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw nose
        mat4 noseWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 9.65f, 2.0f)) * scale(mat4(1.0f), vec3(0.35f, 0.35f, 2.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &noseWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw hair
        mat4 hairWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 11.25f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 3.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &hairWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw hair
        hairWorldMatrix = translate(mat4(1.0f), vec3(-0.4f, 11.25f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 3.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &hairWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw hair
        hairWorldMatrix = translate(mat4(1.0f), vec3(0.4f, 11.25f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 3.0f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &hairWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw mouth
        mat4 mouthWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 9.2f, 0.7f)) * scale(mat4(1.0f), vec3(0.35f, 0.35f, 0.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &mouthWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw left arm
        mat4 armWorldMatrix = translate(mat4(1.0f), vec3(-4.0f, 7.87f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 0.4f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &armWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw right arm
        armWorldMatrix = translate(mat4(1.0f), vec3(4.0f, 7.87f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 0.4f, 1.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &armWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw x-axis
        mat4 axisWorldMatrix = translate(mat4(1.0f), vec3(2.5f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.1f, 0.1f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw y-axis
        axisWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 2.5f, 0.0f)) * scale(mat4(1.0f), vec3(0.1f, 5.0f, 0.1f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw z-axis
        axisWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.5f)) * scale(mat4(1.0f), vec3(0.1f, 0.1f, 5.0f));
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisWorldMatrix[0][0]);
        glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //Draw floor
        mat4 floorWorldMatrix;

        for (float i = -50.0f; i <= 50.0f; i++) {
            floorWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, i)) * scale(mat4(1.0f), vec3(100.0f, 0.02f, 0.02f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &floorWorldMatrix[0][0]);
            glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        for (float i = -50.0f; i <= 50.0f; i++) {
            floorWorldMatrix = translate(mat4(1.0f), vec3(i, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.02f, 0.02f, 100.0f));
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &floorWorldMatrix[0][0]);
            glUniform3fv(colourLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /*for (int i = 0; i < 20; ++i)
        {
            for (int j = 0; j < 20; ++j)
            {
                pillarWorldMatrix = translate(mat4(1.0f), vec3(-100.0f + i * 10.0f, 5.0f, -100.0f + j * 10.0f)) * scale(mat4(1.0f), vec3(1.0f, 10.0f, 1.0f));
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                pillarWorldMatrix = translate(mat4(1.0f), vec3(-100.0f + i * 10.0f, 0.55f, -100.0f + j * 10.0f)) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(1.1f, 1.1f, 1.1f));
                glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &pillarWorldMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }*/
   
        // @TODO 3 - Update and draw projectiles
        // ...


        // Spinning cube at camera position
        spinningCubeAngle += 180.0f * dt;

        // @TODO 7 - Draw in view space for first person camera

        {
            // In third person view, let's draw the spinning cube in world space, like any other models
            mat4 spinningCubeWorldMatrix = translate(mat4(1.0f), cameraPosition) *
                rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) *
                scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));

            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &spinningCubeWorldMatrix[0][0]);
        }
        //glDrawArrays(GL_TRIANGLES, 0, 36);


        glBindVertexArray(0);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = true;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera down
        {
            cameraFirstPerson = false;
        }


        // This was solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


        // @TODO 4 - Calculate mouse motion dx and dy
        //         - Update camera horizontal and vertical angle


        // Please understand the code when you un-comment it!
        /*
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 60.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle   -= dy * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        if (cameraHorizontalAngle > 360)
        {
            cameraHorizontalAngle -= 360;
        }
        else if (cameraHorizontalAngle < -360)
        {
            cameraHorizontalAngle += 360;
        }

        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi)*cosf(theta), sinf(phi), -cosf(phi)*sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector);
         */

         // @TODO 5 = use camera lookat and side vectors to update positions with ASDW
         // adjust code below
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            cameraPosition.x -= currentCameraSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            cameraPosition.x += currentCameraSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
        {
            cameraPosition.y -= currentCameraSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
        {
            cameraPosition.y += currentCameraSpeed * dt;
        }

        // TODO 6
        // Set the view matrix for first and third person cameras
        // - In first person, camera lookat is set like below
        // - In third person, camera position is on a sphere looking towards center
        mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);


        // @TODO 2 - Shoot Projectiles
        //
        // shoot projectiles on mouse left click
        // To detect onPress events, we need to check the last state and the current state to detect the state change
        // Otherwise, you would shoot many projectiles on each mouse press
        // ...


    }


    // Shutdown GLFW
    glfwTerminate();

    return 0;
}

const char* getVertexShaderSource()
{
    // For now, you use a string for your shader code, in the assignment, shaders will be stored in .glsl files
    return
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;"
        "layout (location = 1) in vec3 aColor;"
        ""
        "uniform mat4 worldMatrix;"
        "uniform mat4 viewMatrix = mat4(1.0);"  // default value for view matrix (identity)
        "uniform mat4 projectionMatrix = mat4(1.0);"
        ""
        "out vec3 vertexColor;"
        "void main()"
        "{"
        "   vertexColor = aColor;"
        "   mat4 modelViewProjection = projectionMatrix * viewMatrix * worldMatrix;"
        "   gl_Position = modelViewProjection * vec4(aPos.x, aPos.y, aPos.z, 1.0);"
        "}";
}

const char* getFragmentShaderSource()
{
    return
        "#version 330 core\n"
        "uniform vec3 objectColour;"
        "in vec3 vertexColor;"
        "out vec4 FragColor;"
        "void main()"
        "{"
        "   FragColor = vec4(objectColour.r, objectColour.g, objectColour.b, 1.0f);"
        "}";
}

int compileAndLinkShaders()
{
    // compile and link shader program
    // return shader program id
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSource = getVertexShaderSource();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSource = getFragmentShaderSource();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int createVertexArrayObject()
{
    // Cube model
    vec3 vertexArray[] = {  // position,                            color
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), //left - red
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f), // far - blue
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // bottom - turquoise
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // near - green
        vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // right - purple
        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f), // top - yellow
        vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),

        vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 1.0f),
        vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 1.0f)
    };


    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(vec3),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindVertexArray(0);

    return vertexArrayObject;
}

bool initContext() {     // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif

    // Create Window and rendering context using GLFW, resolution is 800x600
    window = glfwCreateWindow(1024, 768, "COMP371 Winter 2020 Assignment 1", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // @TODO 3 - Disable mouse cursor
    // ...

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return false;
    }
    return true;
}
