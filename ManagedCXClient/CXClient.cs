using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using static System.IO.File;
using static System.IO.Path;
using static System.Text.Encoding;

namespace ManagedCXClient
{
    /// <summary>
    /// The class you need to do all of the interaction with the CXClient in .NET UWP.
    /// Uses Microsofts ugly naming convention. (PascalCase)
    /// </summary>
    public static class CXClient
    {
        //                               can't access the Registry in UWP
        static string MinecraftPath { get; } = "%appdata%\\.minecraft";
        static string AddonPath { get; } = "%appdata%\\.minecraft\\cxclient_addons";
        static string EapiPath { get; } = "%appdata%\\.minecraft\\cxclient_eapi";
        static string ModsPath { get; } = "%appdata%\\.minecraft\\cxclient_eapi\\mods";
        static string DisableIauiFile { get; } = "%appdata%\\.minecraft\\cxclient_eapi\\disable_iaui";
        static string EnabledModsFile { get; } = "%appdata%\\.minecraft\\cxclient_eapi\\mods\\enabled";
        static string RunningFile { get; } = "%appdata%\\.minecraft\\cxclient_eapi\\running";
        
        /// <summary>
        /// Usually the last exception thrown by any failed function in ManagedCXClient.
        /// (can be set manually, but DO NOT do that)
        /// </summary>
        public static Exception LastException { get; set; } = null;

        /// <summary>
        /// Get/set if the InAppUserInterface is disabled.
        /// (true means the IAUI is disabled, false means it's enabled)
        /// </summary>
        public static bool IauiDisabled
        {
            get
            {
                return Exists(DisableIauiFile);
            }
            set
            {
                if (value)
                    Create(DisableIauiFile);
                else if (Exists(DisableIauiFile))
                    Delete(DisableIauiFile);
            }
        }

        /// <summary>
        /// If CXClient is running.
        /// </summary>
        public static bool Running
        {
            get
            {
                return Exists(RunningFile);
            }
        }

        /// <summary>
        /// Returns all the mods with their values.
        /// </summary>
        public static Mod[] Mods
        {
            //basically a parser for the enabled format
            get
            {
                FileStream s = Open(EnabledModsFile, FileMode.Open, FileAccess.Read);
                List<Mod> mods = new List<Mod>();
                int i;
                List<byte> b = new List<byte>();
                while ((i = s.ReadByte()) != -1)
                {
                    b.Add((byte)i);
                    while ((i = s.ReadByte()) != 11)
                    {
                        b.Add((byte)i);
                    }
                    i = b.Count - 1;
                    bool enabled = b[i] != 0;
                    b.RemoveAt(i);
                    Mod m = new Mod(ASCII.GetString(b.ToArray()), enabled);
                    string t = ModsPath + "\\" + m.name;
                    if (Directory.Exists(t))
                    {
                        foreach (string u in Directory.GetFiles(t))
                        {
                            string v = GetFileName(u);
                            byte[] f = ReadAllBytes(u);
                            m.AddValue(v, f);
                        }
                    }
                    mods.Add(m);
                    b.Clear();
                }
                return mods.ToArray();
            }
        }

        /// <summary>
        /// The paths of all the addons.
        /// </summary>
        public static Addon[] Addons
        {
            get
            {
		        List<Addon> a = new List<Addon>();
		        foreach(string p in Directory.GetFiles(AddonPath, "*.jar"))
		        {
			        a.Add(new Addon(p));
		        }
		        return a.ToArray();
            }
        }

        /// <summary>
        /// Tries to add an addon to the addon path.
        /// </summary>
        /// <param name="file">The addon file.</param>
        /// <returns>The error code if one occured, else 0.</returns>
        public static int AddAddon(string file)
        {
            try
            {
                Copy(file, Combine(AddonPath, GetFileName(file)));
                return 0;
            }
            catch (Exception e)
            {
                LastException = e;
                return e.HResult;
            }
        }

        static byte[][] Split(byte[] bytes)
        {
            try
            {
                List<byte> current = new List<byte>();
                List<byte[]> split = new List<byte[]>();
                foreach (byte b in bytes)
                {
                    if (b == 11)
                    {
                        split.Add(current.ToArray());
                        current = new List<byte>();
                    }
                    else
                        current.Add(b);
                }
                return split.ToArray();
            }
            catch (Exception e)
            {
                LastException = e;
                return null;
            }
        }
    }
}
