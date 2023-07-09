#include "exe_container.h"

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

static const wxUint32 FNV_OFFSET_BASIS_32 = 2166136261U;
static const wxUint32 FNV_PRIME_32 = 16777619U;

static wxUint32 Fnv1Hash32(const std::string& str) {
    wxUint32 hash = FNV_OFFSET_BASIS_32;
    for (const char& c : str) hash = (FNV_PRIME_32 * hash) ^ c;
    return hash;
}

static const wxUint32 EXE_SIZE_MAX = 20000000;  // Allowed size of exe
static const wxUint32 JSON_SIZE_MAX = 1000000;  // Allowed size of json

bool ExeContainer::Read(const wxString& exe_path) {
    m_exe_path = exe_path;
    wxFile* file_io = new wxFile(exe_path, wxFile::read);
    if (!file_io->IsOpened()) {
        m_err_msg = "Failed to open a file. (" + exe_path + ")";
        return false;
    }
    // Read the last 4 bytes
    file_io->SeekEnd();
    wxUint32 end_off = file_io->Tell();
    file_io->Seek(-4, wxFromCurrent);
    wxString magic = ReadMagic(file_io);

    if ( magic != "JSON" ) {
        // Json data not found
        m_exe_size = end_off;
        CloseFileIO(file_io);
        return true;
    }

    // Read exe size
    file_io->Seek(-8, wxFromCurrent);
    m_exe_size = end_off + ReadUint32(file_io);
    if (EXE_SIZE_MAX <= m_exe_size || end_off < m_exe_size) {
        m_err_msg = wxString::Format("Unexpected exe size. (%d)", m_exe_size);
        return false;
    }
    file_io->Seek(m_exe_size);

    // Read a header for json data
    magic = ReadMagic(file_io);
    if (magic != "JSON") {
        m_err_msg = "Invalid magic. (" + magic + ")";
        return false;
    }
    wxUint32 json_size = ReadUint32(file_io);
    wxUint32 stored_hash = ReadUint32(file_io);
    if (JSON_SIZE_MAX <= json_size || end_off < m_exe_size + json_size + 20) {
        m_err_msg = wxString::Format("Unexpected json size. (%d)", json_size);
        return false;
    }

    // Read json data
    std::string json_str = "";
    ReadStr(file_io, json_str, json_size);
    if (json_str.length() != json_size) {
        m_err_msg = "Unexpected char detected.";
        return false;
    }
    if (stored_hash != Fnv1Hash32(json_str)) {
        m_err_msg = wxString::Format("Invalid JSON hash. (%d)", stored_hash);
        return false;
    }

    rapidjson::ParseResult ok = m_json.Parse(json_str);
    if (!ok) {
        m_err_msg = wxString::Format("Failed to parse JSON: %s (offset: %d)",
                                   wxString::FromUTF8(rapidjson::GetParseError_En(ok.Code())),
                                   ok.Offset());
        return false;
    }

    CloseFileIO(file_io);
    return true;
}

bool ExeContainer::Write(const wxString& exe_path) {
    assert(m_exe_path != "");
    std::string json_str = "";
    if (HasJson()) {
        json_str = json_utils::JsonToString(m_json);
    }
    wxUint32 json_size = json_str.length();
    if (JSON_SIZE_MAX <= json_size) {
        m_err_msg = wxString::Format("Json file is too large. (%d)", json_size);
        return false;
    }

    wxFile* old_io = new wxFile(m_exe_path, wxFile::read);
    if (!old_io->IsOpened()) {
        m_err_msg = "Failed to open a file. (" + m_exe_path + ")";
        return false;
    }
    wxFile* new_io = new wxFile(exe_path, wxFile::write);
    if (!new_io->IsOpened()) {
        m_err_msg = "Failed to open a file. (" + exe_path + ")";
        return false;
    }
    m_exe_path = exe_path;

    CopyBinary(old_io, new_io, m_exe_size);

    if (old_io->Tell() != old_io->Length()) {
        wxString magic = ReadMagic(old_io);
        if (magic != "JSON") {
            m_err_msg = "Invalid magic. (" + magic + ")";
            return false;
        }
    }

    CloseFileIO(old_io);
    if (json_size == 0) {
        CloseFileIO(new_io);
        return true;
    }

    // Write json data
    new_io->Write("JSON", 4);
    WriteUint32(new_io, json_size);
    WriteUint32(new_io, Fnv1Hash32(json_str));
    WriteStr(new_io, json_str);
    WriteUint32(new_io, m_exe_size - new_io->Tell() - 8);
    new_io->Write("JSON", 4);
    CloseFileIO(new_io);
    return true;
}
