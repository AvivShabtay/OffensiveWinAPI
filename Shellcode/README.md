# DLL Loader shellcode:

Steps
-
1. Find the PEB address.
2. Find the InMemory Loaded-Module-List.
3. Find the desired imports: GetProcAddress, LoadLibrary.
4. Push the malicious DLL name.
5. Load the Malicious DLL.
---

Notes:
-
1. Tested on x86.
2. Position Independent Code.
2. Null-terminator free.
3. Relative Jumps.
4. Not reflective, assume the DLL is on the disk.
---

Attack scenario:
-
1. Gain foot hold in target machine.
2. Deliver malicious DLL into the target machine.
3. Choose injection method that takes shellcode.
4. Embed the shellcode in your payload.
5. Exploit.
---

Helper Python script to print Hex values of given string:
-
```python
str = "DL.dll"[::-1]		# saved as: 'lld.LD'
for x in str :
	print(hex(ord(x)))
```
---

Helper command to convert executable binary to bytes-code:
-
```bash
objdump -d ./your_program|grep '[0-9a-f]:'|grep -v 'file'|cut -f2 -d:|cut -f1-6 -d' '|tr -s ' '|tr '\t' ' '|sed 's/ $//g'|sed 's/ /\\x/g'|paste -d '' -s |sed 's/^/"/'|sed 's/$/"/g'
```
---

TODO:
-
- [ ] Add architecture detection for x86 and x64.
- [ ] Fix address/offset/other issues for support both x86 and x64 architecture.
- [ ] Add integrated NOP-Sled (maybe not ???).
- [ ] Add Anti-Detection support.
---

Useful links:
-
1. Convert to bytes using objdump: https://reverseengineering.stackexchange.com/a/15926
Sometime the command misses few bytes, check it before use it (!!!)
2. Introduction to Windows shellcode development – Part 3 -
https://securitycafe.ro/2016/02/15/introduction-to-windows-shellcode-development-part-3/
3. Digital Wispher 82 - Windows Shellcoding:
https://www.digitalwhisper.co.il/files/Zines/0x52/DW82-1-WinShellcoding.pdf
4. x86 Shellcode Obfuscation - Part 1 (Fixing jumps):
https://breakdev.org/x86-shellcode-obfuscation-part-1/
5. Art of Anti Detection 3 – Shellcode Alchemy:
https://pentest.blog/art-of-anti-detection-3-shellcode-alchemy/
6. Architecture Detection (x86 or x64) Assembly Stub:
https://zerosum0x0.blogspot.com/2014/12/detect-x86-or-x64-assembly-stub.html
7. Win32 Thread Information Block:
https://en.wikipedia.org/wiki/Win32_Thread_Information_Block
8. TEB and PEB (SEH, PEB, DLL & API functions, Anti-Debugging - TODO):
https://rvsec0n.wordpress.com/2019/09/13/routines-utilizing-tebs-and-pebs/
---


Disclaimer
-
This repository is for research purposes only, the use of this code is your responsibility.

I take NO responsibility and/or liability for how you choose to use any of the source code available here. By using any of the files available in this repository, you understand that you are AGREEING TO USE AT YOUR OWN RISK. Once again, ALL files available here are for EDUCATION and/or RESEARCH purposes ONLY.

Any actions and/or activities related to the material contained within this repository is solely your responsibility.

This repository does not promote any hacking related activity. All the information in this repository is for educational purposes only.