using System.IO;
using System.IO.Compression;

namespace ManagedCXClient
{
    public class Addon
    {
        public string path;
        public string name;

        public Addon(string path)
        {
            this.path = path;
            name = Path.GetFileNameWithoutExtension(path);
        }

        public ZipArchive Open()
        {
            return ZipFile.Open(path, ZipArchiveMode.Read);
        }
    }
}
