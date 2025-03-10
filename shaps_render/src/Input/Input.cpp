#include "Input.h"

//#include <ft2build.h>
//#include FT_FREETYPE_H
//#include FT_STROKER_H

#include <GLFW/glfw3.h>
#include <iostream>

#include "BackEnd/BackEnd.h"
namespace Input
{

/*
                   ,,                    ,,        ,,                                            ,,  ,,        ,,
                 `7MM                   *MM      `7MM                                            db *MM      `7MM
                   MM                    MM        MM                                                MM        MM
         .P"Ybmmm  MM  ,pW"Wq.   ,6"Yb.  MM,dMMb.  MM  .gP"Ya      `7M'   `MF',6"Yb.  `7Mb,od8 `7MM  MM,dMMb.  MM  .gP"Ya
        :MI  I8    MM 6W'   `Wb 8)   MM  MM    `Mb MM ,M'   Yb       VA   ,V 8)   MM    MM' "'   MM  MM    `Mb MM ,M'   Yb
         WmmmP"    MM 8M     M8  ,pm9MM  MM     M8 MM 8M""""""        VA ,V   ,pm9MM    MM       MM  MM     M8 MM 8M""""""
        8M         MM YA.   ,A9 8M   MM  MM.   ,M9 MM YM.    ,         VVV   8M   MM    MM       MM  MM.   ,M9 MM YM.    ,
         YMMMMMb .JMML.`Ybmd9'  `Moo9^Yo.P^YbmdP'.JMML.`Mbmmd'          W    `Moo9^Yo..JMML.   .JMML.P^YbmdP'.JMML.`Mbmmd'
        6'     dP
        Ybmmmd'
*/
    GLFWwindow* g_window;
    double g_mouseOffsetX = 0;
    double g_mouseOffsetY = 0;
    double g_mouseX = 0;
    double g_mouseY = 0;
    int g_scrollWheelYOffset = 0;
    //keyborad bind
    bool g_keyPressed[372];
    bool g_keyDown[372];
    bool g_keyDownLastFrame[372];
    //call back function decloration
    void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset);


    //=================================================================================================



	void Init()
	{
        double x, y;
        g_window = BackEnd::GetWindowPointer();
        glfwSetScrollCallback(g_window, scroll_callback);
        glfwGetCursorPos(g_window, &x, &y);
        //DisableCursor();
        g_mouseOffsetX = x;
        g_mouseOffsetY = y;
        g_mouseX = x;
        g_mouseY = y;
	}

    void Update()
    {


        //get key event using glfw
        for (int i = 32; i < 349; i++) {
            //xif (g_keyPressed[i]) std::cout << i << std::endl;

            // to spam the key
            // down
            if (glfwGetKey(g_window, i) == GLFW_PRESS)
                g_keyDown[i] = true;
            else
                g_keyDown[i] = false;

            // to trigger the key once 
            // press
            if (g_keyDown[i] && !g_keyDownLastFrame[i])
                g_keyPressed[i] = true;
            else
                g_keyPressed[i] = false;
            g_keyDownLastFrame[i] = g_keyDown[i];
        }
    }


    /////////////////////////////
    //                         //
    //      Keybord bind      //
    bool KeyPressed(unsigned int keycode)
    {
        return g_keyPressed[keycode];
    }

    bool KeyDown(unsigned int keycode)
    {
        return g_keyDown[keycode];
    }

    void HandleWindow()
    {
        if (Input::KeyPressed(MK_KEY_ESCAPE))
        {
            BackEnd::ForceWindowClose();
        }
    }





    /////////////////////////
    //                     //
    //      Callbacks      //
    //!because we dont want to do anything to window and xOffset we pass it as void because the 
    //!call back function need to pass this two parmater
    void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) {
        g_scrollWheelYOffset = (int)yoffset;
    }
}
