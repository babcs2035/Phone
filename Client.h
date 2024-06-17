#pragma once

class Client
{
public:
	void Init();
	void Update();
	void Draw();

private:
	IPv4Address ip;
	uint16 port;
	bool connected = false, hasIP = false;
	TCPClient client;
	Point serverPlayerPos, clientPlayerPos;
	TextEditState ipAddrText;
	uint8 ipAddrInt[4];
	Font font;
	Wave wave;

	void DrawWave(Wave wave);
};
