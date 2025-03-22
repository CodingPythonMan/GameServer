using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Collections.Generic;

class IDLCompiler
{
    static void Main(string[] args)
    {
        if (args.Length < 3)
        {
            Console.WriteLine("Usage: IDLCompiler <proto_file_path> <output_header_path> <output_cpp_path>");
            return;
        }

        string protoPath = args[0];
        string headerOutputPath = args[1];
        string cppOutputPath = args[2];

        if (!File.Exists(protoPath))
        {
            Console.WriteLine("Proto file not found: " + protoPath);
            return;
        }

        string protoContent = File.ReadAllText(protoPath);

        // .proto 파일에서 'message' 이름을 추출 (PacketHeader는 제외)
        var matches = Regex.Matches(protoContent, @"message\s+(\w+)");
        List<string> messages = new List<string>();
        foreach (Match match in matches)
        {
            string msgName = match.Groups[1].Value;
            if (msgName == "PacketHeader")
                continue;
            messages.Add(msgName);
        }

        // ClientPacketHandler.h 생성
        using (StreamWriter writer = new StreamWriter(headerOutputPath))
        {
            writer.WriteLine("#pragma once");
            writer.WriteLine("#include <cstdint>");
            writer.WriteLine("#include <functional>");
            writer.WriteLine("#include <unordered_map>");
            writer.WriteLine("#include \"google/protobuf/message.h\"");
            writer.WriteLine();
            writer.WriteLine("using PacketHandlerFunc = std::function<void(const google::protobuf::Message&)>;");
            writer.WriteLine();
            writer.WriteLine("class ClientPacketHandler {");
            writer.WriteLine("public:");
            writer.WriteLine("    static void Init();");
            writer.WriteLine("    static void HandlePacket(uint32_t id, const std::string& data);");
            writer.WriteLine("private:");
            writer.WriteLine("    static std::unordered_map<uint32_t, PacketHandlerFunc> handlers;");
            foreach (var msg in messages)
            {
                writer.WriteLine($"    static void Handle{msg}(const google::protobuf::Message& msg);");
            }
            writer.WriteLine("};");
        }

        // ClientPacketHandler.cpp 생성
        using (StreamWriter writer = new StreamWriter(cppOutputPath))
        {
            writer.WriteLine("#include \"ClientPacketHandler.h\"");
            writer.WriteLine("#include <iostream>");
            // 각 메시지에 해당하는 Protobuf 헤더 포함 (예: Login.pb.h, Move.pb.h 등)
            foreach (var msg in messages)
            {
                writer.WriteLine($"#include \"{msg}.pb.h\"");
            }
            writer.WriteLine();
            writer.WriteLine("std::unordered_map<uint32_t, PacketHandlerFunc> ClientPacketHandler::handlers;");
            writer.WriteLine();
            writer.WriteLine("void ClientPacketHandler::Init() {");
            uint id = 1;
            foreach (var msg in messages)
            {
                writer.WriteLine($"    handlers[{id}] = Handle{msg};");
                id++;
            }
            writer.WriteLine("}");
            writer.WriteLine();
            writer.WriteLine("void ClientPacketHandler::HandlePacket(uint32_t id, const std::string& data) {");
            writer.WriteLine("    google::protobuf::Message* message = nullptr;");
            writer.WriteLine("    switch(id) {");
            id = 1;
            foreach (var msg in messages)
            {
                writer.WriteLine($"        case {id}: {{");
                writer.WriteLine($"            {msg} packet;");
                writer.WriteLine("            if (!packet.ParseFromString(data)) {");
                writer.WriteLine($"                std::cerr << \"Failed to parse {msg} packet.\" << std::endl;");
                writer.WriteLine("                return;");
                writer.WriteLine("            }");
                writer.WriteLine($"            message = new {msg}(packet);");
                writer.WriteLine("            break;");
                writer.WriteLine("        }");
                id++;
            }
            writer.WriteLine("        default:");
            writer.WriteLine("            std::cerr << \"Unknown packet id: \" << id << std::endl;");
            writer.WriteLine("            return;");
            writer.WriteLine("    }");
            writer.WriteLine("    if (handlers.find(id) != handlers.end()) {");
            writer.WriteLine("        handlers[id](*message);");
            writer.WriteLine("    }");
            writer.WriteLine("    delete message;");
            writer.WriteLine("}");
            writer.WriteLine();
            id = 1;
            foreach (var msg in messages)
            {
                writer.WriteLine($"void ClientPacketHandler::Handle{msg}(const google::protobuf::Message& message) {{");
                writer.WriteLine($"    const {msg}& packet = static_cast<const {msg}&>(message);");
                writer.WriteLine($"    std::cout << \"[Handle{msg}] Handling {msg} packet.\" << std::endl;");
                writer.WriteLine("    // TODO: Add handling logic here");
                writer.WriteLine("}");
                writer.WriteLine();
                id++;
            }
        }

        Console.WriteLine("ClientPacketHandler.h generated at " + headerOutputPath);
        Console.WriteLine("ClientPacketHandler.cpp generated at " + cppOutputPath);
    }
}