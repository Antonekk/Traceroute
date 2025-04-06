# Traceroute

A simple implementation of the `traceroute -I` in C.
It maps the path of IP packets to a particular host.

## Compilation

To compile the program, run:

```bash
make
```

The `traceroute` executable will be created.

## Usage

To run traceroute to a specific IP address (e.g., Google DNS):

```bash
sudo ./traceroute 8.8.8.8
```

**Note**: Root privileges (`sudo`) are necessary, as raw sockets are used by the program.

## Makefile Targets

- `make` – compiles the program
- `make clean` – removes object files
- `make distclean` – removes object files and executable
- `make test` – runs test traceroute to `8.8.8.8`
