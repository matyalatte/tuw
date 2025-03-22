#include "exe_container.h"
#include <cassert>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"

#include "string_utils.h"

static uint32_t ReadUint32(FILE* io) noexcept {
    unsigned char int_as_bin[4];
    if (fread(int_as_bin, 1, 4, io) != 4)
        return 0;
    return static_cast<uint32_t>(int_as_bin[0]) | static_cast<uint32_t>(int_as_bin[1] << 8) |
           static_cast<uint32_t>(int_as_bin[2] << 16) | static_cast<uint32_t>(int_as_bin[3] << 24);
}

static void WriteUint32(FILE* io, const uint32_t& num) noexcept {
    unsigned char int_as_bin[4] = {
        static_cast<unsigned char>(num & 0xFF),
        static_cast<unsigned char>((num >> 8) & 0xFF),
        static_cast<unsigned char>((num >> 16) & 0xFF),
        static_cast<unsigned char>((num >> 24) & 0xFF)
    };
    fwrite(int_as_bin, 1, 4, io);
}

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define BUF_SIZE 1024

static noex::string ReadStr(FILE* io, const uint32_t& size) noexcept {
    noex::string str(size);
    if (fread(str.data(), 1, size, io) != size)
        return "";
    return str;
}

static void WriteStr(FILE* io, const noex::string& str) noexcept {
    fwrite(str.data(), 1, str.size(), io);

    // Zero padding
    size_t padding = (8 - ftell(io) % 8) % 8;
    char padding_bytes[8] = { 0 };
    fwrite(padding_bytes, 1, padding, io);
}

static bool CopyBinary(FILE* reader, FILE* writer, uint32_t size) noexcept {
    char buff[BUF_SIZE];
    while (size > 0) {
        size_t copy_size = MIN(BUF_SIZE, size);
        size_t read_size = fread(buff, 1, copy_size, reader);
        if (read_size != copy_size)
            return false;
        fwrite(buff, 1, read_size, writer);
        size -= static_cast<uint32_t>(read_size);
    }
    return true;
}

static uint32_t Length(FILE* io) noexcept {
    uint32_t cur = ftell(io);
    fseek(io, 0, SEEK_END);
    uint32_t len = ftell(io);
    fseek(io, cur, SEEK_SET);
    return len;
}

constexpr uint32_t EXE_SIZE_MAX = 20000000;  // Allowed size of exe
constexpr uint32_t JSON_MAGIC = 0x4A534F4E; // 'J', 'S', 'O', 'N'

noex::string ExeContainer::Read(const noex::string& exe_path) noexcept {
    if (noex::get_error_no() != noex::OK) {
        // Reject the operation as the exe_path might have an unexpected value.
        return "Fatal error has occurred while editing strings or vectors.";
    }

    m_exe_path = exe_path;
    FILE* file_io = FileOpen(exe_path.c_str(), "rb");
    if (!file_io)
        return "Failed to open " + exe_path;

    // Read the last 4 bytes
    fseek(file_io, 0, SEEK_END);
    uint32_t end_off = ftell(file_io);
    fseek(file_io, -4, SEEK_CUR);
    uint32_t magic = ReadUint32(file_io);

    if (magic != JSON_MAGIC) {
        // Json data not found
        m_exe_size = end_off;
        fclose(file_io);
        return "";
    }

    // Read exe size
    fseek(file_io, -8, SEEK_CUR);
    m_exe_size = end_off + ReadUint32(file_io);
    if (EXE_SIZE_MAX <= m_exe_size || end_off < m_exe_size) {
        fclose(file_io);
        return noex::string("Unexpected exe size. (") + m_exe_size + ")";
    }
    fseek(file_io, m_exe_size, SEEK_SET);

    // Read a header for json data
    magic = ReadUint32(file_io);
    if (magic != JSON_MAGIC) {
        fclose(file_io);
        return noex::string("Invalid magic. (") + magic + ")";
    }

    uint32_t json_size = ReadUint32(file_io);
    uint32_t stored_hash = ReadUint32(file_io);
    if (JSON_SIZE_MAX <= json_size || end_off < m_exe_size + json_size + 20) {
        fclose(file_io);
        return noex::string("Unexpected json size. (") + json_size + ")";
    }

    // Read json data
    noex::string json_str = ReadStr(file_io, json_size);
    fclose(file_io);

    if (json_str.length() != json_size)
        return "Unexpected char detected.";

    if (stored_hash != Fnv1Hash32(json_str))
        return noex::string("Invalid JSON hash. (") + stored_hash + ")";

    rapidjson::ParseResult ok = m_json.Parse(json_str.c_str());
    if (!ok) {
        return noex::string("Failed to parse JSON: ") +
                rapidjson::GetParseError_En(ok.Code()) +
                " (offset: " + ok.Offset() + ")";
    }

    return "";
}

noex::string ExeContainer::Write(const noex::string& exe_path) noexcept {
    if (noex::get_error_no() != noex::OK) {
        // Reject the operation as the exe_path might have an unexpected value.
        return "Fatal error has occurred while editing strings or vectors.";
    }

    assert(!m_exe_path.empty());
    noex::string json_str;
    if (HasJson())
        json_str = json_utils::JsonToString(m_json);

    uint32_t json_size = static_cast<uint32_t>(json_str.length());
    if (JSON_SIZE_MAX <= json_size)
        return noex::string("Unexpected json size. (") + json_size + ")";

    FILE* old_io = FileOpen(m_exe_path.c_str(), "rb");
    if (!old_io)
        return "Failed to open a file. (" + m_exe_path + ")";

    FILE* new_io = FileOpen(exe_path.c_str(), "wb");
    if (!new_io) {
        fclose(old_io);
        return "Failed to open a file. (" + exe_path + ")";
    }
    m_exe_path = exe_path;

    bool ok = CopyBinary(old_io, new_io, m_exe_size);
    if (!ok) {
        fclose(old_io);
        fclose(new_io);
        return "Failed to copy the original executable (" +
                m_exe_path + ")";
    }

    uint32_t pos = ftell(old_io);
    if (pos != Length(old_io)) {
        uint32_t magic = ReadUint32(old_io);
        if (magic != JSON_MAGIC) {
            fclose(old_io);
            fclose(new_io);
            return noex::string("Invalid magic. (") + magic + ")";
        }
    }

    fclose(old_io);
    if (json_size == 0) {
        fclose(new_io);
        return "";
    }

    // Write json data
    WriteUint32(new_io, JSON_MAGIC);
    WriteUint32(new_io, json_size);
    WriteUint32(new_io, Fnv1Hash32(json_str));
    WriteStr(new_io, json_str);
    WriteUint32(new_io, m_exe_size - ftell(new_io) - 8);
    WriteUint32(new_io, JSON_MAGIC);
    fclose(new_io);
    return "";
}
