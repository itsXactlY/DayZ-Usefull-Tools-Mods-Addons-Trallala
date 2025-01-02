class PastebinCallback : RestCallback
{
    UtopiaAuthenticator m_Authenticator;
    string m_PastebinName;

    void PastebinCallback(UtopiaAuthenticator authenticator, string pastebinName)
    {
        m_Authenticator = authenticator;
        m_PastebinName = pastebinName;
    }

    override void OnSuccess(string data, int dataSize)
    {
        m_Authenticator.OnPastebinResponse(data, m_PastebinName);
    }

    override void OnError(int errorCode)
    {
        //UtopiaLogger.Log("PastebinCallback: Error occurred with code: " + errorCode, "AuthAPI");
        m_Authenticator.OnPastebinResponse("", m_PastebinName); 
    }
}

class DiscordCallback extends RestCallback
{
    autoptr Class disHook;

    void SetDiscordChat(Class Discordhook)
    {
        disHook = Discordhook;
    }

    override void OnSuccess(string data, int dataSize)
    {
        //UtopiaLogger.Log("DiscordCallback: Webhook sent successfully. Response: " + data, "AuthAPI");
    }

    override void OnError(int errorCode)
    {
        //UtopiaLogger.Log("DiscordCallback: Error occurred when sending webhook. Code: " + errorCode, "AuthAPI");
    }
}

[CF_RegisterModule(UtopiaAuthenticator)]
class UtopiaAuthenticator : CF_ModuleGame
{
    private string obfv_webhook;
    private ref map<string, bool> m_ModAuthStatus = new map<string, bool>();  
    private int m_TotalModsChecked = 0;
    private int m_TotalMods = 0;  

    override void OnInit()
    {
        super.OnInit();
        EnableMissionStart();
    }

    override void OnMissionStart(Class sender, CF_EventArgs args)
    {
        super.OnMissionStart(sender, args);

        string ip = GetIP();
        //UtopiaLogger.Log("OnMissionStart: Server IP is " + ip, "AuthAPI");

        m_TotalMods = 0;
        m_ModAuthStatus.Clear();

        #ifdef Utopia_KOTH
        m_TotalMods++;
        m_ModAuthStatus.Set("Utopia_KOTH", false);  // Use consistent mod names
        //UtopiaLogger.Log("OnMissionStart: Found mod Utopia_KOTH", "AuthAPI");
        #endif

        #ifdef Utopia_Airdrop
        m_TotalMods++;
        m_ModAuthStatus.Set("Utopia_Airdrop", false);
        //UtopiaLogger.Log("OnMissionStart: Found mod Utopia_Airdrop", "AuthAPI");
        #endif

        #ifdef Utopia_CapturetheFlag
        m_TotalMods++;
        m_ModAuthStatus.Set("Utopia_CapturetheFlag", false);
        //UtopiaLogger.Log("OnMissionStart: Found mod Utopia_CapturetheFlag", "AuthAPI");
        #endif

        //UtopiaLogger.Log("OnMissionStart: Total mods to be checked: " + m_TotalMods.ToString(), "AuthAPI");

        #ifdef Utopia_KOTH
        CheckPastebin("https://pastebin.com/raw/K40qtZNX", ip, "Utopia_KOTH");
        #endif

        #ifdef Utopia_Airdrop
        CheckPastebin("https://pastebin.com/raw/aQU9NY3q", ip, "Utopia_Airdrop");
        #endif

        #ifdef Utopia_CapturetheFlag
        CheckPastebin("https://pastebin.com/raw/jS7DSt6U", ip, "Utopia_CapturetheFlag");
        #endif
    }

    void CheckPastebin(string pastebinUrl, string ip, string modName)
    {
        if (!GetGame().IsClient() && GetGame().IsServer()) 
        {
            //UtopiaLogger.Log("CheckPastebin: Checking IP against Pastebin for " + modName, "AuthAPI");

            RestApi curlCore = CreateRestApi();
            RestContext curlContext = curlCore.GetRestContext(pastebinUrl);

            PastebinCallback callback = new PastebinCallback(this, modName);
            curlContext.GET(callback, "");  
        }
    }

    void OnPastebinResponse(string responseText, string modName)
    {
        bool isAuthenticated = false;  

        //UtopiaLogger.Log("OnPastebinResponse: Processing response for " + modName, "AuthAPI");

        if (responseText != "")
        {
            array<ref WhitelistEntry> allowedEntries = new array<ref WhitelistEntry>;
            JsonFileLoader<array<ref WhitelistEntry>>.JsonLoadData(responseText, allowedEntries);

            //UtopiaLogger.Log("OnPastebinResponse: Parsed Whitelist Entries for " + modName + ": " + allowedEntries.Count().ToString(), "AuthAPI");

            string ip = GetIP(); 

            foreach (WhitelistEntry entry : allowedEntries)
            {
                if (entry.ip == ip)  
                {
                    //UtopiaLogger.Log("OnPastebinResponse: Authentication successful for " + modName + " with IP: " + ip, "AuthAPI");
                    isAuthenticated = true;
                    break;
                }
            }
        }

        m_ModAuthStatus.Set(modName, isAuthenticated);
        m_TotalModsChecked++;

        if (!isAuthenticated)
        {
            //UtopiaLogger.Log("OnPastebinResponse: Authentication failed for " + modName + ". Sending rejection webhook.", "AuthAPI");
            SendServerWebhook(GetServerName(), GetIP(), m_ModAuthStatus, true);  
        }

        if (m_TotalModsChecked >= m_TotalMods)
        {
            CheckOverallAuthStatus();
        }
    }

