using Akka.Actor;
using Server;

class Program
{
    static async Task Main(string[] args)
    {
        // 1. 액터 시스템 생성
        var system = ActorSystem.Create("helloWorldSystem");

        // 2. 액터 생성
        var helloActor = system.ActorOf<HelloActor>("helloActor");

        // 3. 메세지 전송
        helloActor.Tell("Akka.NET");

        await Task.Delay(1000);

        await system.Terminate();
    }
}