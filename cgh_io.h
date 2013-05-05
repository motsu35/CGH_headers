#ifndef CGH_io
#define CGH_io

/************
Coded by Motsu35. have fun and play safe :)
************/

#ifdef RESIZEWINDOW
    #if (_WIN32_WINNT < 0x0500)     // This switch is needed to make the program compile
        #undef _WIN32_WINNT         // because GetConsoleWindow needs it. See Documentation
        #define _WIN32_WINNT 0x0500 // for GetConsoleWindow in MSDN.
    #endif
#endif


#include <iostream>
#include <windows.h>
#include <string>

#ifdef RESIZEWINDOW
    void resizeConsole(int x, int y)
    {


        HWND hwnd = GetConsoleWindow(); // Get the window handle for Console Window
        int newX = 0;                   // We will ignore this parameter by using the SWP_NOMOVE flag in SetWindowPos
        int newY = 0;                   // We will ignore this parameter by using the SWP_NOMOVE flag in SetWindowPos
        int newWidth = x;               // New Width in Pixels
        int newHeight = y;              // New Height in Pixels
        // Position the console window so that the left and right corners are not changed but the height and width are.
        SetWindowPos(hwnd, HWND_TOP, newX, newY, newWidth, newHeight, SWP_NOMOVE); // Using SWP_NOMOVE will cause newX and new Y to be ignored
    }
#endif

using namespace std;

///color values used in windows.
enum color
{
    black = 0,
    darkBlue,
    darkGreen,
    darkTeal,
    darkRed,
    darkPurple,
    darkYellow,
    white,
    gray,
    blue,
    green,
    teal,
    red,
    purple,
    yellow

};


void write(string c, int x,int y,color Foreground = white, color Background = black) /** write a character or string to the console window at an X,Y (like in garrysmod) WITH COLOR!!! **/
{
    if(int(Foreground) >= 0 && int(Foreground) <= 15) //check to see if color is in bounds, if using the above color enum it always should be.
    {
        if(int(Background) >= 0 && (Background) <= 15)
        {
            HANDLE Screen = GetStdHandle(STD_OUTPUT_HANDLE); //get the handel to the console window
            COORD position = { x, y };                       //make a COORD data type with our x,y
            SetConsoleCursorPosition( Screen, position );    //move to said position
            SetConsoleTextAttribute( Screen,Foreground + (Background*16)); // set color
            std::cout<<c;
            SetConsoleTextAttribute(Screen,0); // reset color
        }
        else
        {
            std::cerr<<"ERROR! Background must be between 0 and 15 \n";
        }
    }
    else
    {
        std::cerr<<"ERROR! Foreground must be between 0 and 15 \n";
    }
}

void write(char c, int x,int y,color Foreground = white, color Background = black) /** write a character or string to the console window at an X,Y (like in garrysmod) WITH COLOR!!! **/
{
    if(int(Foreground) >= 0 && int(Foreground) <= 15)
    {
        if(int(Background) >= 0 && (Background) <= 15)
        {
            HANDLE Screen = GetStdHandle(STD_OUTPUT_HANDLE);
            COORD position = { x, y };
            SetConsoleCursorPosition( Screen, position );
            SetConsoleTextAttribute( Screen,Foreground + (Background*16)); // set color
            std::cout<<c;
            SetConsoleTextAttribute(Screen,0); // reset color
        }
        else
        {
            std::cout<<"ERROR! Background must be between 0 and 15 \n";
        }
    }
    else
    {
        std::cout<<"ERROR! Foreground must be between 0 and 15 \n";
    }
}

void setCursorSize(int size)
{
    HANDLE hOut;
    CONSOLE_CURSOR_INFO ConCurInf;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(size <= 0)
    {
        ConCurInf.dwSize = 25;
        ConCurInf.bVisible = FALSE; //hmm, not working... for whatever reason.
    }
    else if(size > 100)
    {
        ConCurInf.dwSize = 25;
        ConCurInf.bVisible = TRUE;
        cerr<<"setCursorSize out of bounds (greater than 100)\n";
    }
    else
        ConCurInf.dwSize = size;
    ConCurInf.bVisible = TRUE;

    SetConsoleCursorInfo(hOut,&ConCurInf);

    return;
}



void setConsoleTitle(char* c)
{
    SetConsoleTitle(c); // i know this is a bit stupid, but it binds the function to this classes naming convention. lol (ocd... maybe?)
}

#endif
