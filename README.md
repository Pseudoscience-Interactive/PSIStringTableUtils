# PSI String Table Utilities

The **PSI String Table Utilities** plugin provides a Blueprint API to modify String Tables and to easily write and
retrieve their metadata.

<!-- TODO: Add the Fab listing link once the listing is published. -->
[Documentation](https://pseudoscienceinteractive.com/psi-string-table-utilities/) |
[Discord](https://discord.gg/xFj7AXC9Ut) |
[Website & Support](https://pseudoscienceinteractive.com/psi-string-table-utilities/) |
[PSI Property Spreadsheet](https://www.fab.com/listings/ce66d2c8-5fbb-43a7-85d7-bd84e3ad03de)

## Why does this plugin exist?

[String Tables](https://dev.epicgames.com/documentation/unreal-engine/using-string-tables-in-unreal-engine) can store
additional **metadata** for each of their entries. In vanilla Unreal, however, Blueprints can only read this metadata
as plain strings. If you want to use it for anything other than display purposes, you have to write a conversion for
every single metadata value you define.

This plugin removes this step. It reads and writes the metadata of an entry as a struct, where each struct property
corresponds to a metadata value. In addition, String Tables and their entries can be referenced as proper values in
Blueprints, rather than as raw table names and key strings.

## Features

**String Table Types**
- String Tables and individual entries can be referenced directly using *PSI String Table ID* and *PSI String Table
  Entry Handle*, similar to Data Table Row Handles.
- Both types can be picked from dropdowns in the Details panel and on Blueprint pins.

**Metadata Structs**
- Use structs to write metadata of String Table entries, and read the metadata back into a struct.
- Struct properties are matched with metadata values by name and converted to the property's type.

**Blueprint Nodes & C++ API**
- Nodes for reading String Tables, their entries, and metadata (Editor and packaged games).
- Nodes for modifying String Tables and their metadata (Editor only).
- Nodes for importing and exporting String Tables as CSV (Editor and packaged games).
- The metadata-as-struct functions are also available in C++.

**Synergy with PSI Property Spreadsheet**
- With the [PSI Property Spreadsheet](https://www.fab.com/listings/ce66d2c8-5fbb-43a7-85d7-bd84e3ad03de) you can
  inspect and modify your metadata directly in the Unreal Editor.
- Metadata structs defined and edited in the Spreadsheet can be read back as the same structs.
- The Property Spreadsheet is not required. This plugin works on its own.

## Requirements

- Unreal Engine 5.3 to 5.8.
- No other plugins are required.

| Module | Type | Platforms |
| --- | --- | --- |
| `PSIStringTableUtils` | Runtime | Win64, Mac, Linux, LinuxArm64, Android, iOS, tvOS |
| `PSIStringTableUtilsEditor` | Editor | Win64, Mac, Linux |

## Installation

**From Fab.** Add the plugin to your library on Fab and install it to your engine through the Epic Games Launcher.
It is then available in every project of that engine version.

**From source.** Clone or download this repository into the `Plugins` folder of your project, e.g.
`MyProject/Plugins/PSIStringTableUtils`. The plugin is compiled together with your project, so you need a C++
toolchain for Unreal Engine.

If the plugin is not enabled by default, go to *Edit > Plugins*, search for *PSI String Table Utilities* and enable
it.

## Usage

In Blueprints, the nodes are listed under the *PSI > String Table* category. The most important ones are
*Get String Table Entry Meta-Data as Struct* and *Set String Table Entry Meta-Data from Struct*.

In C++, add `PSIStringTableUtils` to the dependencies of your module and include `PSIStringTableUtilsHelper.h`:

```cpp
FMyDialogueMetaData MetaData;
FPSIStringTableUtilsHelper::GetStringTableEntryMetaDataAsStruct(StringTable, TextKey, MetaData);
```

See the [documentation](https://pseudoscienceinteractive.com/psi-string-table-utilities/) for all nodes and types.

## Tests

The plugin contains automation tests. Run them in the Session Frontend or from the command line with
`Automation RunTests PSI.StringTableUtils`.

## Support

If you have questions, found a bug, or just want to get in touch, join our [Discord server](https://discord.gg/xFj7AXC9Ut)
or use the [contact form](https://pseudoscienceinteractive.com/contact/).

## License

This plugin is free and licensed under the [MIT License](LICENSE). The copy on Fab is the same plugin and is also
distributed under the Fab Standard License, so you can use it under either license.
