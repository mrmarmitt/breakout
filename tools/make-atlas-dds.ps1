# Gera assets/textures/atlas.dds — a spritesheet unica do Breakout (task 03).
#
# Mesma tecnica do make-atlas-dds.ps1 do spaceinvaders (DDS RGBA8 SEM
# compressao, header DX10, lido direto pelo tinydds do ResourceLoader): nada de
# ferramenta externa, o arquivo e escrito byte a byte aqui.
#
# Atlas 32x16, em celulas:
#
#   (0,0)  16x8   tijolo   — com BISEL: borda mais transparente que o miolo
#   (16,0) 16x4   raquete  — bisel so nas laterais
#   (16,8) 4x4    bola     — solida
#
# Os pixels sao BRANCOS de proposito: a cor final vem do tint por vertice do
# batcher (o jogo passa a cor por linha de tijolo), como nos arcades com overlay
# de cor. O relevo vem do ALPHA, nao da cor — assim o tint continua mandando.
#
# As regioes em pixels correspondentes vivem em BreakoutSprites.h (do JOGO —
# tabela de regioes nao entra no common). Mudou o atlas aqui, atualize la.
#
# Uso: powershell -File tools\make-atlas-dds.ps1   (a partir da raiz do repo)

$ErrorActionPreference = 'Stop'

$repoRoot = Split-Path -Parent $PSScriptRoot
$outDir = Join-Path $repoRoot 'assets\textures'
$outFile = Join-Path $outDir 'atlas.dds'
New-Item -ItemType Directory -Force $outDir | Out-Null

$texW = 32; $texH = 16
$pixels = New-Object byte[] ($texW * $texH * 4)   # RGBA, ja zerado (transparente)

# Pinta um pixel branco com o alpha dado.
function Set-Pixel([int]$x, [int]$y, [int]$alpha) {
    $i = ($y * $script:texW + $x) * 4
    $script:pixels[$i] = 255
    $script:pixels[$i + 1] = 255
    $script:pixels[$i + 2] = 255
    $script:pixels[$i + 3] = $alpha
}

# Retangulo branco com borda mais fraca (o bisel que da relevo ao tijolo).
function Blit-Beveled([int]$ox, [int]$oy, [int]$w, [int]$h, [int]$edgeAlpha) {
    for ($y = 0; $y -lt $h; $y++) {
        for ($x = 0; $x -lt $w; $x++) {
            $onEdge = ($x -eq 0) -or ($y -eq 0) -or ($x -eq $w - 1) -or ($y -eq $h - 1)
            $alpha = if ($onEdge) { $edgeAlpha } else { 255 }
            Set-Pixel ($ox + $x) ($oy + $y) $alpha
        }
    }
}

Blit-Beveled 0  0 16 8 150   # tijolo: bisel em volta
Blit-Beveled 16 0 16 4 190   # raquete: bisel mais discreto
Blit-Beveled 16 8 4  4 255   # bola: solida

$stream = [System.IO.File]::Create($outFile)
$w = New-Object System.IO.BinaryWriter($stream)
try {
    $w.Write([uint32]0x20534444)          # magic "DDS "
    # DDS_HEADER (124 bytes)
    $w.Write([uint32]124)                 # dwSize
    $w.Write([uint32]0x2100F)             # CAPS|HEIGHT|WIDTH|PITCH|PIXELFORMAT|MIPMAPCOUNT
    $w.Write([uint32]$texH)               # dwHeight
    $w.Write([uint32]$texW)               # dwWidth
    $w.Write([uint32]($texW * 4))         # dwPitchOrLinearSize (bytes por linha)
    $w.Write([uint32]0)                   # dwDepth
    $w.Write([uint32]1)                   # dwMipMapCount
    for ($i = 0; $i -lt 11; $i++) { $w.Write([uint32]0) }  # dwReserved1[11]
    # DDS_PIXELFORMAT (32 bytes): fourCC DX10 -> formato vem do header estendido
    $w.Write([uint32]32)                  # dwSize
    $w.Write([uint32]0x4)                 # DDPF_FOURCC
    $w.Write([uint32]0x30315844)          # 'DX10'
    for ($i = 0; $i -lt 5; $i++) { $w.Write([uint32]0) }   # masks (nao usadas)
    $w.Write([uint32]0x1000)              # dwCaps = DDSCAPS_TEXTURE
    for ($i = 0; $i -lt 4; $i++) { $w.Write([uint32]0) }   # caps2..4 + reserved2
    # DDS_HEADER_DXT10 (20 bytes)
    $w.Write([uint32]28)                  # DXGI_FORMAT_R8G8B8A8_UNORM
    $w.Write([uint32]3)                   # D3D10_RESOURCE_DIMENSION_TEXTURE2D
    $w.Write([uint32]0)                   # miscFlag
    $w.Write([uint32]1)                   # arraySize
    $w.Write([uint32]0)                   # miscFlags2 (alpha mode: unknown)
    $w.Write($pixels)
}
finally {
    $w.Dispose()
}

Write-Host "OK: $outFile ($((Get-Item $outFile).Length) bytes - esperado $(128 + 20 + $pixels.Length))"
