#pragma once
// GetExecutablePath is from wxStandardPaths.
// This file will make us possible to disable wxStandardPaths.
// And, no need to make wxApp for the function.

#include "wx/string.h"

#ifdef _WIN32
#include "wx/msw/private.h"

namespace stdpath {
    inline wxString GetExecutablePath(const wxString& argv0) {
        return wxGetFullModuleName((HMODULE)wxGetInstance());
    }
}  // namespace stdpath

#else
#include "wx/filename.h"

namespace stdpath {
    inline wxString GetExecutablePath(const wxString& argv0) {
        if (wxIsAbsolutePath(argv0))
            return argv0;

        // Search PATH.environment variable...
        wxPathList pathlist;
        pathlist.Add(wxGetCwd());
        pathlist.AddEnvList(wxT("PATH"));
        wxString path = pathlist.FindAbsoluteValidPath(argv0);
        if ( path.empty() )
            return argv0;       // better than nothing

        return wxFileName(path).GetAbsolutePath();
    }
}  // namespace stdpath
#endif
