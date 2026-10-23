# Verified Ultimate ASI Loader asset

`dinput8.dll` matches the DLL extracted from the Win32/x86 [v9.7.1 upstream release](https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/tag/v9.7.1), product version `9.7.1.0-2155f21`.

On 2026-10-23 the release ZIP was downloaded from the URL in `loader-manifest.json`, its SHA-256 was checked against GitHub's release-asset digest, and the extracted DLL hash was compared with this bundled file. Both comparisons matched. The archive contains one file, `dinput8.dll`.

- Archive SHA-256: `B69853DF1099DE97B9622B9F693F2FC62269CD7774A26408974F1BCABF806FC3`
- DLL SHA-256: `7675B24A2E408349C8A59B87E3AE8724AD8A89EFF21C96A9C109650A3C1316E0`

Packaging and installation enforce the DLL pin before copying anything into their destinations. To update the loader, verify a specific upstream archive and its extracted DLL, then update the asset and manifest together. Do not derive a new trusted pin from an unexplained replacement DLL alone. The upstream license is in `Ultimate-ASI-Loader-LICENSE.txt`, obtained from the same release tag.
