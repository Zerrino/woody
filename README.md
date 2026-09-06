# Woodywood Packer - Ecole 42 project

## Objectives

The goal of this project is to code a program that will
- At first encrypt a program given as parameter.
- Only 64 bits ELF files will be managed here.
- A new program called `woody` will be generated from this execution.
- When this new program (`woody`) will be executed, it will have to be decrypted to be run. Its execution has to be totally identical to the program given as parameter in the last step. Even though we won’t get into compression possibilities directly in this subject, we strongly advise you to explore the possible methods!

## How To?

```bash
make

# infect the file
./woody_woodpacker <file=elf64_bin>

# run the infected file
./woody
```

## General Instructions

Your project must be written in `C` (the version is up to you) and submit a makefile with the usual rules.
- Within the mandatory part, you are allowed to use the following functions:
	- `open`, `close`, `exit`
	- `fpusts`, `fflush`, `lseek`
	- `mmap`, `munmap`, `mprotect`
	- `perror`, `strerror`
	- `syscall`
	- the functions of the `printf` family
- The function authorized within your libft (`read`, `write`, `malloc`, `free`, for example)
- You are allowed to use other functions to complete the bonus part as long as their use is justified during your defence. Be smart!

## Mandatory part

- The executable must be named `woody_woodpacker`.
- The program takes a binary file parameter (64 bits ELF only).
- At the end of the execution of your program, a second file will be created named `woody`.
- You are free to choose the encryption algorithm.

## Quick Theory about Packers

Let's go back to the premises of packers. An interesting research paper ("*Revealing Packed Malware*", Wei YAN, Zheng ZHANG, 2007) states the following

> ### The packer problem
> To evade malicious content detection, malware authors use **packers**, binary tools that instigate code obfuscation. By using executable packers, modern malware can completely bypass personal firewalls and antivirus (AV) scanners.
> [...]
> **Packers** are software programs that compress and encrypt other executable files in a disk and restore the original executable images when the packed files are loaded into memories. A packed file is a type of archived file, so we can’t say that just because a file is packed, it’s bad. Some commercial packers help protect Windows applications against modern cracking tools by putting those applications into a strong protection “shell.”
> [...]
> However, viruses have used packers widely to avoid detection, and packers are increasingly incorporated into some malware families. Reportedly, among 735 malwares collected for the WildList in March 2006, more than 92 percent were packed by crypters and packers from 30 different families.
> [...]
> Packed malware must unpack itself at runtime before it executes, and so security researchers can use RE tools to find the exact moment and location at which the original data will be uncompressed and available.
> [...]

> ### How packers work
> *ndlr; the paper is explaining PE format, which does not belong to this project, so we skip it*
>
> Aside from shrinking the original file size, packing is an efficient way to obfuscate a file’s original contents, and, as of publication time, packers are malware authors’ favored binary tools for obscuring their codes.
> Code obfuscation has evolved from simple compression/encryption to polymorphism/metamorphism and finally to packing. Based on their purposes and behaviors, we can broadly classify packers into four categories:
> - *Compressors* that shrink files sizes with little or no anti-unpacking tricks. Popular compressors include [UPX](https://upx.github.io/)
> - *Crypters* that encrypt and obfuscate the original file contents and prevent the files from being unpacked without any compression.
> - *Protectors* combines features from both compressors and crypters
> - *Blunders* pack a software package of multiple executable and data files into a single bundled executable file, which unpacks and accesses files within the package, without extracting them to disk.
>
> To perform packing, a packer first parses PE/ELF internal structures. Then, it reorganizes PE/ELF headers, sections, import tables, and export tables into new structures and attaches a code segment that the malware will invoke before the OEP. This code is called the `stub`, and it decompresses the original data and locates the OEP (original entry point).
>
> During packing, a packer compresses and encrypts the code and resource sections using the compression and encryption libraries. With randomization, the packer can also generate different variants of a single file every time the file is packed. For some powerful packers, the polymorphism engine also adds a protection layer against RE and debugging. Generally, when a computer invokes a packed file, the packer **stub** will first be invoked to unpack the file in the memory, and then the codes in the original file will get executed. There are several steps the stub engine needs to follow:
> - Save register context at the entry point
> - Decrypt and decompress the code and data sections
> - Load and link the libraries and APIs that the original executable imported
> - Restore the register context saved at the entry point
> - Continue to execute the instructions at the OEP, usually with an intersection jump instruction
>
> Another obfuscation technology is *API call redirection*, which aims to make an executable file smaller and prevent it from running if a security application doesn’t unpack it correctly.

> ### Unpacking malware
> Unpacking is the process of stripping the packer layer (or layers) off packed executables to restore the original contents[...]. We can use three different techniques to unpack a packed file: *manual unpacking*, *static unpacking*, or *generic unpacking*.
> [...]
> Aside from some rare exceptions, most obfuscated programs require an intersection long jump to transit the execution flow from the packer section to the section containing the OEP. If a generic unpacking engine can capture the intersection jumps, the AV engine could use the following heuristics to determine whether the OEP has arrived:
> - *Instruction pointer rule* - IDA Pro’s universal unpacker plug-in tracks the destination instruction pointer (referred to as EIP for Intel IA32 processors) and assumes that the OEP has been reached once EIP falls within a section located before the packer layer and that the packed file has been fully unpacked before the OEP jumps.
> - *Stack pointer rule* - to ensure that the original executable executes correctly, most packers will restore the stack level (referred to as ESP for Intel IA32 processors) to the value it had when the packer codes start to execute.
> - *Signature rule* - For example, Graf proposed searching for popular compilers’ entry signatures, for example, Microsoft Visual C++, GNU C++, or Delphi, whose signatures are relatively static among all executables these compilers generate.
> - *Behavior rule* - For example, Graf also proposed stopping the OEP searching at some Windows API functions, such as CreateWindowA, which aren’t usually called by the packer codes.

## Project reflections and choices

- **Infection Method** - *Code Cave Injection* - Every executable format aligns sections/segments to page boundaries (typically 4096 bytes). This alignment creates **gaps** — unused padding bytes between the end of actual section content and the next aligned offset. The stub gets injected into one of these caves. The original entry point is patched to point to the cave. After decryption, the stub jumps back to the original OEP. **No new sections or segments are added** — making this the most portable and least detectable method.
- **Cipher** - *SPECK-128/256*
	- Security properties checklist:
	- Unpredictability - ✅ - Without knowing internal state
	- Backtracking resistance - ✅ - past outputs safe even if state leaked
	- Forward secrecy - ✅ - Future outputs safe after state compromise
	- Statistical randomness - ✅ - Passes **NIST SP800-22** tests
	- Entropy estimation - ⚠️  - Requires initial boot-time collection
