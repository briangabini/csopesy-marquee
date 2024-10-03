#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <Windows.h>
#include <conio.h>
#include <vector>

// Function to clear the console below the header
void ClearConsoleBelowHeader(int headerLines)
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

std::vector<char> inputs{};
std::atomic<bool> isRunning{ true };

void captureInputs()
{
	while (isRunning)
    {
	    // Check for user input
        if (_kbhit())
        {
            char key = _getch();
            inputs.push_back(key);
		}

    }
}

// Function to display the diagonal bouncing marquee
void displayDiagonalMarquee(const std::string& message, int delay, int headerLines)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(console, &csbi);
    const int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    const int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    int x = 0, y = headerLines;
    int dx = 1, dy = 1;

    while (true)
    {
        ClearConsoleBelowHeader(headerLines);
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
        if (y <= headerLines || y >= consoleHeight - 2) // Leave space for footer
        {
            dy = -dy;
        }

        // Display footer
        COORD footerPos = { 0, static_cast<SHORT>(consoleHeight - 1) };
        SetConsoleCursorPosition(console, footerPos);
        std::cout << "Enter a command for MARQUEE_CONSOLE: ";

        for (const auto& input : inputs)
        {
			std::cout << input;
        }
    }
}

// Function to display the header
void displayHeader()
{
    std::cout << "*****************************************\n";
    std::cout << "* Displaying a marquee console!         *\n";
    std::cout << "*****************************************\n";
}

int main()
{
    DisplayHeader();
    std::string message = "Hello world in marquee!";
    int delay = 100;            // Delay in milliseconds
    int headerLines = 3;        // Number of lines in the header

    std::thread inputThread(captureInputs);
    inputThread.join();

    DisplayDiagonalMarquee(message, delay, headerLines);

    return 0;
}