$UTF8_NO_BOM = New-Object System.Text.UTF8Encoding $False

$old_code = '#include "wx/scopedarray.h"'
$new_code = '#include "wx/scopedarray.h"
#include "wx/scopedptr.h"'
$template = Get-Content -Path "$pwd\dib.cpp" -Encoding UTF8 -Raw
$template = $template -replace , $old_code, $new_code
[System.IO.File]::WriteAllText("$pwd\dib.cpp", $template, $UTF8_NO_BOM)
