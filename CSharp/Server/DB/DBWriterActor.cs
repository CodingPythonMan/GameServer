using Akka.Actor;
using Microsoft.Data.SqlClient;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class DBWriterActor : ReceiveActor
{
    private readonly IActorRef mFileWriter;
    private SqlConnection mConnection = null!;

    public DBWriterActor(IActorRef fileWriter)
    {
        mFileWriter = fileWriter;

        _ConnectDB();

        Receive<string>(message =>
        {
            _InsertLog(message);
        });
    }

    private void _ConnectDB()
    {
        try
        {
            mConnection = new SqlConnection("Server=127.0.0.1,1433;Database=Test;User Id=Robo;Password=1234;TrustServerCertificate=True;");
            mConnection.Open();
            mFileWriter.Tell("DB 연결 성공!");
        }
        catch(Exception ex) 
        {
            mFileWriter.Tell($"DB 연결 실패: {ex.Message}");
        }
    }

    private void _InsertLog(string message)
    {
        var command = new SqlCommand("INSERT INTO Logs (Message) VALUES (@message)", mConnection);
        command.Parameters.AddWithValue("@message", message);
        command.ExecuteNonQuery();
    }
}