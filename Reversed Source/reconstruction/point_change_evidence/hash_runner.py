from pathlib import Path
p=Path('work/point_change/check_syntax_overlay.ps1');s=p.read_text();needle='$syntaxArgs=@($syntaxManifest.compiler_args)';insert='''$candidatePath=Join-Path $OverlayRoot 'game/src/PointChange.syntax.cpp'
if((Get-FileHash -LiteralPath $candidatePath -Algorithm SHA256).Hash.ToLower() -ne $syntaxManifest.candidate_sha256){throw 'Candidate changed after snapshot; prepare a fresh overlay'}
foreach($header in $syntaxManifest.copied_complete_headers){
    $headerPath=Join-Path $OverlayRoot $header.relative
    if((Get-FileHash -LiteralPath $headerPath -Algorithm SHA256).Hash.ToLower() -ne $header.overlay_sha256){throw "Header changed after snapshot: $($header.relative)"}
}
'''+needle
p.write_text(s.replace(needle,insert))
