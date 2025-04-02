@echo on

set PROTO_FILE=..\..\Common\Protobuf\bin\Protocol.proto
set GENERATED_HEADER=Protocol.pb.h

rem 생성된 파일이 없으면 무조건 실행
if not exist %GENERATED_HEADER% goto :generate

rem .proto 파일이 더 최신이면 실행
for %%F in (%PROTO_FILE%) do set PROTO_TIME=%%~tF
for %%F in (%GENERATED_HEADER%) do set HEADER_TIME=%%~tF

if "%PROTO_TIME%" GTR "%HEADER_TIME%" goto :generate

echo [INFO] Protocol.proto has not changed. Skipping generation.
goto :eof

:generate
echo [INFO] Generating protobuf...

echo Starting BuildProto...
.\..\..\Common\Protobuf\bin\protoc.exe --proto_path=.\..\..\Common\Protobuf\bin\ --cpp_out=./ Protocol.proto

XCOPY /Y Protocol.pb.h "..\Client\"
XCOPY /Y Protocol.pb.cc "..\Client\"

.\..\..\Common\Protobuf\bin\IDLCompiler.exe --proto=.\..\..\Common\Protobuf\bin\Protocol.proto --out=.\ClientPacketHandler.h --prefix=CS
.\..\..\Common\Protobuf\bin\IDLCompiler.exe --proto=.\..\..\Common\Protobuf\bin\Protocol.proto --out=..\Client\ServerPacketHandler.h --prefix=SC

echo BuildProto completed.
pause