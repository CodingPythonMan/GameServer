@echo on
echo Starting BuildProto...
for %%f in (*.proto) do (
  echo Processing %%f
  protoc.exe --proto_path=. "%%f" --cpp_out="D:\Source\GameServer\StockGame\CPP\Game"
)
echo BuildProto completed.
pause