using Akka.Actor;
using Google.Protobuf;
using K4os.Compression.LZ4;
using System.IO;
using System.Net;
using System.Net.Sockets;

public class LogReceiverActor : ReceiveActor
{
    private readonly IActorRef mFileWriter;
    private readonly IActorRef mDBWriter;

    private TcpListener mListener = null!;

    public LogReceiverActor(IActorRef fileWriter, IActorRef DBWriter)
    {
        mFileWriter = fileWriter;
        mDBWriter = DBWriter;

        _StartListen(IPAddress.Any, 35301);
    }

    private void _StartListen(IPAddress bindAddress, int port)
    {
        mListener = new TcpListener(bindAddress, port);
        mListener.Start();

        Console.WriteLine($"[LogReceiver] Listening on {bindAddress}:{port}");

        _ = _AcceptClientAsync(); // fire-and-forget async 처리
    }

    private async Task _AcceptClientAsync()
    {
        while (true)
        {
            var client = await mListener.AcceptTcpClientAsync();

            // 1. NetConnect 메시지 생성
            var netConnect = new NetConnect
            {
                ConnectIP = "127.0.0.1", // 이 서버의 IP
                ConnectPort = 35301,     // 이 서버의 포트
                TimeoutMS = 30000,        // 예시 타임아웃
                ServerType = 1,           // 예: 서버 타입 (로그 서버 등)
                ServerID = 1001           // 예: 서버 ID
            };

            // 2. Serialize to ByteArray
            var body = netConnect.ToByteArray();
            int messageId = (int)EMessageID.NetConnect;
            int bodyLength = body.Length;

            var stream = client.GetStream();

            // 3. Header 만들기
            byte[] header = new byte[8];
            Array.Copy(BitConverter.GetBytes(messageId), 0, header, 0, 4);
            Array.Copy(BitConverter.GetBytes(bodyLength), 0, header, 4, 4);

            // 4. Send Header + Body
            await stream.WriteAsync(header, 0, header.Length);
            await stream.WriteAsync(body, 0, body.Length);
            await stream.FlushAsync();

            Console.WriteLine("[Client] NetConnect 패킷 전송 완료");

            _ = _HandleClientAsync(client); // 클라이언트마다 처리
        }
    }

    private async Task _HandleClientAsync(TcpClient client)
    {
        var remoteEndPoint = client.Client.RemoteEndPoint as IPEndPoint;
        if (remoteEndPoint != null)
        {
            Console.WriteLine($"[Client] Start Handle {remoteEndPoint.Address}:{remoteEndPoint.Port}");
        }
        using var stream = client.GetStream();

        while (true)
        {
            try
            {
                // 1. Header 읽기 (6 bytes)
                byte[] header = new byte[8];
                int totalHeaderRead = 0;
                while (totalHeaderRead < 8)
                {
                    int read = await stream.ReadAsync(header, totalHeaderRead, 8 - totalHeaderRead);
                    if (read == 0)
                    {
                        throw new Exception("클라이언트 연결 끊김 (Header)");
                    }
                    totalHeaderRead += read;
                }

                int messageId = BitConverter.ToInt32(header, 0);
                int bodyLength = BitConverter.ToInt32(header, 4);

                // 2. Body 읽기
                byte[] body = new byte[bodyLength];
                int totalBodyRead = 0;
                while (totalBodyRead < bodyLength)
                {
                    int read = await stream.ReadAsync(body, totalBodyRead, bodyLength - totalBodyRead);
                    if (read == 0)
                    {
                        throw new Exception("클라이언트 연결 끊김 (Body)");
                    }
                    totalBodyRead += read;
                }

                // 3. 패킷 처리
                await _HandleMessageAsync(messageId, body);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[에러] 클라이언트 처리 실패: {ex.Message}");
                break;
            }
        }
    }

    private async Task _HandleMessageAsync(int messageIdRaw, byte[] body)
    {
        const int COMPRESSED_BIT_FLAG = unchecked((int)0x80000000);

        bool isCompressed = (messageIdRaw & COMPRESSED_BIT_FLAG) != 0;
        int messageId = messageIdRaw & ~COMPRESSED_BIT_FLAG;

        byte[] actualBody = isCompressed ? _DecompressLZ4(body) : body;

        switch (messageId)
        {
            case (int)EMessageID.NetConnect:
                var message = NetConnect.Parser.ParseFrom(actualBody);
                Console.WriteLine($"[NetConnect] ServerID: {message.ServerID}, IP: {message.ConnectIP}");
                break;
            case (int)EInnerMessageID.LogLogin:
                var logLogin = LogLogin.Parser.ParseFrom(actualBody);
                Console.WriteLine($"[Packet] LogLogin 수신: {logLogin.AccountUID}");
                // TODO: 여기서 DBWriterActor로 전달
                break;
            default:
                Console.WriteLine($"[Packet] 알 수 없는 메시지 ID: {messageId}");
                break;
        }

        await Task.CompletedTask; // 비동기 인터페이스 유지
    }

    private byte[] _DecompressLZ4(byte[] compressed, int maxDecompressedSize = 65536)
    {
        byte[] result = new byte[maxDecompressedSize];
        int decoded = LZ4Codec.Decode(compressed, 0, compressed.Length, result, 0, result.Length);
        return result[..decoded];
    }
}