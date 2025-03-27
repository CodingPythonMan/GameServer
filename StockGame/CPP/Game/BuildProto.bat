@echo on
cd .\..\..\Common\Protobuf\bin\
echo Starting BuildProto...
protoc.exe --proto_path=. Protocol.proto --cpp_out=..\..\..\CPP\Game\
IDLCompiler.exe Protocol.proto ..\..\..\CPP\Game\ClientPacketHandler.h
echo BuildProto completed.
pause