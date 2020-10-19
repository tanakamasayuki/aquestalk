# AquesTalk ESP32

See [Documentation](https://www.a-quest.com/).

## In the case of ESP32-Arduino version 1.0.4
You need to edit the platform.local.txt file.
https://github.com/espressif/arduino-esp32/pull/4209

C:\Users\%username%\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4

```
compiler.libraries.ldflags=

recipe.c.combine.pattern="{compiler.path}{compiler.c.elf.cmd}" {compiler.c.elf.flags} {compiler.c.elf.extra_flags} -Wl,--start-group {object_files} "{archive_file_path}" {compiler.c.elf.libs} {compiler.libraries.ldflags} -Wl,--end-group -Wl,-EL -o "{build.path}/{build.project_name}.elf"
```
