using System;
using System.Collections.Generic;
using System.IO;
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
        /// Usually the last exception thrown by any failed function in ManagedCXClient. (can be set manually, but DO NOT do that)
        /// </summary>
        public static Exception LastException { get; set; } = null;

        /// <summary>
        /// Get/set if the InAppUserInterface is disabled. (true means the IAUI is disabled, false means it's enabled)
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
            get
            {
                byte[] b = ReadAllBytes(EnabledModsFile);
                List<Mod> mods = new List<Mod>();
                byte[][] c = Split(b, 11); // 11 is VT in ASCII which is the separator in our custom format
                foreach (byte[] d in c)
                {
                    string s = ASCII.GetString(d, 0, d.Length - 1);
                    bool e = d[d.Length - 1] != 0;
                    Mod m = new Mod(s, e);
                    string t = ModsPath + "\\" + s;
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
                }
                return mods.ToArray();
            }
        }

        /// <summary>
        /// The paths of all the addons.
        /// </summary>
        public static string[] Addons
        {
            get
            {
                return Directory.GetFiles(AddonPath, "*.jar");
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

        static byte[][] Split(byte[] bytes, byte separator)
        {
            try
            {
                List<byte> current = new List<byte>();
                List<byte[]> split = new List<byte[]>();
                foreach (byte b in bytes)
                {
                    if (b == separator)
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

    /// <summary>
    /// One of the mods of CXClient.
    /// </summary>
    public class Mod
    {
        /// <summary>
        /// The name of this mod.
        /// </summary>
        public string name;
        /// <summary>
        /// If this mod is enabled.
        /// </summary>
        public bool enabled;
        /// <summary>
        /// The values saved in the mod's eAPI dir.
        /// </summary>
        Dictionary<string, byte[]> values;

        /// <summary>
        /// Constructs a new mod with the specified name, enabled flag and an empty value table.
        /// </summary>
        /// <param name="name">The name of the mod.</param>
        /// <param name="enabled">If the mod is enabled.</param>
        public Mod(string name, bool enabled)
        {
            this.name = name;
            this.enabled = enabled;
            values = new Dictionary<string, byte[]>();
        }

        /// <summary>
        /// Adds a read value to the mod.
        /// </summary>
        /// <param name="name">The name of the value.</param>
        /// <param name="value">The value.</param>
        /// <returns>0 if the operation succeeded, else the HRESULT.</returns>
        public int AddValue(string name, byte[] value)
        {
            try
            {
                values.Add(name, value);
                return 0;
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return e.HResult;
            }
        }

        /// <summary>
        /// Gets the raw bytes of a value.
        /// </summary>
        /// <param name="name">The name of the value.</param>
        /// <returns>The value bytes.</returns>
        public byte[] GetRawValue(string name)
        {
            return values[name];
        }

        /// <summary>
        /// Tries to get the value as a sbyte by name.
        /// Else returns LSB of HRESULT and stores the exception
        /// into <see cref="CXClient.LastException"/>
        /// </summary>
        /// <param name="name">The name of the value.</param>
        /// <returns>The read byte.</returns>
        public sbyte GetByte(string name)
        {
            try
            {
                return (sbyte)values[name][0];
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return (sbyte)e.HResult;
            }
        }

        public short GetShort(string name)
        {
            try
            {
                byte[] b = values[name];
                return (short)((b[0] << 8) | b[1]);
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return (short)e.HResult;
            }
        }

        public int GetInt(string name)
        {
            try
            {
                byte[] b = values[name];
                return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return e.HResult;
            }
        }

        public long GetLong(string name)
        {
            try
            {
                byte[] b = values[name];
                return (b[0] << 56) | (b[1] << 48) | (b[2] << 40) |
                    (b[3] << 32) | (b[4] << 24) | (b[5] << 16) |
                    (b[6] << 8) | b[7];
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return e.HResult;
            }
        }

        public Dictionary<string, byte[]>.Enumerator Values
        {
            get
            {
                return values.GetEnumerator();
            }
        }
    }
}
