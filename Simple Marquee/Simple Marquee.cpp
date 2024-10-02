#include <iostream>
#include <conio.h>
// #include <Windows.h>
#include <Windows.h>

// Interface for Keyboard Events
class IKeyboardEvent
{
public:
	virtual void OnKeyDown(char key) = 0;
	virtual void OnKeyUp(char key) = 0;
};

// Implementation of IKeyboardEvent
class KeyboardEventHandler : public IKeyboardEvent
{
public:
	void OnKeyDown(char key) override
	{
		std::cout << "Key Down: " << key << std::endl;
	}

	void OnKeyUp(char key) override
	{
		std::cout << "Key Up: " << key << std::endl;
	}
};

// Keyboard Polling
void PollKeyboard(IKeyboardEvent& keyboardEvent)
{
	while (true)
	{
		if (_kbhit())
		{
			char key = _getch();

			// Check if it's a key down or key up event
			if (GetAsyncKeyState(key) & 0x8000)
			{
				keyboardEvent.OnKeyDown(key);
			} else
			{
				keyboardEvent.OnKeyUp(key);
			}

			// Other program logic can continue here
		}
	}
}

int main()
{
	KeyboardEventHandler keyboardHandler;

	// Start keyboard polling
	PollKeyboard(keyboardHandler);
	return 0;
}
