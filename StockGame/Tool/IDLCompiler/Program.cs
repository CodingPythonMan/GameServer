using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Collections.Generic;

class IDLCompiler
{
    // Enum용 메시지 이름 처리: "CS" 또는 "SC" 뒤에 언더바('_')가 없으면 추가합니다.
    static string ProcessEnumMessageName(string msgName)
    {
        if ((msgName.StartsWith("CS") || msgName.StartsWith("SC")) && msgName.Length > 2 && msgName[2] != '_')
        {
            return msgName.Substring(0, 2) + "_" + msgName.Substring(2);
        }
        return msgName;
    }

    static void Main(string[] args)
    {
        // 명령줄 인자 예시:
        // --proto=./Protocol.proto --out=./Output.h --prefix=CS
        string protoPath = null!;
        string headerOutputPath = null!;
        string prefix = null!;

        foreach (string arg in args)
        {
            if (arg.StartsWith("--proto="))
            {
                protoPath = arg.Substring("--proto=".Length);
            }
            else if (arg.StartsWith("--out="))
            {
                headerOutputPath = arg.Substring("--out=".Length);
            }
            else if (arg.StartsWith("--prefix="))
            {
                prefix = arg.Substring("--prefix=".Length);
            }
        }

        if (string.IsNullOrEmpty(protoPath) || string.IsNullOrEmpty(headerOutputPath) || string.IsNullOrEmpty(prefix))
        {
            Console.WriteLine("Usage: --proto=./Protocol.proto --out=./Output.h --prefix=CS");
            return;
        }

        if (!File.Exists(protoPath))
        {
            Console.WriteLine("Proto file not found: " + protoPath);
            return;
        }

        string protoContent = File.ReadAllText(protoPath);

        var matches = Regex.Matches(protoContent, @"message\s+(\w+)");
        List<string> messages = new List<string>();
        foreach (Match match in matches)
        {
            string msgName = match.Groups[1].Value;
            messages.Add(msgName);
        }

        // Header 파일 생성
        using (StreamWriter writer = new StreamWriter(headerOutputPath))
        {
            writer.WriteLine("#pragma once");
            writer.WriteLine("#include <functional>");
            writer.WriteLine("#include \"PacketSession.h\"");
            writer.WriteLine("#include \"Macro.h\"");
            writer.WriteLine("#include \"CoreGlobal.h\"");
            writer.WriteLine("#include \"SendBuffer.h\"");
            writer.WriteLine("#include \"Protocol.pb.h\"");
            writer.WriteLine();
            writer.WriteLine("using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;");
            writer.WriteLine("extern PacketHandlerFunc GPacketHandler[UINT16_MAX];");
            writer.WriteLine();
            writer.WriteLine("enum : uint16");
            writer.WriteLine("{");
            int id = 1;
            foreach (var msg in messages)
            {
                // Enum에서는 접두어 뒤에 언더바가 삽입된 이름 사용 (예: "CS_EchoReq")
                string enumName = ProcessEnumMessageName(msg);
                writer.WriteLine($"\t{enumName} = {id},");
                id++;
            }
            writer.WriteLine("};");
            writer.WriteLine();
            writer.WriteLine("// Custom Handlers");
            writer.WriteLine("bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);");
            // 핸들러 프로토타입은 원본 메시지 이름 그대로 사용 (예: "OnCSEchoReq")
            foreach (var msg in messages)
            {
                // CS 메시지에 대해서만 핸들러 함수 선언 (필요 시 조건 조정)
                if (msg.StartsWith(prefix))
                {
                    writer.WriteLine($"bool On{msg}(PacketSessionRef& session, {msg}& pkt);");
                }
            }
            writer.WriteLine();
            if (prefix == "CS")
            {
                writer.WriteLine("class ClientPacketHandler");
            }
            else
            {
                writer.WriteLine("class ServerPacketHandler");
            }
            writer.WriteLine("{");
            writer.WriteLine("public:");
            writer.WriteLine("\tstatic void Initialize()");
            writer.WriteLine("\t{");
            writer.WriteLine("\t\tfor (int32 i = 0; i < UINT16_MAX; i++)");
            writer.WriteLine("\t\t{");
            writer.WriteLine("\t\t\tGPacketHandler[i] = Handle_INVALID;");
            writer.WriteLine("\t\t}");
            // 핸들러 등록 시, enum에서 생성된 이름과 핸들러 함수 이름 사용
            foreach (var msg in messages)
            {
                if (msg.StartsWith(prefix))
                {
                    string enumName = ProcessEnumMessageName(msg);
                    writer.WriteLine($"\t\tGPacketHandler[{enumName}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {{ return HandlePacket<{msg}>(On{msg}, session, buffer, len); }};");
                }
            }
            writer.WriteLine("\t}");
            writer.WriteLine();
            writer.WriteLine("\tstatic bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)");
            writer.WriteLine("\t{");
            writer.WriteLine("\t\tPacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);");
            writer.WriteLine("\t\treturn GPacketHandler[header->id](session, buffer, len);");
            writer.WriteLine("\t}");
            // SC 메시지 관련 송신 함수 생성 (필요에 따라 조건 수정)
            foreach (var msg in messages)
            {
                // 옵션에 따라 반대 접두어 메시지에 대해서만 핸들러 함수 선언
                if ((prefix == "CS" && msg.StartsWith("SC")) || (prefix == "SC" && msg.StartsWith("CS")))
                {
                    string enumName = ProcessEnumMessageName(msg);
                    writer.WriteLine($"\tstatic SendBufferRef MakeSendBuffer({msg}& pkt) {{ return MakeSendBuffer(pkt, {enumName}); }}");
                }
            }
            writer.WriteLine();
            writer.WriteLine("private:");
            writer.WriteLine("\ttemplate<typename PacketType, typename ProcessFunc>");
            writer.WriteLine("\tstatic bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)");
            writer.WriteLine("\t{");
            writer.WriteLine("\t\tPacketType pkt;");
            writer.WriteLine("\t\tif (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)");
            writer.WriteLine("\t\t\treturn false;");
            writer.WriteLine("\t\treturn func(session, pkt);");
            writer.WriteLine("\t}");
            writer.WriteLine();
            writer.WriteLine("\ttemplate<typename T>");
            writer.WriteLine("\tstatic SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)");
            writer.WriteLine("\t{");
            writer.WriteLine("\t\tconst uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());");
            writer.WriteLine("\t\tconst uint16 packetSize = dataSize + sizeof(PacketHeader);");
            writer.WriteLine();
            writer.WriteLine("\t\tSendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);");
            writer.WriteLine("\t\tPacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());");
            writer.WriteLine("\t\theader->size = packetSize;");
            writer.WriteLine("\t\theader->id = pktId;");
            writer.WriteLine("\t\tASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));");
            writer.WriteLine("\t\tsendBuffer->Close(packetSize);");
            writer.WriteLine();
            writer.WriteLine("\t\treturn sendBuffer;");
            writer.WriteLine("\t}");
            writer.WriteLine("};");
        }

        Console.WriteLine("Header file generated at: " + headerOutputPath);
    }
}