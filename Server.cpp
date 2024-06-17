#include "Server.h"

void Server::Init()
{
	server.startAccept(port);
	Window::SetTitle(U"[server] Waiting for connection...");

	if (System::EnumerateMicrophones().isEmpty())
	{
		throw Error{ U"No microphone is connected" };
	}

	// 音声バッファ 5 秒、バッファ終端まで到達したら最初からループ、即座に録音開始
	// デフォルトの音声入力デバイス（マイク）、推奨サンプリングレートを使用
	mic = Microphone{ 5s, Loop::Yes, StartImmediately::No };
}

void Server::Update()
{
	serverPlayerPos = Cursor::Pos();

	if (server.hasSession())
	{
		if (not connected) // クライアントが接続
		{
			connected = true;

			Window::SetTitle(U"[server] Connection established!");
			mic.start();
			if (not mic.isRecording())
			{
				throw Error{ U"Failed to start recording" };
			}
		}

		// 送信
		server.send(serverPlayerPos);

		const Wave& wave = mic.getBuffer();
		server.send(wave.lengthSample());
		for (auto buf : wave.asArray())
		{
			server.send(buf);
		}

		// 受信
		while (server.read(clientPlayerPos));
	}

	// クライアントが切断
	if (connected && (not server.hasSession()))
	{
		// 切断
		server.disconnect();

		connected = false;

		Window::SetTitle(U"[server] Waiting for connection...");

		// 新たな接続を受け付け開始
		server.startAccept(port);
	}
}

void Server::Draw()
{
	Circle{ serverPlayerPos, 30 }.draw(Palette::Skyblue);
	Circle{ clientPlayerPos, 10 }.draw(Palette::Orange);

	const Wave& wave = mic.getBuffer();
	Server::DrawWave(wave);
}

void Server::DrawWave(Wave wave)
{
	LineString points(1600, Vec2{ 0, 300 });

	// 現在のバッファ位置をもとに、直近の 1600 サンプルの波形を取得
	const size_t writePos = mic.posSample();
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
	const double mean = mic.mean();

	// 音の波形の振幅の二乗平均平方根 [0.0, 1.0]
	const double rootMeanSquare = mic.rootMeanSquare();

	// 音の波形のピーク [0.0, 1.0]
	const double peak = mic.peak();

	// 波形の描画
	points.draw(2);

	Line{ 0, (300 - mean * 280), Arg::direction(800, 0) }.draw(2, HSV{ 200 });
	Line{ 0, (300 - rootMeanSquare * 280), Arg::direction(800, 0) }.draw(2, HSV{ 120 });
	Line{ 0, (300 - peak * 280), Arg::direction(800, 0) }.draw(2, HSV{ 40 });
}
