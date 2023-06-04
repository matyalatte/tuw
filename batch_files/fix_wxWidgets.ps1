$UTF8_NO_BOM = New-Object System.Text.UTF8Encoding $False

$replacement = '#include "wx/scopedarray.h"
#include "wx/scopedptr.h"'
$template = Get-Content -Path "$pwd\dib.buckup.cpp" -Encoding UTF8 -Raw
$template = $template -replace '#include "wx/scopedarray.h"', $replacement
[System.IO.File]::WriteAllText("$pwd\dib.fixed.cpp", $template, $UTF8_NO_BOM)
