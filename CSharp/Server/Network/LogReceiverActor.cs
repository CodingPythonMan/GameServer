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
                ConnectIP = "127.0.0.1",    // 이 서버의 IP
                ConnectPort = 35301,        // 이 서버의 포트
                TimeoutMS = 60000,          // 예시 타임아웃
                ServerType = 6,             // 예: 서버 타입 (로그 서버 등)
                ServerID = 50101                // 예: 서버 ID
            };

            // 2. Serialize to ByteArray
            var body = netConnect.ToByteArray();
            int messageId = (int)EMessageID.NetConnect;

            var stream = client.GetStream();

            byte[] full = new byte[4 + body.Length];
            Array.Copy(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(messageId)), 0, full, 0, 4);
            Array.Copy(body, 0, full, 4, body.Length);

            byte[] header = BitConverter.GetBytes(IPAddress.HostToNetworkOrder(full.Length));

            // 4. Send Header + Body
            await stream.WriteAsync(header, 0, header.Length);
            await stream.WriteAsync(full, 0, full.Length);
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

        // 여기서 Accept 를 하게 되었다면, EventHandler 를 호출한다.

        using var stream = client.GetStream();

        while (true)
        {
            try
            {
                // 1. 길이 읽기 (4바이트, big endian)
                int totalRead = 0;
                byte[] lengthBytes = new byte[4];
                while (totalRead < 4)
                {
                    int read = await stream.ReadAsync(lengthBytes, totalRead, 4 - totalRead);
                    if (read == 0)
                        throw new Exception("연결 끊김 (길이)");

                    totalRead += read;
                }

                int messageLength = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(lengthBytes, 0));
                if (messageLength <= 0 || messageLength > 10_000_000)
                    throw new Exception($"비정상적인 메시지 길이: {messageLength}");

                // 2. 전체 패킷 읽기
                byte[] packet = new byte[messageLength];
                totalRead = 0;
                while (totalRead < messageLength)
                {
                    int read = await stream.ReadAsync(packet, totalRead, messageLength - totalRead);
                    if (read == 0)
                        throw new Exception("연결 끊김 (패킷 본문)");

                    totalRead += read;
                }

                // 3. messageId 추출 (앞 4바이트), 나머지는 Protobuf 메시지
                int messageId = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(packet, 0));
                byte[] protoBody = new byte[messageLength - 4];
                Array.Copy(packet, 4, protoBody, 0, protoBody.Length);

                await _HandleMessageAsync(messageId, protoBody);
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
        int messageID = messageIdRaw & ~COMPRESSED_BIT_FLAG;

        byte[] actualBody = isCompressed ? _DecompressLZ4(body) : body;

        switch (messageID)
        {
            case (int)EMessageID.NetConnect:
                var message = NetConnect.Parser.ParseFrom(actualBody);
                Console.WriteLine($"[NetConnect] ServerID: {message.ServerID}, IP: {message.ConnectIP}");
                break;
            case (int)EInnerMessageID.LogLogin:
                var logLogin = LogLogin.Parser.ParseFrom(actualBody);
                Console.WriteLine($"[Packet] LogLogin 수신");
                mDBWriter.Tell(logLogin);
                break;
            default:
                Console.WriteLine($"[Packet] 알 수 없는 메시지 ID: {messageID}");
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