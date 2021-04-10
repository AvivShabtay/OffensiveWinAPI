# PIC Injection
Related projects under this solution:
* PIC
* PicInjection

---

## Background
PIC (Position Independent Code) is very useful for writing shellcode that going to run in target processes without the need of knowing the target relative API function addresses.

---

## Writing PIC
You can write your PIC in pure assembly, compile it and use the compiled bytes, save the them  in char bytes and inject them into the target process. Check my Shellcode project for more info.

The alternative way is creating functions which act as PIC that would represent the shellcode to be injected into the target process. The project represent this method, which in my opinion is more stable and maintainable.<br>Their are more cautions one should take when using this method, check the first link below for more info.

---

## Compiler configurations
In order to keep the PIC as "pure" as possible from compiler optimizations, some restriction need to be made, check the first link below for more information:

Disable runtime checks: 
```
#pragma runtime_checks( "", off )
// ... PIC goes here
#pragma runtime_checks( "", restore)
```

Disable optimizations:
```
#pragma optimize("", off)
// ... PIC goes here
#pragma optimize("", on)
```

Prevent security cookie:
```
DWORD __declspec(safebuffers) picFunction(...) {...}
```

Prevent inline optimizations:
```
DWORD __declspec(noinline) picFunction(...) {...}
```

Disable `SupportJustMyCode` if you need to debug your PIC in target process.

---

## Useful Information
* Very detailed example of PID injection - https://www.codeguru.com/cpp/w-p/system/processesmodules/article.php/c5767/Three-Ways-To-Inject-Your-Code-Into-Another-Process.htm#section_3
* Writing shellcode in C++ - http://www.rohitab.com/discuss/topic/40820-writing-shellcode-in-c/?p=10095205
* Simple example of the injected function signature - http://www.rohitab.com/discuss/topic/38357-code-injection-using-setthreadcontext/?p=10084410


## Self cleanup Examples
* 32 bit ROP for cleanup PIC after execution - https://www.unknowncheats.me/forum/1171005-post3.html?s=1cd370b099e1ea56177fcd39f9c69679
* 32 bit ROP for cleanup PIC after execution - https://www.unknowncheats.me/forum/2267820-post1.html