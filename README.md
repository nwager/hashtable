# C HashTable Library

This C library provides an implementation and API for a hashtable created from scratch. I started this little project on vacation to kill time. Note the the Makefile may contain Mac-specific commands.

I chose to export this library by linking the `.a` file to get practice with the C library system, as opposed to directly including the source files in another project.

## Dependencies

This library requires the archive file and headers from the [linkedlist repo](https://github.com/nwager/linkedlist). Place `liblinkedlist.a` in the `lib/` directory and `linkedlist.h` in the `include` directory.

If you have the linkedlist repo locally, you can automate this process by running
```bash
./updatelib.sh /path/to/linkedlist
```
with the path to the local linkedlist repo.

## Usage

Run `make` to generate a C archive file at `out/libhashtable.a`. Alternatively, run `make lib` to generate the `libout` folder which contains the archive file and any necessary header files.

To use the library, include the header files in your source code and link the library with your linker.

## Testing

Run `make runleak` to run the test suite and check for memory leaks. If successful, it should exit without errors.