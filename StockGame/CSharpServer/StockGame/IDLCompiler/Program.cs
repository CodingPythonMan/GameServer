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

    // 핸들러 함수용 메시지 이름은 그대로 사용 (즉, "CSEchoReq", "CSEnterGameReq", "CSMoveReq" 등)
    static string ProcessHandlerMessageName(string msgName)
    {
        return msgName;
    }

    static void Main(string[] args)
    {
        if (args.Length < 2)
        {
            Console.WriteLine("Usage: IDLCompiler <proto_file_path> <output_header_path>");
            return;
        }

        string protoPath = args[0];
        string headerOutputPath = args[1];

        if (!File.Exists(protoPath))
        {
            Console.WriteLine("Proto file not found: " + protoPath);
            return;
        }

        string protoContent = File.ReadAllText(protoPath);

        // .proto 파일에서 'message' 이름 추출 (PacketHeader는 제외)
        var matches = Regex.Matches(protoContent, @"message\s+(\w+)");
        List<string> messages = new List<string>();
        foreach (Match match in matches)
        {
            string msgName = match.Groups[1].Value;
            if (msgName == "PacketHeader")
                continue;
            messages.Add(msgName);
        }

        // Header 파일 생성
        using (StreamWriter writer = new StreamWriter(headerOutputPath))
        {
            writer.WriteLine("#pragma once");
            writer.WriteLine("#include <functional>");
            writer.WriteLine("#include \"GameSession.h\"");
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
                // 만약 핸들러를 등록할 메시지가 CS로 시작하는 경우에만 생성합니다.
                if (msg.StartsWith("CS"))
                {
                    string handlerName = ProcessHandlerMessageName(msg);
                    writer.WriteLine($"bool On{handlerName}(PacketSessionRef& session, {handlerName}& pkt);");
                }
            }
            writer.WriteLine();
            writer.WriteLine("class ClientPacketHandler");
            writer.WriteLine("{");
            writer.WriteLine("public:");
            writer.WriteLine("\tstatic void Initialize()");
            writer.WriteLine("\t{");
            writer.WriteLine("\t\tfor (int32 i = 0; i < UINT16_MAX; i++)");
            writer.WriteLine("\t\t{");
            writer.WriteLine("\t\t\tGPacketHandler[i] = Handle_INVALID;");
            writer.WriteLine("\t\t}");
            // 핸들러 등록 시, enum에서 생성된 이름(예: CS_EchoReq → CS_EchoReq)와
            // 핸들러 함수 이름(예: OnCSEchoReq)을 사용합니다.
            foreach (var msg in messages)
            {
                if (msg.StartsWith("CS"))
                {
                    string enumName = ProcessEnumMessageName(msg);
                    string handlerName = ProcessHandlerMessageName(msg);
                    writer.WriteLine($"\t\tGPacketHandler[{enumName}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {{ return HandlePacket<{handlerName}>(On{handlerName}, session, buffer, len); }};");
                }
            }
            writer.WriteLine("\t}");
            writer.WriteLine();
            writer.WriteLine("\tstatic bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)");
            writer.WriteLine("\t{");
            writer.WriteLine("\t\tPacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);");
            writer.WriteLine("\t\treturn GPacketHandler[header->id](session, buffer, len);");
            writer.WriteLine("\t}");
            writer.WriteLine("\tstatic SendBufferRef MakeSendBuffer(SCEchoAck& pkt) { return MakeSendBuffer(pkt, SC_EchoAck); }");
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