    void CheckOverallAuthStatus()
    {
        UtopiaLogger.Log("CheckOverallAuthStatus: Verifying all mods' authentication statuses...", "AuthAPI");

        bool allPassed = true;

        foreach (string modName, bool isAuthenticated : m_ModAuthStatus)
        {
            UtopiaLogger.Log("CheckOverallAuthStatus: Mod " + modName + " - Authentication Status: " + isAuthenticated.ToString(), "AuthAPI");
            if (!isAuthenticated)  
            {
                allPassed = false;
            }
        }

        SendServerWebhook(GetServerName(), GetIP(), m_ModAuthStatus, !allPassed);

        if (!allPassed)
        {
            UtopiaLogger.Log("CheckOverallAuthStatus: At least one mod failed authentication. Crashing server.", "AuthAPI");
            CrashServer();
        }
        else
        {
            UtopiaLogger.Log("CheckOverallAuthStatus: All mods authenticated successfully.", "AuthAPI");
        }
    }

    string GetIP() 
    {
        if (!GetGame().IsClient() && GetGame().IsServer()) 
        {
            RestApi curlCore = CreateRestApi();
            RestContext curlContext = curlCore.GetRestContext("https://ipv4.icanhazip.com/");
            string response = curlContext.GET_now("");
            return response;
        }
        return "";
    }

    string GetServerName()
    {
        string name = "";
        string cfgPath;
        GetGame().CommandlineGetParam("config", cfgPath);
        string fullPathToConfig = "$CurrentDir:" + cfgPath;
        if (cfgPath == string.Empty) fullPathToConfig = "$CurrentDir:serverDZ.cfg";
        if (!FileExist(fullPathToConfig)) return name;

        FileHandle serverCfg = OpenFile(fullPathToConfig, FileMode.READ);
        if (serverCfg != 0)
        {
            array<string> cfgData = new array<string>;
            string line_content = "";
            int char_count = FGets(serverCfg, line_content);
            cfgData.Insert(line_content);
            while (char_count != -1)
            {
                cfgData.Insert(line_content);
                char_count = FGets(serverCfg, line_content);
            }
            CloseFile(serverCfg);
            foreach (string line: cfgData)
            {
                if (line.Contains("hostname"))  //skip
                {
                    name = line; //skip
                    name.Replace("hostname=\"", ""); //skip
                    name.Replace("hostname = \"", ""); //skip
                    name.Replace("hostname =\"", ""); //skip
                    name.Replace("\";", ""); //skip
                    int commentIndex = name.IndexOf("//"); //skip
                    if (commentIndex != -1) name = name.Substring(0, commentIndex); //skip
                    name.Trim(); //skip
                }
            }
        }
        return name;
    }

    void SendServerWebhook(string hostname, string ip, map<string, bool> modStatus, bool isBlacklisted = false)
    {
        string blacklisted = "";
        string mod_name = "Utopia Core";  
        string title = "Server Authentication Status";
        int color = 65280;  
        string thumbnail_url = "https://a.imgfoto.host/2023/10/07/check_mark.png";

        if (isBlacklisted)
        {
            blacklisted = "[REJECTED] : ";
            thumbnail_url = "https://a.imgfoto.host/2023/10/07/no_check_mark.png";
            color = 16711680;  
        }

        string modStatusMessage = "";
        foreach (string modName, bool isAuthenticated : modStatus)
        {
            string status = ""; 

            if (isAuthenticated)
            {
                status = "Passed";
            }
            else
            {
                status = "Failed";
            }

            modStatusMessage += "Mod: **" + modName + "** - Status: " + status + "\\n"; 
        }

        if (modStatusMessage.Length() > 2)
        {
            modStatusMessage = modStatusMessage.Substring(0, modStatusMessage.Length() - 2);
        }

        string embeds = "[{\"title\":\"" + title + "\", \"color\": " + color.ToString() + ", \"thumbnail\":{\"url\":\"" + thumbnail_url + "\"}, \"fields\":[{\"name\":\"Server Name\", \"value\":\"" + hostname + "\", \"inline\":false}, {\"name\":\"IP Address\", \"value\":\"" + ip + "\", \"inline\":false}, {\"name\":\"Mods Checked\", \"value\":\"" + modStatusMessage + "\", \"inline\":false}]}]"; //skip
        string sendData = "{\"embeds\":" + embeds + "}"; //skip

        DiscordCallback webHookCB = new DiscordCallback();
        webHookCB.SetDiscordChat(this);

        string webhookURL = "https://discord.com/api/webhooks/1291794016312229909/ZoEfRodJNyjoWVBzhpmHDM1L4QChU4WHh5w8yFT6cFOxGgy9nlPr8DJ2ePOAG_0SS0lK";//skip
        //UtopiaLogger.Log("SendServerWebhook: Using webhook URL: " + webhookURL, "AuthAPI");

        RestContext cURLCtx = GetRestApi().GetRestContext(webhookURL); //skip
        cURLCtx.SetHeader("application/json"); //skip

        //UtopiaLogger.Log("SendServerWebhook: Sending webhook...", "AuthAPI");

        cURLCtx.POST(webHookCB, "", sendData); //skip

        //UtopiaLogger.Log("SendServerWebhook: POST request sent to webhook URL.", "AuthAPI");
    }

    void CrashServer()
    {
        UtopiaLogger.Log("CrashServer: Authentication failed. Crashing the server.", "AuthAPI");
        int number = 7 % (1 - 1);  
    }
}

class WhitelistEntry
{
    string ip;
    string identifier;

    void WhitelistEntry(string ip = "", string identifier = "")
    {
        this.ip = ip;
        this.identifier = identifier;
    }
}
