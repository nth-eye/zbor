# zbor

Small C++20 CBOR codec. No dynamic memory allocation, everything is de/encoded on-the-fly in a given buffer, full `constexpr` support for both de/encoder (few limitations for text strings) and half-float support included with [`utl::`][1]. Decoding can be done manually or with range-based for loop.

## Guide

`using namespace zbor`

Decoder toolset consists of `decode()`, which returns decoded `item`, status `err`, and pointer to next byte past last interpreted. For convenient use in range-based for loop there is `seq` wrapper, which decodes adjacent items in a sequence one by one. Range safely stops at anything invalid, but doesn't provide info about failure if one happens. To get exact error you need to decode and check manually every item.

Encoder can be created with memory provided by user as `view`, or self-contained template as `codec<>`. To pass either of those to handler functions use `ref` and `cref`. All these classes provide same functionality through CRTP base class, so no overhead of virtual function calls, and no unnecessary pointer to self-contained memory for `codec<>`. Encoder also provides `zbor::literals` to make use of overloading for `encode()` and variadic `encode_(...)` API. Encoder is almost fully `constexpr` except for text strings `const char*` and `std::string_view`, because it involves `reinterpret_cast` which is forbidden. At compile time text can instead be encoded with explicit `encode_text()` with byte arrays or special `_txt` literal for strings.

> ⚠️ Use `_txt` literal only in `constexpr` context. At runtime this can lead to unnecessary overhead, so prefer std::string_view.

## Examples

### Logger

```cpp
const uint8_t example_seq[] = { 
    0x01,                           // 1
    0x64, 0x5a, 0x42, 0x4f, 0x52,   // "ZBOR"
    0x83, 0x01, 0x02, 0x03,         // [1, 2, 3]
    0x9f, 0x01, 0x02, 0x03, 0xff,   // [_ 1, 2, 3]
    0xf5,                           // true
};
const uint8_t example_map[] = { 
    0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03, // {"a": 1, "b": [2, 3]}
};
zbor::log_seq(example_seq);
zbor::log_seq_with_pad(example_map);
```
> \>\>\>
```
+-----------HEX-----------+
| 01 64 5a 42 4f 52 83 01  02 03 9f 01 02 03 ff f5  |.dZBOR..........|
+--------DIAGNOSTIC-------+
| 1) 1
| 2) "ZBOR"
| 3) [1, 2, 3]
| 4) [_ 1, 2, 3]
| 5) true
+-------------------------+
```
```
+-----------HEX-----------+
| a2 61 61 01 61 62 82 02  03                       |.aa.ab..........|
+--------DIAGNOSTIC-------+
{
 "a": 1,
 "b": [
   2,
   3,
 ],
}
+-------------------------+
```

### Decode

#### Range-based for loop

```cpp
const uint8_t example[] = { 
    0x01,                           // 1
    0x64, 0x5a, 0x42, 0x4f, 0x52,   // "ZBOR"
    0x83, 0x01, 0x02, 0x03,         // [1, 2, 3]
    0x9f, 0x20, 0x21, 0x22, 0xff,   // [_ -1, -2, -3]
    0xf5,                           // true
};

for (auto& it : zbor::seq{example}) {
    printf("<%s> ", zbor::str_type(it.type)); 
    switch (it.type) {
        case zbor::type_uint:
            printf("%lu \n", it.uint); 
        break;
        case zbor::type_text:
            printf("\"%.*s\" \n", int(it.text.size()), it.text.data()); 
        break;
        case zbor::type_array:
            if (it.arr.indef())
                printf("indefinite \n");
            else
                printf("size %lu \n", it.arr.size());
            for (auto arr_it : it.arr)
                printf("\t <%s> \n", zbor::str_type(arr_it.type));
        break;
        default:
            puts("! unexpected !");
    }
}
```

#### Manually

```cpp
const uint8_t illformed[] = {
    0xbf, 0x81, 0x00, 0xf5, 0xff, // {_ [0]: true}
    0xfe, // <invalid> - AI 30 is reserved
};

zbor::item obj;
zbor::err err;
auto ptr = illformed;
auto end = illformed + sizeof(illformed);

while (1) {
    if (ptr >= end) {
        puts("finished");
        break;
    }
    std::tie(obj, err, ptr) = zbor::decode(ptr, end);
    
    if (err != zbor::err_ok) {
        printf("error, %d -> %s", err, zbor::str_err(err));
        break;
    }
    printf("<%s> ", zbor::str_type(obj.type)); 

    switch (obj.type) {
        case zbor::type_map:
            if (obj.map.indef())
                printf("indefinite \n");
            else
                printf("size %lu \n", obj.map.size());
            for (auto [key, val] : obj.map)
                printf("\t <%s>: <%s>\n", zbor::str_type(key.type), zbor::str_type(val.type));
        break;
        default:
            puts("! unexpected !");
    }
}
```

### Encode

#### `codec<>` and explicit interface

```cpp
using namespace std::literals;          // for std::string_view 

zbor::codec<26> msg;

msg.encode_arr(4);                      // start fixed size array
msg.encode_sint(-1);                    // signed
msg.encode_sint(1);                     // signed
msg.encode_uint(1);                     // unsigned
msg.encode_bool(false);                 // simple "false"

msg.encode_indef_map();                 // start indefinite map
msg.encode_text("hello");               // text string from const char*
msg.encode_data({0xde, 0xad});          // byte string from std::initializer_list
msg.encode_text("world"sv);             // text string from std::string_view
msg.encode_text({0xe2, 0x9c, 0x9d});    // text string from std::initializer_list
msg.encode_break();                     // break, end of indefinite map
```
> \>\>\>
```
+-----------HEX-----------+
| 84 20 01 01 f4 bf 65 68  65 6c 6c 6f 42 de ad 65  |. ....ehelloB..e|
| 77 6f 72 6c 64 63 e2 9c  9d ff                    |worldc..........|
+--------DIAGNOSTIC-------+
| 1) [-1, 1, 1, false]
| 2) {_ "hello": h'dead', "world": "✝"}
+-------------------------+
```

