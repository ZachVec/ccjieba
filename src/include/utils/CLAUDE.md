## src/include/utils — Binary I/O utilities

`bostream` / `bistream` for compact, portable binary serialization into contiguous byte buffers. Header-only, no standard `iostream` integration — cursor-based sequential read/write with `operator<<`/`operator>>` chaining.

### Files

- **io.hh** — `bostream` (growable byte buffer) and `bistream` (cursor over read-only byte span). Inline templates for `basic_string` serialization; string writes are length-prefixed (size then raw data).

### Serialization conventions

- All primitive types serialize via `reinterpret_cast<const byte*>(&value) + sizeof(value)`.
- Strings are length-prefixed: write size first, then raw character bytes.
- `bistream::read()` uses `memmove` for safety; caller validates buffer availability before reading.
