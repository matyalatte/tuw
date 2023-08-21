#include "exec.h"
#include <vector>
#include "subprocess.h"

// from wxWidgets/src/common/cmdline.cpp
static std::vector<std::string>
wxConvertStringToArgs(const std::string& cmdline)
{
    std::vector<std::string> args;

    std::string arg;
    arg.reserve(1024);

    const std::string::const_iterator end = cmdline.end();
    std::string::const_iterator p = cmdline.begin();

    for ( ;; )
    {
        // skip white space
        while ( p != end && (*p == ' ' || *p == '\t') )
            ++p;

        // anything left?
        if ( p == end )
            break;

        // parse this parameter
        bool lastBS = false,
             isInsideQuotes = false;
        char chDelim = '\0';
        for ( arg.clear(); p != end; ++p )
        {
            const char ch = *p;

        #ifdef _WIN32
            {
                if ( ch == '"' )
                {
                    if ( !lastBS )
                    {
                        isInsideQuotes = !isInsideQuotes;

                        // don't put quote in arg
                        continue;
                    }
                    //else: quote has no special meaning but the backslash
                    //      still remains -- makes no sense but this is what
                    //      Windows does
                }
                // note that backslash does *not* quote the space, only quotes do
                else if ( !isInsideQuotes && (ch == ' ' || ch == '\t') )
                {
                    ++p;    // skip this space anyhow
                    break;
                }

                lastBS = !lastBS && ch == '\\';
            }
        #else
            {
                if ( !lastBS )
                {
                    if ( isInsideQuotes )
                    {
                        if ( ch == chDelim )
                        {
                            isInsideQuotes = false;

                            continue;   // don't use the quote itself
                        }
                    }
                    else // not in quotes and not escaped
                    {
                        if ( ch == '\'' || ch == '"' )
                        {
                            isInsideQuotes = true;
                            chDelim = ch;

                            continue;   // don't use the quote itself
                        }

                        if ( ch == ' ' || ch == '\t' )
                        {
                            ++p;    // skip this space anyhow
                            break;
                        }
                    }

                    lastBS = ch == '\\';
                    if ( lastBS )
                        continue;
                }
                else // escaped by backslash, just use as is
                {
                    lastBS = false;
                }
            }
        #endif

            arg += ch;
        }

        args.push_back(arg);
    }

    return args;
}

inline bool IsReturn(const char& input) {
    return input == '\n' || input == '\r';
}

std::string GetLastLine(const std::string& input) {
    if (input.length() <= 2) return input;
    size_t position = input.length() - 3;
    while ((!IsReturn(input[position])) && position > 0) position--;
    if (IsReturn(input[position])) position += 1;
    return input.substr(position);
}

ExecuteResult Execute(const std::string& cmd)
{
    std::vector<std::string> args_str = wxConvertStringToArgs(cmd);
    std::vector<const char*> argv;
    argv.reserve(args_str.size());

    for(size_t i = 0; i < args_str.size(); ++i)
        argv.push_back(const_cast<char*>(args_str[i].c_str()));

    struct subprocess_s process;
    int options = subprocess_option_inherit_environment
                  | subprocess_option_enable_async;
    int result = subprocess_create(&argv[0], options, &process);
    if (0 != result)
        return { -1, "Failed to create a subprocess.\n"};

    const unsigned BUF_SIZE = 1024;
    char out_buf[BUF_SIZE + 1];
    char err_buf[BUF_SIZE + 1];
    std::string last_line;
    std::string err_msg;
    unsigned out_read_size;
    unsigned err_read_size;

    while (subprocess_alive(&process) || out_read_size || err_read_size) {
        out_read_size = subprocess_read_stdout(&process, out_buf, BUF_SIZE);
        out_buf[out_read_size] = 0;
        err_read_size = subprocess_read_stderr(&process, err_buf, BUF_SIZE);
        err_buf[err_read_size] = 0;
        printf(out_buf);
        last_line += out_buf;
        err_msg += err_buf;
        if (last_line.length() > 2048) {
            last_line = last_line.substr(last_line.length() - 1024, 1024);
        }
        if (err_msg.length() > 4096) {
            err_msg = err_msg.substr(err_msg.length() - 2048, 2048);
        }
    }

    int return_code;
    result = subprocess_join(&process, &return_code);
    if (0 != result)
        return { -1, "Failed to join a subprocess.\n"};

    result = subprocess_destroy(&process);
    if (0 != result)
        return { -1, "Failed to destroy a subprocess.\n"};

    last_line = GetLastLine(last_line);

    return { return_code, err_msg, last_line };

}
