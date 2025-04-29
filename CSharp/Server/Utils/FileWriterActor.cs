using Akka.Actor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class FileWriterActor : ReceiveActor
{
    public FileWriterActor()
    {
        Receive<string>(message =>
        {
            Console.WriteLine($"[FileWriter] 파일에 저장: {message}");
        });
    }
}