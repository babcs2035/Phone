#pragma once

class Server
{
public:
	void Init();
	void Update();
	void Draw();

private:
	uint16 port = 50000;
	bool connected = false;
	TCPServer server;
	Point clientPlayerPos, serverPlayerPos;
	Microphone mic;

	void DrawWave(Wave wave);
};
