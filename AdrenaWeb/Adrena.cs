using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Web;

namespace AdrenaWeb {
    static public class ADRENA_GLOBALS {
        static public DateTime LastTime = DateTime.Now;
        static public bool IsErasingOldFiles;
    }

    public class AdrenaSystem {
        static public void AdrenaCleanIfNeeded(string OutputFolder) {
            try {
                TimeSpan Span = DateTime.Now - ADRENA_GLOBALS.LastTime;
                if(1 == Span.TotalDays) {
                    ADRENA_GLOBALS.IsErasingOldFiles = true;

                    string[] Fls = Directory.GetFiles(OutputFolder);
                    foreach(string CurrFl in Fls) {
                        File.Delete(Path.Combine(OutputFolder, CurrFl));
                    }

                    ADRENA_GLOBALS.LastTime = DateTime.Now;
                    ADRENA_GLOBALS.IsErasingOldFiles = false;
                }
            }

            catch { }
        }
    }

    /*
     *  Information about a member's account
     */
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 1)]
    [Serializable]
    public struct ADRENA_MEMBER {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 25)]
        public string Name;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 40)]
        public string Secret;
        public long BuyTimeTick;
        public uint NumberOfUploads;
        public uint IsSingleUse;
    }

    /*
     *  Utility functions for a member
     */
    public class AdrenaMemberHelper {
        static public bool AdrenaMemberGenerateSecret(out string Secret) {
            try {
                StringBuilder Str = new StringBuilder(40);
                string Chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
                var Rnd = new Random();

                for (int i = 0; i < 40; ++i) {
                    Str.Append(Chars[Rnd.Next(Chars.Length)]);
                }

                Secret = Str.ToString();

                return true;
            }

            catch {
                Secret = "";

                return false;
            }
        }

        static public bool AdrenaMemberWrite(in ADRENA_MEMBER Member, in FileStream File) {
            try {
                BinaryFormatter Bin = new BinaryFormatter();
                Bin.Serialize(File, Member);

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberRead(in FileStream File, out ADRENA_MEMBER Member) {
            try {
                BinaryFormatter Bin = new BinaryFormatter();
                Member = (ADRENA_MEMBER)(Bin.Deserialize(File));

                return true;
            }

            catch {
                Member = new ADRENA_MEMBER();

                return false;
            }
        }

        static public bool AdrenaMemberUpdate(string MemberFolder, in ADRENA_MEMBER Member) {
            try {
                StringBuilder Sb = new StringBuilder();
                Sb.AppendFormat("{0}\\{1}.ADRMEMB", MemberFolder, Member.Secret);

                FileStream Fs = File.Open(Sb.ToString(), FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);
                bool Status = AdrenaMemberWrite(Member, in Fs);

                Fs.Close();

                return Status;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberHasLicenseExpired(in ADRENA_MEMBER Member) {
            try {
                TimeSpan Span = DateTime.Now - new DateTime(Member.BuyTimeTick);
                if (Span.TotalDays >= 30)
                    return true;

                return false;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberIsLicenseSingleUse(in ADRENA_MEMBER Member) {
            try {
                if (0 != Member.IsSingleUse && 0 != Member.NumberOfUploads)
                    return true;

                return false;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberFindWithSecret(out ADRENA_MEMBER Member, string MemberFolderPath, string Secret) {
            Member = new ADRENA_MEMBER();

            try {
                string[] Members = Directory.GetFiles(MemberFolderPath);
                foreach (string CurrMemb in Members) {
                    string CurrMembPth = Path.Combine(MemberFolderPath, CurrMemb);
                    FileStream Fs = File.Open(CurrMembPth, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

                    AdrenaMemberRead(in Fs, out Member);
                    if (Member.Secret == Secret) {
                        return true;
                    }

                    Fs.Close();
                }

                return false;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberAdd(string Name, string MemberFolderPath, string Secret, bool SingleUse) {
            try {
                StringBuilder FilePth = new StringBuilder();
                FilePth.AppendFormat("{0}\\{1}.ADRMEMB", MemberFolderPath, Secret);

                FileStream Fs = File.Open(FilePth.ToString(), FileMode.Create, FileAccess.Write, FileShare.ReadWrite);

                ADRENA_MEMBER Memb = new ADRENA_MEMBER();
                Memb.Name = Name;
                Memb.Secret = Secret;
                Memb.BuyTimeTick = DateTime.Now.Ticks;
                Memb.IsSingleUse = (SingleUse) ? 1u : 0u;

                AdrenaMemberWrite(in Memb, in Fs);

                Fs.Close();

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberBan(string MemberFolderPath, string Secret) {
            try {
                StringBuilder FilePth = new StringBuilder();
                FilePth.AppendFormat("{0}\\{1}.ADRMEMB", MemberFolderPath, Secret);
                if (File.Exists(FilePth.ToString()))
                    File.Delete(FilePth.ToString());

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberGetAll(out List<ADRENA_MEMBER> Members, string MemberFolderPath) {
            Members = new List<ADRENA_MEMBER>();

            try {
                string[] MbFls = Directory.GetFiles(MemberFolderPath);
                foreach (string CurrFl in MbFls) {
                    FileStream CurrFs = File.Open(CurrFl, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                    ADRENA_MEMBER CurrMb = new ADRENA_MEMBER();

                    if (AdrenaMemberRead(in CurrFs, out CurrMb))
                        Members.Add(CurrMb);

                    CurrFs.Close();
                }

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaMemberStoreBanLog(string LogLoc, string Secret) {
            try {
                string FlPth = Path.Combine(LogLoc, "LOG.ADR");
                FileStream Fs = File.Open(FlPth, FileMode.Append, FileAccess.ReadWrite, FileShare.ReadWrite);
                StreamWriter Sw = new StreamWriter(Fs);
                Sw.WriteLine(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + ": " + Secret);
                Sw.Close();

                return true;
            }

            catch {
                return false;
            }
        }

        static public string SpecialSecret = "yxC45aJERyAePbttFiS07qiSGTeFQmun9mARLUM0";
    }

    public class AdrenaProtectHelper {
        static public bool AdrenaTemporarilySaveUploadedFile(in HttpPostedFileBase FileData, in string InputFolder, 
            out string InputFile) {
            InputFile = "";

            try {
                if(null == FileData || 1000 >= FileData.ContentLength || 5000000 <= FileData.ContentLength)
                    return false;

                string SavePth = Path.Combine(InputFolder, Path.GetFileName(FileData.FileName));
                FileData.SaveAs(SavePth);

                InputFile = SavePth;

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaEraseTemporaryUploadedFile(in string InputFile) {
            try {
                File.Delete(InputFile);

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaProtectIs64Bit(string InputFile) {
            try {
                FileStream Fs = File.Open(InputFile, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                StreamReader Sr = new StreamReader(Fs);

                char[] PEH = new char[0x100];
                Sr.Read(PEH, 0, 0x100);

                Sr.Close();

                if (0x8664 == PEH[0x84])
                    return true;

                return false;
            }

            catch {
                return true;
            }
        }

        static public bool AdrenaProtectIsNET(string InputFile) {
            try {
                System.Reflection.AssemblyName.GetAssemblyName(InputFile);

                return true;
            }

            catch (System.BadImageFormatException) {
                return false;
            }
        }

        static public bool AdrenaProtectUploadedFile(string Version, in string BinFolder, in string OutputFolder,
            in string InputFile, out string OutputFileName, out string Identification) {
            Identification = "";
            OutputFileName = "";

            try {
                string EncFile = Path.Combine(BinFolder, "_" + Version, "enc.exe");
                string HrtFile = Path.Combine(BinFolder, "_" + Version, "hrt.exe");

                if (!File.Exists(EncFile) || !File.Exists(HrtFile))
                    return false;

                OutputFileName = Path.GetRandomFileName();
                OutputFileName = OutputFileName.Remove(OutputFileName.Length - 4);

                StringBuilder OutSb = new StringBuilder();
                OutSb.AppendFormat("{0}\\{1}.exe", OutputFolder, OutputFileName);

                string Flag = "1";
                if (AdrenaProtectIs64Bit(InputFile)) {
                    Identification += "[64-BIT]";
                    Flag = "10";
                }
                else {
                    Identification += "[32-BIT]";
                }
                if(AdrenaProtectIsNET(InputFile)) {
                    Identification += "[.NET]";
                    Flag = "10";
                }
                else {
                    Identification += "[NATIVE]";
                }

                StringBuilder Sb = new StringBuilder();
                Sb.AppendFormat("{0} {1} {2} {3}", HrtFile, InputFile, OutSb.ToString(), Flag);

                ProcessStartInfo Si = new ProcessStartInfo();
                Si.FileName = EncFile;
                Si.Arguments = Sb.ToString();
                Si.WindowStyle = ProcessWindowStyle.Hidden;
                Process Proc = new Process();
                Proc.StartInfo = Si;

                Proc.Start();
                Proc.WaitForExit(10000);

                if (!File.Exists(OutSb.ToString()))
                    return false;

                return true;
            }

            catch {
                return false;
            }
        }

        static public bool AdrenaProtectFile(in HttpPostedFileBase FileData, in string Version, in string BinFolder,
            in string InputFolder, in string OutputFolder, out string ProtectedFile, out string Identification) {
            Identification = "";
            ProtectedFile = "";

            try {
                string InputFile;
                if (!AdrenaTemporarilySaveUploadedFile(FileData, InputFolder, out InputFile))
                    return false;

                if (!AdrenaProtectUploadedFile(Version, BinFolder, OutputFolder, in InputFile, out ProtectedFile,
                    out Identification)) {
                    AdrenaEraseTemporaryUploadedFile(in InputFile);

                    return false;
                }

                if (!AdrenaEraseTemporaryUploadedFile(in InputFile))
                    return false;

                StringBuilder Sb = new StringBuilder();
                Sb.AppendFormat("{0}/AdrenaOutput/{1}.exe", HttpContext.Current.Request.Url.GetLeftPart(UriPartial.Authority),
                    ProtectedFile);
                ProtectedFile = Sb.ToString();

                return true;
            }

            catch {
                return false;
            }
        }
    }
}