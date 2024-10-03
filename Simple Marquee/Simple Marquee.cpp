#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <mutex>
#include <atomic>

class ConsoleManager
{
public:
    static void ClearConsoleBelowHeader(int headerLines)
    {
        COORD topLeft = { 0, static_cast<SHORT>(headerLines) };
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO screen;
        DWORD written;

        GetConsoleScreenBufferInfo(console, &screen);
        FillConsoleOutputCharacter(
            console, ' ', screen.dwSize.X * (screen.dwSize.Y - headerLines - 1), topLeft, &written
        );
        SetConsoleCursorPosition(console, topLeft);
    }

    static void DisplayHeader()
    {
        std::cout << "*****************************************\n";
        std::cout << "* Displaying a marquee console!         *\n";
        std::cout << "*****************************************\n";
    }
};

class InputHandler
{
public:
    InputHandler() : isRunning(true) {}

    void start()
    {
        inputThread = std::thread(&InputHandler::captureInputs, this);
    }

    void stop()
    {
        isRunning = false;
        if (inputThread.joinable())
        {
            inputThread.join();
        }
    }

    std::vector<char> getInputs()
    {
        std::lock_guard<std::mutex> lock(inputMutex);
        return inputs;
    }

private:
    std::vector<char> inputs;
    std::thread inputThread;
    std::mutex inputMutex;
    std::atomic<bool> isRunning;

    void captureInputs()
    {
        while (isRunning)
        {
            if (_kbhit())
            {
                char key = _getch();
                std::lock_guard<std::mutex> lock(inputMutex);
                inputs.push_back(key);
            }
        }
    }
};

class MarqueeDisplay
{
public:
    MarqueeDisplay(int headerLines, const std::string& message, int delay)
        : headerLines(headerLines), message(message), delay(delay), isRunning(true) {}

    void start(InputHandler& inputHandler)
    {
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(console, &csbi);
        const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

        int x = 0, y = headerLines;
        int dx = 1, dy = 1;

        while (isRunning)
        {
            ConsoleManager::ClearConsoleBelowHeader(headerLines);
            COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
            SetConsoleCursorPosition(console, pos);
            std::cout << message << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(delay));

            x += dx;
            y += dy;

            if (x <= 0 || x + message.length() >= consoleWidth)
            {
                dx = -dx;
            }
            if (y <= headerLines || y >= consoleHeight - 2)
            {
                dy = -dy;
            }

            DisplayFooter(console, consoleHeight, inputHandler);
        }
    }

    void stop()
    {
        isRunning = false;
    }

private:
    int headerLines;
    std::string message;
    int delay;
    std::atomic<bool> isRunning;

    void DisplayFooter(HANDLE console, int consoleHeight, InputHandler& inputHandler)
    {
        COORD footerPos = { 0, static_cast<SHORT>(consoleHeight - 1) };
        SetConsoleCursorPosition(console, footerPos);
        std::cout << "Enter a command for MARQUEE_CONSOLE: ";

        auto inputs = inputHandler.getInputs();
        for (const auto& input : inputs)
        {
            std::cout << input;
        }
    }
};

int main()
{
    ConsoleManager::DisplayHeader();

    std::string message = "Hello world in marquee!";
    int delay = 100;            // Delay in milliseconds
    int headerLines = 3;        // Number of lines in the header

    InputHandler inputHandler;
    inputHandler.start();

    MarqueeDisplay marquee(headerLines, message, delay);
    marquee.start(inputHandler);

    inputHandler.stop();
    return 0;
}
