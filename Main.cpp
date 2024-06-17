#include <Siv3D.hpp>
#include "Main.h"
#include "Server.h"
#include "Client.h"

void Main()
{
	AppMode mode = unselected;

	Server serverC;
	Client clientC;

	while (System::Update())
	{
		switch (mode)
		{
		case serverMode:
			serverC.Update();
			serverC.Draw();
			break;
		case clientMode:
			clientC.Update();
			clientC.Draw();
			break;
		default:
			if (SimpleGUI::Button(U"Server", Vec2{ 10, 10 }, 100))
			{
				mode = serverMode;
				serverC.Init();
			}
			if (SimpleGUI::Button(U"Client", Vec2{ 120, 10 }, 100))
			{
				mode = clientMode;
				clientC.Init();
			}
			break;
		}
	}
}
