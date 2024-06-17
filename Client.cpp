#include "Client.h"

void Client::Init()
{
	hasIP = false;
	Window::SetTitle(U"[client] Waiting for input...");

	ipAddrText.clear();
	for (size_t i = 0; i < 4; ++i)
	{
		ipAddrInt[i] = 0;
	}

	font = Font{ FontMethod::MSDF, 48 };
}

void Client::Update()
{
	if (hasIP)
	{
		clientPlayerPos = Cursor::Pos();

		if (client.isConnected())
		{
			if (not connected) // 接続された
			{
				connected = true;

				Window::SetTitle(U"[client] Connection established!");
			}

			// 送信
			client.send(clientPlayerPos);

			// 受信
			while (client.read(serverPlayerPos));

			size_t sampleLength;
			client.read(sampleLength);
			Array<WaveSample>sampleArr(sampleLength);
			for (size_t i = 0; i < sampleLength; ++i)
			{
				client.read(sampleArr[i]);
			}
			wave = Wave(sampleArr, sampleLength);
		}

		if (client.hasError()) // 切断/接続エラー
		{
			client.disconnect();

			connected = false;

			Window::SetTitle(U"[client] Waiting for connection...");

			// 接続を再試行
			client.connect(ip, port);
		}
	}
	else
	{
		SimpleGUI::TextBox(ipAddrText, Vec2{ 10, 10 }, 200);
		if (SimpleGUI::Button(U"connect", Vec2{ 250, 10 }))
		{
			const auto ipAddrStrs = ipAddrText.text.split('.');
			for (size_t i = 0; i < 4; ++i)
			{
				ipAddrInt[i] = Parse<uint8>(ipAddrStrs[i]);
			}
			ip = IPv4Address(ipAddrInt);
			Window::SetTitle(U"[client] Waiting for connection...");

			// ポート番号
			port = 50000;

			// 接続を試行
			client.connect(ip, port);

			hasIP = true;
		}
	}
}

void Client::Draw()
{
	Circle{ clientPlayerPos, 30 }.draw(Palette::Skyblue);
	Circle{ serverPlayerPos, 30 }.draw(Palette::Orange);

	if (hasIP)
	{
		font(U"{}.{}.{}.{}"_fmt(ipAddrInt[0], ipAddrInt[1], ipAddrInt[2], ipAddrInt[3])).draw(28, Vec2{ 10, 10 }, Palette::White);
	}

	Client::DrawWave(wave);
}

void Client::DrawWave(Wave wave)
{
	LineString points(1600, Vec2{ 0, 300 });

	// 現在のバッファ位置をもとに、直近の 1600 サンプルの波形を取得
	const size_t writePos = wave.lengthSample();
	{
		constexpr size_t samples_show = 1600;
		const size_t headLength = Min(writePos, samples_show);
		const size_t tailLength = (samples_show - headLength);
		size_t pos = 0;

		for (size_t i = 0; i < tailLength; ++i)
		{
			const float a = wave[wave.size() - tailLength + i].left;
			points[pos].set(pos * 0.5, 300 + a * 280);
			++pos;
		}

		for (size_t i = 0; i < headLength; ++i)
		{
			const float a = wave[writePos - headLength + i].left;
			points[pos].set(pos * 0.5, 300 + a * 280);
			++pos;
		}
	}

	// 音の波形の平均振幅 [0.0, 1.0]
	//const double mean = mic.mean();

	// 音の波形の振幅の二乗平均平方根 [0.0, 1.0]
	//const double rootMeanSquare = mic.rootMeanSquare();

	// 音の波形のピーク [0.0, 1.0]
	//const double peak = mic.peak();

	// 波形の描画
	points.draw(2);

	//Line{ 0, (300 - mean * 280), Arg::direction(800, 0) }.draw(2, HSV{ 200 });
	//Line{ 0, (300 - rootMeanSquare * 280), Arg::direction(800, 0) }.draw(2, HSV{ 120 });
	//Line{ 0, (300 - peak * 280), Arg::direction(800, 0) }.draw(2, HSV{ 40 });
}
