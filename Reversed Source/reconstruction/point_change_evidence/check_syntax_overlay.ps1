param([Parameter(Mandatory=$true)][string]$OverlayRoot,[Parameter(Mandatory=$true)][string]$Compiler)
# Explicit syntax-only frontend invocation; no object files or linking.
$manifestPath=Join-Path $OverlayRoot 'SYNTAX_MANIFEST.json'
$syntaxManifest=Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$candidatePath=Join-Path $OverlayRoot 'game/src/PointChange.syntax.cpp'
if((Get-FileHash -LiteralPath $candidatePath -Algorithm SHA256).Hash.ToLower() -ne $syntaxManifest.candidate_sha256){throw 'Candidate changed after snapshot; prepare a fresh overlay'}
foreach($header in $syntaxManifest.copied_complete_headers){
    $headerPath=Join-Path $OverlayRoot $header.relative
    if((Get-FileHash -LiteralPath $headerPath -Algorithm SHA256).Hash.ToLower() -ne $header.overlay_sha256){throw "Header changed after snapshot: $($header.relative)"}
}
$syntaxArgs=@($syntaxManifest.compiler_args)
Push-Location -LiteralPath $syntaxManifest.compiler_cwd
try {
    & $Compiler @syntaxArgs
    $syntaxExit=$LASTEXITCODE
    $receipt=[ordered]@{kind='syntax-only; NOT functional recovery';compiler=$Compiler;arguments=$syntaxArgs;candidate_sha256=$syntaxManifest.candidate_sha256;exit_code=$syntaxExit;functional_proof=$false}
    $receipt | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $OverlayRoot 'SYNTAX_RECEIPT.json')
    if($syntaxExit -ne 0){throw "PointChange syntax frontend failed with exit $syntaxExit"}
} finally { Pop-Location }
