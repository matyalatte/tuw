$UTF8_NO_BOM = New-Object System.Text.UTF8Encoding $False

$old_code = '#include "wx/scopedarray.h"'
$new_code = '#include "wx/scopedarray.h"
#include "wx/scopedptr.h"'
$template = Get-Content -Path "$pwd\dib.cpp" -Encoding UTF8 -Raw
$template = $template -replace , $old_code, $new_code
[System.IO.File]::WriteAllText("$pwd\dib.cpp", $template, $UTF8_NO_BOM)

$old_code = 'USE_PROTOCOL\(wxFileProto\)'
$new_code = '#if wxUSE_PROTOCOL_FILE
USE_PROTOCOL(wxFileProto)
#endif'
$template = Get-Content -Path "$pwd\url.cpp" -Encoding UTF8 -Raw
$template = $template -replace , $old_code, $new_code
[System.IO.File]::WriteAllText("$pwd\url.cpp", $template, $UTF8_NO_BOM)
