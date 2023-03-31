#pragma once


#include <GeoProcess/System/CoreSystem/Application.h>
#include <GeoProcess/System/CoreSystem/Logger.h>



extern GP::Application* GP::CreateApplication();

int main(int argc, char** argv)
{
    // We first initialize the logger
    // this way, we can use logging macros
    GP::Logger::Init();
    GP_WARN("Logger has been initialized.");
    GP::Application* Application = GP::CreateApplication();
    Application->Run();

    delete Application;
    return 0;
}