using Akka.Actor;
using Google.Protobuf;
using Microsoft.Data.SqlClient;
using System;
using System.Collections.Generic;
using System.Data;
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

        Receive<LogPlaytime>(packet =>
        {
            try
            {
                _InsertLogPlaytime(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogCreateHero>(packet =>
        {
            try
            {
                _InsertLogCreateHero(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogDeleteHero>(packet =>
        {
            try
            {
                _InsertLogDeleteHero(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogDeserializeFail>(packet =>
        {
            try
            {
                _InsertLogDeserializeFail(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogLogin>(packet =>
        {
            try
            {
                _InsertLogLogin(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogDebug>(packet =>
        {
            try
            {
                _InsertLogDebug(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogExchangeRegister>(packet =>
        {
            try
            {
                _InsertLogExchangeRegister(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogExchangeDeregister>(packet =>
        {
            try
            {
                _InsertLogExchangeDeregister(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogExchangeReregister>(packet =>
        {
            try
            {
                _InsertLogExchangeReregister(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogExchangeBuy>(packet =>
        {
            try
            {
                _InsertLogExchangeBuy(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogExchangeCalculate>(packet =>
        {
            try
            {
                _InsertLogExchangeCalulate(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogQuestCompleteMission>(packet =>
        {
            try
            {
                _InsertLogQuestCompleteMission(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogQuestGiveUpMission>(packet =>
        {
            try
            {
                _InsertLogQuestGiveUpMission(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogMonsterSpawn>(packet =>
        {
            try
            {
                _InsertLogMonsterSpawn(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
        });

        Receive<LogMonsterKill>(packet =>
        {
            try
            {
                _InsertLogMonsterKill(packet);
            }
            catch (Exception ex)
            {
                mFileWriter.Tell($"[DB] 메시지 처리 실패: {ex.Message}");
            }
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

    private void _InsertLogPlaytime(LogPlaytime packet)
    {
        using var cmd = new SqlCommand("pLogInsertPlaytime", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@PlaytimeSeconds", packet.PlaytimeSeconds);
        cmd.Parameters.AddWithValue("@Type", packet.Type);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogCreateHero(LogCreateHero packet)
    {
        using var cmd = new SqlCommand("pLogInsertCreateHero", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@HeroName", packet.HeroName);
        cmd.Parameters.AddWithValue("@HeroTribe", packet.HeroTribe);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogDeleteHero(LogDeleteHero packet)
    {
        using var cmd = new SqlCommand("pLogInsertDeleteHero", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUID);
        cmd.Parameters.AddWithValue("@Flag", packet.Flag);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogDeserializeFail(LogDeserializeFail packet)
    {
        using var cmd = new SqlCommand("pLogInsertDeserializeFail", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@FailMessageID", packet.FailMessageID);
        cmd.Parameters.AddWithValue("@IPAddress", packet.IPAddress);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogLogin(LogLogin packet)
    {
        using var cmd = new SqlCommand("spInsertLoginLog", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@AccountUniqueID", packet.AccountUID);
        cmd.Parameters.AddWithValue("@HeroUniqueID", packet.HeroUID);
        cmd.Parameters.AddWithValue("@IPAddress", packet.IP);
        cmd.Parameters.AddWithValue("@BuildStampID", packet.BuildStamp);
        cmd.Parameters.AddWithValue("@DeviceType", packet.DeviceType);
        cmd.Parameters.AddWithValue("@Flag", packet.Flag);
        cmd.Parameters.AddWithValue("@HostID", packet.HostID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogDebug(LogDebug packet)
    {
        using var cmd = new SqlCommand("spInsertDebugLog", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUID);
        cmd.Parameters.AddWithValue("@Remark", packet.Remark);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogExchangeRegister(LogExchangeRegister packet)
    {
        using var cmd = new SqlCommand("pLogExchangeRegister", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@ExchangeUID", packet.ExchangeUniqueID);
        cmd.Parameters.AddWithValue("@ItemUID", packet.ItemUniqueID);
        cmd.Parameters.AddWithValue("@ItemTemplateID", packet.ItemTemplateID);
        cmd.Parameters.AddWithValue("@StackCount", packet.StackCount);
        cmd.Parameters.AddWithValue("@Enchant", packet.Enchant);
        cmd.Parameters.AddWithValue("@Price", packet.Price);

        // Poco 라이브러리 확인하여 업로드 필요
        //cmd.Parameters.AddWithValue("@StartTime", packet.StartTime);
        //cmd.Parameters.AddWithValue("@EndTime", packet.EndTime);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogExchangeDeregister(LogExchangeDeregister packet)
    {
        using var cmd = new SqlCommand("pLogExchangeDeregister", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ExchangeUID", packet.ExchangeUniqueID);
        cmd.Parameters.AddWithValue("@ReregisterCount", packet.ReregisterCount);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@NewItemUID", packet.NewItemUniqueID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogExchangeReregister(LogExchangeReregister packet)
    {
        using var cmd = new SqlCommand("pLogExchangeReregister", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@ExchangeJson", packet.JsonWorldExchangeUniqueID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogExchangeBuy(LogExchangeBuy packet)
    {
        using var cmd = new SqlCommand("pLogExchangeBuy", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ExchangeUID", packet.ExchangeUniqueID);
        cmd.Parameters.AddWithValue("@ReregisterCount", packet.ReregisterCount);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@NewItemUID", packet.NewItemUniqueID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogExchangeCalulate(LogExchangeCalculate packet)
    {
        using var cmd = new SqlCommand("pLogExchangeCalculate", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@ExchangeUID", packet.ExchangeUniqueID);
        cmd.Parameters.AddWithValue("@ReregisterCount", packet.ReregisterCount);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@CalculatePrice", packet.CalculatePrice);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogQuestCompleteMission(LogQuestCompleteMission packet)
    {
        using var cmd = new SqlCommand("pLogQuesCompletetMission", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@QuestID", packet.QuestID);
        cmd.Parameters.AddWithValue("@RewardGroupID", packet.RewardGroupID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogQuestGiveUpMission(LogQuestGiveUpMission packet)
    {
        using var cmd = new SqlCommand("pLogQuestGiveUpMission", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@AccountUID", packet.AccountUniqueID);
        cmd.Parameters.AddWithValue("@HeroUID", packet.HeroUniqueID);
        cmd.Parameters.AddWithValue("@QuestID", packet.QuestID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogMonsterSpawn(LogMonsterSpawn packet)
    {
        using var cmd = new SqlCommand("pLogMonsterSpawn", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@ServerUpTimeTick", packet.ServerUpTimeTick);
        cmd.Parameters.AddWithValue("@SequenceID", packet.SequenceID);
        cmd.Parameters.AddWithValue("@NPCTemplateID", packet.NPCTemplateID);
        cmd.Parameters.AddWithValue("@NPCGrade", packet.NPCGrade);
        cmd.Parameters.AddWithValue("@ZoneID", packet.ZoneID);
        cmd.Parameters.AddWithValue("@PosX", packet.PosX);
        cmd.Parameters.AddWithValue("@PosY", packet.PosY);
        cmd.Parameters.AddWithValue("@BossScheduleGroupID", packet.BossScheduleGroupID);
        cmd.ExecuteNonQuery();
    }

    private void _InsertLogMonsterKill(LogMonsterKill packet)
    {
        using var cmd = new SqlCommand("pLogMonsterKill", mConnection);
        cmd.CommandType = CommandType.StoredProcedure;
        cmd.Parameters.AddWithValue("@ServerTick", packet.ServerTick);
        cmd.Parameters.AddWithValue("@GroupSeq", packet.GroupSeq);
        cmd.Parameters.AddWithValue("@ServerID", packet.ServerID);
        cmd.Parameters.AddWithValue("@ServerUpTimeTick", packet.ServerUpTimeTick);
        cmd.Parameters.AddWithValue("@SequenceID", packet.SequenceID);
        cmd.Parameters.AddWithValue("@NPCTemplateID", packet.NPCTemplateID);
        cmd.Parameters.AddWithValue("@NPCGrade", packet.NPCGrade);
        cmd.Parameters.AddWithValue("@ZoneID", packet.ZoneID);
        cmd.Parameters.AddWithValue("@PosX", packet.PosX);
        cmd.Parameters.AddWithValue("@PosY", packet.PosY);
        cmd.Parameters.AddWithValue("@BossScheduleGroupID", packet.BossScheduleGroupID);

        // string Json 처리 참조하여 확인 필요
        //cmd.Parameters.AddWithValue("@HeroListJson", packet.HeroListJson);
        cmd.ExecuteNonQuery();
    }
}