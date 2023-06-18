#include "exe_container.h"

static wxFile* OpenFileIO(const wxString& path, wxFile::OpenMode mode) {
    wxFile* io = new wxFile(path, mode);
    if (!io->IsOpened()) {
        throw std::runtime_error(std::string("Failed to open a file. (") + path + ")");
    }
    return io;
}

static void CloseFileIO(wxFile* io) {
    io->Close();
    delete io;
}

static wxUint32 ReadUint32(wxFile* io) {
    unsigned char int_as_bin[4];
    io->Read(int_as_bin, 4);
    wxUint32 num = 0;
    for (size_t i = 0; i < 4; i++)
        num += int_as_bin[i] << (i * 8);
    return num;
}

static void WriteUint32(wxFile* io, const wxUint32& num) {
    unsigned char int_as_bin[4];
    for (size_t i = 0; i < 4; i++)
        int_as_bin[i] = num >> (i * 8);
    io->Write(int_as_bin, 4);
}

static void ReadStr(wxFile* io, std::string& str, const wxUint32& size) {
    // assert str == "";
    char buff[1025];
    buff[1024] = 0;
    wxUint32 pos = 0;
    while (pos < size) {
        if (size - pos < 1024) {
            io->Read(buff, size - pos);
            buff[size - pos] = 0;
            str += buff;
            break;
        }
        io->Read(buff, 1024);
        str += buff;
        pos += 1024;
    }
    if (str.length() != size)
        throw std::runtime_error("Unexpected char detected.");
}

static void WriteStr(wxFile* io, const std::string& str) {
    wxUint32 size = str.length();
    wxUint32 pos = 0;
    while (pos < size) {
        if (size - pos <= 1024) {
            io->Write(str.substr(pos, size).c_str(), size - pos);
            break;
        }
        io->Write(str.substr(pos, pos + 1024).c_str(), 1024);
        pos += 1024;
    }
    size_t padding = (8 - (io->Tell()) % 8) % 8;
    char buff[8];
    io->Write(buff, padding);
}

static void CopyBinary(wxFile* reader, wxFile* writer, const wxUint32& size) {
    wxUint32 pos = 0;
    char buff[1024];
    while (pos < size) {
        wxUint32 rest = size - pos;
        if (rest < 1024) {
            reader->Read(buff, rest);
            writer->Write(buff, rest);
            break;
        }
        reader->Read(buff, 1024);
        writer->Write(buff, 1024);
        pos += 1024;
    }
}

static wxString ReadMagic(wxFile* io) {
    char magic[5];
    magic[4] = 0;
    io->Read(magic, 4);
    return wxString(magic);
}

static void CheckMagic(wxFile* io, const wxString& true_magic) {
    wxString magic = ReadMagic(io);
    if (magic != true_magic)
        throw std::runtime_error(std::string("Invalid magic. (") + true_magic + ")");
}

static const wxUint32 FNV_OFFSET_BASIS_32 = 2166136261U;
static const wxUint32 FNV_PRIME_32 = 16777619U;

static wxUint32 Fnv1Hash32(const std::string& str) {
    wxUint32 hash = FNV_OFFSET_BASIS_32;
    for (const char& c : str) hash = (FNV_PRIME_32 * hash) ^ c;
    return hash;
}

static const wxUint32 EXE_SIZE_MAX = 20000000;  // Allowed size of exe
static const wxUint32 JSON_SIZE_MAX = 1000000;  // Allowed size of json

void ExeContainer::Read(const wxString& exe_path) {
    m_exe_path = exe_path;
    wxFile* file_io = OpenFileIO(exe_path, wxFile::read);

    // Read the last 4 bytes
    file_io->SeekEnd();
    wxUint32 end_off = file_io->Tell();
    file_io->Seek(-4, wxFromCurrent);
    wxString magic = ReadMagic(file_io);

    if ( magic != "JSON" ) {
        // Json data not found
        m_exe_size = end_off;
        CloseFileIO(file_io);
        return;
    }

    // Read exe size
    file_io->Seek(-8, wxFromCurrent);
    m_exe_size = end_off + ReadUint32(file_io);
    if (EXE_SIZE_MAX <= m_exe_size || end_off < m_exe_size) {
        wxString num;
        num << m_exe_size;
        throw std::runtime_error(std::string("Unexpected exe size. (") + num + ")");
    }
    file_io->Seek(m_exe_size);

    // Read a header for json data
    CheckMagic(file_io, "JSON");
    wxUint32 json_size = ReadUint32(file_io);
    wxUint32 stored_hash = ReadUint32(file_io);
    if (JSON_SIZE_MAX <= json_size || end_off < m_exe_size + json_size + 20) {
        wxString num;
        num << json_size;
        throw std::runtime_error(std::string("Unexpected json size. (") + num + ")");
    }

    // Read json data
    std::string json_str = "";
    ReadStr(file_io, json_str, json_size);
    if (stored_hash != Fnv1Hash32(json_str)) {
        wxString num;
        num << stored_hash;
        throw std::runtime_error(std::string("Invalid JSON hash. (") + num + ")");
    }
    m_json = nlohmann::json::parse(json_str);

    CloseFileIO(file_io);
}

void ExeContainer::Write(const wxString& exe_path) {
    assert(m_exe_path != "");
    std::string json_str = "";
    if (!m_json.empty()) json_str = m_json.dump();
    wxUint32 json_size = json_str.length();
    if (JSON_SIZE_MAX <= json_size) {
        wxString num;
        num << json_size;
        throw std::runtime_error(std::string("Json file is too large. (") + num + ")");
    }

    wxFile* old_io = OpenFileIO(m_exe_path, wxFile::read);
    wxFile* new_io = OpenFileIO(exe_path, wxFile::write);
    m_exe_path = exe_path;

    CopyBinary(old_io, new_io, m_exe_size);

    if (old_io->Tell() != old_io->Length())
        CheckMagic(old_io, "JSON");

    CloseFileIO(old_io);
    if (json_size == 0) {
        CloseFileIO(new_io);
        return;
    }

    // Write json data
    new_io->Write("JSON", 4);
    WriteUint32(new_io, json_size);
    WriteUint32(new_io, Fnv1Hash32(json_str));
    WriteStr(new_io, json_str);
    WriteUint32(new_io, m_exe_size - new_io->Tell() - 8);
    new_io->Write("JSON", 4);
    CloseFileIO(new_io);
}
