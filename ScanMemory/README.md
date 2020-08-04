# Scan memory in order to find hidden modules

## Background
* Using <code>VirtualQueryEx</code> to get the virtual memory ranges of another process.
* Using <code>GetMappedFileName</code> on every memory range in order to find the mapped module to the virtual memory and get it's file-name.
---

## Demo
<img src="images/example.png" width=800></img>
Running the executable file with given PID.

---

## Notes
* This method could find modules that was hide from detection using unlinking the module from the list::
  * <code>InLoadOrderModuleList</code>
  * <code>InMemoryOrderModuleList</code>
  * <code>InInitializationOrderModuleList</code>
* Whenever you use <code>VirtualQueryEx</code>  there is an option that the process is currently loading new module into memory and you could missed it.
* In order to find module that was loaded reflectively (not from file in the file-system), you should scan virtual memory ranges for "MZ" signatures or similar indicators for recognizing PE file. 
---

## TODO:
- [ ] Add option to print less verbose output
- [ ] Suspend the process before scanning it
- [ ] Add scanner for reflective loaded modules
---