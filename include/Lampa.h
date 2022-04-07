//
// Created by djole on 6.4.22..
//

#ifndef RG_PROJEKAT_LAMPA_H
#define RG_PROJEKAT_LAMPA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


// Default camera values
//const float YAW         = -90.0f;
//const float PITCH       =  0.0f;
//const float SPEED       =  5.0f;
//const float SENSITIVITY =  0.1f;
//const float ZOOM        =  90.0f;

const float BRZINA =  1.0f;

enum Lamp_Movement {
    GORE,
    DOLE,
    LEVO,
    DESNO
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Lampa
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;


    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float Sensitivity;



    // constructor with vectors
    Lampa(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Sensitivity(BRZINA)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateLampaVectors();
    }
    // constructor with scalar values
    Lampa(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Sensitivity(BRZINA)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateLampaVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Lamp_Movement direction, float deltaTime, GLboolean constrainPitch = true)
    {

        if (direction == GORE)
            Pitch += Sensitivity;
        if (direction == DOLE)
            Pitch -= Sensitivity;
        if (direction == LEVO)
            Yaw -= Sensitivity;
        if (direction == DESNO)
            Yaw += Sensitivity;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateLampaVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateLampaVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif //RG_PROJEKAT_LAMPA_H
