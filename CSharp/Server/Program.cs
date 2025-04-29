using Akka.Actor;
using Akka.Routing;

class Program
{
    static async Task Main(string[] args)
    {
        var system = ActorSystem.Create("LogSystem");

        var fileWriter = system.ActorOf<FileWriterActor>("FileWriter");

        // 현재 DBWriter 는 각각의 DB 연결을 가지고 있으므로, DB 연결을 꼭 여러개 할 필요는 없다.
        // 나중에 수정 필요
        var dbWriter = system.ActorOf(Props.Create(() => new DBWriterActor(fileWriter)).WithRouter(new RoundRobinPool(10)), "DBWriterPool");
        
        var logReceiver = system.ActorOf(Props.Create(() => new LogReceiverActor(fileWriter, dbWriter)), "LogReceiver");

        await Task.Delay(Timeout.InfiniteTimeSpan);

        await system.Terminate();
    }
}