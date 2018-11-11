using System;
using System.Collections.Generic;

namespace ManagedCXClient
{
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
        /// Else returns -1, true and stores the exception
        /// into <see cref="CXClient.LastException"/>.
        /// </summary>
        /// <param name="name">The name of the value.</param>
        /// <returns>(the read byte, false) or (-1, true)</returns>
        public sbyte? GetByte(string name)
        {
            try
            {
                return (sbyte)values[name][0];
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return null;
            }
        }

        public short? GetShort(string name)
        {
            try
            {
                byte[] b = values[name];
                return (short)((b[0] << 8) | b[1]);
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return null;
            }
        }

        public int? GetInt(string name)
        {
            try
            {
                byte[] b = values[name];
                return (b[0] << 24) | (b[1] << 16)
                    | (b[2] << 8) | b[3];
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return null;
            }
        }

        public long? GetLong(string name)
        {
            try
            {
                byte[] b = values[name];
                return (b[0] << 56) | (b[1] << 48) |
                    (b[2] << 40) | (b[3] << 32) | (b[4] << 24) |
                    (b[5] << 16) | (b[6] << 8) | b[7];
            }
            catch (Exception e)
            {
                CXClient.LastException = e;
                return null;
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
