#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>

namespace fs = std::filesystem;

int main()
{
    fs::create_directories("build");
    fs::create_directories("results");

    std::cout << "=============================\n";
    std::cout << "   RUN ALL UNIT TESTS\n";
    std::cout << "=============================\n\n";

    int failCount = 0;

    for (const auto &entry : fs::directory_iterator("unit"))
    {
        if (entry.path().extension() != ".cpp")
            continue;

        std::string file = entry.path().string();
        std::string name = entry.path().stem().string();

        std::string exePath = "build\\" + name + ".exe";
        std::string resultFile = "results\\" + name + ".txt";

        std::string compileCmd =
            "g++ \"" + file + "\" "
                              "../mid/ble/blemanager.cpp "
                              "-I ../mid/ble "
                              "-I ../mid/i2c "
                              "-I ../app/include "
                              "-I ../config "
                              "-I . "
                              "-I unit "
                              "-o \"" +
            exePath + "\"";

        std::cout << "[BUILD] " << name << "\n";

        if (system(compileCmd.c_str()) != 0)
        {
            std::cout << "[FAIL BUILD] " << name << "\n";
            failCount++;
            continue;
        }

        std::cout << "[RUN] " << name << "\n";

        std::string runCmd =
            "cmd /C \"\"" + exePath + "\" > \"" + resultFile + "\"\"";

        std::cout << runCmd << std::endl;

        int runStatus = system(runCmd.c_str());

        if (system(runCmd.c_str()) != 0)
        {
            std::cout << "[FAIL RUN] " << name << "\n";
            failCount++;
        }
        else
        {
            std::cout << "[OK] " << name << "\n";
        }
    }

    std::cout << "\n=============================\n";
    std::cout << "RESULT: " << (failCount == 0 ? "OK" : "FAIL") << "\n";
    std::cout << "=============================\n";

    return failCount;
}