#### `view` and implicit interface with literals

```cpp
using namespace std::literals;
using namespace zbor::literals;

uint8_t data[] = {0x44, 0x45};
uint8_t buf[99];
zbor::view msg{buf};

msg.encode(2_map);                  // start fixed size map
msg.encode(42);                     // unsigned
msg.encode(3_arr);                  // start fixed size array
msg.encode(zbor::prim_undefined);   // simple "undefined"
msg.encode(zbor::prim_null);        // simple "null"
msg.encode(16_prim);                // simple 16
msg.encode(69_tag);                 // tag number, next object will be content
msg.encode("text"_txt);             // text string from zbor::enc::txt
msg.encode(data);                   // byte string from zbor::span

msg.encode(zbor::indef_arr);        // start indefinite array
msg.encode(zbor::indef_txt);        // start indefinite text string made from chunks
msg.encode("const char*");          // chunk from const char*
msg.encode("string_view"sv);        // chunk from std::string_view
msg.encode("literal_txt"_txt);      // chunk from zbor::enc::txt
msg.encode(zbor::breaker);          // break, end of indefinite text string
msg.encode(zbor::indef_dat);        // start indefinite byte string made from chunks
msg.encode({});                     // chunk from empty std::initializer_list
msg.encode({0x77, 0x77, 0x77});     // chunk from std::initializer_list
msg.encode({data, sizeof(data)});   // chunk from explicit span (not necessary in this case)
msg.encode(zbor::breaker);          // break, end of indefinite byte string
msg.encode(zbor::breaker);          // break, end of indefinite array
```
> \>\>\>
```
+-----------HEX-----------+
| a2 18 2a 83 f7 f6 f0 d8  45 64 74 65 78 74 42 44  |..*.....EdtextBD|
| 45 9f 7f 6b 63 6f 6e 73  74 20 63 68 61 72 2a 6b  |E..kconst char*k|
| 73 74 72 69 6e 67 5f 76  69 65 77 6b 6c 69 74 65  |string_viewklite|
| 72 61 6c 5f 74 78 74 ff  5f 40 43 77 77 77 42 44  |ral_txt._@CwwwBD|
| 45 ff ff                                          |E...............|
+--------DIAGNOSTIC-------+
| 1) {42: [undefined, null, simple(16)], 69("text"): h'4445'}
| 2) [_ (_ "const char*", "string_view", "literal_txt"), (_ h'', h'777777', h'4445')]
+-------------------------+
```

#### Variadic interface with literals

```cpp
using namespace std::literals;
using namespace zbor::literals;

zbor::codec<42> msg;

msg.encode_(
    1_map,                  // start fixed size map
        42.0f,              // float32, will be compressed to half-float if possible     
        42.0,               // float64, will be compressed to half-float if possible
    2_arr,                  // start fixed size array
        true,               // simple "true"
        false,              // simple "false"
    zbor::indef_map,        // start indefinite size map
        zbor::prim(255),    // simple 255 explicit
        255_prim,           // simple 255 with literal
        666u,               // unsigned
        666,                // unsigned
        -777,               // signed
        777_tag,            // tag number
            "_txt"_txt,     // text string from zbor::enc::txt
    zbor::breaker,          // break, end of indefinite map
    "variadic"sv            // text string from std::string_view
);
```
> \>\>\>
```
+-----------HEX-----------+
| a1 f9 51 40 f9 51 40 82  f5 f4 bf f8 ff f8 ff 19  |..Q@.Q@.........|
| 02 9a 19 02 9a 39 03 08  d9 03 09 64 5f 74 78 74  |.....9.....d_txt|
| ff 68 76 61 72 69 61 64  69 63                    |.hvariadic......|
+--------DIAGNOSTIC-------+
| 1) {42.0: 42.0}
| 2) [true, false]
| 3) {_ simple(255): simple(255), 666: 666, -777: 777("_txt")}
| 4) "variadic"
+-------------------------+
```

#### Passing `ref`

```cpp
zbor::codec<16> msg;

msg.encode_(true, false, zbor::prim_null);

[] (zbor::ref ref) {
    ref.encode(66);
}(msg);
```
> \>\>\>
```
+-----------HEX-----------+
| f5 f4 f6 18 42                                    |....B...........|
+--------DIAGNOSTIC-------+
| 1) true
| 2) false
| 3) null
| 4) 66
+-------------------------+
```

#### With dynamically allocated memory

```cpp
auto buf = (uint8_t*) malloc(10);   // let's assume someone still uses malloc...
auto msg = zbor::view{{buf, 10}};   // NOTE: there is also initial size argument if buffer already contains CBOR, default is 0
auto err = msg.encode("too long string");

if (err == zbor::err_ok)
    zbor::log_seq(msg);
else
    printf("error: %d -> %s \n", err, zbor::str_err(err));
```
> \>\>\>
```
error: 1 -> no_memory 
```

## TODO

- [x] source
- [x] tests
- [x] reamde

[1]: https://github.com/nth-eye/utl
