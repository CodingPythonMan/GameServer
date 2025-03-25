@echo on
echo Starting BuildProto...
for %%f in (*.proto) do (
	echo Processing %%f
	protoc.exe --proto_path=. "%%f" --cpp_out="D:\Source\GameServer\StockGame\CPP\Game"
	IDLCompiler.exe Protocol.proto D:\Source\GameServer\StockGame\CPP\Game\ClientPacketHandler.h
)
echo BuildProto completed.
pause