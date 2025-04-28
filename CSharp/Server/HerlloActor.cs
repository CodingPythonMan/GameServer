using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Akka.Actor;

namespace Server
{
    public class HelloActor : ReceiveActor
    {
        public HelloActor()
        {
            Receive<string>(name =>
            {
                Console.WriteLine($"Hello, {name}!");
            });
        }
    }
}
